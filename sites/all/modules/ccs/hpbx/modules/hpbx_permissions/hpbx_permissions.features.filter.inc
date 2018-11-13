<?php
/**
 * @file
 * hpbx_permissions.features.filter.inc
 */

/**
 * Implements hook_filter_default_formats().
 */
function hpbx_permissions_filter_default_formats() {
  $formats = array();

  // Exported format: html mail
  $formats['html_mail'] = array(
    'format' => 'html_mail',
    'name' => 'html mail',
    'cache' => '1',
    'status' => '1',
    'weight' => '0',
    'filters' => array(
      'filter_autop' => array(
        'weight' => '0',
        'status' => '1',
        'settings' => array(),
      ),
      'filter_url' => array(
        'weight' => '0',
        'status' => '1',
        'settings' => array(
          'filter_url_length' => '300',
        ),
      ),
    ),
  );

  return $formats;
}
