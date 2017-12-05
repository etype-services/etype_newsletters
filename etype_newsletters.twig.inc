<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 12/4/17
 * Time: 2:52 p.m.
 */

/**
 * @param array $nids
 * Build Twig template
 */
function etype_newsletters_build_newsletter($nids = ['193651', '193653']) {

  require_once '/data/web/vendor/autoload.php';
  $template_path = drupal_get_path('module', 'etype_newsletters') ;
  $loader = new Twig_Loader_Filesystem($template_path . '/templates');
  $twig = new Twig_Environment($loader, array(
    'cache' => false,
  ));

  $theme_key = variable_get('theme_default');
  $logo_url = file_create_url(theme_get_setting('logo_path', $theme_key));

  $template = $twig->load('white-blue.html');
  $content = $template->render([
    'logo_url' => $logo_url,
  ]);

  // get node data
  if (!empty($nids)) {
    var_dump($nids);
    $nodes = entity_load('node', $nids);
    var_dump($nodes);
    foreach ($nodes as $node) {
      dpm($node);
    }
  }

  // echo $content;
  exit;
}