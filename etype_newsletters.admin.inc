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
  dpm($nodes);

  $form = [];

  return system_settings_form($form);
}

/**
 * @return mixed
 */
function _etype_newsletters_get_selected_nodes() {

  $query = db_select('etype_newsletters', 'n');
  $query->fields('nid');
  $results = $query->execute();
  return $results;

}