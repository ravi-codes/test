<?php

$databases = array(
    'default' => 
    array(
        'default' => 
        array(
            'database' => 'voice_portal',
            'username' => 'voice_portal_username',
            'password' => 'voice_portal_password',
            'host' => 'voice_portal_hostname',
            'port' => '3306',
            'driver' => 'mysql',
            'prefix' => 'vp_',
            'pdo' => array(
                PDO::MYSQL_ATTR_SSL_CA => 'PATH_TO_MYSQL_SERVER_CA_CERTIFICATE',
            ),
        ),
    ),
);

$drupal_hash_salt = null;

# Reverse proxy settings - necessary for correct use of IP address
$conf['reverse_proxy'] = TRUE;
$conf['reverse_proxy_header'] = 'HTTP_X_FORWARDED_FOR';
$conf['reverse_proxy_addresses'] = array('127.0.0.1');

# If role restrictions based on HTTP headers is required, here is how to do it
$conf['role_restrictions'] = array(
    'public_roles' => array( 
        'authenticated user',
        'HostedPBX Subscriber',
        'HostedPBX Customer Administrator',
    ),    
    'private_header' => 'X-Admin-Header',
    'private_header_value' => '__value__',
);

$conf['sipwise']['default'] = array(
    'sipwise_api_endpoint' => 'https://127.0.0.1:1443',
    'sipwise_api_cert' => '/path/to/sipwise/client/cert.pem',
    'sipwise_prov_endpoint' => 'https://public-url'
);

$conf['sipwise'][3] = $conf['sipwise']['default'];

$conf['stretto']['api_endpoint'] = 'http://127.0.0.1:9202';
$conf['stretto']['username'] = 'stretto_username';
$conf['stretto']['password'] = 'stretto_password';

$conf['hpbx_rps'] = array(
  'yealink' => array(
    'auth'=> array(
      'username' => 'yealink_username',
      'password' => 'yealink_password'
    ),
    'servername' => 'lg-sme-prod-prov-y',
    'endpoint' => 'https://rps.yealink.com/xmlrpc',
  ),
  'panasonic' => array(
    'auth' => array(
      'username' => 'panasonic_username',
      'password' => 'panasonic_password'
    ),
    'servername' => 'https://prov-p.h-pbx.eu:443/sync/abcd12345/{mac}',
    'endpoint' => 'https://provisioning.e-connecting.net/redirect/xmlrpc',
  ),
);
