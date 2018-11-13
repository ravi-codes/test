<?php
/**
 * @file
 * hpbx_help_content_types.features.inc
 */

/**
 * Implements hook_views_api().
 */
function hpbx_help_content_types_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function hpbx_help_content_types_node_info() {
  $items = array(
    'hpbx_help_article' => array(
      'name' => t('HPBX Help article'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'hpbx_help_article_steps' => array(
      'name' => t('HPBX Help article steps'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
