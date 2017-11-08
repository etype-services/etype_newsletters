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
function etype_newsletters_admin() {

  $form = [];
  $nids = _etype_newsletters_get_selected_nodes();
  if (count($nids) > 0) {
    $options = [];
    foreach ($nids as $nid) {
      $node = node_load($nid);
      $options[$nid] = $node->title;
    }
    $form['newsletters']['etype_newsletter_nodes'] = [
      '#title' => t('Selected Stories'),
      '#multiple' => TRUE,
      '#description' => t("These stories will be used to build the newsletter."),
      '#weight' => '1',
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $nids,
    ];
  } else {
    $form['newsletters']['etype_newsletter_nodes'] = [
      '#markup' => '<p>Select nodes to add to the newsletter at the <a href="/admin/content">Content page<a/>.</p>',
    ];
  }

  // Set a submit handler manually because the default submit handler
  // gets overridden by the system_settings_form() submit handler.
  $form['#submit'][] = 'etype_newsletters_admin_submit';
  return system_settings_form($form);
}

/**
 * @return mixed
 */
function _etype_newsletters_get_selected_nodes() {
  $query = db_select('etype_newsletters', 'n');
  $query->fields('n', array('nid'));
  $results = $query->execute();
  $nids = [];
  while ($n = $results->fetchObject()) {
    $nids[] = $n->nid;
  }
  return $nids;
}

function etype_newsletters_admin_submit($form, &$form_state) {
  $content = '';
  dpm($form['newsletters']['etype_newsletter_nodes']['#value']);
  $nids = $form['newsletters']['etype_newsletter_nodes']['#value'];
  if (count($nids) > 0) {
    foreach ($nids as $nid) {
      $node = node_load($nid);
      dpm($node);
      $content .= "<h2>" . $node->title . "</h2>";
    }
  }

}