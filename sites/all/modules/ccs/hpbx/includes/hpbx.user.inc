<?php

/**
 * @param stdClass $user
 * @return boolean
 */
function hpbx_users_is_hpbx_user($user = NULL) {
  return hpbx_users_preg_match_role('HostedPBX', $user);
}

/**
 * @param string $username
 * @return bool
 */
function hpbx_users_get_user($username) {
  $ldap_server = hpbx_users_get_ldap_server();
  $entry = ccs_ldap_server_search_wrapper($ldap_server, $ldap_server->user_attr. '='. ldap_escape($username, NULL, LDAP_ESCAPE_FILTER), 'DC=a,DC=local');
  return (isset($entry[0]) ? $entry[0] : FALSE);
}

/**
 * @param SipwiseSubscriber $Subscriber
 */
function hpbx_users_remove(SipwiseSubscriber $Subscriber) {
  $ldap_server = hpbx_users_get_ldap_server();

  if (!empty($Subscriber->webusername)) {

    // Search for user entry.
    $entries = ccs_ldap_server_search_wrapper($ldap_server, $ldap_server->user_attr. '='. ldap_escape($Subscriber->webusername,  NULL, LDAP_ESCAPE_FILTER), 'OU=HostedPBX,DC=a,DC=local');

    // Verify if only one item is found.
    if (isset($entries['count']) && $entries['count'] === 1) {

      // Try to delete entry.
      if (ldap_delete($ldap_server->connection, $entries[0]['dn']) === FALSE) {
        watchdog('hpbx_users', 'Failed to remove user with username from AD: @username', array('@username' => $Subscriber->webusername));
        watchdog('hpbx_users', ldap_error($ldap_server->connection));
        return;
      }

      // Remove drupal user
      if ($account = user_load_by_name($Subscriber->webusername)) {

        try {
          //dpm('Delete drupal user: '. $account->uid);

          user_delete($account->uid);
        }
        catch (Exception $e) {
          watchdog('hpbx_users', 'Failed to remove user with username from drupal: @username @exception_msg', array('@username' => $Subscriber->webusername, '@exception_message' => $e->getMessage()));
          return;
        }

        // Set message.
        drupal_set_message(t('User succesfully removed'));
      }
    }
    else {
      watchdog('hpbx_users', 'Failed to remove user with username: @username (not found in AD)', array('@username' => $Subscriber->webusername));
    }
  }
  else {
    watchdog('hpbx_users', 'Failed to remove user with empty web username');
  }
}

function hpbx_user_pass_reset_form($form, &$form_state) {
  global $user;

  $form =array();

  drupal_set_title('');

  // To skip the current password field, the user must have logged in via a
  // one-time link and have the token in the URL.
  $pass_reset = isset($_SESSION['pass_reset_' . $user->uid]) && isset($_GET['pass-reset-token']) && ($_GET['pass-reset-token'] == $_SESSION['pass_reset_' . $user->uid]);

  $form['password_current'] = array(
    '#type' => 'password',
    '#access' => !$pass_reset, // Disable this field in case the user used a password reset link.
  );

  $form['password_new1'] = array(
    '#type' => 'password',
  );
  $form['password_new2'] = array(
    '#type' => 'password',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function hpbx_user_pass_reset_form_validate($form, &$form_state) {

  global $user;
  if (!empty($form_state['values']['password_new1'])) {
  	if(variable_get('hpbx_password_policy')){
	  $requirements = array();

      //uppercase
      $requirements['uppercase']['pattern'] = '/[A-Z]/';
      $requirements['uppercase']['error'] = t('Your password must contain at least one uppercase letter.');

      //lowercase
      $requirements['lowercase']['pattern'] = '/[a-z]/';
      $requirements['lowercase']['error'] = t('Your password must contain at least one lowercase letter.');

      //requires a number
      $requirements['number']['pattern'] = '/[0-9]/';
      $requirements['number']['error'] = t('Your password must contain at least one number.');

      //special characters
      $requirements['special_character']['pattern'] = '/[!@#$%^&*()\\-_=+{};\:,<\.>]/';
      $requirements['special_character']['error'] = t('Your password must contain at least one special character.');

      //length
      $requirements['length']['pattern'] = '/^.{8,}/';
      $requirements['length']['error'] = t('Your password must be at least 8 characters in length total.');
    
      foreach($requirements as $idx => $req){
  	    if(!preg_match($req['pattern'], $form_state['values']['password_new1'], $matches)){
	      form_set_error('password_new1', $req['error']);
	    }
      }	
	}
  	
    if ($form_state['values']['password_new1']!=$form_state['values']['password_new2']) {
      form_set_error('password_new1', t('The passwords do not match'));
      form_set_error('password_new2', '');
    }
    elseif ($form['password_current']['#access']!=FALSE &&
        empty($form_state['values']['password_current'])) {

      form_set_error('password_current', t('Your current password is missing or incorrect'));
    }

    // Disable this field in case the user used a password reset link.
    elseif ($form['password_current']['#access']!=FALSE) {

      $correct_password = FALSE;

      $ldap_server = ccs_ldap_get_valid_server();

      foreach ($ldap_server->basedn as $basedn) {

        if ($ldap_user = $ldap_server->user_lookup($user->name)) {
          $result = $ldap_server->bind($ldap_user['dn'], $form_state['values']['password_current']);
          if ($result == LDAP_SUCCESS) {
            $correct_password = TRUE;
            break;
          }
        }
      }

      if (!$correct_password) {
        form_set_error('password_current', t('Your current password is missing or incorrect'));
      }
    }
  }
}

function hpbx_user_pass_reset_form_submit($form, &$form_state) {

  global $user;

  // Password reset.
  if (!empty($form_state['values']['password_new1'])) {
    watchdog('ccs', t('Will enable user %user', array('%user' => $user->name)));
    ccs_ldap_enable_account($user->name);
    sleep(4);

    // Set the new password on the LDAP server.
    if (!ccs_ldap_set_password($user->name, $form_state['values']['password_new1'])) {
      //drupal_set_message(t('Problem while trying to change the password, please try again or contact the support desk.'), 'error');
      drupal_goto('hpbx/password');
    }
    else {
      ccs_ldap_unlock_account($user->name);
      drupal_set_message(t('Password successfully changed'));
    }
  }
  drupal_goto('');
}

/**
 *
 * @param SipwiseEntity $Customer
 */
function hpbx_users_remove_ou(SipwiseEntity $Customer) {
  $ldap_server = hpbx_users_get_ldap_server();

  $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);

  // Search for user entry.
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($CustomerContact->company), NULL, LDAP_ESCAPE_FILTER), hpbx_users_get_reseller_basedn_on_customer($Customer));

  // Verify if only one item is found.
  if (isset($entries['count']) && $entries['count'] === 1) {

    // Try to delete entry.
    if (ldap_delete($ldap_server->connection, $entries[0]['dn']) === FALSE) {
      watchdog('hpbx_users', 'Failed to remove organisation unit: @company', array('@company' => $CustomerContact->company));
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      return;
    }
  }
  else {
    watchdog('hpbx_users', 'Failed to remove organisation unit: @company', array('@company' => $CustomerContact->company));
  }
}

/**
 * Helper function to return a working ldap_server.
 *
 * @return bool|LdapCCS
 */
function hpbx_users_get_ldap_server() {

  require_once(drupal_get_path('module', 'ccs') . '/LdapCCS.class.php');
  $ldap_server = ccs_ldap_get_valid_server();
  $ldap_result = $ldap_server->connect();
  $bind_result = $ldap_server->bind();
  ldap_set_option($ldap_server->connection, LDAP_OPT_PROTOCOL_VERSION, 3);

  return $ldap_server;
}

/**
 *
 * @param string $username
 * @param string $mail
 * @param string $firstname
 * @param string $lastname
 * @param string $role
 * @return void|boolean
 */
function hpbx_users_update_user($username, $mail, $firstname, $lastname, $role) {

  $ldap_server = hpbx_users_get_ldap_server();

  // Lookup user within AD for current CN.
  if ($ad_user = hpbx_users_get_user($username)) {

    // User is found.
    $current_entry = $ad_user;
    $current_entry['memberof'] = ccs_ldap_server_search_wrapper($ldap_server, $ldap_server->user_attr. '='. $username, 'DC=A, DC=local', array('memberOf'));
    
   // // Remove all existing memberships, before adding the new memberships.
    if (isset($current_entry['memberof'])) {
        unset($current_entry['memberof']['count']);
        foreach ($current_entry['memberof'] as $memberof) {			
          if (preg_match('/HostedPBX/', $memberof['memberof'][0]) && ldap_mod_del($ldap_server->connection, $memberof['memberof'][0], array('member' => $memberof['dn']))===FALSE) {
            watchdog('hpbx_users', t('Failed to remove group assignment.'));
            watchdog('hpbx_users', ldap_error($ldap_server->connection));            
            return;
          }
      }
    }
    // Set attributes needed to modify.
    $entry = array();
    $entry['givenName'] = trim($firstname);
    $entry['sn'] =  trim($lastname);
    $entry['mail'] = trim($mail);

    watchdog('roeland_user_update', var_export($entry, TRUE));

    // Modify AD through LDAP.
    if (ldap_modify($ldap_server->connection, $current_entry['dn'], $entry)===FALSE) {
      watchdog('hpbx_users', t('Failed to modify user @username', array('@username' => $username)));
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      return;
    }

    // Set the correct name.
    $newRdn = ldap_escape(trim($firstname) . ' '. trim($lastname), NULL, LDAP_ESCAPE_DN);

    if (ldap_rename($ldap_server->connection, $current_entry['dn'], 'cn='. $newRdn, NULL, TRUE) === FALSE) {
      watchdog('hpbx_users', t('Failed to rename user @username', array('@username' => $username)));
      watchdog('hpbx_users', ldap_error($ldap_server->connection));

      echo "<pre>";
      echo  'cn='. $newRdn. PHP_EOL;

      die();
      return;
    }      
      
      // Refresh.
      if ($current_entry = hpbx_users_get_user($username)) {
      
      $role_entries = ccs_ldap_server_search_wrapper($ldap_server, "CN=". ldap_escape($role, NULL, LDAP_ESCAPE_FILTER), "OU=Groups,DC=a, DC=local");
      // Add the role.
      if ((!count($role_entries)) || (ldap_mod_add($ldap_server->connection, $role_entries[0]['dn'], array('member' => $current_entry['dn']))===FALSE)) {
         watchdog('hpbx_users', t('Failed to add role.'));
         watchdog('hpbx_users', ldap_error($ldap_server->connection));
            
         return FALSE;
      }

      // Change the Drupal user account email address.
      if ($accounts = user_load_multiple(NULL, array('name' => $current_entry[$ldap_server->user_attr]))) {
        $account = reset($accounts);

        if ($d_role = user_role_load_by_name($role)) {
          $account->roles = array(
              DRUPAL_AUTHENTICATED_RID => TRUE,
              $d_role->rid => TRUE,
          );
        }

        user_save($account, array('mail' => $mail));
        return TRUE;
      }
    }
  }
  else {
    watchdog('hpbx_users', t('Failed to modify user @username', array('@username' => $username)));
    watchdog('hpbx_users', ldap_error($ldap_server->connection));
    return FALSE;
  }
}

/**
* Escapes any special chars (RFC 4515) from a string representing a
* a search filter assertion value.|
*
* @param string $query
* @return string
*/
function hpbx_users_escape_ldap_query($query) {
  return ldap_escape($query, NULL, LDAP_ESCAPE_DN);
}

/**
 *
 * @param SipwiseCustomer $Customer
 * @param string $company_from
 * @param string $company_to
 *
 * @return boolean
 */
function hpbx_user_rename_customer_organizational_unit($Customer, $company_from, $company_to) {

  $ldap_server = hpbx_users_get_ldap_server();

  $reseller_basedn = hpbx_users_get_reseller_basedn_on_customer($Customer);

  // Now verify if the customer already has it's own Organization Unit (OU).
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($company_from), NULL, LDAP_ESCAPE_FILTER), $reseller_basedn);

  // Verify if  item is found.
  if (isset($entries['count']) && $entries['count'] === 1) {

    $old_dn = $entries[0]['dn'];
    $newrdn = 'OU='. hpbx_users_escape_ldap_query(trim($company_to));

    if (ldap_rename($ldap_server->connection, $old_dn, $newrdn, $reseller_basedn, TRUE)===FALSE) {
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

function hpbx_user_rename_bp_organizational_unit($reseller_id, $bp_name_old, $bp_name_new){
  
  global $ActiveReseller;
  
  if(!empty($reseller_id)){
  	sipwise_api_activate_reseller($reseller_id);
  } 
 
  $ldap_server = hpbx_users_get_ldap_server();

  $reseller_basedn = hpbx_users_get_reseller_basedn($ActiveReseller);
  $baseDn = 'OU=BusinessPartners,'. $reseller_basedn;
  
  // Now verify if the customer already has it's own Organization Unit (OU).
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($bp_name_old), NULL, LDAP_ESCAPE_FILTER), $reseller_basedn);
  
  // Verify if  item is found.
  if (isset($entries['count']) && $entries['count'] === 1) {

    $old_dn = $entries[0]['dn'];
    $newrdn = 'OU='. hpbx_users_escape_ldap_query(trim($bp_name_new));

    if (ldap_rename($ldap_server->connection, $old_dn, $newrdn, $baseDn, TRUE)===FALSE) {
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

function hpbx_user_create_reseller_organizational_unit($Reseller) {

  $ldap_server = hpbx_users_get_ldap_server();
  if (empty($Reseller->name)) {
    watchdog('hpbx_users', t('No Reseller name set, so can\'t create OU'));
    return FALSE;
  }

  // Now verify if the customer already has it's own Organization Unit (OU).
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($Reseller->name),  NULL, LDAP_ESCAPE_FILTER), 'OU=HostedPBX,DC=a,DC=local');

  // Verify if no item is found.
  if (!isset($entries['count']) || $entries['count'] !== 1) {

    // Create the OU.
    $entry = array();
    $entry['objectclass'][0] = 'top';
    $entry['objectclass'][1] = 'organizationalUnit';
    $entry['ou'] = hpbx_users_escape_ldap_query(trim($Reseller->name));

    $dn = hpbx_users_get_reseller_basedn($Reseller);
    if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      watchdog('hpbx_users', 'dn='. $dn);
      return FALSE;
    }
    watchdog('hpbx_users', 'Reseller OU for '. $Reseller->name .'successfully created');
  }
  else {
    watchdog('hpbx_users', 'Reseller OU for '. $Reseller->name .'already exists');
  }

  return TRUE;
}

function hpbx_user_create_bp_organizational_unit($Reseller) {	
  
  $ldap_server = hpbx_users_get_ldap_server();
  
  //hpbx_user_create_reseller_organizational_unit($Reseller);
  
  // Now verify if the bp OU already has it's own reseller Organization Unit (OU).
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU=BusinessPartners','ou='.ldap_escape($Reseller->name, NULL, LDAP_ESCAPE_DN).',ou=HostedPBX,dc=a,dc=local');  
  
  // Verify if no item is found.
  if (!isset($entries['count']) || $entries['count'] !== 1) {

    // Create the OU.
    $entry = array();
    $entry['objectclass'][0] = 'top';
    $entry['objectclass'][1] = 'organizationalUnit';
    $entry['ou'] = 'BusinessPartners';    
    
    $dn = hpbx_users_get_bp_basedn($Reseller);
    if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
    	echo  ldap_error($ldap_server->connection);  
    	die;
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      watchdog('hpbx_users', 'dn='. $dn);
      return FALSE;
    }
    watchdog('hpbx_users', 'Business Partner OU for '. $Reseller->name .'successfully created');
  }
  else {
    watchdog('hpbx_users', 'Business Partner OU for '. $Reseller->name .'already exists');
  }

  return TRUE;
}
function hpbx_user_create_bp_name_on_bp_ou($Reseller){
	
	$ldap_server = hpbx_users_get_ldap_server();
	$entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($Reseller->bp_name),  NULL, LDAP_ESCAPE_FILTER),hpbx_users_get_bp_basedn($Reseller));  
     
  // Verify if no item is found.
  if (!isset($entries['count']) || $entries['count'] !== 1) {

    // Create the OU.
    $entry = array();
    $entry['objectclass'][0] = 'top';
    $entry['objectclass'][1] = 'organizationalUnit';
    $entry['ou'] = trim($Reseller->bp_name); 
       
    
    $dn = hpbx_users_get_bp_name_basedn($Reseller);
    if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
    	echo  ldap_error($ldap_server->connection);  
    	die;
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      watchdog('hpbx_users', 'dn='. $dn);
      return FALSE;
    }
    watchdog('hpbx_users', 'Business Partner OU for '. $Reseller->name .'successfully created');
  }
  else {
    watchdog('hpbx_users', 'Business Partner OU for '. $Reseller->name .'already exists');
  }

  return TRUE;
}

function hpbx_user_create_customer_organizational_unit($Customer) {
  $ldap_server = hpbx_users_get_ldap_server();
  // Load the customer contact.
  $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);

  if (empty($CustomerContact->company)) {
    watchdog('hpbx_users', t('No company name set, so can\'t create OU'));
    return FALSE;
  }

  // Now verify if the customer already has it's own Organization Unit (OU).
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'OU='. ldap_escape(trim($CustomerContact->company),  NULL, LDAP_ESCAPE_FILTER), hpbx_users_get_reseller_basedn_on_customer($Customer));

  // Verify if no item is found.
  if (!isset($entries['count']) || $entries['count'] !== 1) {

    // Create the OU.
    $entry = array();
    $entry['objectclass'][0] = 'top';
    $entry['objectclass'][1] = 'organizationalUnit';
    $entry['ou'] = hpbx_users_escape_ldap_query(trim($CustomerContact->company));

    $dn = hpbx_users_get_customer_basedn($Customer);
    if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
      watchdog('hpbx_users', ldap_error($ldap_server->connection));
      watchdog('hpbx_users', 'dn='. $dn);
      return FALSE;
    }
  }
  return TRUE;
}

function hpbx_users_create_customer_user(SipwiseEntity $Customer, $subscriber_id, $mail, $firstname, $lastname, $role) {
  return hpbx_users_create_user(NULL, $Customer, $subscriber_id, $mail, $firstname, $lastname, $role);
}

function hpbx_users_create_reseller_user(SipwiseEntity $Reseller, $mail, $firstname, $lastname, $role) {
  return hpbx_users_create_user($Reseller, NULL, NULL, $mail, $firstname, $lastname, $role);
}

function hpbx_users_create_bp_user(SipwiseEntity $Reseller, $bp_name, $bp_id, $email, $firstname, $lastname, $role){
 	
  global $user;
  global $language;
  //global $ActiveReseller;
  
  $ldap_server = hpbx_users_get_ldap_server();

  // Create a Drupal user in order to retrieve the drupal uid, which will be used as active directory username.
  $account = new stdClass();
  $account->is_new = TRUE;
  $account->mail = $account->init = trim($email);
  $account->pass = uniqid();
  $account->status = 1;
  $account->language = $language->language;
  $account->name = uniqid();  
  if(!empty($bp_id)){
  	$account->data['sipwise']['bp_id'] = $bp_id;
  }
  if(!empty($bp_name)){
  	$account->data['sipwise']['bp_name'] = $bp_name;
  }
  if (!is_null($Reseller)) {
    $account->data['sipwise']['reseller_id'] = $Reseller->id;
  }
  elseif(!empty($user->data['sipwise']['reseller_id'])) {
    $account->data['sipwise']['reseller_id'] = $user->data['sipwise']['reseller_id'];
  }  
  
  if ($d_role = user_role_load_by_name($role)) {
    $account->roles = array(
      DRUPAL_AUTHENTICATED_RID => TRUE,
      $d_role->rid => TRUE,
    );
  }
  
  $Reseller->bp_name = $bp_name;
 
  if ($account = user_save($account)) {

    // Set the username to the uid and update the db entry for this account.
    if ($account = user_save($account, array('name' => $account->uid))) {

      if (!is_null($Reseller)) {

         if (!hpbx_user_create_bp_organizational_unit($Reseller)){
         	
		   // Remove the drupal account.
            if (!empty($account->uid)) {
               user_delete($account->uid);            
                 //to delete the bp_id record from local table
               db_delete('hpbx_business_partners')->condition('bp_id', $bp_id)->execute();
            }
            return;		
			          
         }
         if(!hpbx_user_create_bp_name_on_bp_ou($Reseller)){
         	if (!empty($account->uid)) {
               user_delete($account->uid);            
                 //to delete the bp_id record from local table
               db_delete('hpbx_business_partners')->condition('bp_id', $bp_id)->execute();
            }
            return;	
         }      
      }

      // Set the complete name for AD.
      $cn = trim($firstname) . ' '.  trim($lastname);

      if(!is_null($Reseller)) {
        // Add user to existing or created customer specific OU.
        $dn = 'CN='. ldap_escape($cn, NULL, LDAP_ESCAPE_DN) .',OU='.ldap_escape($bp_name, NULL, LDAP_ESCAPE_DN).', OU=BusinessPartners,'.hpbx_users_get_reseller_basedn($Reseller);        
      }
      else {
        // Remove the drupal account.
        if (!empty($account->uid)) {
          user_delete($account->uid);
          db_delete('hpbx_business_partners')->condition('bp_id', $bp_id)->execute();
        }
        return;
      }

      $entry = array();
      $entry['cn'] = $cn;
      $entry['givenName'] = trim($firstname);
      $entry['sn'] = trim($lastname);
      //$entry['name'] = $cn;
      $entry['objectclass'][0] = 'top';
      $entry['objectclass'][1] = 'person';
      $entry['objectclass'][2] = 'organizationalPerson';
      $entry['mail'] = trim($email);
      $entry[$ldap_server->user_attr] = $account->uid;

      if ($ldap_server->ldap_type!='openldap') {
        $entry['userprincipalname'] = $account->uid . '@a.local';
        $entry['useraccountcontrol'] = '66080';
        $entry['objectclass'][3] = 'user';
      }
      else {
        $entry['objectclass'][3] = 'inetOrgPerson';
      }
     
      if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
        drupal_set_message(t('Failed to add a user account for @firstname @lastname', array(
        '@firstname' => trim($firstname),
        '@lastname' => trim($lastname),
        )), 'error');

        watchdog('hpbx_users', ldap_error($ldap_server->connection) . 'dn: '. $dn. ',entry: '. var_export($entry, TRUE));

        // Remove the drupal account.
        if (!empty($account->uid)) {
          user_delete($account->uid);
        }

        return FALSE;
      }

      // Add the role.
      $role_entries = ccs_ldap_server_search_wrapper($ldap_server, "CN=". ldap_escape($role, NULL, LDAP_ESCAPE_FILTER), "OU=Groups,DC=a, DC=local");
 
      if ((!count($role_entries)) || (ldap_mod_add($ldap_server->connection, $role_entries[0]['dn'], array('member' => $dn))===FALSE)) {
        drupal_set_message(t('Failed to add role.'), 'error');
        watchdog('hpbx_users', ldap_error($ldap_server->connection));
        return FALSE;
      }    

      // Drupal user and AD user successfully created.
      return $account;
    }
  }
  return FALSE;
}

/**
 * @param SipwiseEntity|NULL $Reseller
 * @param SipwiseEntity|NULL $Customer
 * @param null $subscriber_id
 * @param $mail
 * @param $firstname
 * @param $lastname
 * @param $role
 * @return bool|stdClass
 */
function hpbx_users_create_user(SipwiseEntity $Reseller = NULL, SipwiseEntity $Customer = NULL, $subscriber_id = NULL, $mail, $firstname, $lastname, $role) {
  global $user;
  global $language;
  global $ActiveReseller;

  $ldap_server = hpbx_users_get_ldap_server();

  // Create a Drupal user in order to retrieve the drupal uid, which will be used as active directory username.
  $account = new stdClass();
  $account->is_new = TRUE;
  $account->mail = $account->init = trim($mail);
  $account->pass = uniqid();
  $account->status = 1;
  $account->language = $language->language;

  if (!is_null($Customer)) {
    $account->data['sipwise']['customer_id'] = $Customer->id;

    if (!empty($Customer->billing_profile_id)) {

      $BillingProfile = SipwiseEntity::load('billingprofiles', $Customer->billing_profile_id);

      if (!empty($BillingProfile->reseller_id)) {
        $account->data['sipwise']['reseller_id'] = $BillingProfile->reseller_id;
      }
    }
  }

  if (!is_null($subscriber_id)) {
    $account->data['sipwise']['subscriber_id'] = $subscriber_id;
  }

  if (!is_null($Reseller)) {
    $account->data['sipwise']['reseller_id'] = $Reseller->id;
  }
  elseif(!empty($user->data['sipwise']['reseller_id'])) {
    $account->data['sipwise']['reseller_id'] = $user->data['sipwise']['reseller_id'];
  }

  // Dummy name will be changed later in the process to the (Drupal) uid.
  $account->name = uniqid();

  if ($d_role = user_role_load_by_name($role)) {
    $account->roles = array(
      DRUPAL_AUTHENTICATED_RID => TRUE,
      $d_role->rid => TRUE,
    );
  }

  // Save all the essential information to the Drupal user account.
  if ($account = user_save($account)) {

    // Set the username to the uid and update the db entry for this account.
    if ($account = user_save($account, array('name' => $account->uid))) {

      if (!is_null($Customer)) {

        if (!hpbx_user_create_reseller_organizational_unit($ActiveReseller)) {
          // Remove the drupal account.
          if (!empty($account->uid)) {
            user_delete($account->uid);
          }
          return;
        }

        if (!hpbx_user_create_customer_organizational_unit($Customer)) {

          // Remove the drupal account.
          if (!empty($account->uid)) {
            user_delete($account->uid);
          }
          return;
        }
      }

      // Set the complete name for AD.
      $cn = trim($firstname) . ' '.  trim($lastname);

      if (!is_null($Customer)) {
        // Add user to existing or created customer specific OU.
        $dn = 'CN='. ldap_escape($cn, NULL, LDAP_ESCAPE_DN) .','. hpbx_users_get_customer_basedn($Customer);
      }
      elseif(!is_null($Reseller)) {
        // Add user to existing or created customer specific OU.
        $dn = 'CN='. ldap_escape($cn, NULL, LDAP_ESCAPE_DN) .',OU=Administrators,'. hpbx_users_get_reseller_basedn($Reseller);
      }
      else {
        // Remove the drupal account.
        if (!empty($account->uid)) {
          user_delete($account->uid);
        }
        return;
      }

      $entry = array();
      $entry['cn'] = $cn;
      $entry['givenName'] = trim($firstname);
      $entry['sn'] = trim($lastname);
      //$entry['name'] = $cn;
      $entry['objectclass'][0] = 'top';
      $entry['objectclass'][1] = 'person';
      $entry['objectclass'][2] = 'organizationalPerson';
      $entry['mail'] = trim($mail);
      $entry[$ldap_server->user_attr] = $account->uid;

      if ($ldap_server->ldap_type!='openldap') {
        $entry['userprincipalname'] = $account->uid . '@a.local';
        $entry['useraccountcontrol'] = '66080';
        $entry['objectclass'][3] = 'user';
      }
      else {
        $entry['objectclass'][3] = 'inetOrgPerson';
      }

      if (ldap_add($ldap_server->connection, $dn, $entry)===FALSE) {
        drupal_set_message(t('Failed to add a user account for @firstname @lastname', array(
        '@firstname' => trim($firstname),
        '@lastname' => trim($lastname),
        )), 'error');

        watchdog('hpbx_users', ldap_error($ldap_server->connection) . 'dn: '. $dn. ',entry: '. var_export($entry, TRUE));

        // Remove the drupal account.
        if (!empty($account->uid)) {
          user_delete($account->uid);
        }

        return FALSE;
      }

      // Add the role.
      $role_entries = ccs_ldap_server_search_wrapper($ldap_server, "CN=". ldap_escape($role, NULL, LDAP_ESCAPE_FILTER), "OU=Groups,DC=a, DC=local");

      if ((!count($role_entries)) || (ldap_mod_add($ldap_server->connection, $role_entries[0]['dn'], array('member' => $dn))===FALSE)) {
        drupal_set_message(t('Failed to add role.'), 'error');
        watchdog('hpbx_users', ldap_error($ldap_server->connection));
        return FALSE;
      }

      if ($role == 'HostedPBX Customer Administrator' || $role == 'HostedPBX Subscriber') {

        if (!is_null($Customer)) {
          // Reload for settings.
          $Customer = SipwiseEntity::load('customers', $Customer->id);
        }

        // Load the customer contact.
        $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);

        global $language ;
        $params = array();
        $params['firstname'] =  trim($firstname);
        $params['lastname'] =  trim($lastname);
        $params['company'] = $CustomerContact->company;
        $params['lgi_product_name'] = 'My BIZ';

        // Load the subscriber profile set.
        $SubscriberProfileSet = SipwiseEntity::load('subscriberprofilesets', $Customer->_settings->profile_set_id);
        $params['profile_set_name'] = $SubscriberProfileSet->name;

        $params['lgi_brand_name'] = $ActiveReseller->name;
        $params['account'] = $account;
        
        if($CustomerContact->gpp9 != ''){
			$bp_name = trim($CustomerContact->gpp9);
			$query = db_select('hpbx_business_partners','bp');
            $query->fields('bp');
            $query->condition('bp_name', $bp_name);     	
	        $result = $query->execute();
            $bp = $result->fetchAll();
            
            $params['bp_id'] = $bp[0]->bp_id;
            $params['reseller_id'] = $bp[0]->reseller_id;
		}
        
        $from = NULL;

        global $ActiveReseller;

        if (isset($ActiveReseller->_settings->mail_from_address)) {
          $from = $ActiveReseller->_settings->mail_from_address;
        }
        if($CustomerContact->gpp9 == ''){
		  if ($role == 'HostedPBX Customer Administrator') {
            $mail_res = drupal_mail("hpbx", "customer_administrator_welcome", $entry['mail'], $language, $params, $from);
          }elseif($role == 'HostedPBX Subscriber') {
            $mail_res = drupal_mail("hpbx", "customer_subscriber_welcome", $entry['mail'], $language, $params, $from);
          }
		}else{
		  if ($role == 'HostedPBX Customer Administrator') {
            $mail_res = drupal_mail("hpbx", "bp_customer_administrator_welcome", $entry['mail'], $language, $params, $from);
          }elseif($role == 'HostedPBX Subscriber') {
            $mail_res = drupal_mail("hpbx", "bp_subscriber_welcome", $entry['mail'], $language, $params, $from);
          }	
		}
        
        if (!empty($mail_res)) {
          drupal_set_message(t('Further instructions have been sent to @mail', array('@mail' => $entry['mail'])));
        }
        else {
          watchdog('hpbx_user', t('Could not sent e-mail with instructions.'));
        }
      }

      // Drupal user and AD user successfully created.
      return $account;
    }
  }
  return FALSE;
}

/**
 * @param SipwiseEntity $Customer
 * @return bool
 */
function hpbx_users_disable_users(SipwiseEntity $Customer) {
  $ldap_server = hpbx_users_get_ldap_server();
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'cn=*', hpbx_users_get_customer_basedn($Customer));

  unset($entries['count']);

  foreach ($entries as $ad_entry) {

    if (!ccs_ldap_disable_account($ad_entry[$ldap_server->user_attr][0])) {

      hpbx_users_enable_users($Customer);
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * @param SipwiseEntity $Customer
 * @return bool|string
 */
function hpbx_users_get_customer_basedn(SipwiseEntity $Customer) {
  if (!$CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id)) {
    return FALSE;
  }
  if (!$Reseller = SipwiseEntity::load('resellers', $CustomerContact->reseller_id)) {
    return FALSE;
  }
  if (empty($CustomerContact->company)) {
    return FALSE;
  }
  if (empty($Reseller->name)) {
    return FALSE;
  }

  $ou = array();
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($CustomerContact->company));
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->name));
  $ou[] = 'OU=HostedPBX';
  $ou[] = 'DC=a';
  $ou[] = 'DC=local';
  return implode(',', $ou);
}

/**
 * @param SipwiseEntity $Reseller
 * @return bool|string
 */
function hpbx_users_get_reseller_basedn(SipwiseEntity $Reseller) {
  if (empty($Reseller->name)) {
    return FALSE;
  }

  $ou = array();
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->name));
  $ou[] = 'OU=HostedPBX';
  $ou[] = 'DC=a';
  $ou[] = 'DC=local';
  return implode(',', $ou);
}

function hpbx_users_get_bp_basedn(SipwiseEntity $Reseller) {
  if (empty($Reseller->name)) {
    return FALSE;
  }

  $ou[] = 'OU=BusinessPartners';
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->name));
  $ou[] = 'OU=HostedPBX';
  $ou[] = 'DC=a';
  $ou[] = 'DC=local';
  return implode(',', $ou);
}

function hpbx_users_get_bp_name_basedn(SipwiseEntity $Reseller){
  if (empty($Reseller->name)) {
    return FALSE;
  }
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->bp_name));
  $ou[] = 'OU=BusinessPartners';
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->name));
  $ou[] = 'OU=HostedPBX';
  $ou[] = 'DC=a';
  $ou[] = 'DC=local';
  return implode(',', $ou);	
}

function hpbx_users_get_reseller_basedn_on_customer(SipwiseEntity $Customer) {
  if (!$CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id)) {
    return FALSE;
  }
  if (!$Reseller = SipwiseEntity::load('resellers', $CustomerContact->reseller_id)) {
    return FALSE;
  }
  if (empty($Reseller->name)) {
    return FALSE;
  }

  $ou = array();
  $ou[] = 'OU='. hpbx_users_escape_ldap_query(trim($Reseller->name));
  $ou[] = 'OU=HostedPBX';
  $ou[] = 'DC=a';
  $ou[] = 'DC=local';
  return implode(',', $ou);
}

/**
 *
 * @param SipwiseEntity $Customer
 * @return boolean
 */
function hpbx_users_enable_users(SipwiseEntity $Customer) {
  $ldap_server = hpbx_users_get_ldap_server();
  $entries = ccs_ldap_server_search_wrapper($ldap_server, 'cn=*', hpbx_users_get_customer_basedn($Customer));
  unset($entries['count']);
  foreach ($entries as $ad_entry) {

    if (!ccs_ldap_enable_account($ad_entry[$ldap_server->user_attr][0])) {

      hpbx_users_disable_users($Customer);
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * @param array $form
 * @param array $form_state
 * @param integer $reseller_id
 * @return array
 */
function hpbx_users_overview_form($form, &$form_state, $reseller_id = NULL) {

  global $user;

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();;
  }

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  // Set title.
  drupal_set_title(t('Affiliate users'));

  // Init form.
  $form = array();
  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create user') . '</h5>';

  $url = user_access('hpbx edit own reseller users') ? 'hpbx/user/add' : 'hpbx/affiliate/'. (int)$reseller_id .'/user/add';
  $add = l($text, $url, array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form['table'] = array(
    '#add' => array('data' => $add, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-resellers hpbx-table-sortable'),
    '#theme' => 'hpbx_table',
    '#header' => array(
      'name' => array('data'=> t('Name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'mail' => array('data'=> t('Login name'), 'class' => array('hpbx-sortable', 'hpbx-show-sm')),
      'role' => array('data'=> t('Role'), 'class' => array('hpbx-sortable', 'hpbx-show-xs')),
    ),
    '#remove_title' => t('Delete user'),
  );

  $ldap_server = hpbx_users_get_ldap_server();

  $sort_names = $user_table = array();
  $attributes = array('samaccountname', 'sn', 'givenname', 'memberof', 'mail');
  if ($ad_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '=*)(|(memberof=CN=HostedPBX Administrator,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Support,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Content Manager,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Customer Manager,OU=Groups,DC=a, DC=local)))','DC=a, DC=local', $attributes)) {
    unset($ad_entries['count']);
    foreach ($ad_entries as $ad_entry) {

      $account = user_load_by_name($ad_entry[$ldap_server->user_attr][0]);

      if (!empty($account->data['sipwise']['reseller_id']) &&  ($account->data['sipwise']['reseller_id']==$reseller_id)) {

        $name = array();
        if (!empty($ad_entry['sn'][0])) {
          $name[] = $ad_entry['sn'][0];
        }

        if (!empty($ad_entry['givenname'][0])) {
          $name[] = $ad_entry['givenname'][0];
        }

        $roles = array();
        foreach ($ad_entry['memberof'] as $mo) {
          if (preg_match('/cn=HostedPBX\s([^,]+),/', $mo, $matches)) {
            $roles[] = $matches[1];
          }
        }

        // Delete url.
        $url = user_access('hpbx edit own reseller users') ? 'hpbx/user/'.  $ad_entry[$ldap_server->user_attr][0] . '/delete': 'hpbx/affiliate/'. $reseller_id .'/user/'. $ad_entry[$ldap_server->user_attr][0] . '/delete';
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['#attributes']['data-delete-url'] =url($url);

        if ($account->uid == $user->uid) {

          // Currently logged in user, is not allowed to delete his/her own user account.
          $user_table[$ad_entry[$ldap_server->user_attr][0]]['#attributes']['delete'] = FALSE;
        }

        // Edit url.
        $url = user_access('hpbx edit own reseller users') ? 'hpbx/user/'.  $ad_entry[$ldap_server->user_attr][0] . '/edit': 'hpbx/affiliate/'. $reseller_id .'/user/'. $ad_entry[$ldap_server->user_attr][0] . '/edit';
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['#url'] = url($url);

        $user_table[$ad_entry[$ldap_server->user_attr][0]]['name']['data'] = array('#markup' =>  implode(', ', $name));
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['name']['#attributes']['class'] = array('hpbx-show');

        $user_table[$ad_entry[$ldap_server->user_attr][0]]['mail']['data'] = array('#markup' => $ad_entry['mail'][0]);
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['mail']['#attributes']['class'] = array('hpbx-show-sm');

        $user_table[$ad_entry[$ldap_server->user_attr][0]]['role']['data'] = array('#markup' => implode(', ', $roles));
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['role']['#attributes']['class'] = array('hpbx-show-xs');

        $sort_names[$ad_entry['cn'][0]] = $user_table[$ad_entry[$ldap_server->user_attr][0]]['name']['data']['#markup'];
      }
    }
  }

  // Sort based on $sort_names.
  $a = array($user_table, $sort_names);

  array_multisort($a[0], SORT_ASC, SORT_NUMERIC,
  $a[1], SORT_ASC, SORT_NATURAL|SORT_FLAG_CASE);
  $form['table'] = array_merge($form['table'], $a[0]);

  return $form;
}

/**
 * @param integer|null $reseller_id
 * @param string $username
 */
function hpbx_users_delete_page($reseller_id = NULL, $username = '') {


  $ldap_server = hpbx_users_get_ldap_server();

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();
  }

  $account = NULL;

  if (!empty($username)) {

    if ($entries = ccs_users_get_user($username)) {

      // Verify if only one item is found.
      if (isset($entries['count']) && $entries['count'] === 1) {

        // Load the Drupal account.
        if ($account = user_load_by_name($entries[0][$ldap_server->user_attr][0])) {

          // Verify the correct reseller is assigned.
          if ($account->data['sipwise']['reseller_id'] != $reseller_id) {
            drupal_set_message(t('Not allowed to remove the user account'), 'error');
          }
          else {

            // Try to delete entry.
            if (ldap_delete($ldap_server->connection, $entries[0]['dn']) === FALSE) {
              watchdog('hpbx_users', 'Failed to remove user with username from AD: @username', array('@username' => $entries[0]['$ldap_server->user_attr'][0]));
              watchdog('hpbx_users', ldap_error($ldap_server->connection));
              drupal_set_message(t('Failed to remove the user account'), 'error');
            }
            else {
              // Successfully removed the user account from active directory. Now remove it from Drupal.
              try {
                user_delete($account->uid);
                watchdog('hpbx_users', 'Successfully removed affiliate user with @username', array('@username' => $entries[0]['$ldap_server->user_attr'][0]));
                drupal_set_message(t('User succesfully removed'));
              }
              catch (Exception $e) {
                watchdog('hpbx_users', 'Failed to remove user with username from drupal: @username @exception_message', array('@username' => $entries[0]['$ldap_server->user_attr'][0], '@exception_message' => $e->getMessage()));
                drupal_set_message(t('Only partially removed the user account'), 'error');
              }
            }
          }
        }
        else {
          // Drupal user account not found, can't verify reseller.
          drupal_set_message(t('Failed to remove the user account'), 'error');
          watchdog('hpbx_users', 'Failed to load drupal user with username @username', array('@username' => $entries[0]['$ldap_server->user_attr'][0]));
        }
      }
      else {
        // Drupal user account not found, can't verify reseller.
        drupal_set_message(t('Failed to remove the user account'), 'error');
        watchdog('hpbx_users', 'Failed to load drupal user with username @username (more then 1 user found?)', array('@username' => $entries[0]['$ldap_server->user_attr'][0]));
      }
    }
  }

  if (user_access('hpbx edit own reseller users')) {
    drupal_goto('hpbx/users');
  }
  elseif (user_access('hpbx view administrator user')) {
    drupal_goto('hpbx/affiliate/'. $reseller_id .'/users');
  }
  else {
    drupal_goto('hpbx');
  }
}

/**
 * @param array $form
 * @param array$form_state
 * @param integer|NULL $reseller_id
 * @param string $username
 * @return array
 */
function hpbx_users_edit_form($form, $form_state, $reseller_id = NULL, $username = '') {

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();;
  }

  $ldap_server = ccs_ldap_get_valid_server();

  $account = NULL;

  if (!empty($username)) {

    if ($ad_entries = ccs_users_get_user($username)) {
      $entry = $ad_entries[0];

      $account = user_load_by_name($entry[$ldap_server->user_attr][0]);
      drupal_set_title(t('User @cn', array('@cn' => $entry['cn'][0])));
    }
  } 
  $entries['memberof'] = ccs_ldap_server_search_wrapper($ldap_server, $ldap_server->user_attr. '='. $username, 'DC=A, DC=local', array('memberof'));
  //ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '= $username)(|(memberof=CN=HostedPBX Administrator,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Support,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Content Manager,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Customer Manager,OU=Groups,DC=a, DC=local)))','DC=a, DC=local',$attributes)

  foreach($entries['memberof'] as $mem){
    $entry['memberof'] = $mem['memberof'];
  }


  $form = array(
      '#prefix' => '<div class="hpbx-main-content-inner">',
      '#suffix' => '</div>',
      '#attributes' => array('class' => array('hpbx-tab-form')),
  );

  $form['general'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );

  $form['drupal_name'] = array(
    '#type' => 'value',
    '#value' => !empty($account->name) ? $account->name : NULL,
  );

  // Reseller.
  $form['reseller_id'] = array(
    '#type' => 'value',
    '#value' => $reseller_id,
  );

  $roles = array();
  if (isset($entry['memberof'])) {
    foreach ($entry['memberof'] as $mo) {
      if (preg_match('/cn=(HostedPBX\s[^,]+),/', $mo, $matches)) {
        $roles[] = $matches[1];
      }
    }
  }

  // Role
  $form['general']['role']['value'] = array(
    '#type' => 'select',
    '#options' => drupal_map_assoc(array('HostedPBX Support', 'HostedPBX Administrator', 'HostedPBX Content Manager', 'HostedPBX Customer Manager')),
    '#default_value' => count($roles) === 1 ? current($roles) : NULL,
    '#required' => TRUE,
    '#title' => t('Role'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  // Firstname.
  $form['general']['firstname']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['givenname'][0]) ? $entry['givenname'][0] : '',
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('First name')),
    '#required' => TRUE,
    '#title' => t('First name'),
  );

  // Lastname.
  $form['general']['lastname']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['sn'][0]) ? $entry['sn'][0] : '',
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Last name')),
    '#required' => TRUE,
    '#title' => t('Last name'),
  );

  // E-mail adres.
  $form['general']['email']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['mail'][0]) ? $entry['mail'][0] : '',
    '#required' => TRUE,
    '#rules' => array(
      'email'
     ),
    '#title' => t('Email'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Email')),

  );
  // Submit button.
  $form['general']['actions'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',

  );
  
  if (user_access('hpbx view reseller')) {
     $url = 'hpbx/affiliate/'. $reseller_id .'/users';
  }
  else {
    $url = 'hpbx/users';
  }  
  
  $form['general']['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => $url,
    '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
    '#prefix' => '<div class="col-xs-12">',    
  );
  $form['general']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span></div>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );

  return $form;
}

function hpbx_users_edit_form_validate($form, &$form_state) {

  $resellers = hpbx_get_resellers();

  if (!isset($resellers[$form_state['values']['reseller_id']])) {
    form_set_error('reseller_id', t('Reseller not found'));
  }
  else {

    $Reseller = $resellers[$form_state['values']['reseller_id']];

    $firstname = trim($form_state['values']['general']['firstname']['value']);
    $lastname = trim($form_state['values']['general']['lastname']['value']);
    $email = trim($form_state['values']['general']['email']['value']);
    $drupal_name = trim($form_state['values']['drupal_name']);

    $ldap_server = hpbx_users_get_ldap_server();

    if ($entries = ccs_ldap_server_search_wrapper($ldap_server, 'cn='. ldap_escape($firstname . ' '.  $lastname, NULL, LDAP_ESCAPE_FILTER), 'OU=Administrators,'. hpbx_users_get_reseller_basedn($Reseller))) {

      if ((isset($entries['count']) && $entries['count']) && (empty($drupal_name) || $drupal_name != $entries[0][$ldap_server->user_attr][0])) {
        form_set_error('general][settings][firstname][value', t('A user with the provided first name and last name is already present within the HPBX administrator group.'));
        form_set_error('general][settings][lastname][value', '');
        return FALSE;
      }
    }

    // Lookup the user in complete directory.
    if ($ldap_username = ccs_ldap_get_user_on_mail_address($email)) {

      $form_state['values']['drupal_name'] = $ldap_username;
      if (!$ldap_user = ccs_ldap_user_loopup($ldap_username)) {
        form_set_error('name', t('Unable to retrieve existing ldap user %name.'), array('%name' => $ldap_username));
        return;
      }

      // Check if this user already does have some HPBX Roles. 
      if (isset($ldap_user['attr']['memberof']) && is_array($ldap_user['attr']['memberof']) && preg_grep("/^HPBX.*/", $ldap_user['attr']['memberof'])) {
        form_set_error('general][settings][email][value', t('A user - with one of the HPBX roles - with the provided email address is already present.'));
        return FALSE;
      }
    }
  }
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_users_edit_form_submit($form, $form_state) {
  global $language ;
  $resellers = hpbx_get_resellers();
  $Reseller = $resellers[$form_state['values']['reseller_id']];

  $firstname = trim($form_state['values']['general']['firstname']['value']);
  $lastname = trim($form_state['values']['general']['lastname']['value']);
  $email = trim($form_state['values']['general']['email']['value']);
  $role = $form_state['values']['general']['role']['value'];

  if (empty($form_state['values']['drupal_name'])) {

    if ($account = hpbx_users_create_reseller_user($Reseller, $email, $firstname, $lastname, $role)) {

      $params = array();
      $params['firstname'] =  $firstname;
      $params['lastname'] =  $lastname;
      $params['lgi_brand_name'] = $Reseller->name;
      $params['account'] = $account;
      $params['reseller_id'] = $Reseller->id;
      $from = (isset($Reseller->_settings->mail_from_address)) ? $Reseller->_settings->mail_from_address : NULL;

      $mail_res = drupal_mail("hpbx", "affiliate_user_welcome", $email, $language, $params, $from);

      if (!empty($mail_res)) {
        drupal_set_message(t('Further instructions have been sent to @mail', array('@mail' => $email)));
      }
      else {
        watchdog('hpbx_user', t('Could not sent e-mail with instructions.'));
      }

      $url = user_access('hpbx edit own reseller users') ? 'hpbx/users' : 'hpbx/affiliate/'.$form_state['values']['reseller_id'].'/users';
      drupal_goto($url);
    }
    else {
      drupal_set_message(t('Failed to create reseller administrator account'), 'error');
    }
  }
  else {

    if (!hpbx_users_update_user($form_state['values']['drupal_name'], $email, $firstname, $lastname, $role)) {
      drupal_set_message(t('Failed to update reseller administrator account'), 'error');
    }
    else {
      // Change the Drupal user account reseller.
      if ($accounts = user_load_multiple(NULL, array('name' => $form_state['values']['drupal_name']))) {
        $account = reset($accounts);

        $account->data['sipwise']['reseller_id'] = $Reseller->id;

        // Set the role.
        if ($d_role = user_role_load_by_name($role)) {
          $account->roles = array(
              DRUPAL_AUTHENTICATED_RID => TRUE,
              $d_role->rid => TRUE,
          );
        }
        user_save($account);
      }
      else {
        drupal_set_message(t('Failed to update reseller administrator account'), 'error');
      }

      drupal_set_message(t('User succesfully updated'), 'status');

      $url = user_access('hpbx edit own reseller users') ? 'hpbx/users' : 'hpbx/affiliate/'.$form_state['values']['reseller_id'].'/users';
      drupal_goto($url);
    }
  }
}

function hpbx_users_validate_reseller_user(SipwiseEntity $Reseller, $firstname, $lastname, $email, $drupal_name = NULL) {

  $ldap_server = hpbx_users_get_ldap_server();

  if ($ad_entry = ccs_ldap_server_search_wrapper($ldap_server, 'cn='. ldap_escape($firstname . ' '.  $lastname, NULL, LDAP_ESCAPE_FILTER), 'OU=Administrators,'. hpbx_users_get_reseller_basedn($Reseller))) {
    if (empty($drupal_name) || $drupal_name != $ad_entry[0][$ldap_server->user_attr][0]) {
      form_set_error('general][settings][firstname][value', t('A user with the provided first name and last name is already present.'));
      form_set_error('general][settings][lastname][value', '');
      return FALSE;
    }
  }

  // Validate e-mail account.
  if ($account = user_load_by_mail($email)) {
    if (empty($drupal_name) ||  $drupal_name != $account->uid) {

      form_set_error('general][settings][email][value', t('A user with the provided email address is already present.'));
      return FALSE;
    }
  }

  return TRUE;
}


function hpbx_users_validate_customer_user(SipwiseEntity $Customer, $firstname, $lastname, $email, $subscriber_id = NULL) {

  if (!is_null($subscriber_id)) {
    $Subscriber = SipwiseEntity::load('subscribers', $subscriber_id);
  }
  $ldap_server = hpbx_users_get_ldap_server();

  if ($ad_entry = ccs_ldap_server_search_wrapper($ldap_server, 'cn='. ldap_escape($firstname . ' '.  $lastname, NULL, LDAP_ESCAPE_FILTER), hpbx_users_get_customer_basedn($Customer))) {
    if (isset($ad_entry['count']) && $ad_entry['count'] > 0) {
      if (!isset($Subscriber) || $Subscriber->webusername != $ad_entry[0][$ldap_server->user_attr][0]) {
        form_set_error('general][settings][firstname][value', t('A user with the provided first name and last name is already present.'));
        form_set_error('general][settings][lastname][value', '');
        return FALSE;
      }
    }
  }

  // Validate e-mail account iwhtin the Drupal database.
  if ($account = user_load_by_mail($email)) {
    if (!isset($Subscriber) || $Subscriber->webusername != $account->uid) {
      form_set_error('general][settings][email][value', t('A user with the provided email address is already present.'));
      return FALSE;
    }
  }

  if ($ldap_username = ccs_ldap_get_user_on_mail_address($email)) {
    if (!isset($Subscriber) || $Subscriber->webusername != $ldap_username) {
      form_set_error('general][settings][email][value', t('A user with the provided email address is already present.'));
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Implementation of hook_user_presave().

 * @param array $edit
 * @param stdClass $account
 * @param string $category
 */
function hpbx_user_presave(&$edit, $account, $category) {

  if (hpbx_users_is_hpbx_user($account)) {
    if ($account->is_new && !empty($edit['name'])) {
      $name = $edit['name'];
    }
    else {
      $name = $account->name;
    }

    // Get user assigned numbers and/or municipalities.
    $edit['data']['sipwise']['account'] = hpbx_users_get_user($name);
  }
}

