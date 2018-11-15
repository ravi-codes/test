<?php

define('DRUPAL_ROOT', getcwd());
require_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);


$resellers = sipwise_api_get_all('resellers', array());

if(count($resellers) < 1){
  header('HTTP/1.1 503 Service Unavailable');
  echo 'ERR09';
  exit;
}


$servers = ldap_servers_get_servers(NULL, 'enabled');
$counter = 0;
foreach ($servers as $server) {
        
        try {
          $connect_result = $server->connect();
          if ($connect_result == LDAP_SUCCESS) {
            $bind_result = $server->bind();
            if ($bind_result == LDAP_SUCCESS) {
             $counter++;
            }
          }
          $server->disconnect();
        }
        catch (Exception $e) {	       
        }
      
}
if($counter == 0){
  header('HTTP/1.1 503 Service Unavailable');
  echo 'ERR05';
  exit;
}


echo 'OK';
