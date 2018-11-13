<?php
/**
 * @file
 * hpbx_permissions.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function hpbx_permissions_user_default_permissions() {
  $permissions = array();

  // Exported permission: hpbx add customer.
  $permissions['hpbx add customer'] = array(
    'name' => 'hpbx add customer',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx add reseller.
  $permissions['hpbx add reseller'] = array(
    'name' => 'hpbx add reseller',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx add reseller customer.
  $permissions['hpbx add reseller customer'] = array(
    'name' => 'hpbx add reseller customer',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx administer reseller.
  $permissions['hpbx administer reseller'] = array(
    'name' => 'hpbx administer reseller',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx basic.
  $permissions['hpbx basic'] = array(
    'name' => 'hpbx basic',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'HostedPBX Customer Administrator',
      2 => 'HostedPBX LGI Administrator',
      3 => 'HostedPBX Subscriber',
      4 => 'HostedPBX Support',
      5 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete auto attendant.
  $permissions['hpbx delete auto attendant'] = array(
    'name' => 'hpbx delete auto attendant',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete conference.
  $permissions['hpbx delete conference'] = array(
    'name' => 'hpbx delete conference',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete customer.
  $permissions['hpbx delete customer'] = array(
    'name' => 'hpbx delete customer',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete huntgroup.
  $permissions['hpbx delete huntgroup'] = array(
    'name' => 'hpbx delete huntgroup',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete pbxdevice.
  $permissions['hpbx delete pbxdevice'] = array(
    'name' => 'hpbx delete pbxdevice',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete reseller.
  $permissions['hpbx delete reseller'] = array(
    'name' => 'hpbx delete reseller',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete reseller customer.
  $permissions['hpbx delete reseller customer'] = array(
    'name' => 'hpbx delete reseller customer',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx delete subscriber.
  $permissions['hpbx delete subscriber'] = array(
    'name' => 'hpbx delete subscriber',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit administrator user.
  $permissions['hpbx edit administrator user'] = array(
    'name' => 'hpbx edit administrator user',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit audio.
  $permissions['hpbx edit audio'] = array(
    'name' => 'hpbx edit audio',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit auto attendant.
  $permissions['hpbx edit auto attendant'] = array(
    'name' => 'hpbx edit auto attendant',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit conference.
  $permissions['hpbx edit conference'] = array(
    'name' => 'hpbx edit conference',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit customer.
  $permissions['hpbx edit customer'] = array(
    'name' => 'hpbx edit customer',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'HostedPBX LGI Administrator',
      2 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit huntgroup.
  $permissions['hpbx edit huntgroup'] = array(
    'name' => 'hpbx edit huntgroup',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit own customer settings.
  $permissions['hpbx edit own customer settings'] = array(
    'name' => 'hpbx edit own customer settings',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit own subscriber settings.
  $permissions['hpbx edit own subscriber settings'] = array(
    'name' => 'hpbx edit own subscriber settings',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'HostedPBX Subscriber',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit pbxdevice.
  $permissions['hpbx edit pbxdevice'] = array(
    'name' => 'hpbx edit pbxdevice',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit reseller.
  $permissions['hpbx edit reseller'] = array(
    'name' => 'hpbx edit reseller',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit reseller customer.
  $permissions['hpbx edit reseller customer'] = array(
    'name' => 'hpbx edit reseller customer',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx edit subscriber.
  $permissions['hpbx edit subscriber'] = array(
    'name' => 'hpbx edit subscriber',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx masquerade customer.
  $permissions['hpbx masquerade customer'] = array(
    'name' => 'hpbx masquerade customer',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'HostedPBX LGI Administrator',
      2 => 'HostedPBX Support',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx sync pbxdevice.
  $permissions['hpbx sync pbxdevice'] = array(
    'name' => 'hpbx sync pbxdevice',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view administrator user.
  $permissions['hpbx view administrator user'] = array(
    'name' => 'hpbx view administrator user',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view auto attendant.
  $permissions['hpbx view auto attendant'] = array(
    'name' => 'hpbx view auto attendant',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view call records.
  $permissions['hpbx view call records'] = array(
    'name' => 'hpbx view call records',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'HostedPBX Subscriber',
      2 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view conference.
  $permissions['hpbx view conference'] = array(
    'name' => 'hpbx view conference',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view customer.
  $permissions['hpbx view customer'] = array(
    'name' => 'hpbx view customer',
    'roles' => array(
      0 => 'HostedPBX Administrator',
      1 => 'HostedPBX Support',
      2 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view huntgroup.
  $permissions['hpbx view huntgroup'] = array(
    'name' => 'hpbx view huntgroup',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view numbers.
  $permissions['hpbx view numbers'] = array(
    'name' => 'hpbx view numbers',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view pbxdevice.
  $permissions['hpbx view pbxdevice'] = array(
    'name' => 'hpbx view pbxdevice',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view reseller.
  $permissions['hpbx view reseller'] = array(
    'name' => 'hpbx view reseller',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view reseller customer.
  $permissions['hpbx view reseller customer'] = array(
    'name' => 'hpbx view reseller customer',
    'roles' => array(
      0 => 'HostedPBX LGI Administrator',
    ),
    'module' => 'hpbx',
  );

  // Exported permission: hpbx view subscriber.
  $permissions['hpbx view subscriber'] = array(
    'name' => 'hpbx view subscriber',
    'roles' => array(
      0 => 'HostedPBX Customer Administrator',
      1 => 'administrator',
    ),
    'module' => 'hpbx',
  );

  return $permissions;
}
