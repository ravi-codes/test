<?php

require_once(drupal_get_path('module', 'ldap_servers') . '/LdapServer.class.php');

class LdapCCS extends LdapServer {
	function __get($name) { 
		if ($name == 'connection') { 
			return $this->connection;
		}
	}
}