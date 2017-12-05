<?php
/**
 * Created by PhpStorm.
 * User: charlie
 * Date: 12/4/17
 * Time: 2:52 p.m.
 */

/**
 * @param array $nids
 * @return mixed
 * Build Twig template
 */
function etype_newsletters_build_newsletter($nids = []) {

  //if (empty($nids)) {
  // $nids = ['193651', '193653'];
  //}

  require_once '/data/web/vendor/autoload.php';
  $template_path = drupal_get_path('module', 'etype_newsletters') ;
  $loader = new Twig_Loader_Filesystem($template_path . '/templates');
  $twig = new Twig_Environment($loader, array(
    'cache' => false,
  ));

  $theme_key = variable_get('theme_default');
  $logo_src = file_create_url(theme_get_setting('logo_path', $theme_key));
  $big_image_src = '';
  $site_name = variable_get('site_name');
  $str = substr($site_name, 0, 3);
  if (strtolower($str) !== 'the') {
    $site_name = 'the ' . $site_name;
  }
  $items = [];

  // get node data
  if (!empty($nids)) {
    $nodes = entity_load('node', $nids);
    $ptr = 1;
    foreach ($nodes as $node) {
      dpm($node);
      if ($ptr == 1) {
        if (isset($node->field_image)) {
          $big_image_src = file_create_url($node->field_image['und'][0]['uri']);
        }
      }
      $ptr = 2;
      $body = empty($node->body['und'][0]['summary'])? substr($node->body['und'][0]['safe_value'], 0 , 100): $node->body['und'][0]['summary'];
      $alias = drupal_get_path_alias('node/' . $node->nid);
      $url = $GLOBALS['base_url'] . '/' . $alias;
      $items[] = [
        'title' => $node->title,
        'body' => $body,
        'url' => $url,
      ];
    }
  }

  $template = $twig->load('content.html.twig');
  $content = $template->render([
    'logo_url' => $logo_src,
    'featured_image' => $big_image_src,
    'site_name' => $site_name,
    'items' => $items,
  ]);

  return $content;
}