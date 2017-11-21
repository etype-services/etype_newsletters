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
function etype_newsletters_admin_form() {

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
  } else {
    $form['etype_newsletter_nodes'] = [
      '#markup' => '<p>Select nodes to add to the newsletter at the <a href="/admin/content">Content page<a/>.</p>',
    ];
  }

  $form['confirm'] = ['#type' => 'hidden', '#value' => 'true'];
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Build Newsletter'));
  return $form;
}

/**
 * @param $form_state
 * @return mixed
 */
function etype_newsletters_admin_form_confirm(&$form_state) {

  dpm($form_state);

  $form['#attributes']['class'][] = 'confirmation';
  $form['description'] = array('#markup' => "Click Send to send the newsletter now.");
  $form['confirmed'] = array('#type' => 'hidden', '#value' => 1);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/etype/etype_newsletters/settings',
  );

  if (!isset($form['#theme'])) {
    $form['#theme'] = 'confirm_form';
  }
  return $form;

}


/**
 * @param $form
 * @param $form_state
 * @return mixed
 */
function etype_newsletters_admin_form_submit($form, &$form_state) {

  dpm($form_state);

  if (isset($form_state['values']['confirm'])) {
    return etype_newsletters_admin_form_confirm($form_state);
  }

  $content = '';
  $nids = $form['etype_newsletter_nodes']['#value'];
  $subject = $form['etype_newsletter_subject']['#value'];
  if (count($nids) > 0) {
    foreach ($nids as $nid) {
      $node = node_load($nid);
      $view = node_view($node, 'teaser'); // use teaser mode
      $content .= drupal_render($view);   // needs cleaning up to remove links
    }
  }

  // send rendered content to subscriber system
  $pub_id = variable_get('etype_pub');
  if (!empty($pub_id)) {
    ini_set("soap.wsdl_cache_enabled", "0");
    $params = array('PubID' => $pub_id, 'Subject' => $subject, 'MailBody' =>
      $content);
    $client = new SoapClient('https://www.etypeservices.com/Service_EmailtoListViaDrupal.asmx?WSDL');
    // $response = $client->EmailtoListViaDrupal($params);
    // dpm($response);

    /* remove queued nodes from module table
    db_delete('etype_newsletters')
      ->execute();
    */

    $form_state['rebuild'] = TRUE;

  } else {
    drupal_set_message("Please set Pub Id in eType Settings. The newsletter was not sent.", 'warning');
  }

}