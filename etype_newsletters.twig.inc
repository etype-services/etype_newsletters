<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 12/4/17
 * Time: 2:52 p.m.
 */

/**
 * Twig
 */
function etype_newsletters_build_newsletter() {
  require_once '/data/web/vendor/autoload.php';
  $template_path = drupal_get_path('module', 'etype_newsletters') ;
  $loader = new Twig_Loader_Filesystem($template_path . '/templates');
  $twig = new Twig_Environment($loader, array(
    'cache' => false,
  ));

  global $theme_key;
  echo $theme_key;
  $logo_url = theme_get_setting('logo_path', $theme_key);
  echo $logo_url;

  $template = $twig->load('white-blue.html');
  //echo $template->render([
  //  'logo_url' => $logo_url,
  //]);
}