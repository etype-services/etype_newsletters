<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 11/7/17
 * Time: 11:49 a.m.
 */

/* Admin Settings */
function etype_newsletters_admin() {

  $form = [];

  return system_settings_form($form);
}