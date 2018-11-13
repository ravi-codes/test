<?php
/**
 * @file
 * hpbx_permissions.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function hpbx_permissions_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-hostedpbx-management
  $menus['menu-hostedpbx-management'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'title' => 'HostedPBX Management',
    'description' => '',
    'language' => 'und',
    'i18n_mode' => '0',
  );
  // Exported menu: menu-main-menu-hosted-pbx
  $menus['menu-main-menu-hosted-pbx'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'title' => 'Main menu Hosted PBX',
    'description' => '',
    'language' => 'und',
    'i18n_mode' => '0',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('HostedPBX Management');
  t('Main menu Hosted PBX');


  return $menus;
}
