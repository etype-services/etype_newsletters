<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 11/7/17
 * Time: 11:49 a.m.
 */

require('etype_newsletters.twig.inc');

/**
 * @return mixed
 */
function etype_newsletters_get_selected_nodes() {
  $query = db_select('etype_newsletters_nodes', 'n');
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
 * @param $form
 * @param $form_state
 * @return array
 */
function etype_newsletters_admin_form($form, &$form_state) {

  $pub_id = variable_get('etype_pub');
  if (empty($pub_id)) {
    $form['etype_newsletter_nodes'] = [
      '#markup' => '<p>Please set eType Pub Id in eType Settings<a/>.</p>',
    ];
    return $form;

  } else {

    $form = [];
    $nids = etype_newsletters_get_selected_nodes();
    if (count($nids) > 0) {

      $date = format_date(REQUEST_TIME, 'custom', 'Y-m-d H:i:s');
      $format = 'Y-m-d H:i';

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
        '#required' => true,
      ];

      $form['etype_newsletter_subject'] = [
        '#title' => t('Subject'),
        '#description' => t("Enter a subject line for the newsletter."),
        '#type' => 'textfield',
        '#default_value' => 'Test Newsletter',
        '#required' => true,
      ];

      $form['etype_newsletter_subscriber_type'] = [
        '#title' => t('Subscriber Type'),
        '#description' => t("Choose the subscriber type for this mailing."),
        '#type' => 'radios',
        '#options' => ['Regular' => 'Regular', 'NewsAlerts' => 'News Alerts'],
        '#default_value' => 'Regular',
        '#required' => true,
      ];

      $form['etype_newsletter_queue_date'] = [
        '#type' => 'date_select',
        '#title' => t('Queue Newsletter'),
        '#description' => t("Select a date &amp; time to send newsletter."),
        '#default_value' => $date,
        '#date_format' => $format,
        '#date_label_position' => 'within',
        '#date_increment' => 15,
        '#date_year_range' => '-0:+1',
      ];

      $form['etype_newsletter_is_test'] = [
        '#title' => t('Send newsletter to test email'),
        '#description' => t("If checked, the newsletter will not be queued or saved, but sent immediately to a valid test email entered below."),
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
function etype_newsletters_admin_confirm_form($form, &$form_state) {

  if (isset($_POST['etype_newsletter_nodes'])) {

    if (!isset($_POST['etype_newsletter_subscriber_type'])) {
      drupal_set_message("Please choose at least one Subscriber Type.", 'error');
      drupal_goto('admin/config/etype_newsletters/send');
      exit;
    }

    $form['etype_newsletter_nodes'] = array('#type' => 'hidden', '#value' => $_POST['etype_newsletter_nodes']);
    $form['etype_newsletter_subscriber_type'] = array('#type' => 'hidden', '#value' => $_POST['etype_newsletter_subscriber_type']);
    $form['etype_newsletter_subject'] = array('#type' => 'hidden', '#value' => filter_xss($_POST['etype_newsletter_subject']));
    $form['etype_newsletter_test_email'] = array('#type' => 'hidden', '#value' => filter_xss($_POST['etype_newsletter_test_email']));

    // turn date into timestamp
    if (isset($_POST['etype_newsletter_queue_date'])) {
      $date_array = $_POST['etype_newsletter_queue_date'];
      if (is_array($date_array)) {
        $queue_time = sprintf('%04d', $date_array['year']) . '-' . sprintf('%02d', $date_array['month']) . '-' . sprintf('%02d', $date_array['day']) . ' ' . $date_array['hour'] . ':' . $date_array['minute'];
        $queue_date = new DateTime($queue_time);
        $send_date = $queue_date->format('U');
        $form['etype_newsletter_queue_date'] = array('#type' => 'hidden', '#value' => $send_date);
      }
    }

    $form['etype_newsletter_is_test'] = array('#type' => 'hidden', '#value' => 'no');
    $pub_id = variable_get('etype_pub');
    if ($pub_id == '4149') {
      $message = "Click Send to schedule sending to the Test Group.";
    } else {
      $message = "Click Send to schedule sending to the " . $_POST['etype_newsletter_subscriber_type'] . " list.";
    }

    if (isset($_POST['etype_newsletter_is_test'])) {
      if (isset($_POST['etype_newsletter_is_test']["yes"])) {
        if ($_POST['etype_newsletter_is_test']["yes"] === 'yes') {
          $form['etype_newsletter_is_test'] = array('#type' => 'hidden', '#value' => 'yes');
          $email = trim(filter_xss($_POST['etype_newsletter_test_email']));
          $true = valid_email_address($email);
          if ($true === true) {
            $message = 'Click Send to send the newsletter to ' . $email . ' now.';
            $form['etype_newsletter_test_email'] = array('#type' => 'hidden', '#value' => $email);
          } else {
            drupal_set_message("The test email is not valid. Please check it and try again.", 'error');
            drupal_goto('admin/config/etype_newsletters/send');
            exit;
          }
        }
      }
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
      '#href' => '/admin/config/etype_newsletters/send',
    );

    if (!isset($form['#theme'])) {
      $form['#theme'] = 'confirm_form';
    }
    $form['#submit'][] = 'etype_newsletters_admin_confirm_form_submit';
    return $form;
  } else {
    drupal_set_message("No nodes selected to send. Select nodes at the Content page", 'error');
    drupal_goto('admin/config/etype_newsletters/send');
  }
}


/**
 * @param $form
 * @param $form_state
 *
 * @throws \Twig_Error_Loader
 * @throws \Twig_Error_Runtime
 * @throws \Twig_Error_Syntax
 */
function etype_newsletters_admin_confirm_form_submit($form, &$form_state) {

  $nids = explode(' ', $form['etype_newsletter_nodes']['#value']);
  $type = $form['etype_newsletter_subscriber_type']['#value'];
  $subject = $form['etype_newsletter_subject']['#value'];
  $test = $form_state['input']['etype_newsletter_is_test']; // why?
  $email = $form['etype_newsletter_test_email']['#value'];
  $send_date = (int) $form_state['input']['etype_newsletter_queue_date']; // why, oh why?

  // what's the time?
  $date = new DateTime();
  $now = $date->format('U');

  if (count($nids) > 0) {

    $clean_nids = [];
    foreach ($nids as $nid) {
      if (is_numeric($nid)) {
        $clean_nids[] = trim($nid);
      }
    }
    // send rendered content to subscriber system
    $pub_id = variable_get('etype_pub');
    if (!empty($pub_id)) {
      $content = etype_newsletters_build_newsletter($clean_nids);
      ini_set("soap.wsdl_cache_enabled", "0");
      $params = [
        'PubID' => $pub_id,
        'Subject' => $subject,
        'MailBody' => $content,
        'SubscriberType' => $type,
      ];
      // is this a test?
      if ($test !== 'no') {
        // validate email address
        $true = valid_email_address($email);
        if ($true === true) {
          $params['TestEmailAddress'] = $email;
        } else {
          drupal_set_message("The test email is not valid. Please check it and try again.", 'error');
          drupal_goto('admin/config/etype_newsletters/send');
          exit;
        }
      }

      if (($send_date > $now) && ($test !== 'yes')) {

        // save newsletter for sending in the future
        etype_newsletters_save($send_date, $params, 0);
        $formatted = date('m/d/Y H:i', $send_date);
        drupal_set_message("The newsletter was saved successfully and will be sent to the $type list on $formatted.");

      } else {

        $client = new SoapClient('https://www.etypeservices.com/Service_EmailtoListViaDrupal.asmx?WSDL');

        /*$response = $client->EmailtoListViaDrupal($params);

        // dpm($response);
        // dpm($content);

        if ($response->EmailtoListViaDrupalResult !== 1) {
          drupal_set_message("An error ocurred, and the newsletter may not have been sent. Please contact eType Customer Service.", 'error');
        } else {
          if ($test !== 'yes') {
            etype_newsletters_save($send_date, $params, 1);
            drupal_set_message("The newsletter was sent to the $type list.");
          } else {
            drupal_set_message("The newsletter was sent to $email.");
          }
        }*/
      }
    } else {
      drupal_set_message("Please set eType Pub Id in eType Settings. The newsletter was not sent.", 'warning');
    }
  } else {
    drupal_set_message("No content selected. The newsletter was not sent.", 'warning');
  }
  drupal_goto('admin/config/etype_newsletters/complete');
}