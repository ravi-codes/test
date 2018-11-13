<?php

$_SERVER['REMOTE_ADDR'] = null;
define('DRUPAL_ROOT', __DIR__);
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

$health_config = json_decode(
    file_get_contents(__DIR__ . '/../health-config.json'),
	true
);

/* health-config.json sample:
{
	"ldap": {
		"servers": ["ldap1", "ldap2"]
	},
	"sipwise": {
		"reseller_id": 2,
		"customer_id": 1063,
		"customer_company": "Prodapt Consulting B.V."
	}
}
*/

foreach ($health_config['ldap']['servers'] as $ldap_server_name) {
	$server = ldap_servers_get_servers($ldap_server_name, null, true);

	if ($server->connect() !== LdapServer::LDAP_SUCCESS) {
		header('HTTP/1.1 503 Service Unavailable');
		echo 'ERR05';
		exit;
	}
	
}

$sipwise_api_reseller_id = $health_config['sipwise']['reseller_id'];
$ActiveReseller = SipwiseEntity::load('resellers', $sipwise_api_reseller_id);
$customer = SipwiseEntity::load('customers', 1063);
$contact = SipwiseEntity::load('customercontacts', $customer->contact_id);

if ($contact->company != $health_config['sipwise']['customer_company']) {
	header('HTTP/1.1 503 Service Unavailable');
	echo 'ERR09';
	exit;
}

echo 'OK';
