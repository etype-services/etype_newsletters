<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 11/7/17
 * Time: 11:49 a.m.
 */

/**
 * @return mixed
 */
function etype_newsletters_get_selected_nodes() {
  $query = db_select('etype_newsletters', 'n');
  $query->fields('n', array('nid'));
  $results = $query->execute();
  $nids = [];
  while ($n = $results->fetchObject()) {
    $nids[] = $n->nid;
  }
  return $nids;
}

/**
 * Send Newsletter form
 * @param $form_state
 * @return array
 */
function etype_newsletters_admin_form($form, &$form_state) {

  $pub_id = variable_get('etype_pub');
  if (empty($pub_id)) {
    $form['etype_newsletter_nodes'] = [
      '#markup' => '<p>Please set eType Pub Id in eType Settings.<a/>.</p>',
    ];
    return $form;
  } else {

    $form = [];
    $nids = etype_newsletters_get_selected_nodes();
    if (count($nids) > 0) {

      $options = [];
      foreach ($nids as $nid) {
        $node = node_load($nid);
        $options[$nid] = $node->title;
      }

      $form['etype_newsletter_nodes'] = [
        '#title' => t('Selected Stories'),
        '#multiple' => TRUE,
        '#description' => t("These stories will be used to build the newsletter. Deselect any that you do not wish to include."),
        '#type' => 'checkboxes',
        '#options' => $options,
        '#default_value' => $nids,
      ];

      $form['etype_newsletter_subject'] = [
        '#title' => t('Subject'),
        '#description' => t("Enter a subject line for the newsletter."),
        '#type' => 'textfield',
        '#default_value' => 'Test Newsletter',
      ];

      $form['etype_newsletter_is_test'] = [
        '#title' => t('Send newsletter to test email'),
        '#description' => t("If checked, the newsletter will only be sent to the test email below."),
        '#type' => 'checkboxes',
        '#options' => ['yes' => 'Yes'],
        '#default_value' => ['yes'],
      ];

      $setting = variable_get('etype_newsletter_test_email');
      $test_email =  empty($setting)? variable_get('site_mail'): $setting;
      $form['etype_newsletter_test_email'] = [
        '#title' => t('Test email address'),
        '#description' => t("Enter a test email address."),
        '#type' => 'textfield',
        '#default_value' => $test_email,
      ];

      $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Send Newsletter'));
      $form['#action'] = '/admin/config/etype_newsletters/confirm_send';
      return $form;

    } else {

      $form['etype_newsletter_nodes'] = [
        '#markup' => '<p>Select nodes to add to the newsletter at the <a href="/admin/content">Content page<a/>.</p>',
      ];
      return $form;
    }

  }

}

/**
 * @return mixed
 */
function etype_newsletters_admin_confirm_form() {

  if (isset($_POST['etype_newsletter_nodes'])) {

    $form['etype_newsletter_nodes'] = array('#type' => 'hidden', '#value' => $_POST['etype_newsletter_nodes']);
    $form['etype_newsletter_subject'] = array('#type' => 'hidden', '#value' => filter_xss($_POST['etype_newsletter_subject']));
    if (isset($_POST['etype_newsletter_is_test'])) {
      $form['etype_newsletter_is_test'] = array('#type' => 'hidden', '#value' => 'yes');
    } else {
      $form['etype_newsletter_is_test'] = array('#type' => 'hidden', '#value' => 'no');
    }

    if (isset($_POST['etype_newsletter_test_email'])) {

      $email = trim(filter_xss($_POST['etype_newsletter_test_email']));
      $true = valid_email_address($email);
      if ($true === true) {
        $message = 'Click Send to send the newsletter to ' . $email . '.';
        $form['etype_newsletter_test_email'] = array('#type' => 'hidden', '#value' => $email);
      } else {
        drupal_set_message("The test email is not valid. Please check it and try again.", 'error');
        drupal_goto('admin/config/etype/etype_newsletters/send');
      }

    } else {
      $message = "Click Send to send the newsletter to subscribers now.";
    }

    $form['#attributes']['class'][] = 'confirmation';
    $form['description'] = array('#markup' => $message);

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Send'),
    );
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => '/admin/config/etype/etype_newsletters/send',
    );

    if (!isset($form['#theme'])) {
      $form['#theme'] = 'confirm_form';
    }
    $form['#submit'][] = 'etype_newsletters_admin_confirm_form_submit';
    return $form;
  } else {
    drupal_goto('admin/config/etype/etype_newsletters/send');
  }
}


/**
 * @param $form
 * @param $form_state
 * @return mixed
 */
function etype_newsletters_admin_confirm_form_submit($form, &$form_state) {

  $content = '';
  $nids = explode(' ', $form['etype_newsletter_nodes']['#value']);
  $subject = $form['etype_newsletter_subject']['#value'];
  $test = $form['etype_newsletter_is_test']['#value'];
  $email = $form['etype_newsletter_test_email']['#value'];

  //dpm($test);
  //dpm($email);

  if (count($nids) > 0) {

    foreach ($nids as $nid) {
      $nid = trim($nid);
      if (is_numeric($nid)) {
        $node = node_load($nid);
        $view = node_view($node, 'teaser'); // use teaser mode
        $content .= drupal_render($view);   // needs cleaning up to remove links
      }
    }

    // send rendered content to subscriber system
    $pub_id = variable_get('etype_pub');
    if (!empty($pub_id)) {

      ini_set("soap.wsdl_cache_enabled", "0");
      $params = array('PubID' => $pub_id, 'Subject' => $subject, 'MailBody' => $content);
      // is this a test?
      if ($test === 'yes') {
        // validate email address
        $true = valid_email_address($email);
        if ($true === true) {
          $params['TestEmailAddress'] = $email;
        } else {
          drupal_set_message("The test email is not valid. Please check it and try again.", 'error');
          drupal_goto('admin/config/etype/etype_newsletters/send');
        }
      }
      $client = new SoapClient('https://www.etypeservices.com/Service_EmailtoListViaDrupal.asmx?WSDL');
      $response = $client->EmailtoListViaDrupal($params);

      //dpm($response);
      //dpm($content);

      if ($response->EmailtoListViaDrupalResult !== 1) {
        drupal_set_message("An error ocurred, and the newsletter may not have been sent. Please contact eType Customer Service.", 'error');
      } else {
        if ($test !== 'yes') {
          // remove queued nodes from module table
          db_delete('etype_newsletters')
          ->execute();
        }
        drupal_set_message("The newsletter was sent.");
        drupal_goto('admin/config/etype/etype_newsletters/send');
      }


    } else {
      drupal_set_message("Please set eType Pub Id in eType Settings. The newsletter was not sent.", 'warning');
    }

  } else {
    drupal_set_message("No content selected. The newsletter was not sent.", 'warning');
  }

}