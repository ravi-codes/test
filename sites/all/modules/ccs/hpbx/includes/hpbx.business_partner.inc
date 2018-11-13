<?php


function hpbx_business_partner_edit_form($form, $form_state, $reseller_id = NULL){
	global $ActiveReseller;
  
  // Temporary set the reseller_id.
  

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js'); 

  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }
  else {
  	$reseller_id = sipwise_api_get_active_reseller();
  	sipwise_api_activate_reseller((int)$reseller_id);
    drupal_set_title(t('Create Business Partner'));   
   
  }

  // Init form.
  $form = array(
    '#prefix' => '<div class="hpbx-main-content-inner hpbx-c">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );

  $form['general'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list', 'container'),
    '#attributes' => array('class' => array('hpbx-tab-form-foldable-list')),
  );

  $form['general']['table'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    //'#theme' => 'hpbx_table',    
    '#title' => t('Business Partner Settings'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#collapsed' => FALSE,
  );

  // Set the existing id.
  $form['reseller_id'] = array(
    '#type' => 'value',
    '#value' => !is_null($reseller_id) ? $reseller_id : FALSE,
  ); 
 
  
  // External ID.
  $form['general']['table']['bp_name']['value'] = array(
	  '#type' => 'textfield',
      '#required' => TRUE,
      '#maxlength' => 255,
      '#title' => t('Name'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Business Partner Name')),
  );
  
  $form['general']['bp_admin'] = array(
      '#type' => 'container',
      '#title' => t('Business Partner Administrator'),
      '#tree' => TRUE,      
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#collapsed' => FALSE,
    );

    $form['general']['bp_admin']['table'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#theme' => 'hpbx_table',
      '#title' => t('Administrative user'),
    );

    // Product.
    $form['general']['bp_admin']['table']['firstname']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#maxlength' => 40,
      '#title' => t('First name'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('First name')),
    );

    $form['general']['bp_admin']['table']['lastname']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#maxlength' => 40,
      '#title' => t('Last name'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Last name')),
    );

    $form['general']['bp_admin']['table']['email']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('Email'),
      '#rules' => array(
        'email'
       ),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Email')),
    );
    
   $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
   );

    
    if (!is_null($reseller_id)) {
      $url = 'hpbx/affiliate/'. $reseller_id .'/bps';
    }
    else {
      $url = 'hpbx/bps';
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
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary'), 'data-confirmed' => FALSE),
  );
  
  return $form;
}

function hpbx_business_partner_edit_form_validate($form, &$form_state){	
  
  $resellers = hpbx_get_resellers();

  if (!isset($resellers[$form_state['values']['reseller_id']])) {
    form_set_error('reseller_id', t('Reseller not found'));
  }
  else {

    $Reseller = $resellers[$form_state['values']['reseller_id']];
    $bp_name = trim($form_state['values']['general']['table']['bp_name']['value']);
    $firstname = trim($form_state['values']['general']['bp_admin']['table']['firstname']['value']);
    $lastname = trim($form_state['values']['general']['bp_admin']['table']['lastname']['value']);
    $email = trim($form_state['values']['general']['bp_admin']['table']['email']['value']);
    //$drupal_name = trim($form_state['values']['drupal_name']);

    $ldap_server = hpbx_users_get_ldap_server();

    if ($entries = ccs_ldap_server_search_wrapper($ldap_server, 'cn='. ldap_escape($firstname . ' '.  $lastname, NULL, LDAP_ESCAPE_FILTER), 'OU='.ldap_escape($bp_name, NULL, LDAP_ESCAPE_DN).',OU=BusinessPartners,'. hpbx_users_get_reseller_basedn($Reseller))) {

      if ((isset($entries['count']) && $entries['count'])) {
        form_set_error('general][bp_admin][table][firstname][value', t('A user with the provided first name and last name is already present within the HPBX administrator group.'));
        form_set_error('general][bp_admin][table][lastname][value', '');        
      }
    }
    if(hpbx_business_partner_name_in_use($bp_name)){
	  form_set_error('general][table][bp_name][value', t('Name is already assigned to Business Partner @bp', array(
      '@bp' => $form_state['values']['general']['table']['bp_name']['value']
    )));
	}

    // Lookup the user in complete directory.
    if ($ldap_username = ccs_ldap_get_user_on_mail_address($email)) {
      //$form_state['values']['drupal_name'] = $ldap_username;
      if (!$ldap_user = ccs_ldap_user_loopup($ldap_username)) {
        form_set_error('name', t('Unable to retrieve existing ldap user %name.'), array('%name' => $ldap_username));        
      }

      // Check if this user already does have some HPBX Roles. 
      if (isset($ldap_user['attr']['memberof']) && is_array($ldap_user['attr']['memberof']) && preg_grep("/^HPBX.*/", $ldap_user['attr']['memberof'])) {
        form_set_error('general][bp_admin][table][email][value', t('A user - with one of the HPBX roles - with the provided email address is already present.'));        
      }
    }
  }
 	
  if (!empty($email) &&
    user_load_by_mail(trim($email))) {
    form_set_error('general][bp_admin][table][email][value', t('A user with the provided email address is already present.'));
  }	
	
}

function hpbx_business_partner_edit_form_submit($form, &$form_state){	
 
  global $language, $ActiveReseller;
  
  $resellers = hpbx_get_resellers();
  $reseller_id = $form_state['values']['reseller_id'];
  $Reseller = $resellers[$reseller_id];
  $bp_name = trim($form_state['values']['general']['table']['bp_name']['value']);
  $firstname = trim($form_state['values']['general']['bp_admin']['table']['firstname']['value']);
  $lastname = trim($form_state['values']['general']['bp_admin']['table']['lastname']['value']);
  $email = trim($form_state['values']['general']['bp_admin']['table']['email']['value']);
  $role = 'HostedPBX Business Partner Administrator'; 
  $datetime = date("Y-m-d H:i:s");
  
  if (!is_null($reseller_id)) {

    sipwise_api_activate_reseller($reseller_id);
  }
  
  $settings = array(); 
  $settings['help_telnumber'] = $ActiveReseller->_settings->help_telnumber;
  $settings['help_website'] = $ActiveReseller->_settings->help_website;
  $settings['help_mail'] = $ActiveReseller->_settings->help_mail;
  $settings['help_name'] = $ActiveReseller->_settings->help_name;
  $settings['logo_path'] = $ActiveReseller->_settings->logo_path;
  $settings['mail_footer'] = $ActiveReseller->_settings->mail_footer;
  $settings = serialize($settings); 
  
  try{
  	//insert the business partner values to local 
  	$bp_id = db_insert('hpbx_business_partners')->fields(array('reseller_id'=> $reseller_id,'bp_name' => $bp_name ,'create_date' => $datetime ))->execute();
  	
  	$query = db_merge('hpbx_business_partner_settings')
      ->key(array(
        'reseller_id' => $reseller_id,
        'bp_id' => $bp_id
      ))
      ->fields(array(
        'bp_id' => $bp_id, 
        'reseller_id'=> $reseller_id, 
        'settings' => $settings, 
        'update_date' => $datetime        
      ));

    $query->execute();
  }
  catch (Exception $e) {
      watchdog('hpbx_business_partner', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
      drupal_set_message('Failed to create business partner','error');
  }  
  
  
  
  if ($account = hpbx_users_create_bp_user($Reseller, $bp_name, $bp_id, $email, $firstname, $lastname, $role)) {
  	
  	  $mail_key = 'bp_customer_administrator_welcome_'. $Reseller->id .'_'. $bp_id;
      $reseller_mail_key = 'customer_administrator_welcome_'. $Reseller->id;
  
      //saving the affiliate Administrator welcome email subject to Business Partner Customer Administrator welcome email subject  
      if(variable_get('user_mail_'. $mail_key .'_subject', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_subject', variable_get('user_mail_'. $reseller_mail_key .'_subject'));  	
      }
      if(variable_get('user_mail_'. $mail_key .'_body', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_body', variable_get('user_mail_'. $reseller_mail_key .'_body'));
      }  
  
      $mail_key = 'bp_subscriber_welcome_'. $Reseller->id .'_'. $bp_id;
      $reseller_mail_key = 'customer_subscriber_welcome_'. $Reseller->id;
  
      //saving the affiliate Subscriber welcome email subject to Business Partner Subscriber welcome email subject
      if(variable_get('user_mail_'. $mail_key .'_subject', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_subject', variable_get('user_mail_'. $reseller_mail_key .'_subject'));  	
      }   
      if(variable_get('user_mail_'. $mail_key .'_body', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_body', variable_get('user_mail_'. $reseller_mail_key .'_body'));
      }  
  
      $mail_key = 'bp_user_welcome_'. $Reseller->id .'_'. $bp_id;
      $reseller_mail_key = 'bp_administrator_welcome_'. $Reseller->id;
  
      //saving the affiliate Business Partner welcome email subject to Busines Partner User welcome email subject
      if(variable_get('user_mail_'. $mail_key .'_subject', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_subject', variable_get('user_mail_'. $reseller_mail_key .'_subject'));  	
      }
      if(variable_get('user_mail_'. $mail_key .'_body', FALSE) == FALSE){
  	    variable_set('user_mail_'. $mail_key .'_body', variable_get('user_mail_'. $reseller_mail_key .'_body'));
      }

      $params = array();
      $params['firstname'] =  $firstname;
      $params['lastname'] =  $lastname;
      $params['lgi_brand_name'] = $Reseller->name;
      $params['account'] = $account;
      $params['reseller_id'] = $Reseller->id;
      $from = (isset($ActiveReseller->_settings->mail_from_address)) ? $ActiveReseller->_settings->mail_from_address : NULL;

      $mail_res = drupal_mail("hpbx", "bp_administrator_welcome", $email, $language, $params, $from);

      if (!empty($mail_res)) {
        drupal_set_message(t('Further instructions have been sent to @mail', array('@mail' => $email)));
      }
      else {
        watchdog('hpbx_user', t('Could not sent e-mail with instructions.'));
      }

      $url = user_access('hpbx add business partner customer') ? 'hpbx/bps' : 'hpbx/affiliate/'.$form_state['values']['reseller_id'].'/bps';
      drupal_goto($url);
  }
  else {
      drupal_set_message(t('Failed to create reseller business partner administrator account'), 'error');
  }  	
    
}


function hpbx_business_partner_name_in_use($bp_name) {
  global $ActiveReseller;

  try {

    // Search for a number.
    $query = db_select('hpbx_business_partners', 'bp')
      ->fields('bp')
      ->condition('bp.bp_name', trim($bp_name))
      ->condition('reseller_id', $ActiveReseller->id)
      ->execute();    
	$result = $query->rowCount();	
    if($result > 0){
	  return TRUE;
	}else{
	  return FALSE;	
	}     
  }
  catch (Exception $e) {
    watchdog('hpbx_bp_name', $e->getMessage());
    return FALSE;
  }
}

function hpbx_bp_overview_page($reseller_id = NULL){
  global $user;

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();;
  }

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  // Set title.
  drupal_set_title(t('Business Partners'));

  // Init form.
  $form = array();
  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create Business Partners') . '</h5>';

  $url = user_access('hpbx edit own reseller users') ? 'hpbx/bp/add' : 'hpbx/affiliate/'. (int)$reseller_id .'/bp/add';
  $add = l($text, $url, array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form['general'] = array(
    '#add' => array('data' => $add, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-resellers hpbx-table-sortable'),
    '#theme' => 'hpbx_table',
    '#header' => array(
      'name' => array('data'=> t('Business Partner Name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'operations' => array('data'=> '', 'class' => array( 'hpbx-show'))           
    ),
    '#remove_title' => t('Delete Business Partner'),
  );
  
 foreach( hpbx_business_partners_get_local($reseller_id) as $bp){	  	
  	$form['general'][$bp->bp_name]['name']['data'] = array('#markup' => $bp->bp_name);
    $form['general'][$bp->bp_name]['name']['#attributes']['class'] = array('hpbx-show');
    //sipwise_api_activate_reseller($reseller_id);
   if(user_access('hpbx edit business partner settings')){
   	 $settingsUrl = 'hpbx/bp/'. $bp->bp_id .'/settings';
   	 $usersUrl = 'hpbx/bp/'. $bp->bp_id .'/users';
   }else{
   	 $settingsUrl = 'hpbx/affiliate/'. $bp->reseller_id . '/bp/'. $bp->bp_id .'/settings';
   	 $usersUrl = 'hpbx/affiliate/'. $bp->reseller_id . '/bp/'. $bp->bp_id .'/users';
   }
   
    $op = array();
    $op[] =  l(t('Settings'), $settingsUrl, array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
    $op[] = l(t('Users'), $usersUrl, array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
      
    $form['general'][$bp->bp_name]['operations']['data'] = array('#markup' => implode(' ', $op));
    $form['general'][$bp->bp_name]['operations']['#attributes']['class'] = array('hpbx-show'); 
    $url = user_access('hpbx edit own reseller users') ? 'hpbx/bp/'.  $bp->bp_id . '/delete': 'hpbx/affiliate/'. $reseller_id .'/bp/'. $bp->bp_id . '/delete';
    $form['general'][$bp->bp_name]['#attributes']['data-delete-url'] =url($url);    
    
  } 
  return $form;
}

function hpbx_business_partners_get_local($reseller_id = NULL, $bp_id = NULL){
  
  if (!$reseller_id) return array();

  try {
    // Search for company name and external id.
    $query = db_select('hpbx_business_partners','bp');
    $query->fields('bp');
    if(!is_null($bp_id)){
	  $query->condition('bp_id', $bp_id);
	}
	if(!is_null($reseller_id)){
	  $query->condition('reseller_id', $reseller_id);
	}
	$query->orderBy('bp_name', 'ASC');   
	
    $result = $query->execute();

    return $result->fetchAll();
  }
  catch (Exception $e) {
    watchdog('hpbx_bp_search', $e->getMessage());
    return FALSE;
  }
}

function hpbx_bp_users_overview_page($reseller_id = NULL, $bp_id = NULL){
  global $user, $ActiveReseller;
 
  if(is_null($reseller_id)){
  	$reseller_id = sipwise_api_get_active_reseller();
  }
  if(is_null($bp_id)){
  	$bp_id = $user->data['sipwise']['bp_id'];
  }
  
  sipwise_api_activate_reseller($reseller_id);
	
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  // Set title.
  drupal_set_title(t('Business Partner users'));

  // Init form.
  $form = array();
  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create user') . '</h5>';
  
  if(user_access('hpbx view reseller')){
  	$url = 'hpbx/affiliate/'. (int)$reseller_id .'/bp/'.(int)$bp_id.'/user/add';
  }elseif(user_access('hpbx edit business partner users')){
  	$url =  'hpbx/bp/'.(int)$bp_id.'/user/add';
  }else{
  	$url =  'hpbx/bp/user/add';
  }
  
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
  if ($ad_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '=*)(|(memberof=CN=HostedPBX Business Partner Administrator,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Support,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Customer Manager,OU=Groups,DC=a, DC=local)))','DC=a, DC=local', $attributes)) {
  	
  	unset($ad_entries['count']);
  	
    foreach ($ad_entries as $ad_entry) {
   	  $account = user_load_by_name($ad_entry[$ldap_server->user_attr][0]);   	  
   	   if (!empty($account->data['sipwise']['reseller_id']) &&  ($account->data['sipwise']['reseller_id'] == $reseller_id)){
   	   	if (!empty($account->data['sipwise']['bp_id']) &&  ($account->data['sipwise']['bp_id'] == $bp_id)) {

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
        $url = 'hpbx/affiliate/'. $reseller_id .'/user/'. $ad_entry[$ldap_server->user_attr][0] . '/bp/'.$bp_id.'/delete';
        $user_table[$ad_entry[$ldap_server->user_attr][0]]['#attributes']['data-delete-url'] =url($url);

        if ($account->uid == $user->uid) {

          // Currently logged in user, is not allowed to delete his/her own user account.
          $user_table[$ad_entry[$ldap_server->user_attr][0]]['#attributes']['delete'] = FALSE;
        }

        // Edit url.
        $url = user_access('hpbx edit own reseller users') ? 'hpbx/bp/'.$bp_id. '/user/' .  $ad_entry[$ldap_server->user_attr][0] . '/edit': 'hpbx/affiliate/'. $reseller_id .'/bp/'.$bp_id. '/user/' .  $ad_entry[$ldap_server->user_attr][0] . '/edit';
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
  }
   $a = array($user_table, $sort_names);

  array_multisort($a[0], SORT_ASC, SORT_NUMERIC,
  $a[1], SORT_ASC, SORT_NATURAL|SORT_FLAG_CASE);
  $form['table'] = array_merge($form['table'], $a[0]);

  return $form;
	
}

function hpbx_bp_users_edit_form($form, $form_state, $reseller_id = NULL, $bp_id = NULL, $username = ''){
	
  global $user;
 
  if(is_null($reseller_id)){
  	$reseller_id = $user->data['sipwise']['reseller_id'];
  }
  if(is_null($bp_id)){
  	$bp_id = $user->data['sipwise']['bp_id'];
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
  
  $form['bp_id'] = array(
    '#type' => 'value',
    '#value' => !empty($bp_id) ? $bp_id : NULL,
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
    '#options' => drupal_map_assoc(array('HostedPBX Business Partner Support', 'HostedPBX Business Partner Administrator', 'HostedPBX Business Partner Customer Manager')),
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
    $url = 'hpbx/affiliate/'. $reseller_id .'/bp/'. $bp_id .'/users';
  }
  elseif(user_access('hpbx edit business partner users')){
  	$url = 'hpbx/bp/'. $bp_id .'/users';
  }
  else {
    $url = 'hpbx/bp/users';
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

function hpbx_bp_users_edit_form_validate($form, &$form_state){
	
  $resellers = hpbx_get_resellers();
  
  $reseller_id = $form_state['values']['reseller_id'];
  $bp_id = $form_state['values']['bp_id'];

  if($bp = hpbx_business_partners_get_local($reseller_id, $bp_id)){
  	if (!isset($resellers[$form_state['values']['reseller_id']])) {
      form_set_error('reseller_id', t('Reseller not found'));
    }
    else {
	
      $Reseller = $resellers[$form_state['values']['reseller_id']];
      $bp_name = trim($bp[0]->bp_name);
      $firstname = trim($form_state['values']['general']['firstname']['value']);
      $lastname = trim($form_state['values']['general']['lastname']['value']);
      $email = trim($form_state['values']['general']['email']['value']);
      $drupal_name = trim($form_state['values']['drupal_name']);
      $ldap_server = hpbx_users_get_ldap_server();
	
      if ($entries = ccs_ldap_server_search_wrapper($ldap_server, 'cn='. ldap_escape($firstname . ' '.  $lastname, NULL, LDAP_ESCAPE_FILTER), 'OU='.ldap_escape($bp_name, NULL, LDAP_ESCAPE_DN).',OU=BusinessPartners,'. hpbx_users_get_reseller_basedn($Reseller)))      {
	    if ((isset($entries['count']) && $entries['count']) && (empty($drupal_name) || $drupal_name != $entries[0][$ldap_server->user_attr][0])) {
          form_set_error('general][firstname][value', t('A user with the provided first name and last name is already present within the HPBX administrator group.'));
          form_set_error('general][lastname][value', '');
          return FALSE;
        }
      }
      if ($entries = ccs_ldap_server_search_wrapper($ldap_server, 'mail='. ldap_escape($email, NULL, LDAP_ESCAPE_FILTER), 'OU='.ldap_escape($bp_name, NULL, LDAP_ESCAPE_DN).',OU=BusinessPartners,'. hpbx_users_get_reseller_basedn($Reseller)))      {
      	if ((isset($entries['count']) && $entries['count']) && (empty($drupal_name) || $drupal_name != $entries[0][$ldap_server->user_attr][0])) {
          form_set_error('general][email][value', t('A user with the provided email address is already present within the HPBX administrator group.'));
          
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
          form_set_error('general][email][value', t('A user - with one of the HPBX roles - with the provided email address is already present.'));
          return FALSE;
        }
      }      
    }
  }  
}

function hpbx_bp_users_edit_form_submit($form, &$form_state){
	
  global $language, $ActiveReseller;
  
  $resellers = hpbx_get_resellers();
  
  $reseller_id = $form_state['values']['reseller_id'];
  $Reseller = $resellers[$reseller_id];
  $bp_id = $form_state['values']['bp_id'];
  //$bp_settings = hpbx_business_partner_settings_get_local($bp_id);
  $bp = hpbx_business_partners_get_local($reseller_id, $bp_id);
  $bp_name = trim($bp[0]->bp_name);
  $firstname = trim($form_state['values']['general']['firstname']['value']);
  $lastname = trim($form_state['values']['general']['lastname']['value']);
  $email = trim($form_state['values']['general']['email']['value']);
  $role = trim($form_state['values']['general']['role']['value']);  
  
  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }  
  if (empty($form_state['values']['drupal_name'])) {    
    if ($account = hpbx_users_create_bp_user($Reseller, $bp_name, $bp_id, $email, $firstname, $lastname, $role)) {    	      
      $params = array();
      $params['firstname'] =  $firstname;
      $params['lastname'] =  $lastname;
      $params['lgi_brand_name'] = $Reseller->name;
      $params['account'] = $account;
      $params['reseller_id'] = $Reseller->id;
      $params['bp_id'] = $bp_id;
      $from = (isset($ActiveReseller->_settings->mail_from_address)) ? $ActiveReseller->_settings->mail_from_address : NULL;

      $mail_res = drupal_mail("hpbx", "bp_user_welcome", $email, $language, $params, $from);

      if (!empty($mail_res)) {
        drupal_set_message(t('Further instructions have been sent to @mail', array('@mail' => $email)));
      }
      else {
        watchdog('hpbx_user', t('Could not sent e-mail with instructions.'));
      }
	  if(user_access('hpbx edit own business partner users')){
	  	$url = 'hpbx/bp/users';
	  }elseif(user_access('hpbx edit business partner users')){
	  	$url = 'hpbx/bp/'.$bp_id.'/users';
	  }else{
	  	$url = '/hpbx/affiliate/'.$reseller_id.'/bp/'.$bp_id.'/users';
	  }      
      drupal_goto($url);
    }
    else {
      drupal_set_message(t('Failed to create reseller business partner administrator account'), 'error');
    }
  }else{
  	if (!hpbx_users_update_user($form_state['values']['drupal_name'], $email, $firstname, $lastname, $role)) {
      drupal_set_message(t('Failed to update Business Partner administrator account'), 'error');
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
        drupal_set_message(t('Failed to update Business Partner administrator account'), 'error');
      }

      drupal_set_message(t('User successfully updated'), 'status');
	  if(user_access('hpbx edit own business partner users')){
	  	$url = 'hpbx/bp/users';
	  }elseif(user_access('hpbx edit business partner users')){
	  	$url = 'hpbx/bp/'.$bp_id.'/users';
	  }else{
	  	$url = '/hpbx/affiliate/'.$reseller_id.'/bp/'.$bp_id.'/users';
	  }	     
      drupal_goto($url);
    }
  }  	
}

function hpbx_bp_delete_page($reseller_id = NULL, $bp_id = NULL){
  
  global $ActiveReseller;
  
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.customer.inc');
  if(!is_null($reseller_id)){
  	sipwise_api_activate_reseller($reseller_id);
  }else{
  	$reseller_id = sipwise_api_get_active_reseller();
  	sipwise_api_activate_reseller($reseller_id);
  }  
  $bp = hpbx_business_partners_get_local($reseller_id, $bp_id);
  $bp_name = $bp[0]->bp_name;
  $ldap_server = hpbx_users_get_ldap_server();
  
  if ($ad_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '=*)(|(memberof=CN=HostedPBX Business Partner Administrator,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Support,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Customer Manager,OU=Groups,DC=a, DC=local)))','DC=a, DC=local')) {
    unset($ad_entries['count']);
    
    foreach ($ad_entries as $ad_entry) {    	
      $account = user_load_by_name($ad_entry[$ldap_server->user_attr][0]);
      if (!empty($account->data['sipwise']['reseller_id']) &&  ($account->data['sipwise']['reseller_id']==$reseller_id) && !empty($account->data['sipwise']['bp_name']) &&  ($account->data['sipwise']['bp_name'] == $bp_name)) {
        if(user_delete($account->uid)=== FALSE){
			 drupal_set_message(t('Failed to remove the user account'), 'error');
		}
        if (ldap_delete($ldap_server->connection, $ad_entry['dn']) === FALSE) {
          watchdog('hpbx_users', 'Failed to remove user with username from AD: @username', array('@username' => $entries[0]['$ldap_server->user_attr'][0]));
          watchdog('hpbx_users', ldap_error($ldap_server->connection));
          drupal_set_message(t('Failed to remove the user account'), 'error');
        }  
      }     
    }
    
    $ActiveReseller->bp_name = $bp_name;	    	  	
    if($dn = hpbx_users_get_bp_name_basedn($ActiveReseller)){
	  if (ldap_delete($ldap_server->connection, $dn) === FALSE) {      	
        watchdog('hpbx_users', 'Failed to remove business partner from AD: @username', array('@username' => $bp_name));
        watchdog('hpbx_users', ldap_error($ldap_server->connection));            
        drupal_set_message(t('Failed to remove the user account'), 'error');        
      }
	}
	  // Remove the bp entry from the local database.
	try{
	  db_delete('hpbx_business_partners')
        ->condition('bp_name', $bp_name)
        ->execute();
              
      db_delete('hpbx_business_partner_settings')
        ->condition('bp_id', $bp_id)
        ->execute();
	  }catch (Exception $e) {    
        drupal_set_message(t('Failed to remove the user account'), 'error');   
         
      }   
      if($Customers = hpbx_customer_get_local($reseller_id, $bp[0]->bp_name)){
	    foreach ($Customers as $key => $value){
          if($Customer = SipwiseEntity::load('customers', $key)){
	  	    $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);
	  	    $CustomerContact->gpp9 = '';
	  	    $CustomerContact->save();
	      }     
        }
	  }
	  
	  hpbx_customer_search_empty($reseller_id);
	  
      drupal_set_message('Business Partner successfully removed');
      if(user_access('hpbx view reseller')){
	    $url = 'hpbx/affiliate/'.$reseller_id.'/bps';
	  }else{
	    $url = 'hpbx/bps';
	  }
      drupal_goto($url);  	       
           
  }   
}

function hpbx_bp_settings_edit_form($form, $form_state, $reseller_id = NULL, $bp_id = NULL){
    
  global $user;
 
  if(is_null($reseller_id)){
  	$reseller_id = sipwise_api_get_active_reseller();
  }
  if(is_null($bp_id)){
  	$bp_id = $user->data['sipwise']['bp_id'];
  }	
 
  $bp_settings = hpbx_business_partner_settings_get_local($bp_id);
  $bp = hpbx_business_partners_get_local($reseller_id, $bp_id);
  $bp_name = trim($bp[0]->bp_name);
  
  if(user_access('hpbx view reseller')){
  	$url = 'hpbx/affiliate/'.$reseller_id.'/bps';
  }elseif(user_access('hpbx edit own business partner settings')){
  	$url = 'hpbx/customers';
  }else{
  	$url = 'hpbx/bps';
  }
  
  
  $actions = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );
  $actions['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => $url,
    '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
    '#prefix' => '<div class="col-xs-12">',    
  );
  
  $actions['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span></div>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
  $form = array(
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  
  // Set the existing id.
  $form['reseller_id'] = array(
    '#type' => 'value',
    '#value' => $reseller_id,
  );
  
  $form['bp_id'] = array(
    '#type' => 'value',
    '#value' => $bp_id,
  );
  
   // Create fieldsets.
  
  $fieldsets = array(
    'general' => array('title' => t('General')),  
    'dashboard' => array('title' => t('Dashboard')),
    'mail' => array('title' => t('Email'))    
  ); 
  	
    
  // Create tabs.
  $items = array();
  foreach ($fieldsets as $key => $element) {
    if (user_access('hpbx edit '. $key .' business partner settings')) {
    
    	$data = '<div class="hpbx-tab-item-title">
                  <a href="#tab-'. strtolower(str_replace(' ', '-', $element['title'])) . '">'. $element['title'] .'</a>
              </div>';
	    $data .= '<div class="hpbx-tab-item-background"></div>';
  	  $item = array('data' => $data);
    	$items[] = $item;
    }
  }

  // This is to specify the vertical tab holder.
  // One page can have two vertical tabs, but it's always one main tab with sub tabs.
  $form['tabs'] = array(
    '#type' => 'markup',
    '#markup' => substr(str_replace('<div class="item-list">', '', theme('item_list', array('items' => $items))), 0, -6),
    '#prefix' => '<div class="hpbx-tabs">',
  );

  foreach ($fieldsets as $key => $fieldset) {
    $form[$key] = array(
      '#type' => 'container',
      '#tabform' => TRUE,
      '#title' => !empty($fieldset['title']) ? $fieldset['title'] : '',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#access' => user_access('hpbx edit '. $key .' business partner settings'),
      '#attributes' => array(
        'id' => 'tab-'. strtolower(str_replace(' ', '-', $fieldset['title'])),
      ),
    );
  }
 
  $form['general']['bp_name']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Business Partner Name'),
    '#default_value' => !empty($bp_name) ? $bp_name : '',
  );
  
  $form['bp_name_old'] = array(
    '#type' => 'value',
    '#value' => !empty($bp_name) ? $bp_name : '',
  );
    
  $form['general']['actions'] = $actions;
   
  $form['mail']['#theme_wrappers'] = array('hpbx_foldable_list', 'container');
  $form['mail']['#attributes']['class'] = array('hpbx-tab-form hpbx-tab-form-foldable-list');
  $form['mail']['#tabform'] = FALSE; 
  
  $form['mail']['general'] = array(
    '#type' => 'container',
    '#title' => t('Logo & Footer'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => TRUE,
  );
  
  if (!empty($bp_settings['logo_path'])) {
    $form['mail']['general']['logo_preview']['value'] = array(
      '#type' => 'markup',
      '#title' => '',
      '#markup' => theme('image', array('path' => $bp_settings['logo_path'])),
      '#title' => t('Logo preview'),
    );
    
    $form['logo_path'] = array(
      '#type' => 'value',
      '#value' => $bp_settings['logo_path'],
    );
  }
  
  
  $form['mail']['general']['logo']['value'] = array(
    '#type' => 'file',
    '#title' => t('Logo'),
    '#default_value' => '',
    '#description' => t('Upload an email logo. Allowed extensions: jpg, jpeg, png'),
  );
  
  
  $form['mail']['general']['mail_footer']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Footer'),
     '#default_value' => isset($bp_settings['mail_footer']) ? $bp_settings['mail_footer'] : '',
    '#required' => FALSE,
  ); 
 
  $mail_key = 'bp_customer_administrator_welcome_'. $reseller_id .'_'. $bp_id;
  $reseller_mail_key = 'customer_administrator_welcome_'. $reseller_id;
     
  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Business Partner Customer Administrator welcome email'),
  );
  
  
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => !is_null(variable_get('user_mail_'. $mail_key .'_subject'))? variable_get('user_mail_'. $mail_key .'_subject') : variable_get('user_mail_'. $reseller_mail_key .'_subject', 'New login credentials for [hpbx:company] in [hpbx:profile_set_name]'),
    '#required' => TRUE,
  );

  $body = <<<EOF
Hi [hpbx:firstname] [hpbx:lastname],
  
Welcome to [hpbx:profile_set_name]. At [hpbx:profile_set_name] as an administrator, you can manage your telephone system, create users and view other data from your [hpbx:profile_set_name] subscription.
  
Click this link to log in and choose a new password.
<small>(If the link does not work, you can also copy and paste it into your browser)</small>
  
[hpbx:one-time-login-url]
  
This link is valid for 1 day.
  
With kind regards,
  
[hpbx:profile_set_name] of [hpbx:lgi_brand_name]
EOF;
  
  
  $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => !is_null(variable_get('user_mail_'. $mail_key .'_body')) ? variable_get('user_mail_'. $mail_key .'_body') : variable_get('user_mail_'. $reseller_mail_key .'_body', $body) ,
    '#required' => TRUE,
    '#rows' => 17,
  );
  
  $mail_key = 'bp_subscriber_welcome_'. $reseller_id .'_'. $bp_id;
  $reseller_mail_key = 'customer_subscriber_welcome_'. $reseller_id;
  
  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Business Partner Subscriber welcome email'),
  );
  // Customer_subscriber_welcome subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => !is_null(variable_get('user_mail_'. $mail_key .'_subject'))? variable_get('user_mail_'. $mail_key .'_subject') : variable_get('user_mail_'. $reseller_mail_key .'_subject', 'New login credentials for [hpbx:firstname] [hpbx:lastname] in [hpbx:lgi_product_name]') ,
    '#required' => TRUE,
  );
  
  $body = <<<EOF
Hi [hpbx:firstname] [hpbx:lastname],
  
Welcome to [hpbx:profile_set_name]. At [hpbx:profile_set_name] you can view and manage your phone settings.
  
Click this link to log in and choose a new password.
<small>(If the link does not work, you can also copy and paste it into your browser)</small>
  
[hpbx:one-time-login-url]
  
This link is valid for 1 day.
  
With kind regards,
  
[hpbx:profile_set_name] of [hpbx:lgi_brand_name]
EOF;
  
  // Customer_subscriber_welcome body.
  $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => !is_null(variable_get('user_mail_'. $mail_key .'_body'))? variable_get('user_mail_'. $mail_key .'_body'):variable_get('user_mail_'. $reseller_mail_key .'_body', $body),
    '#required' => TRUE,
    '#rows' => 17,
  );

  
  $mail_key = 'bp_user_welcome_'. $reseller_id .'_'. $bp_id;
   
  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Business Partner User welcome email'),
  );
  // reset_password subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_subject', 'Login information for [hpbx:firstname] [hpbx:lastname] at [hpbx:lgi_brand_name]'),
    '#required' => TRUE,
  );

  $body = <<<EOF
[hpbx:firstname] [hpbx:lastname],

An account has been created for you by [hpbx:lgi_brand_name].
  
You may now log in by clicking this link or copying and pasting it to your browser:
<small>(If the link does not work, you can also copy and paste it into your browser)</small>
  
[hpbx:one-time-login-url]
  
This link can only be used once to log in and will lead you to a page where you can set your password. It expires after one day and nothing will happen if it's not used.
  
-- [hpbx:lgi_brand_name]
EOF;

  
  $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_body', $body),
    '#required' => TRUE,
    '#rows' => 17,
  );

   
  // Customer_subscriber_welcome body.
  $form['mail']['tokens']['value'] = array(
    '#type' => 'item',
    '#title' => t('Replacements'),
    '#markup' => theme('item_list', array('items' => array(
      '[hpbx:firstname]',
      '[hpbx:lastname]',
      '[hpbx:lgi_product_name]',
      '[hpbx:profile_set_name]',
      '[hpbx:lgi_brand_name]',
      '[hpbx:one-time-login-url]',
      '[hpbx:company]'
      )))
  );

  $form['mail']['actions'] = $actions;
  $form['mail']['actions']['submit']['#suffix'] = '<br/><br/>'; 
  
  $form['dashboard']['help_telnumber']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Phonenumber'),
    '#default_value' => isset($bp_settings['help_telnumber']) ? $bp_settings['help_telnumber'] : '',
    '#required' => TRUE,
    '#maxlength' => 15,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['dashboard']['help_website']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help website url'),
    '#default_value' => isset($bp_settings['help_website']) ? $bp_settings['help_website'] : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['dashboard']['help_mail']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help mail address'),
    '#default_value' => isset($bp_settings['help_mail']) ? $bp_settings['help_mail'] : '',
    '#required' => TRUE,
    '#rules' => array(
      'email'
    ),
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['dashboard']['help_name']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help name'),
    '#default_value' => isset($bp_settings['help_name']) ? $bp_settings['help_name'] : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['dashboard']['actions'] = $actions;
  
  return $form;
}

function hpbx_bp_settings_edit_form_submit($form, &$form_state){
	
  $bp_id = $form_state['values']['bp_id'];
  $reseller_id = $form_state['values']['reseller_id'];
  
  if(!empty($form_state['values']['bp_name_old']) && ($form_state['values']['bp_name_old'] != $form_state['values']['general']['bp_name']['value'])){
  	if(!hpbx_change_business_partner_name($reseller_id, $bp_id, $form_state['values']['bp_name_old'], $form_state['values']['general']['bp_name']['value'])){
      drupal_set_message('Failed to update the business partner name', 'error');
      return;
	}
  }
    
  $settings = array();
  
  $settings['logo_path'] = $form_state['values']['logo_path'];
  $settings['mail_footer'] = $form_state['values']['mail']['general']['mail_footer']['value'];	
  
  foreach ($form_state['values']['dashboard'] as $key => $field) {
    $settings[$key] = $field['value'];
  } 
  
  if (!empty($_FILES['files']['name']['mail']) && !$_FILES['files']['name']['error']) {
    $validators = array(
      'file_validate_is_image' => array(),
      'file_validate_extensions' => array('jpg jpeg png'),
    );
  
    $destination = variable_get('file_public_path', conf_path() . '/files');
    if ($file = file_save_upload('mail', $validators, 'public://', FILE_EXISTS_REPLACE)) {
      $file->status = 1;
      file_save($file);
      $settings['logo_path'] = $file->destination; 
    }
  }
     
  $settings = serialize($settings);
  $datetime = date("Y-m-d H:i:s");  
 
  try {
    $query = db_merge('hpbx_business_partner_settings')
      ->key(array(
        'reseller_id' => $reseller_id,
        'bp_id' => $bp_id
      ))
      ->fields(array(
        'bp_id' => $bp_id, 
        'reseller_id'=> $reseller_id, 
        'settings' => $settings, 
        'update_date' => $datetime        
      ));

    $query->execute();
  }
  catch (Exception $e) {
      watchdog('hpbx_business_partner_settings', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
      drupal_set_message('Failed to update the business partner settings');
  }
  
  foreach (array('bp_customer_administrator_welcome', 'bp_subscriber_welcome', 'bp_user_welcome') as $mail_key) {
    $mail_key .= '_'. $reseller_id .'_' . $bp_id;
    variable_set('user_mail_'. $mail_key .'_subject', $form_state['values']['mail']['user_mail_'. $mail_key]['subject']['value']);
    variable_set('user_mail_'. $mail_key .'_body', $form_state['values']['mail']['user_mail_'. $mail_key]['body']['value']);
  }
  
  drupal_set_message('Business Partner settings successfully updated');
}

function hpbx_business_partner_settings_get_local($bp_id){
	
  try {
    // Search for company name and external id.
    $query = db_select('hpbx_business_partner_settings', 'bps')
      ->fields('bps')
      ->condition('bp_id', $bp_id);

    $result = $query->execute()->fetchObject();
    
    return unserialize($result->settings);    
  }
  catch (Exception $e) {
    watchdog('hpbx_business_partner_settings', $e->getMessage());
    return FALSE;
  }
}

function hpbx_bp_user_delete_page($reseller_id = NULL, $username = '', $bp_id = NULL){
  $ldap_server = hpbx_users_get_ldap_server();

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();
  }

  $account = NULL;  
  $ldap_server = hpbx_users_get_ldap_server();
  
  if ($ad_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '=*)(|(memberof=CN=HostedPBX Business Partner Administrator,OU=Groups,DC=a, DC=local)))','DC=a, DC=local')) {
  	unset($ad_entries['count']);
  	foreach ($ad_entries as $ad_entry){
  	  if($account = user_load_by_name($ad_entry[$ldap_server->user_attr][0])){
	    if(($account->data['sipwise']['reseller_id']==$reseller_id) && ($account->data['sipwise']['bp_id']==$bp_id)){
		  $bp_admin[]=$ad_entry[$ldap_server->user_attr][0];
		}
	  }		
	}	
	if((count($bp_admin)==1) && ($bp_admin[0] == $username)){
		drupal_set_message('Not allowed to delete the last administrator','error');
		
	}else{
	
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
    }
    if(user_access('hpbx edit own business partner users')){
		$url = 'hpbx/bp/users';
	}elseif(user_access('hpbx view reseller')){
		$url = 'hpbx/affiliate/'. $reseller_id .'/bp/'.$bp_id.'/users';
	}else{
		$url = 'hpbx/bp/'.$bp_id.'/users';
	}    
    drupal_goto($url); 
  }
  
   
}

function hpbx_change_business_partner_name($reseller_id, $bp_id, $bp_name_old, $bp_name_new){

  if (!empty($reseller_id)) {
    // Temporary set the reseller_id.
    sipwise_api_activate_reseller($reseller_id);
  }
  
  global $ActiveReseller;
  	
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.customer.inc');
  
  try {
    $query = db_merge('hpbx_business_partners')
      ->key(array(        
        'bp_id' => $bp_id
      ))
      ->fields(array(        
        'bp_name'=> $bp_name_new,                
      ))
      ->condition(db_and()->condition('bp_id', $bp_id)->condition('bp_name', $bp_name_old));

    $query->execute();
  }
  catch (Exception $e) {
      watchdog('hpbx_business_partner_settings', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
      drupal_set_message('Failed to update the business partner name');
      return FALSE;
  }
  $ldap_server = hpbx_users_get_ldap_server();  

  if ($ad_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&('. $ldap_server->user_attr. '=*)(|(memberof=CN=HostedPBX Business Partner Administrator,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Support,OU=Groups,DC=a, DC=local)(memberof=CN=HostedPBX Business Partner Customer Manager,OU=Groups,DC=a, DC=local)))','DC=a, DC=local')) {
  	
  	unset ($ad_entries['count']);
  	
    foreach ($ad_entries as $ad_entry) {
   	  $account = user_load_by_name($ad_entry[$ldap_server->user_attr][0]);   	  
   	   if (!empty($account->data['sipwise']['reseller_id']) &&  ($account->data['sipwise']['reseller_id']==$reseller_id)){
   	   	if (!empty($account->data['sipwise']['bp_id']) &&  ($account->data['sipwise']['bp_id']== $bp_id)) {
   	   		$account->data['sipwise']['bp_name'] = $bp_name_new;
			if(!$account = user_save($account)){
				drupal_set_message('Failed to update the business partner name', 'error');
				return FALSE;
			}	
        
      }
	 }
	}		 
  }   
    
  if($Customers = hpbx_customer_get_local($reseller_id, $bp_name_old)){
    foreach ($Customers as $key => $value){
      if($Customer = SipwiseEntity::load('customers', $key)){
	  	$CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);
	  	$CustomerContact->gpp9 = $bp_name_new;
	  	$CustomerContact->save();
	  	hpbx_bp_name_update_customers_local($key, $reseller_id, $bp_name_old, $bp_name_new);
	  }     
    }    
  }  
  if (!hpbx_user_rename_bp_organizational_unit($reseller_id, $bp_name_old, $bp_name_new)) {
        drupal_set_message(t('Failed to change the business partner name'));
    }
  return TRUE;
}

function hpbx_bp_name_update_customers_local($cutomer_id = NULL, $reseller_id = NULL, $bp_name_old = NULL, $bp_name_new = NULL){
	require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.customer.inc');
	if($Customers = hpbx_customer_get_local($reseller_id, $bp_name_old)){
		try {
          $query = db_merge('hpbx_customers')
          ->key(array(        
          'customer_id' => $cutomer_id
          ))
          ->fields(array(        
          'bp_name'=> $bp_name_new,              
          ))
          ->condition(db_and()->condition('customer_id', $cutomer_id)->condition('bp_name', $bp_name_old));
          $query->execute();
        }
        catch (Exception $e) {
          watchdog('hpbx_business_partner_settings', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
          drupal_set_message('Failed to update the business partner name');
          return FALSE;
       }
	}
}

