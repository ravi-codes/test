<?php
/**
 * @file
 * hpbx_help_2_0.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function hpbx_help_2_0_views_default_views() {
  $export = array();

  $view = new view();
  $view->name = 'hpbx_help';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'HPBX Help';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Help';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Opnieuw instellen';
  $handler->display->display_options['pager']['type'] = 'none';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['style_plugin'] = 'list';
  $handler->display->display_options['style_options']['grouping'] = array(
    0 => array(
      'field' => 'name',
      'rendered' => 1,
      'rendered_strip' => 0,
    ),
  );
  $handler->display->display_options['style_options']['default_row_class'] = FALSE;
  $handler->display->display_options['style_options']['row_class_special'] = FALSE;
  $handler->display->display_options['style_options']['class'] = 'hpbx-help-category-content';
  $handler->display->display_options['style_options']['wrapper_class'] = 'hpbx-help-category-container';
  $handler->display->display_options['row_plugin'] = 'fields';
  $handler->display->display_options['row_options']['inline'] = array(
    'title' => 'title',
  );
  $handler->display->display_options['row_options']['default_field_elements'] = FALSE;
  /* Kop: Global: Text area */
  $handler->display->display_options['header']['area']['id'] = 'area';
  $handler->display->display_options['header']['area']['table'] = 'views';
  $handler->display->display_options['header']['area']['field'] = 'area';
  $handler->display->display_options['header']['area']['label'] = 'Inhoud toevoegen';
  $handler->display->display_options['header']['area']['empty'] = TRUE;
  $handler->display->display_options['header']['area']['content'] = '<?php
global $user;
$account = $user;
if (!empty($_SESSION[\'ccs\'][\'hpbx\'][\'masquerading_from\'])) {
    $account = user_load($_SESSION[\'ccs\'][\'hpbx\'][\'masquerading_from\']);
}

if (user_access(\'edit any hpbx_help_article content\', $account)) {

  echo \'<div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>

<div class="col-xs-5 col-xxs-12 col-sm-4 col-md-3">\' . l(t(\'Add help article\'),\'node/add/hpbx-help-article\', array(\'attributes\' => array(\'class\' => array(\'sg-btn sg-btn-wizard\')))) . \'</div></br>
<div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>

\';

}

?>';
  $handler->display->display_options['header']['area']['format'] = 'php_code';
  $handler->display->display_options['header']['area']['tokenize'] = TRUE;
  /* Relationship: Inhoud: Taxonomy terms on node */
  $handler->display->display_options['relationships']['term_node_tid']['id'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid']['table'] = 'node';
  $handler->display->display_options['relationships']['term_node_tid']['field'] = 'term_node_tid';
  $handler->display->display_options['relationships']['term_node_tid']['required'] = TRUE;
  $handler->display->display_options['relationships']['term_node_tid']['vocabularies'] = array(
    'hpbx_help_type' => 'hpbx_help_type',
    'tags' => 0,
  );
  /* Veld: Taxonomie term: Naam */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'taxonomy_term_data';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['relationship'] = 'term_node_tid';
  $handler->display->display_options['fields']['name']['label'] = '';
  $handler->display->display_options['fields']['name']['exclude'] = TRUE;
  $handler->display->display_options['fields']['name']['element_type'] = 'div';
  $handler->display->display_options['fields']['name']['element_class'] = 'hpbx-help-category-title';
  $handler->display->display_options['fields']['name']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['name']['element_default_classes'] = FALSE;
  /* Veld: Inhoud: Titel */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['exclude'] = TRUE;
  $handler->display->display_options['fields']['title']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['title']['link_to_node'] = FALSE;
  /* Veld: Inhoud: Nid */
  $handler->display->display_options['fields']['nid']['id'] = 'nid';
  $handler->display->display_options['fields']['nid']['table'] = 'node';
  $handler->display->display_options['fields']['nid']['field'] = 'nid';
  $handler->display->display_options['fields']['nid']['label'] = '';
  $handler->display->display_options['fields']['nid']['exclude'] = TRUE;
  $handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
  /* Veld: Inhoud: Pad */
  $handler->display->display_options['fields']['path']['id'] = 'path';
  $handler->display->display_options['fields']['path']['table'] = 'node';
  $handler->display->display_options['fields']['path']['field'] = 'path';
  $handler->display->display_options['fields']['path']['label'] = '';
  $handler->display->display_options['fields']['path']['exclude'] = TRUE;
  $handler->display->display_options['fields']['path']['element_label_colon'] = FALSE;
  /* Veld: Global: Custom text */
  $handler->display->display_options['fields']['nothing']['id'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['table'] = 'views';
  $handler->display->display_options['fields']['nothing']['field'] = 'nothing';
  $handler->display->display_options['fields']['nothing']['label'] = '';
  $handler->display->display_options['fields']['nothing']['alter']['text'] = '<a class="hpbx-help-category-item" href="[path]">[title]</a>';
  $handler->display->display_options['fields']['nothing']['element_label_colon'] = FALSE;
  /* Veld: Inhoud: Reseller */
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['id'] = 'field_hpbx_reseller_id';
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['table'] = 'field_data_field_hpbx_reseller_id';
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['field'] = 'field_hpbx_reseller_id';
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['label'] = '';
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['field_hpbx_reseller_id']['element_label_colon'] = FALSE;
  /* Sort criterion: Inhoud: Titel */
  $handler->display->display_options['sorts']['title']['id'] = 'title';
  $handler->display->display_options['sorts']['title']['table'] = 'node';
  $handler->display->display_options['sorts']['title']['field'] = 'title';
  /* Filter criterion: Inhoud: Gepubliceerd */
  $handler->display->display_options['filters']['status']['id'] = 'status';
  $handler->display->display_options['filters']['status']['table'] = 'node';
  $handler->display->display_options['filters']['status']['field'] = 'status';
  $handler->display->display_options['filters']['status']['value'] = 1;
  $handler->display->display_options['filters']['status']['group'] = 1;
  $handler->display->display_options['filters']['status']['expose']['operator'] = FALSE;
  /* Filter criterion: Inhoud: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'hpbx_help_article' => 'hpbx_help_article',
    'hpbx_help_article_steps' => 'hpbx_help_article_steps',
  );
  /* Filter criterion: Inhoud: Aangeraden op de voorpagina */
  $handler->display->display_options['filters']['promote']['id'] = 'promote';
  $handler->display->display_options['filters']['promote']['table'] = 'node';
  $handler->display->display_options['filters']['promote']['field'] = 'promote';
  $handler->display->display_options['filters']['promote']['value'] = '1';
  /* Filter criterion: Inhoud: Taal */
  $handler->display->display_options['filters']['language']['id'] = 'language';
  $handler->display->display_options['filters']['language']['table'] = 'node';
  $handler->display->display_options['filters']['language']['field'] = 'language';
  $handler->display->display_options['filters']['language']['value'] = array(
    '***CURRENT_LANGUAGE***' => '***CURRENT_LANGUAGE***',
  );
  /* Filter criterion: Global: PHP */
  $handler->display->display_options['filters']['php']['id'] = 'php';
  $handler->display->display_options['filters']['php']['table'] = 'views';
  $handler->display->display_options['filters']['php']['field'] = 'php';
  $handler->display->display_options['filters']['php']['use_php_setup'] = 0;
  $handler->display->display_options['filters']['php']['php_filter'] = 'global $user;

return isset($user->data[\'sipwise\'][\'reseller_id\']) && $user->data[\'sipwise\'][\'reseller_id\']!=$data->_field_data[\'nid\'][\'entity\']->field_hpbx_reseller_id[\'und\'][0][\'value\'];';

  /* Display: HPBX Help frontpage */
  $handler = $view->new_display('page', 'HPBX Help frontpage', 'page');
  $handler->display->display_options['path'] = 'hpbx/help/home';
  $translatables['hpbx_help'] = array(
    t('Master'),
    t('Help'),
    t('more'),
    t('Apply'),
    t('Opnieuw instellen'),
    t('Sort by'),
    t('Asc'),
    t('Desc'),
    t('Inhoud toevoegen'),
    t('<?php
global $user;
$account = $user;
if (!empty($_SESSION[\'ccs\'][\'hpbx\'][\'masquerading_from\'])) {
    $account = user_load($_SESSION[\'ccs\'][\'hpbx\'][\'masquerading_from\']);
}

if (user_access(\'edit any hpbx_help_article content\', $account)) {

  echo \'<div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>

<div class="col-xs-5 col-xxs-12 col-sm-4 col-md-3">\' . l(t(\'Add help article\'),\'node/add/hpbx-help-article\', array(\'attributes\' => array(\'class\' => array(\'sg-btn sg-btn-wizard\')))) . \'</div></br>
<div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>

\';

}

?>'),
    t('term'),
    t('<a class="hpbx-help-category-item" href="[path]">[title]</a>'),
    t('HPBX Help frontpage'),
  );
  $export['hpbx_help'] = $view;

  return $export;
}
