<?php
/**
 * @file
 * hpbx_permissions.features.menu_links.inc
 */

/**
 * Implements hook_menu_default_menu_links().
 */
function hpbx_permissions_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: menu-hostedpbx-management:hpbx/auto-attendants
  $menu_links['menu-hostedpbx-management:hpbx/auto-attendants'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/auto-attendants',
    'router_path' => 'hpbx/auto-attendants',
    'link_title' => 'Auto Attendants',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-47',
  );
  // Exported menu link: menu-hostedpbx-management:hpbx/conferences
  $menu_links['menu-hostedpbx-management:hpbx/conferences'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/conferences',
    'router_path' => 'hpbx/conferences',
    'link_title' => 'Conference Rooms',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-45',
  );
  // Exported menu link: menu-hostedpbx-management:hpbx/customer
  $menu_links['menu-hostedpbx-management:hpbx/customer'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/customer',
    'router_path' => 'hpbx/customer',
    'link_title' => 'PBX Settings',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-50',
  );
  // Exported menu link: menu-hostedpbx-management:hpbx/huntgroups
  $menu_links['menu-hostedpbx-management:hpbx/huntgroups'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/huntgroups',
    'router_path' => 'hpbx/huntgroups',
    'link_title' => 'Hunting Groups',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-44',
  );
  // Exported menu link: menu-hostedpbx-management:hpbx/pbxdevices
  $menu_links['menu-hostedpbx-management:hpbx/pbxdevices'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/pbxdevices',
    'router_path' => 'hpbx/pbxdevices',
    'link_title' => 'PBX Devices',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-48',
  );
  // Exported menu link: menu-hostedpbx-management:hpbx/subscribers
  $menu_links['menu-hostedpbx-management:hpbx/subscribers'] = array(
    'menu_name' => 'menu-hostedpbx-management',
    'link_path' => 'hpbx/subscribers',
    'router_path' => 'hpbx/subscribers',
    'link_title' => 'Subscribers',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-49',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx
  $menu_links['menu-main-menu-hosted-pbx:hpbx'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx',
    'router_path' => 'hpbx',
    'link_title' => 'Home',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '1',
    'expanded' => '0',
    'weight' => '-50',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/affiliates
  $menu_links['menu-main-menu-hosted-pbx:hpbx/affiliates'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/affiliates',
    'router_path' => 'hpbx/affiliates',
    'link_title' => 'Affiliates',
    'options' => array(),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-49',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/callrecords
  $menu_links['menu-main-menu-hosted-pbx:hpbx/callrecords'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/callrecords',
    'router_path' => 'hpbx/callrecords',
    'link_title' => 'Call Records',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-44',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/customers
  $menu_links['menu-main-menu-hosted-pbx:hpbx/customers'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/customers',
    'router_path' => 'hpbx/customers',
    'link_title' => 'Customers',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-47',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/resellers
  $menu_links['menu-main-menu-hosted-pbx:hpbx/resellers'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/resellers',
    'router_path' => 'hpbx',
    'link_title' => 'Resellers',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '1',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-46',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/settings
  $menu_links['menu-main-menu-hosted-pbx:hpbx/settings'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/settings',
    'router_path' => 'hpbx/settings',
    'link_title' => 'Settings',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-45',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/users
  $menu_links['menu-main-menu-hosted-pbx:hpbx/users'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/users',
    'router_path' => 'hpbx',
    'link_title' => 'Users',
    'options' => array(),
    'module' => 'system',
    'hidden' => '1',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-48',
  );
  // Exported menu link: menu-main-menu-hosted-pbx:hpbx/voicemails
  $menu_links['menu-main-menu-hosted-pbx:hpbx/voicemails'] = array(
    'menu_name' => 'menu-main-menu-hosted-pbx',
    'link_path' => 'hpbx/voicemails',
    'router_path' => 'hpbx/voicemails',
    'link_title' => 'Voicemails',
    'options' => array(
      'attributes' => array(
        'title' => '',
      ),
    ),
    'module' => 'system',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-43',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Affiliates');
  t('Auto Attendants');
  t('Call Records');
  t('Conference Rooms');
  t('Customers');
  t('Home');
  t('Hunting Groups');
  t('PBX Devices');
  t('PBX Settings');
  t('Resellers');
  t('Settings');
  t('Subscribers');
  t('Users');
  t('Voicemails');


  return $menu_links;
}
