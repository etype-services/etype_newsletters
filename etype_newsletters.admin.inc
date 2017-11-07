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

  $nodes = _etype_newsletters_get_selected_nodes();
  if (count($nodes) > 0) {
    $options = [];
    foreach ($nodes as $nid) {
      $node = node_load($nid);
      $options['$nid'] = $node->title;
    }
  }

  $form = [];

  $form['newsletters']['etype_newsletter_nodes'] = [
    '#title' => t('Slected Nodes'),
    '#multiple' => TRUE,
    '#description' => t("These are the nodes that will be used to build the newsletter."),
    '#weight' => '1',
    '#type' => 'checkboxes',
    '#options' => $options,
    '#default_value' => $options,
  ];


  return system_settings_form($form);
}

/**
 * @return mixed
 */
function _etype_newsletters_get_selected_nodes() {
  $query = db_select('etype_newsletters', 'n');
  $query->fields('n', array('nid'));
  $results = $query->execute();
  $nodes = [];
  while ($n = $results->fetchObject()) {
    $nodes[] = $n->nid;
  }
  return $nodes;
}