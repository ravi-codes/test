<?php
function hpbx_sipwise_script_form($form, &$form_state) {
drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer')); 
  $script_options = array(
    'webpassword_reset' => 'Webpassword Reset', 
	'softphone_enable' => 'Softphone Enable',
	'display_name_update' => 'Display Name Update',
	'reseller_gpp_update' => 'Reseller GPP Fields Update',
	'customer_gpp_update' => 'Customer GPP Fields Update',
	'panasonic_rds_update' => 'Panasonic RDS Prov URL Update',
	'yealink_rds_update' => 'Yealink RDS Prov URL Update'		
  );
  
  $form = array(
    '#prefix' => '<div class="hpbx-main-content-inner hpbx-default-forward">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  $form['general']['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#title' => t('Sipwise Scripts'),
  );
	
  $script_default = isset($form_state['values']['script_name']) ? $form_state['values']['script_name'] : '';

  $form['general']['script_name'] = array(
    '#type' => 'select',
    '#title' => t('Script Name'),
    '#options' => $script_options,
	'#required' => TRUE,
    '#default_value' => $reseller_default,
    '#empty_option' => t('Choose an option'),	
	'#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),	
  );
  
  $options2 = array();
  $reseller_options = hpbx_reseller_options();
  foreach ($reseller_options as $key => $value){
    $query = db_select('hpbx_customers', 'c')
      ->fields('c', array('reseller_id', 'customer_id', 'external_id', 'company'))
      ->condition('reseller_id', $key);
     
	$result = $query->execute();

    $customers =  $result->fetchAllAssoc('customer_id');  

    foreach ($customers as $cust) { 
      $cus = array(
		  'value' => $cust->external_id,
		  'label' => $cust->company.'['.$cust->external_id.']',
		);
	  $reseller = SipwiseEntity::load('resellers', $cust->reseller_id);	
      $Customers[$reseller->id.'['.$reseller->name.']'][] = $cus;		
      $options2[$cust->external_id] = $cust->company .'['.$cust->external_id.']' ;
    } 
  }  	
  
  $resellers = array_keys($Customers);
  ksort($resellers);

  $options = array();
  foreach ($resellers as $reseller) {
    ksort($Customers[$reseller]);
    $options[$reseller] = array(
      'label' => $reseller,
      'types' => $Customers[$reseller]
    );
  }  
  
  $reseller_default = isset($form_state['values']['reseller']) ? $form_state['values']['reseller'] : '';

  $form['general']['reseller'] = array(
    '#type' => 'select',
    '#title' => t('Reseller'),
    '#options' => drupal_map_assoc($resellers),
    '#default_value' => $reseller_default,				
    '#empty_option' => t('Choose an option'),	
   	'#attributes' => array(   	  
      'class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-placeholder' => t('Choose an option'),
      'data-custom-change-event' => 'edit-reseller-id',
      'data-device-type-target' => '#edit-cust-external-id',
      'data-device-list' => json_encode($options), 
      'data-selected-value' => $reseller_default ,
    ),		
  );
  
  $form['general']['cust_external_id'] = array(
    '#type' => 'select',
    '#options' => $options2,
    '#header' =>  array(t('Customer')),    
    '#empty_option' => t('All Customers'),    
    '#title' => t('Customer'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'),      
      'data-custom-change-event' => 'edit-cust-external-id',
      'data-selected-value' =>  isset($form_state['values']['cust_external_id']) ? $form_state['values']['cust_external_id'] : '',
    ),
  );
  
  $form['general']['provision_url'] = array(
    '#type' => 'textfield',    
    '#maxlength' => 512,
    //'#required' => TRUE,
    '#title' => t('New Provision URL'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Provisioning URL')),
  );
  $form['general']['provision_server'] = array(
    '#type' => 'textfield',    
    '#maxlength' => 512,
    //'#required' => TRUE,
    '#title' => t('New Provision Server'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Provisioning Server')),
  );
  $options3 = array('panasonic_dev'=>'Panasonic Test RDS', 'panasonic_prod'=>'Panasonic Prod RDS');
  $form['general']['panasonic_rds'] = array(
    '#type' => 'select',
    '#options' => $options3,
    '#header' =>  array(t('Panasonic RDS')),    
    '#empty_option' => t('Choose an option'),    
    '#title' => t('Panasonic RDS'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  ); 

  $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>'
  );
  
  $form['general']['actions']['submit'] = array(
	'#type' => 'submit',
	'#value' => t('Run'),
	'#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
return $form;
}

function hpbx_sipwise_script_form_validate($form, &$form_state){
	$script_name = trim($form_state['values']['script_name']);
	if(!empty($script_name)){
	  if($script_name != 'panasonic_rds_update' & $script_name != 'yealink_rds_update'){
	    if(empty($form_state['values']['reseller'])){
			form_set_error('reseller', t('Reseller field is required'));
		}
	  }
	  if($script_name == 'panasonic_rds_update'){
	  	if(empty($form_state['values']['provision_url'])){
			form_set_error('provision_url', t('New Provisiong URL field is required'));
		}
		if(empty($form_state['values']['panasonic_rds'])){
			form_set_error('panasonic_rds', t('Select Panasonic RDS'));
		}
	  }
	  if($script_name == 'yealink_rds_update')	{
	  	if(empty($form_state['values']['provision_server'])){
			form_set_error('provision_server', t('New Provisiong Server field is required'));
		}
	  }	  
	}	  
}

function hpbx_sipwise_script_form_submit($form, &$form_state, $customer_id = NULL ){  
  global $ActiveReseller;
  $reseller_id = "";
  $customer_id = ""; 
  $subscribers = "";
  $script_name = trim($form_state['values']['script_name']);
  if(!empty($form_state['values']['reseller'])){
  	$reseller = explode("[",$form_state['values']['reseller']);
    $reseller_id = trim($reseller[0]);
  }   
  $customer_id = trim($form_state['values']['cust_external_id']);
  $provision_url = trim($form_state['values']['provision_url']);
  $provision_server = trim($form_state['values']['provision_server']);
  $panasonic_rds = trim($form_state['values']['panasonic_rds']);
  
  if(($script_name != 'panasonic_rds_update') && ($script_name != 'yealink_rds_update')){  	
  	if (!is_null($reseller_id)) {
      sipwise_api_activate_reseller($reseller_id);
    }
    else {
      $reseller_id = $ActiveReseller->id;
    }  
    $query['reseller_id'] = $reseller_id;
    
    if(!empty($customer_id)){
  	  if(($script_name == 'customer_gpp_update') || ($script_name == 'display_name_update')){
	    $query['external_id'] = $customer_id;	
	  }else{
	    $query['customer_external_id'] = $customer_id;	
	  }
    }
  
    if(($script_name == 'customer_gpp_update') || ($script_name == 'display_name_update')){
  	  $customers = sipwise_api_get_all('customers', $query);
    }else{
  	  $subscribers = sipwise_api_get_all('subscribers', $query);
    }  
  }  
 
  //Modified for #9760 - PE DEV: Migration script display name not working when applied for all customers of a reseller
  
  
  switch ($script_name) {
    case 'webpassword_reset':
      webpassword_reset($subscribers);
	  $message = t('Webpassword has been successfully updated');
      break;
    case 'softphone_enable':    
      sofphone_prov_enable($subscribers);
	  $message = t('Softphone auto provision option has been successfully updated');
      break;
	case 'display_name_update':    
      display_name_update($customers);
	  $message = t('Display name has been successfully updated');
      break;
    case 'reseller_gpp_update':    
      reseller_gpp_update($reseller_id);
	  $message = t('Reseller settings fields successfully updated');
      break;
    case 'customer_gpp_update':
      customer_gpp_update($customers);
      $message = t('Customer Settings fields successfully updated');
      break;
    case 'panasonic_rds_update':
      panasonic_rds_update($provision_url,$panasonic_rds);
      $message = t('Provisioning URL for Panasonic successfully updated');
      break;
    case 'yealink_rds_update':
      yealink_rds_update($provision_server);
      $message = t('Provisioning URL for Yealink successfully updated');
      break;    	
  }   
 drupal_set_message($message); 
}

function hpbx_reseller_options() {   
  $options = array(); 
  foreach (hpbx_get_resellers() as $Reseller) {	 	
    $options[$Reseller->id] = t($Reseller->name);	 
  }
  return $options;
}

//Update the webpassword for regular subscribers (non pilot,non HG/AA/Fax/CR)
function webpassword_reset ($subscribers = NULL){  
  foreach ($subscribers as $sub){    
    if($sub->is_regular()){
	  $Subscriber = SipwiseEntity::load('subscribers', $sub->id);
	  $rnd_id_length = 10; 
	  $rnd_id = crypt(uniqid(rand(),1),''); 
	  $rnd_id = strip_tags(stripslashes($rnd_id)); 
	  $rnd_id = str_replace(".","",$rnd_id); 
	  $rnd_id = strrev(str_replace("/","",$rnd_id)); 
	  $rnd_id = strtolower(substr($rnd_id,0,$rnd_id_length));  
	  $Subscriber->webpassword = $rnd_id;
	  $Subscriber->save();
    }	
  } 
}

//Update the softphone provisioning dependent on the subscriber profile
function sofphone_prov_enable($subscribers = NULL){
  foreach ($subscribers as $sub){
  	if($sub->is_regular() && $sub->profile_id != "" && $sub->profile_set_id != '' ){
	  $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $sub->id);
      if (hpbx_attribute_allowed('softphone_autoprov', $sub->profile_id, $sub->profile_set_id) == TRUE){     
        $SubscriberPreferences->softphone_autoprov = TRUE;       
      }
      else {
        $SubscriberPreferences->softphone_autoprov = FALSE;
      }
	  $SubscriberPreferences->save();
	}    
  } 
}

//Update the display name for non pilot subscribers fetching it from AD
function display_name_update($customers = NULL){
  foreach ($customers as $customer){
    foreach (sipwise_api_get_all('subscribers', array('customer_id' => $customer->id)) as $Subscriber){
  	  if($Subscriber->is_pbx_pilot==FALSE && (($Subscriber->webusername != "") || !is_null($Subscriber->webusername))){  			   		  
  	    if(!$ldap = hpbx_users_get_user($Subscriber->webusername)) continue;
  		$Sub = SipwiseEntity::load('subscribers', $Subscriber->id);
		if($ldap['sn'][0] != "" && $ldap['givenname'][0] != ""){		    
	      $Sub->display_name = $ldap['sn'][0].", ".$ldap['givenname'][0];
	      $Sub->external_id = "";
	    }elseif ($Sub->external_id != ""){
	      $Sub->display_name = $Sub->external_id;
	    }
	    $Sub->save(); 		  		  
  	  }  	    		
    }  	
  }   
}

//Update the compressed data to Reseller GPP fields 
function reseller_gpp_update($reseller_id = NULL){
  if(($reseller_details = SipwiseEntity::load('resellers', $reseller_id)) && ($Contract = SipwiseEntity::load('contracts', $reseller_details->contract_id))){
  	if ($SystemContact = SipwiseEntity::load('systemcontacts', $Contract->contact_id )) {          
      if(!$settings = SipwiseEntity::get_settings_from_gpp_chunks_for_migration($SystemContact)){
	  	return;
	  }
      if(SipwiseEntity::set_compressed_gpp_chunks($SystemContact,$settings)){
        $SystemContact->save();      	
	  }
    }
  } 
}

//Update the compressed data to Customer GPP fields
function customer_gpp_update($customers = NULL){  
  foreach($customers as $cust){
      $CustomerContact = SipwiseEntity::load('customercontacts', $cust->contact_id);
      if(!$settings = SipwiseEntity::get_settings_from_gpp_chunks_for_migration($CustomerContact)) continue;
      if(SipwiseEntity::set_compressed_gpp_chunks($CustomerContact,$settings)){
	  	$CustomerContact->save(); 
	  }             
  } 
}

function panasonic_rds_update($provion_url = NULL, $panasonic_rds){  
  global $conf;  
  $key = array_keys($conf['sipwise'])[2];
  if(!empty($key) && is_numeric($key)){
    sipwise_api_activate_reseller($key);	
  }  
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');
  if ($pbxdevices = sipwise_api_get_all('pbxdevices')) {	 
    foreach ($pbxdevices as $pbxdevice){    	
      $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($pbxdevice->profile_id);
      if(!empty($PBXDeviceModel->bootstrap_method) && $PBXDeviceModel->bootstrap_method == 'redirect_panasonic') {      	
      	$pbxds[] = $pbxdevice->profile_id;	    
      }
	}		
  }
  
  foreach($pbxds as $pbxd){
	foreach(sipwise_api_get_all('pbxdevices', array('profile_id'=>$pbxd)) as $pbxdev){		
	  $PBXDeviceMod = hpbx_pbxdevice_get_model_on_profile($pbxdev->profile_id);
      if (!empty($PBXDeviceMod->bootstrap_method) && $PBXDeviceMod->bootstrap_method == 'redirect_panasonic') {          	
        require_once(drupal_get_path('module', 'hpbx_rps') .'/includes/rps.class.inc');        
        list($f, $class) = explode('_', $PBXDeviceMod->bootstrap_method); 
        
		$class_path = drupal_get_path('module', 'hpbx_rps') .'/includes/'. strtolower($class) .'.class.inc';       
        
        if (file_exists($class_path)) {
          require_once($class_path);
          $max_version = hpbx_device_get_max_firmware_version($pbxdev->profile_id);          
          if (!empty($max_version)) {
            $args = array($pbxdev->identifier, $provion_url, $max_version, '');
            if(!$result = panasonic_rds_call('registerPhone2', $args, $panasonic_rds)) continue;
		  }		  
        }
      }
	}
  }	
}

function yealink_rds_update($provion_server = NULL){
  global $conf;
  
  $key = array_keys($conf['sipwise'])[2];
  if(!empty($key) && is_numeric($key)){
    sipwise_api_activate_reseller($key);	
  }
   
  $args = array(); 
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');
  if ($pbxdevices = sipwise_api_get_all('pbxdevices')) {	 
    foreach ($pbxdevices as $pbxdevice){    	
      $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($pbxdevice->profile_id);
      if (!empty($PBXDeviceModel->bootstrap_method) && $PBXDeviceModel->bootstrap_method == 'redirect_yealink') {      	
      	$pbxds[] = $pbxdevice->profile_id;   
      }
	}		
  }
     
  foreach($pbxds as $pbxd){
	foreach(sipwise_api_get_all('pbxdevices', array('profile_id'=>$pbxd)) as $pbxdev){	  
	  $PBXDeviceMod = hpbx_pbxdevice_get_model_on_profile($pbxdev->profile_id);
      if (!empty($PBXDeviceMod->bootstrap_method) && $PBXDeviceMod->bootstrap_method == 'redirect_yealink') {          	
        require_once(drupal_get_path('module', 'hpbx_rps') .'/includes/rps.class.inc');        
        list($f, $class) = explode('_', $PBXDeviceMod->bootstrap_method); 
        
		$class_path = drupal_get_path('module', 'hpbx_rps') .'/includes/'. strtolower($class) .'.class.inc';       
        
        if (file_exists($class_path)) {
          require_once($class_path);
          $args = array($pbxdev->identifier, $provion_server);          	        	
          if(!$result = yealink_rds_call('editDevice', $args)) continue;		  
        }
      }
    }
  } 
}

function panasonic_rds_call($method, $args, $panasonic_rds){
  global $conf;
  
  $request = xmlrpc_encode_request('ipredirect.'. $method, $args);
  $header = array(
    'Content-Type: text/xml',
    'Authorization: Basic '. base64_encode($conf['hpbx_rps'][$panasonic_rds]['auth']['username'] . ':'. $conf['hpbx_rps'][$panasonic_rds]['auth']['password']),
    'Content-length: '. strlen($request),
  );

  if (is_null($ch)) {
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_URL, $conf['hpbx_rps']['panasonic']['endpoint']);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
  }

  curl_setopt($ch, CURLOPT_HTTPHEADER, $header);
  curl_setopt($ch, CURLOPT_POSTFIELDS, $request);

  $data = curl_exec($ch);
  if (curl_errno($ch)) {
    throw new Exception(curl_error($ch));
  }

  $response = xmlrpc_decode($data);
  return $response;  	
}


function yealink_rds_call($method, $args){
  global $conf;
  $request = xmlrpc_encode_request('redirect.'. $method, $args);
  $header = array(
    'Content-Type: text/xml',
    'Authorization: Basic '. base64_encode($conf['hpbx_rps']['yealink']['auth']['username'] . ':'. $conf['hpbx_rps']['yealink']['auth']['password']),
    'Content-length: '. strlen($request),
  );

  if (is_null($ch)) {
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_URL, $conf['hpbx_rps']['yealink']['endpoint']);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
  }

  curl_setopt($ch, CURLOPT_HTTPHEADER, $header);
  curl_setopt($ch, CURLOPT_POSTFIELDS, $request);

  $data = curl_exec($ch);
  if (curl_errno($ch)) {
    throw new Exception(curl_error($ch));
  }

  $response = xmlrpc_decode($data);
  return $response;
}