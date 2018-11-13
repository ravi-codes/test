<?php

/**
 * @param int $subscriber_id
 */
function hpbx_conference_delete_page($subscriber_id) {

  hpbx_subscriber_delete($subscriber_id);
  if ($Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {
    if (!is_null($Subscriber)) {
      $alias_number_original = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
    }
    $Customer = hpbx_get_active_customer();
    $pilot_sub = $Customer->get_pilot_subscriber(); 
    if (!empty($alias_number_original)) {
      $pilot_sub->alias_number_set($Customer, $alias_number_original, FALSE);
      $pilot_sub->save();
    }else{ 
      $sub_alias = $Subscriber->alias_numbers;
      $alias_no = json_decode(json_encode($sub_alias[0]));
      $number = (array)$alias_no;
      foreach ($pilot_sub->alias_numbers as $p_key => $p_an) { 
        if ($p_an->cc == $number['cc'] && $p_an->ac == $number['ac'] && $p_an->sn == $number['sn']) { 
          unset($pilot_sub->alias_numbers[$p_key]);
          $pilot_sub->save();
        }
      }
    }
  }
  drupal_goto('hpbx/conference-rooms');
}

/**
 * @param array $form
 * @param array$form_state
 * @param int $subscriber_id
 *
 * @return array
 */
function hpbx_conference_edit_form($form, &$form_state, $subscriber_id = NULL) {

  global $ActiveReseller;  
 
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));
  
  $Customer = hpbx_get_active_customer();
  
  //Modified for #9730 - BUILD: Multi-domain support
  $PilotSubscriber = $Customer->get_pilot_subscriber();
  $Domain = SipwiseEntity::load('domains', $PilotSubscriber->domain_id);
  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);

  // Request Customer values.
  $Subscriber = NULL;

  // Verify if the customer_id is passed.
  if (!is_null($subscriber_id)) {
    if ($Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {
      
      // Verify if the subscriber to be deleted is owned by the active customer.
      if ($Customer->id != $Subscriber->customer_id) {
        drupal_not_found();
        exit();
      }
      
      $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
      
      if ($Subscriber->profile_id == $ActiveReseller->_settings->conference_room_subscriber_profile_id) {
        
        // Set title.
        drupal_set_title(t('!external_id', array('!external_id' => hpbx_truncate_name_element($Subscriber->external_id, 40))));
      }
      else {
        drupal_not_found();
        exit();
      }
    }
    else {
      drupal_not_found();
      exit();
    }
  }
  else {

    // Set title.
    drupal_set_title(t('Create Conference room'));
  }

  // Init form.
  $form = array('#attributes' => array('class' => array('hpbx-main-content-inner')));
  $form['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#title' => t('Settings'),
    '#tabform' => TRUE,
  );

  // Set the customer id.
  $form['customer_id'] = array(
    '#type' => 'value',
    '#value' => !empty($Subscriber->customer_id) ? $Subscriber->customer_id : $Customer->id,
  );

  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Subscriber->id) ? $Subscriber->id : FALSE,
  );

  // Name.
  $form['table']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($Subscriber->external_id) ? $Subscriber->external_id : NULL,
    '#size' => 30,
    '#required' => TRUE,
    '#title' => t('Name'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Name')),
  );
  
  $multisite = json_decode(json_encode($Customer->_settings->multisite),true);
  $site = array();
  foreach($multisite as $site){
    $sites[$site['site_name']] = $site['site_name'];
  }
  asort($sites);

  if($Customer->_settings->multi_site){
    $form['table']['site']['value'] = array(
      '#required' => TRUE,
      '#type' => 'select',
      '#options' => $sites,
      '#access' => user_access('hpbx edit subscriber'),
      '#default_value' => !empty($SubscriberPreferences->gpp9) ? $SubscriberPreferences->gpp9 : NULL,
      '#title' => t('Site*'),
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element', 'no-search'),
      )
    );
  }

  hpbx_subscriber_number_extension_elements($form, $form_state, $Customer, $Subscriber, 'table');
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
   // Set default value based on cli setting.
    if(!isset($Subscriber->id) || isset($SubscriberPreferences->cli) && $SubscriberPreferences->cli == $ActiveReseller->_settings->country_code . substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls))){
      $default_value = 'default';
    }
    elseif (isset($SubscriberPreferences->cli)) {
      $default_value =  $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->cli, strlen($ActiveReseller->_settings->country_code), -($Customer->_settings->extension_length));
    }
    else {
      $default_value = FALSE;
    }
  	$subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
   	if(isset($Subscriber) && (($form_state['input']['table']['alias_number']['value']=='0') || (!isset($form_state['input']['table']['alias_number']['value']) && empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN))))){
      $subscriber_options = $subscriber_options;	
   }else {
      $subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
   }
    	   
   	  // CLI.
	  $form['table']['cli']['value'] = array(
	    '#type' => 'select',
	    '#options' => $subscriber_options,
	    '#default_value' => $default_value,
	    '#required' => TRUE,
	  	'#prefix' => '<span id="caller_line_identification">',
	    '#title' => t('Caller Line Identification'),
	    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
	    '#access' => hpbx_attribute_allowed('cli'),
	    '#suffix' => '</span>'
	  );
   }

  $form['table']['conference_pin']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($SubscriberPreferences->conference_pin) ? $SubscriberPreferences->conference_pin : NULL,
    '#maxlength' => 5,
    '#title' => t('PIN'),
    '#description' => t('If you enter a PIN, it must consist of 5 digits'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('PIN')),
  );
  
  $announcement_soundsets = array();
  if ($soundsets = sipwise_api_get_all('soundsets', array('customer_id' => 'NULL'))) {
    foreach ($soundsets as $soundset) {
      $announcement_soundsets[$soundset->name] = t($soundset->name);
    }
  }
  
  // Sort options list.
  asort($announcement_soundsets);
  
  // Announcement language.
  $form['table']['sound_set']['value'] = array(
    '#required' => FALSE,
    '#type' => 'select',
    '#title' => t('Announcement language'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
    '#options' => $announcement_soundsets,
    '#default_value' => $SubscriberPreferences->sound_set,
  );

  // Submit button.
  $form['table']['actions'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',
  );
  $form['table']['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'hpbx/conference-rooms',
      '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
      '#prefix' => '<div class="col-xs-12">',
  );
  $form['table']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#prefix' => '<span class="submit-button">',
      '#suffix' => '</span></div>',
      '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  return $form;
}

/**
 * @param array $form
 * @param array$form_state
 * @param int $customer_id
 *
 * @return array
 */
function hpbx_conferences_overview_form($form, $form_state, $customer_id = NULL) {

  global $user, $ActiveReseller;
  
  $Customer = hpbx_get_active_customer($customer_id);

  // Set title.
  drupal_set_title(t('Conference rooms'));

  // Add css.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');
  
  // Init form.
  $form = array();

  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create conference room') . '</h5>';
  $add = l($text, 'hpbx/conference-room/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form['table'] = array(
    '#add' => array('data' => $add, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-huntgroups hpbx-table-sortable'),
    '#header' => array(
      'external_id' => array('data'=> t('Name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'number' => array('data'=> t('Number'), 'class' => array('hpbx-sortable', 'hpbx-show')),
    ),
    '#remove_title' => t('Delete conference room'),
  );

  if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
    $form['table']['#header']['pbx_extension'] = array('data'=> t('Extension'), 'class' => array('hpbx-sortable', 'hpbx-show-sm'));
  }

  // Ignore pbx groups.
  //$query = array('is_pbx_group' => 1);

  if (!is_null($Customer->id)) {
    // Set customer_id as query.
    $query['customer_id'] = $Customer->id;
  }

  // Query subscribers.
  foreach (sipwise_api_get_all('subscribers', $query) as $Subscriber) {
    if (!empty($Subscriber->external_id) && $Subscriber->profile_id == $ActiveReseller->_settings->conference_room_subscriber_profile_id) {

      $form['table'][$Subscriber->id]['#url'] = 'conference-room/'. $Subscriber->id . '/edit';
      $form['table'][$Subscriber->id]['#attributes']['data-delete-url'] = 'conference-room/'. $Subscriber->id . '/delete';
      
      $form['table'][$Subscriber->id]['external_id']['data'] = array('#markup' => hpbx_truncate_name_element($Subscriber->external_id), '#attributes' => array('title' => check_plain($Subscriber->external_id)));
      $form['table'][$Subscriber->id]['external_id']['#attributes']['class'] = array('hpbx-show');

      if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
        $form['table'][$Subscriber->id]['number']['data'] = array('#markup' =>   $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN));
        $form['table'][$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show');
      
        $form['table'][$Subscriber->id]['pbx_extension']['data'] = array('#markup' =>   !empty($Subscriber->pbx_extension) ? $Subscriber->pbx_extension : '');
        $form['table'][$Subscriber->id]['pbx_extension']['#attributes']['class'] = array('hpbx-show-sm');
      }
      else {
      
        $number = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
        if(isset($number) && $number !=''){
          $number = substr($number, 0, $Customer->_settings->extension_length * -1). '<span class="hpbx-pbx-extension" >'. $Subscriber->pbx_extension . '</span>';
        }else{
			    $number = "<span style='padding-left:30px'>&nbsp</span>" . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
		    }
        $form['table'][$Subscriber->id]['number']['data'] = array('#markup' =>  $number);
        $form['table'][$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show');
      }
    }

  }
  return $form;
}

/**
 * Form validate handler for hpbx_conferences_overview_form().
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_conference_edit_form_validate($form, $form_state) {

  global $ActiveReseller;

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  if (!empty($form_state['values']['customer_id'])) {
    $customer_id = $form_state['values']['customer_id'];
  }
  else {
    drupal_set_message(t('Can\'t find the customer ID'), 'error');
    return;
  }

  if (!empty($form_state['values']['table']['conference_pin']['value']) && !preg_match('/^\d{5}$/', $form_state['values']['table']['conference_pin']['value'])) {
    form_set_error('table][conference_pin][value', t('PIN needs to be exactly 5 digits.'));  
  }
  
  if (!$Customer = SipwiseEntity::load('customers', $customer_id)) {
    drupal_set_message(t('Can\'t find customer'));
    return;
  }

  //9726 - Create/update HG/AA/CR - Not allow semi colons (;) in Name field
  if(preg_match('/[;]/', $form_state['values']['table']['external_id']['value'])){
	  form_set_error('table][external_id][value', t('Semicolon is not allowed in !name.', array('!name' => t('Name'))));
  }

  $Subscriber = NULL;

  // Get the sound file for this autoattendant subscriber.
  if (isset($form_state['values']['id'])) {
    $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
  }
  
  // Validate if the external_id is unique for this customer.
  if (hpbx_external_id_exists($Customer, $form_state['values']['table']['external_id']['value'], $Subscriber)) {
    form_set_error('table][external_id][value', t('Name must be unique'));
  }
  
  // Get all pbx_extensions.
  $extensions = hpbx_get_extensions($Customer);

  if ($form_state['values']['id']) {
    unset($extensions[$form_state['values']['id']]);
  }
  
  if(!is_numeric($form_state['input']['table']['alias_number']['value'])){
    form_set_error('table][alias_number][value', t('Number field is required'));
  }
  

  if (!$form_state['values']['id'] &&
    hpbx_bundle_settings_is_allowed($Customer, $ActiveReseller->_settings->conference_room_subscriber_profile_id, HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE_SET|HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE)<=0) {

    $SubscriberProfile = SipwiseEntity::load('subscriberprofiles', $ActiveReseller->_settings->conference_room_subscriber_profile_id);
    form_set_error('', t('You have used up all your available @profile_names', array(
      '@profile_name' => drupal_strtolower($SubscriberProfile->name))
    ));
  }

  if (!empty($form_state['values']['table']['pbx_extension']['value']) &&
      in_array($form_state['values']['table']['pbx_extension']['value'], $extensions)) {
    form_set_error('table][pbx_extension', t('Extension is already in use. Please select another extension.'));
  }

  if ($form_state['values']['table']['pbx_extension']['value']!='') {
    hpbx_extension_validate('table', $form_state['values']['table'], $Customer);
  }
}

/**
 * Form submit handler for hpbx_conferences_overview_form().
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_conference_edit_form_submit($form, $form_state) {

  global $ActiveReseller;
  try {

    if (!empty($form_state['values']['customer_id'])) {
      $customer_id = $form_state['values']['customer_id'];
    }
    else {
      drupal_set_message(t('Can\'t find the customer ID'), 'error');
      return;
    }

    if (!$Customer = SipwiseEntity::load('customers', $customer_id)) {
      drupal_set_message(t('Can\'t find customer'));
      return;
    }

    if (!$CustomerPreferences = SipwiseEntity::load('customerpreferences', $Customer->id)) {
      drupal_set_message(t('Can\'t find customer preferences'));
      return;
    }
    
    if (!$PilotSubscriber = $Customer->get_pilot_subscriber()){
      drupal_set_message(t('Can\'t find the pilot subscriber'));
      return;
    }

    // Verify an subscriber_id is set.
    if (!$form_state['values']['id']) {

      // New subscriber.
      $Subscriber = SipwiseEntity::create('subscribers');

      // Set values which will not change in the future.
      $Subscriber->webusername = uniqid((int)$customer_id);
      $Subscriber->username = uniqid((int)$customer_id);
      $Subscriber->password = hpbx_subscriber_password($customer_id);
      $Subscriber->customer_id = (int)$customer_id;
      //$Subscriber->pbx_group_id = $PilotSubscriber->pbx_group_id;
      $Subscriber->is_pbx_group = FALSE;
      $Subscriber->administrative = FALSE;
      $Subscriber->status = 'active';
      $Subscriber->domain_id = $PilotSubscriber->domain_id;
      $Subscriber->profile_set_id = $ActiveReseller->_settings->generic_profile_set_id;
      $Subscriber->profile_id = $ActiveReseller->_settings->conference_room_subscriber_profile_id;
    }
    else {
      // Existing subscriber.
      $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
    }

    // Name.
    if (!empty($form_state['values']['table']['external_id']['value'])) {
      $Subscriber->external_id = trim($form_state['values']['table']['external_id']['value']);
    }

    if (!empty($form_state['values']['table']['pbx_extension']['value'])) {
      $Subscriber->pbx_extension = trim($form_state['values']['table']['pbx_extension']['value']);
    }

    $alias_number_original = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);

    $an_change = FALSE;
    
    $alias_no_original = $form_state['values']['table']['alias_number']['value'];
    
    if(empty($form_state['values']['table']['alias_number']['value']) && isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
      $form_state['values']['table']['alias_number']['value'] = $form_state['values']['table']['cli']['value'] . $form_state['values']['table']['pbx_extension']['value'];
    } 

    if ($form_state['values']['table']['alias_number']['value']!=$alias_number_original) {

      $an_change = TRUE;

      // Remove the newly set alias number from the pilot subscriber.
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
      if(strlen($form_state['values']['table']['alias_number']['value']) >= $min_length && strlen($form_state['values']['table']['alias_number']['value']) <= $max_length && !empty($alias_no_original)){
        $PilotSubscriber->alias_number_unset($Customer,$form_state['values']['table']['alias_number']['value']);
      }
      // Set the newly set alias number on the subscriber, remove it when empty.
      $Subscriber->alias_number_set($Customer, $form_state['values']['table']['alias_number']['value']);
    }

    // Save the pilot subscriber and subscriber.
    $PilotSubscriber->save();
    $Subscriber->save();

    // Add the original alias number to the pilot number again, so it will be available
    // to others subscribers to be used.
    if ($an_change && !empty($alias_number_original)) {
      $PilotSubscriber->alias_number_set($Customer, $alias_number_original, FALSE);
      $PilotSubscriber->save();
    }

    // Load the subscriberpreferences.
    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
	
    $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;
    if ($area = $Customer->get_area_code($form_state['values']['table']['alias_number']['value'])) {
      $SubscriberPreferences->ac = $area;
    }
     $SubscriberPreferences->gpp1 = $Subscriber->pbx_extension;

    // Name.
    if (isset($form_state['values']['table']['conference_pin']['value'])) {
      
      if (empty($form_state['values']['table']['conference_pin']['value'])) {
        unset($SubscriberPreferences->conference_pin);
      }
      else {
        $SubscriberPreferences->conference_pin = trim($form_state['values']['table']['conference_pin']['value']);
      }
    }
    
    
    // Set announcement language.
    if (!empty($form_state['values']['table']['sound_set']['value'])) {
      $SubscriberPreferences->sound_set = $form_state['values']['table']['sound_set']['value'];
    }
    else {
      unset($SubscriberPreferences->sound_set);
    }
    
    if (isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
        if (isset($alias_no_original) && is_numeric($alias_no_original) && $alias_no_original!=0) {
                
        //Set the gpp0 value equal to the aliasnumber for billing purposses.
          $SubscriberPreferences->cli = $SubscriberPreferences->gpp0  = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['alias_number']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
            
        if (in_array($ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'],$CustomerPreferences->allowed_clis)) {
            $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
          }
          else {
            $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
          }
      } 
      else if(isset($form_state['values']['table']['cli']['value']) && is_numeric($form_state['values']['table']['cli']['value']) && $alias_no_original==0){
        //$SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
        //Set the gpp0 value equal to the aliasnumber for billing purposses.
        $SubscriberPreferences->gpp0  = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
        $SubscriberPreferences->cli = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls)) . $form_state['values']['table']['pbx_extension']['value'];
        $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;

        // Get area number
        $area = $Customer->get_area_code($form_state['values']['table']['cli']['value']);

        // Set area code in case it is found.
        if ($area) {
          $SubscriberPreferences->ac = $area;
        }

        if ($ActiveReseller->_settings->execute_logic_for_area_code_dependant_emergency_suffix) {

          // Set emergency prefix.
          $SubscriberPreferences->emergency_prefix = $ActiveReseller->_settings->emergency_prefix;

          // Get emergency index.
          $ei = hpbx_get_emergency_index($area);

          // Set emergency suffix.
          $SubscriberPreferences->emergency_suffix = $ei;
        }
        // To support routing for non-ported numbers to peer, which is necessary
        // to ensure  that calling  non-ported numbers  from HPBX will be routed
        // correctly  (to the peer), we  will set force_inbound_calls_to_peer to
        // TRUE in case the number is not yet ported or to FALSE when the number
        // is ported. To verify if the number is ported, we will use the allowed
        // -clis variable, which is filled based on the inported checkbox at the
        // customer edit form.
        if (in_array($ActiveReseller->_settings->country_code . $form_state['values']['table']['cli']['value'],
          $CustomerPreferences->allowed_clis)) {

          $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
        }
        else {
          $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
        }		
        $SubscriberPreferences->display_name = $Subscriber->pbx_extension;
	}
    }else{
      //Modified for Bug#9743 - BUILD: Change request - Set display_name for AA/CR/Fax for create/update use cases
      //$SubscriberPreferences->display_name = $Subscriber->pbx_extension;
      $SubscriberPreferences->display_name = $Subscriber->external_id;
      // Set the gpp0 value equal to the aliasnumber for billing purposses.
      $SubscriberPreferences->cli = $SubscriberPreferences->gpp0 = $ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'];
     
      // To support routing for non-ported numbers to peer, which is necessary
      // to ensure  that calling  non-ported numbers  from HPBX will be routed
      // correctly  (to the peer), we  will set force_inbound_calls_to_peer to
      // TRUE in case the number is not yet ported or to FALSE when the number
      // is ported. To verify if the number is ported, we will use the allowed
      // -clis variable, which is filled based on the inported checkbox at the
      // customer edit form.
      if (in_array($ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'],
          $CustomerPreferences->allowed_clis)) {

        $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
      }
      else {
        $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
      }
    }

    $allsite = json_decode(json_encode($Customer->_settings->multisite),true);
    foreach($allsite as $key=>$sites){
      if($sites['site_name'] == $form_state['values']['table']['site']['value']){
        $siteselected = $key;
      }
    }
    
    if(isset($Customer->_settings->multi_site) && $Customer->_settings->multi_site && !empty($allsite[$siteselected]['ips'])){	  
	  	$SubscriberPreferences->allowed_ips = $allsite[$siteselected]['ips'];	   
    }
    elseif (isset($ActiveReseller->_settings->enable_restricted_access) &&
      $ActiveReseller->_settings->enable_restricted_access &&
      isset($Customer->_settings->allowed_ips)
    ) {

      $SubscriberPreferences->allowed_ips = $Customer->_settings->allowed_ips;
    }
    else{  
      // Currently set and not set on customer level, so unset the variable.
      unset($SubscriberPreferences->allowed_ips);
    }


    // Set location id in case polish emergency routing is active for this customer / reseller.
    if(isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing && 
    isset($Customer->_settings->multi_site) && $Customer->_settings->multi_site){
      $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
      $SubscriberPreferences->emergency_mapping_container = $emergency_indexes[$allsite[$siteselected]['region']][$allsite[$siteselected]['township']][$allsite[$siteselected]['city']][$allsite[$siteselected]['z_gm']];
    }
    elseif (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing) {
      $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
      $SubscriberPreferences->emergency_mapping_container = $emergency_indexes[$Customer->_settings->region][$Customer->_settings->township][$Customer->_settings->city][$Customer->_settings->z_gm];
    }


    if(isset($Customer->_settings->multi_site) && $Customer->_settings->multi_site){
      $SubscriberPreferences->gpp9 = $allsite[$siteselected]['site_name'];
    }

    $SubscriberPreferences->save();

    // Set callforwarding unconditionaly to auto attendant.
    $form_state['values']['call-forwards']['cfu']['destination_type'] = 'conference';
    $form_state['values']['call-forwards']['cfu']['destination_timeout'] = 300;

    // Set Callfowarding unconditionaly to conference room
    $Callforwards = SipwiseEntity::load('callforwards', $Subscriber->id);

    hpbx_set_simple_callforward_values('cfu', $form_state, $Callforwards, $Subscriber);

    // Update the callforwards.
    $Callforwards->save();
  }
  catch (Exception $e) {
    drupal_set_message(t('Failed to update'), 'error');
    watchdog(__FUNCTION__, $e->getMessage());
    $catched = TRUE;
  }

  foreach (array('subscribers', 'subscriberpreferences') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $Subscriber->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));
  
    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }
  
  if (!isset($catched)) {
    drupal_set_message(t('Conference room successfully saved'));
  }

  drupal_goto('hpbx/conference-rooms');
}

