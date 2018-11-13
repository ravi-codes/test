<?php
/**
 * @file
 * hpbx_permissions.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function hpbx_permissions_user_default_roles() {
  $roles = array();

  // Exported role: HostedPBX Administrator.
  $roles['HostedPBX Administrator'] = array(
    'name' => 'HostedPBX Administrator',
    'weight' => '41',
  );

  // Exported role: HostedPBX Customer Administrator.
  $roles['HostedPBX Customer Administrator'] = array(
    'name' => 'HostedPBX Customer Administrator',
    'weight' => '42',
  );

  // Exported role: HostedPBX LGI Administrator.
  $roles['HostedPBX LGI Administrator'] = array(
    'name' => 'HostedPBX LGI Administrator',
    'weight' => '44',
  );

  // Exported role: HostedPBX Subscriber.
  $roles['HostedPBX Subscriber'] = array(
    'name' => 'HostedPBX Subscriber',
    'weight' => '43',
  );

  // Exported role: HostedPBX Support.
  $roles['HostedPBX Support'] = array(
    'name' => 'HostedPBX Support',
    'weight' => '45',
  );

  return $roles;
}
