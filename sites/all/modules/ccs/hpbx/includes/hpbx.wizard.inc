<?php

function hpbx_wizard_subscriber_submit() {
    
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.subscriber.inc');

  $Customer = hpbx_get_active_customer();
  $subscriber_form = array();
  $subscriber_form_state['values'] = array('customer_id' => $Customer->id);
  
  $values = array(
    'firstname' => array('value' => filter_xss($_POST['subscriber']['firstname'])),
    'lastname' => array('value' => filter_xss($_POST['subscriber']['lastname'])),
    'email' => array('value' => filter_xss($_POST['subscriber']['email'])),
    'external_id' => array('value' => filter_xss($_POST['subscriber']['external_id'])),
    'administrative' => array('value' => (int)$_POST['subscriber']['administrative']), 
    'alias_number' => array('value' => $_POST['subscriber']['alias_number']), 
    'pbx_extension' => array('value' => check_plain($_POST['subscriber']['pbx_extension'])), 
    'site' => array('value' => $_POST['subscriber']['site']),
    'cli' => array('value' => check_plain($_POST['subscriber']['cli'])), 
    'clir' => array('value' => (int)$_POST['subscriber']['clir']), 
    'profile_id' => array('value' => (int)$_POST['subscriber']['profile_id']), 
    'ncos' => array('value' => (int)$_POST['subscriber']['ncos']), 
    'block_in_clir' => array('value' => (int)$_POST['subscriber']['block_in_clir']), 
    'block_out_override_pin' => array('value' => (int)$_POST['subscriber']['block_out_override_pin']),
    'language' => array('value' => check_plain($_POST['subscriber']['language'])),
    'no_selfcare' => array('value' => (int)$_POST['subscriber']['no_selfcare']),
    'move_from_pilot' => array('value' => (int)$_POST['subscriber']['move_from_pilot']),
  );
  $subscriber_form_state['values']['general']['settings'] = $values;

  // Set the no_redirect option, to prevent a redirect after successfully
  // submitting the form.
  $subscriber_form_state['values']['no_redirect'] = TRUE;

  // Verify if required fields are empty.
  if(!isset($Customer->_settings->allow_subscribers_without_did) && !$Customer->_settings->allow_subscribers_without_did){
    if (empty($subscriber_form_state['values']['general']['settings']['alias_number']['value'])) {
      form_set_error('general][settings][alias_number', t('!name field is required.', array('!name' =>  t('Number'))));
    }
 }
  if (isset($Customer->_settings->multi_site) && !empty($Customer->_settings->multi_site)) {
    if (empty($subscriber_form_state['values']['general']['settings']['site']['value'])) {
      form_set_error('general][settings][site', t('!name field is required.', array('!name' =>  t('Site'))));
    }     
  }
  if (empty($subscriber_form_state['values']['general']['settings']['cli']['value'])) {
    form_set_error('general][settings][cli', t('!name field is required.', array('!name' =>  t('Caller Line Identification'))));
  }
  if (empty($subscriber_form_state['values']['general']['settings']['profile_id']['value'])) {
    form_set_error('general][settings][profile_id', t('!name field is required.', array('!name' =>  t('Subscriber profile'))));
  }
  if (empty($subscriber_form_state['values']['general']['settings']['language']['value'])) {
    form_set_error('general][settings][language', t('!name field is required.', array('!name' =>  t('Language of Voicemail announcements'))));
  }
  //$subscriber_options_cli = hpbx_get_subscriber_options_subscriber_create($Customer, NULL, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
  
  // Validate the form.
  hpbx_subscriber_edit_form_validate($subscriber_form, $subscriber_form_state);
  
  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, 'general][settings][');
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('success' => FALSE, 'form_errors' => $json_form_errors));
    drupal_exit();
  }
  
  // Submit the form.
  hpbx_subscriber_edit_form_submit($subscriber_form, $subscriber_form_state);
  
  echo json_encode(array('success' => TRUE, 'handler' => 'wizardNextStep'));
  drupal_exit();
}

function hpbx_wizard_device_submit() {

  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');
  
  $Customer = hpbx_get_active_customer();
  $device_form = array();
  $device_form_state['values'] = array();
  
  
  $values = array(
    'station_name' => array('value' => filter_xss($_POST['pbxdevice']['station_name'])),
    'device_brand' => array('value' => filter_xss($_POST['pbxdevice']['device_brand'])),
    'profile_id' => array('value' => (int)$_POST['pbxdevice']['profile_id']),
    'identifier' => array('value' => filter_xss($_POST['pbxdevice']['identifier'])),
  );

  $device_form_state['values']['table'] = $values;
  $device_form_state['values']['table']['customer_id'] = $Customer->id;
  
  // Set the no_redirect option, to prevent a redirect after successfully
  // submitting the form.
  $device_form_state['values']['no_redirect'] = TRUE;
  
  if (empty($device_form_state['values']['table']['station_name']['value'])) {
    form_set_error('table][station_name', t('!name field is required.', array('!name' => t('Name'))));
  }
  if (empty($device_form_state['values']['table']['device_brand']['value'])) {
    form_set_error('table][device_brand', t('!name field is required.', array('!name' => t('Brand'))));
  }
  if (empty($device_form_state['values']['table']['profile_id']['value'])) {
    form_set_error('table][profile_id', t('!name field is required.', array('!name' =>  t('Subscriber profile'))));
  }
  if (empty($device_form_state['values']['table']['identifier']['value'])) {
    form_set_error('table][identifier', t('!name field is required.', array('!name' => t('MAC Address'))));
  }

  if (empty($_SESSION['ccs']['hpbx']['wizard']['subscriber_id'])) {
    watchdog('hpbx_wizard', 'Subscriber ID not found, so can\'t assign to device');
    form_set_error('', t('Unknown error occurred. Please try again.'));
  }

  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, array('table][', 'pbxdevice]['));
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('success' => FALSE, 'form_errors' => $json_form_errors));
    drupal_exit();
  }
  
  
  // Add line.
  $line = new stdClass();
  
  $line->subscriber_id = $_SESSION['ccs']['hpbx']['wizard']['subscriber_id'];
  $line->line_type = 'private';
  $line->key_num = 0;

  $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($device_form_state['values']['table']['profile_id']['value']);
  $default_linerange = FALSE;
  $line->linerange = 'Phone Keys';

  foreach ($PBXDeviceModel->linerange as $linerange) {
    if ($linerange->can_private) {
      $line->linerange = $linerange->name;
      break;
    }
  }
  
  $device_form_state['values']['lines'] = json_encode(array($line));
  
  // Validate the form.
  hpbx_pbxdevice_edit_form_validate($device_form, $device_form_state);
  
  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, array('table][', 'pbxdevice]['));
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('success' => FALSE, 'form_errors' => $json_form_errors));
    drupal_exit();
  }
  
  // Submit the form.
  hpbx_pbxdevice_edit_form_submit($device_form, $device_form_state);

  if (empty($_SESSION['ccs']['hpbx']['wizard']['device_id'])) {
    watchdog('hpbx_wizard', 'Device ID not found, so can\'t sync device');
    form_set_error('', t('Unknown error occurred. Please try again.'));
  }

  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, array('table][', 'pbxdevice]['));
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('success' => FALSE, 'form_errors' => $json_form_errors));
    drupal_exit();
  }

  //Bug #9554 - Wizard - Sync instructions for Cisco incorrect when adding ATA's
  if ($values['device_brand']['value'] == 'Cisco') {
    if ($PBXDeviceModel->model == 'SPA232D'){
	   // Goto  step (step 7).
      $handler = 'wizardGoToStep';
      $handler_argument = 7;
	}
	elseif(($PBXDeviceModel->model == 'ATA112') || ($PBXDeviceModel->model == 'ATA122')){
	  // Goto  step (step 8).
      $handler = 'wizardGoToStep';
      $handler_argument = 8;
	}
	else{
      // Goto sync device form (step 5).
      $handler = 'wizardGoToStep';
      $handler_argument = 5;
	}
  }
  else {

    // Goto last step (finished, step 6).
    $handler = 'wizardGoToStep';
    $handler_argument = 6;
  }

  if ($sync_url = hpbx_pbxdevice_get_sync_url((int)$_SESSION['ccs']['hpbx']['wizard']['device_id'])) {
    echo json_encode(array(
      'success' => TRUE,
      'sync_url' => $sync_url,
      'sync_ip_val' => '#edit-sync-ip',
      'handler' => $handler,
      'handler_argument' => $handler_argument
    ));
  }
  else {
    form_set_error('', t('Unknown error occurred. Please try again.'));
    echo json_encode(array('success' => FALSE));
  }
  drupal_exit();
}

/**
 * 
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:
 */
function hpbx_wizard_employee_form($form, $form_state) { 

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx-wizard.js', array('scope' => 'footer'));
  
  
  $form = array();
  $form['step1'] = hpbx_wizard_employee_form_step1($form, $form_state);
  $form['step2'] = hpbx_wizard_employee_form_step2($form, $form_state);
  $form['step3'] = hpbx_wizard_employee_form_step3($form, $form_state);
  $form['step4'] = hpbx_wizard_employee_form_step4($form, $form_state);
  $form['step5'] = hpbx_wizard_employee_form_step5($form, $form_state); 
  
  
  if (arg(5) == 'success') {
    $title = t('Device successfully associated');
    $sub_title = t('Provisioning can take up to 5 minutes.');
  }
  elseif (arg(5) == 'failed') {
    $title = t('Failed to associate device');
    $sub_title = '';
  }
  else {
    $title = t('Device successfully created');
    $sub_title = 'Provisioning can take up to 5 minutes';
  }
  $form['step6'] = hpbx_wizard_employee_form_step6($form, $form_state, $title, $sub_title);
  //Bug #9554 - Wizard - Sync instructions for Cisco incorrect when adding ATA's
  $form['step7'] = hpbx_wizard_employee_form_step7($form, $form_state);
  $form['step8'] = hpbx_wizard_employee_form_step8($form, $form_state);
  $form['current_step'] = array('#type' => 'hidden', '#default_value' => '');
  return $form;
}

/**
 * 
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step1($form, &$form_state) {

  $Customer = hpbx_get_active_customer();

  $form = hpbx_wizard_content_form(1, 1, t('Create a subscriber'), 1, 4);
  
  $form['content']['title'] = array(
    '#markup' => '<h4 class="hpbx-wizard-content-title">'. t('Subscriber details') . '</h4>'
  );
  
      
  $form['content']['subscriber'] = array('#tree' => TRUE);
  
  $form['content']['subscriber']['no_selfcare'] = array(
    '#type' => 'checkbox',
    '#caption' => t('Disable self care rights'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),

    '#attributes' => array(
      'class' => array('sg-custom-check-and-radio', 'sg-custom-check', 'hpbx-subscriber-no-selfcare'),
      'data-show-hide-type' => 'hpbx-disable-self-care',
    ),
  );

  $form['content']['subscriber']['firstname'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#wrapper_attributes' => array('data-hide-type' => 'hpbx-disable-self-care'),
    '#attributes' => array(
      'class' => array('sg-element', 'hpbx-subscriber-firstname'),
      'placeholder' => t('First name'),
    ),
  );
  $form['content']['subscriber']['lastname'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#wrapper_attributes' => array('data-hide-type' => 'hpbx-disable-self-care'),
    '#attributes' => array(
      'class' => array('sg-element', 'hpbx-subscriber-lastname'),
      'placeholder' => t('Last name'),
    ),
  );
  $form['content']['subscriber']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#wrapper_attributes' => array('data-hide-type' => 'hpbx-disable-self-care'),
    '#attributes' => array(
      'class' => array('sg-element', 'hpbx-subscriber-email'),
      'placeholder' => t('Email'),
    ),
  );
  $form['content']['subscriber']['external_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#wrapper_attributes' => array('data-show-type' => 'hpbx-disable-self-care'),
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('Name'),
    ),
  );
  

  // In case the - by upc admin - created admin is still attached to the pilot subscriber, allow the admin to move
  // this admin account to a new subscriber account.
  if (($Customer->get_pilot_subscriber()->id == hpbx_get_active_subscriber()->id) &&
      !user_access('hpbx edit customer') && user_access('hpbx edit subscriber')) {
  
        // Get pilot subscriber.
        $PilotSubscriber = $Customer->get_pilot_subscriber();
  
        if ($ad_entry = hpbx_users_get_user($PilotSubscriber->webusername)) {
  
          $form['pilot_firstname'] = array(
              '#type' => 'hidden',
              '#default_value' => $ad_entry['givenname'][0]
          );
          $form['pilot_lastname'] = array(
              '#type' => 'hidden',
              '#default_value' => $ad_entry['sn'][0]
          );
          $form['pilot_email'] = array(
              '#type' => 'hidden',
              '#default_value' => $ad_entry['mail'][0]
          );
        }
  
        // Current logged in user is the still the pilot user.
        $form['content']['subscriber']['move_from_pilot'] = array(
            '#caption' => t('Assign to current user'),
            '#type' => 'checkbox',
            '#data_hide_type' => 'hpbx-disable-self-care',
            '#attributes' => array(
                'class' => array('sg-element', 'hpbx-subscriber-move-from-pilot'),
                'data-custom-change-event' => 'edit-general-move-from-pilot-change'
            ),
            '#title_display' => 'before',
            '#field_size_class' => array('col-xs-12', 'col-sm-8', 'col-md-9'),

        );
      }
      
  $form['content']['subscriber']['administrative'] = array(
      '#type' => 'checkbox',
      '#caption' => t('Administrative account'),
      '#theme_wrappers' => array('hpbx_wizard_form_element'),
      '#wrapper_attributes' => array('data-hide-type' => 'hpbx-disable-self-care'),
      '#attributes' => array(
          'class' => array('sg-custom-check-and-radio', 'sg-custom-check', 'hpbx-subscriber-administrative'),
      ),
  );

  
  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  $form['buttons']['buttons_right'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );
  
  $form['buttons']['buttons_right']['next'] = array(
    '#type' => 'button',
    '#value' => t('Next'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-little-space'),
      'data-click-handler' => 'wizardNextStep',
    ),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  $form['buttons']['buttons_right']['next_step'] = array(
    '#type' => 'button',
    '#value' => t('Next Step'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-lots-of-space'),
      'data-click-handler' => 'wizardNextStep',
    ),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step2($form, &$form_state) {

  global $ActiveReseller;
  $Customer = hpbx_get_active_customer();

  //Modified for #9730 - BUILD: Multi-domain support
  $PilotSubscriber = $Customer->get_pilot_subscriber();
  $Domain = SipwiseEntity::load('domains', $PilotSubscriber->domain_id);
  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);
  
  $form = hpbx_wizard_content_form(2, 2, t('Create a subscriber'), 2, 4);

  $form['content']['title'] = array(
    '#markup' => '<h4 class="hpbx-wizard-content-title">'.t('Specifications').'</h4>'
  );

  $form['content']['subscriber'] = array('#tree' => TRUE);
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
  	$options=$Customer->get_free_numbers();
  	$options[0] = 'None';
  	foreach(ksort($options) as $key=>$value){
		$options[]=$value;
	}
  	// Get free numbers from the pilot alias_numbers list.
	  $form['content']['subscriber']['alias_number'] = array(
	    '#type' => 'select',
	    '#title' => t('Number'),
	    '#theme_wrappers' => array('hpbx_wizard_form_element'),
	    '#options' => $options,
	    '#attributes' => array(
	      'class' => array('chzn-select', 'sg-element'),
	      'data-search-text' => t('Narrow results'),
	    ),
	    '#empty_option' => t('Choose number')
	  );
  }else{
  // Get free numbers from the pilot alias_numbers list.
  $form['content']['subscriber']['alias_number'] = array(
    '#type' => 'select',
    '#title' => t('Number'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => $Customer->get_free_numbers(),
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element'),
      'data-search-text' => t('Narrow results'),
    ),
    '#empty_option' => t('Choose number')
  );
  }
  $form['content']['subscriber']['pbx_extension'] = array(
    '#type' => 'textfield',
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('Fill in Extension'),
    ),
    '#maxlength' => $Customer->_settings->extension_length,
  );

  if (isset($Customer->_settings->multi_site) && !empty($Customer->_settings->multi_site)) {
    $multisite = json_decode(json_encode($Customer->_settings->multisite),true);
    $sites = array();
    foreach($multisite as $site){
      $sites[$site['site_name']] = $site['site_name'];
    }
    asort($sites);
    $form['content']['subscriber']['site'] = array(
      '#type' => 'select',
      '#title' => t('Site'),
      '#theme_wrappers' => array('hpbx_wizard_form_element'),
      '#options' => $sites,
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element'),
        'data-search-text' => t('Narrow results'),
      ),
      '#empty_option' => t('Choose Site'),
    );
  }

  if ($Customer->_settings->derive_extension_from_did || $Customer->_settings->allow_subscribers_without_did) {
    $form['content']['subscriber']['alias_number']['#attributes'] += array(
      'data-custom-change-event' => 'edit-subscriber-general-number-change',
      'data-derive-extension-from-did' => (int)$Customer->_settings->derive_extension_from_did,
      'data-allow-subscribers-without-did' => (int)$Customer->_settings->allow_subscribers_without_did,
      'data-extension-length' => (int)$Customer->_settings->extension_length,
      'data-extension-id' => '#edit-subscriber-pbx-extension',
      'data-form-id' => $form_state['build_info']['form_id']
    );
  
    $form['content']['subscriber']['pbx_extension']['#attributes']['readonly'] = 'readonly';
    $form['content']['subscriber']['pbx_extension']['#attributes']['class'][] = 'hpbx-incognito';
    $form['content']['subscriber']['pbx_extension']['#attributes']['placeholder'] = ''; 
    // Add customer number ranges for validation purposes.
    $ranges = array();
    foreach ($Customer->_settings->numberranges as $key => $range) {
      unset($range->inported);
      $ranges[$key] = $range;
    }
  
    $form['numberranges'] = array(
        '#markup' => '<div id="hpbx-numberranges" data-json="'. str_replace('"', "'", json_encode($ranges)) . '" style="display:none;"></div>',
    );
  }

  // Get list of subscribers.
  
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
   // Get list of subscribers.
  $subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
    	if(isset($Subscriber) && !empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	}else if(isset($Customer->_settings->numberranges) && !empty($Customer->_settings->numberranges)){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	}
  }else{
    $subscriber_options = hpbx_get_subscriber_options($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
    // Prepend the list with some default settings.
  $subscriber_options = array_merge(array('default' => t('Own number'), 'clir' =>  t('Restricted')), $subscriber_options);
  }
  
  $form['content']['subscriber']['cli'] = array(
    '#type' => 'select',
    '#title' => t('Caller Line Identification'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => $subscriber_options,
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element','no-search'),
    ),
    '#empty_option' => t('Choose Caller Line Identification')
  );
  //Restrict CLI
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
	  $form['content']['subscriber']['clir'] = array(
	    '#type' => 'checkbox',	    
	    '#access' => user_access('hpbx edit subscriber'),
	    '#theme_wrappers' => array('hpbx_wizard_form_element'),
	    '#caption' => t('Restrict CLI')
	  );
  }
  // Get profiles.
  $options = array();
  
  $default_profile_id = NULL;
  foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $Customer->_settings->profile_set_id)) as $SubscriberProfile) {
    if ($SubscriberProfile->set_default) {
      $default_profile_id = $SubscriberProfile->id;
    }
    $options[$SubscriberProfile->id] = t($SubscriberProfile->description);
  }
  asort($options);
  
  $form['content']['subscriber']['profile_id'] = array(
    '#type' => 'select',
    '#title' => t('Subscriber profile'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => $options,
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element', 'no-search'),
    ),
    '#empty_option' => t('Choose Subscriber Profile')
  );
  
  $options = array();
  foreach (sipwise_api_get_all('ncoslevels') as $NCOSLevel) {
    if ((!empty($ActiveReseller->_settings->suspend_ncos_level) && $NCOSLevel->level == $ActiveReseller->_settings->suspend_ncos_level) ||
    (!empty($DomainPreferences->adm_cf_ncos) && $NCOSLevel->level == $DomainPreferences->adm_cf_ncos)) {
      continue;
    }
    if (user_access('hpbx edit customer')) {
      $options[$NCOSLevel->level] = $NCOSLevel->level . ' - '. t($NCOSLevel->description);
    }
    else {
      $options[$NCOSLevel->level] = t($NCOSLevel->description);
    }
  }
  
  $form['content']['subscriber']['ncos'] = array(
    '#type' => 'select',
    '#title' => t('Call barrings'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => $options,
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element','no-search'),
    ),
    '#empty_option' => t('Choose Call Barring')
  );
  
  $form['content']['subscriber']['block_in_clir'] = array(
    '#type' => 'checkbox',
    '#caption' => t('Block anonymous inbound calls'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#access' => hpbx_attribute_allowed('block_in_clir'),
  );


  $form['content']['subscriber']['block_out_override_pin'] = array(
    '#type' => 'textfield',
    '#title' => t('PIN for overriding (temporary) call barrings'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#maxlength' => 4,
    '#access' => hpbx_attribute_allowed('block_out_override_pin'),
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('PIN for overriding (temporary) call barrings'),
    ),
  );

  // System wide announcements.
  $form['content']['subscriber']['language'] = array(
    '#type' => 'select',
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#access' => hpbx_attribute_allowed('language'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
    '#empty_option' => t('Choose language of Voicemail announcements'),
    '#options' => array(),
  );

  if ($preferencedefs = sipwise_get_preferencedefs('subscriber')) {
    if (isset($preferencedefs->language->enum_values)) {
      foreach ($preferencedefs->language->enum_values as $enum) {
        if (!empty($enum->value)) {
          if ($enum->default_val) {
            $form['content']['subscriber']['language']['#default_value'] = $enum->value;
          }
          $form['content']['subscriber']['language']['#options'][$enum->value] = t(drupal_ucfirst($enum->label));
        }
      }
    }
  }

  // Sort options list.
  asort($form['content']['subscriber']['language']['#options']);

  $form['buttons'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('hpbx-wizard-buttons'))
  );
  $form['buttons']['buttons_left'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  $form['buttons']['buttons_left']['prev'] = array(
    '#type' => 'link',
    '#href' => '',
    '#title' => t('Previous'),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
      'data-click-handler' => 'wizardPreviousStep'
    )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
    '#type' => 'link',
    '#href' => '',
    '#title' => t('Previous Step'),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
      'data-click-handler' => 'wizardPreviousStep'
    )
  );
  $form['buttons']['buttons_right'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );
  $form['buttons']['buttons_right']['next'] = array(
    '#type' => 'button',
    '#value' => t('Next'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-little-space'),
      'data-click-handler' => 'wizardSubmit',
      'data-click-handler-argument' => 'subscriber',
    ),
    '#limit_validation_errors' => array(),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  $form['buttons']['buttons_right']['next_step'] = array(
    '#type' => 'button',
    '#value' => t('Next Step'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-lots-of-space'),
      'data-click-handler' => 'wizardSubmit',
      'data-click-handler-argument' => 'subscriber',
    ),
    '#limit_validation_errors' => array(),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  return $form;
}

function hpbx_wizard_content_form($step, $background, $title, $progress_step, $total_steps) {
  $form = array(
    '#type' => 'container',
  );
  $form = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('hpbx-wizard-step-container'),
      'data-step' => $step,
      'data-background-id' => $background,
    )
  );

  $form['title_container'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('hpbx-wizard-title', 'sg-header-3')
    )
  );
  $form['title_container']['container'] = array(
    '#type' => 'markup',
    '#markup' => $title,
  );
  
  $form['progress'] = array(
    '#markup' => theme('hpbx_wizard_progress', array('step' => $progress_step, 'total_steps' => $total_steps)),
  );
  
  $form['content'] = array(
      '#type' => 'container',
      '#attributes' => array(
          'class' => array('hpbx-wizard-content')
      )
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step3($form, &$form_state) {
  
  $form = hpbx_wizard_content_form(3, 3, t('Subscriber successfully created'), 3, 4);
  
  $form['content']['title'] = array(
      '#markup' => '<h4 class="hpbx-wizard-content-title">'.t('Would you like to add a device to this subscriber?').'</h4>'
  );

  
  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  
  $form['buttons']['buttons_left'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  $form['buttons']['buttons_left']['prev'] = array(
    '#type' => 'link',
    '#href' => 'hpbx/dashboard',
    '#title' => t('No'),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
    )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
    '#type' => 'link',
    '#href' => 'hpbx/dashboard',
    '#title' => t("No, I'm done!"),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
    )
  );
  
  $form['buttons']['buttons_right'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['next_step'] = array(
    '#type' => 'button',
    '#value' => t('Add device'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary'),
      'data-click-handler' => 'wizardNextStep',
    ),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>',
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step4($form, &$form_state) {

  $form = hpbx_wizard_content_form(4, 4, t('Create a device'), 4, 4);

  $form['content']['title'] = array(
      '#markup' => '<h4 class="hpbx-wizard-content-title">'. t('Setup new device') .'</h4>'
  );

  $form['content']['pbxdevice'] = array('#tree' => TRUE);
  $form['content']['pbxdevice']['station_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('Name'),
    ),
  );

  // Device profiles.
  $devices = array();
  
  if ($pbxdevicemodels = sipwise_api_get_all('pbxdevicemodels')) {
    foreach ($pbxdevicemodels as $PBXDeviceModel) {
      // @todo remove this. Will exclude panasonic and yealink in case selfcare is not accessed from Techlan.
      //if (substr($_SERVER['REMOTE_ADDR'], 0, 7) != '10.254.' && substr($PBXDeviceModel->bootstrap_method,0,strlen('redirect_')) == 'redirect_') {
      //continue;
        //}
  
        if ($PBXDeviceConfigs = sipwise_api_get_all('pbxdeviceconfigs', array('device_id' => $PBXDeviceModel->id))) {
          foreach ($PBXDeviceConfigs as $PBXDeviceConfig) {
            foreach (sipwise_api_get_all('pbxdeviceprofiles', array('config_id' => $PBXDeviceConfig->id)) as $PBXDeviceProfile) {
  
              $dev = array(
                  'value' => $PBXDeviceProfile->id,
                  'label' => $PBXDeviceProfile->name,
              );
              $devices[$PBXDeviceModel->vendor][] = $dev;
              $options2[$PBXDeviceProfile->id] = $PBXDeviceProfile->name;
            }
          }
        }
      }
    }
  
    $vendors = array_keys($devices);
    natsort($vendors);
  
    $options = array();
    foreach ($vendors as $vendor) {
      natsort($devices[$vendor]);
      $options[$vendor] = array(
          'label' => $vendor,
          'types' => $devices[$vendor]
      );
    }
  
  // Get free numbers from the pilot alias_numbers list.
  $form['content']['pbxdevice']['device_brand'] = array(
    '#type' => 'select',
    '#title' => t('Brand'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => drupal_map_assoc($vendors),
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-show-type' => 'brand-model-ip-address',
      'data-is-show-type-parent' => 'true', 
      'data-device-type-target' => '#edit-pbxdevice-profile-id',
      'data-device-list' => json_encode($options),
      'data-selected-value' => '',
      'data-placeholder' => t('Brand'),
    ),
    '#empty_option' => t('Brand')
  );

  
  // Get free numbers from the pilot alias_numbers list.
  $form['content']['pbxdevice']['profile_id'] = array(
    '#type' => 'select',
    '#title' => t('Model'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#options' => $options2,
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element'),
      'data-selected-value' => '',
      'data-show-type' => 'brand-model-ip-address',
      'data-is-show-type-parent' => 'true',
      'data-placeholder' => t('Model'),
    ),
    '#empty_option' => t('Model')
  );
  
  $form['content']['pbxdevice']['identifier'] = array(
      '#type' => 'textfield',
      '#title' => t('MAC Address'),
      '#theme_wrappers' => array('hpbx_wizard_form_element'),
      '#attributes' => array(
          'class' => array('sg-element'),
          'placeholder' => t('MAC Address'),
      ),
      '#maxlength' => 17,
      '#size' => 30,
  );
  
  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  $form['buttons']['buttons_left'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );

  $form['buttons']['buttons_left']['prev'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t('Done!'),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
      )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t("OK, I'm done!"),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
      )
  );
  
  $form['buttons']['buttons_right'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['next'] = array(
    '#type' => 'button',
    '#value' => t('Next'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary'),
      'data-click-handler' => 'wizardSubmit',
      'data-click-handler-argument' => 'device',
      'data-show-type' => 'brand-model-ip-address',
      'data-show-value' => 'Cisco',
      'data-is-show-type-child' => 'true',
    ),
    '#limit_validation_errors' => array(array('identifier'), array('profile_id'), array('device_brand'), array('station_name')),
    '#suffix' => '<span data-show-type="brand-model-ip-address"  data-is-show-type-child="true" data-show-value="Cisco" class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  
  
  
  
  $form['buttons']['buttons_right']['finish'] = array(
    '#type' => 'button',
    '#value' => t('Finish'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary'),
      'data-click-handler' => 'wizardSubmit',
      'data-click-handler-argument' => 'device',
      'data-show-type' => 'brand-model-ip-address',
      'data-hide-value' => 'Cisco',
      'data-is-show-type-child' => 'true',
    ),
    '#limit_validation_errors' => array(array('identifier'), array('profile_id'), array('device_brand'), array('station_name')),
    '#suffix' => '<span  data-show-type="brand-model-ip-address" data-is-show-type-child="true" data-hide-value="Cisco" class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  
  
  return $form;
}





/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step5($form, &$form_state) {

  $form = hpbx_wizard_content_form(5, 5, t('Device successfully created'), 5, 5);

  $form['content']['title'] = array(
      '#markup' => '<h4 class="hpbx-wizard-content-title">'. t('Associate device') .'</h4>'
  );

  $items = array();
  $items['items'] = array();
  $items['items'][] = t('Press the Setup button.');
  $items['items'][] = t('Scroll to Network and press select.');
  $items['items'][] = t('Scroll to view Current IP. This is the IP address of your device.');
  
  
  $form['content']['sync'] = array('#tree' => TRUE);
  $form['content']['sync']['help'] = array(
    '#type' => 'markup',
    '#markup' => theme('item_list', $items),
  );

  $form['content']['sync']['ip'] = array(
    '#type' => 'textfield',
    '#title' => t('IP address'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#attributes' => array(
        'class' => array('sg-element'),
        'placeholder' => t('IP address'),
    ),
    '#rules' => array('ipv4'),
  );

  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  $form['buttons']['buttons_left'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  $form['buttons']['buttons_left']['prev'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t('Done!'),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
      )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t("OK, I'm done!"),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
      )
  );

  $form['buttons']['buttons_right'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['sync'] = array(
      '#type' => 'button',
      '#value' => t('Sync'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-little-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );


  $form['buttons']['buttons_right']['sync_device'] = array(
      '#type' => 'button',
      '#value' => t('Sync device'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-lots-of-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
function hpbx_wizard_employee_form_step6($form, &$form_state, $title, $sub_title) {
  
  //Bug #9549 - PE DEV: Wizard - Image missing on last step
  $form = hpbx_wizard_content_form(6, 'ready', $title, 5, 5);
  
  if($sub_title == ''){
	$form['content']['title'] = array(
    '#markup' => 'To retry syncing the device please go to the '. l('device overview page','hpbx/pbxdevices')
    ); 
  }else{
  	$form['content']['title'] = array(
    '#markup' => '<h4 class="hpbx-wizard-content-title">'.  $sub_title .'</h4>'
    );  	
  }
  
  

  $form['buttons'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('hpbx-wizard-buttons'))
  );
  $form['buttons']['buttons_left'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  
  $form['buttons']['buttons_left']['prev'] = array(
    '#type' => 'link',
    '#href' => 'hpbx/dashboard',
    '#title' => t('Done!'),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
    )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
    '#type' => 'link',
    '#href' => 'hpbx/dashboard',
    '#title' => t("OK, I'm done!"),
    '#attributes' => array(
      'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
    )
  );


  $form['buttons']['buttons_right'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['next'] = array(
    '#type' => 'button',
    '#value' => t('Add another'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-btn-primary'),
      'data-click-handler' => 'wizardBackToStartCustom',
    ),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
//Bug #9554 - Wizard - Sync instructions for Cisco incorrect when adding ATA's
function hpbx_wizard_employee_form_step7($form, &$form_state) {
	

  $form = hpbx_wizard_content_form(7, 5, t('Device successfully created'), 5, 5);

  $form['content']['title'] = array(
      '#markup' => '<h4 class="hpbx-wizard-content-title">'. t('Associate device') .'</h4>'
  );

  $items = array();
  $items['items'] = array();
  $items['items'][] = t('Enter the phone menu.');
  $items['items'][] = t('Scroll to settings and press select.');
  $items['items'][] = t('Scroll to phone info and press select.');
  $items['items'][] = t('Scroll to view IP Address. This is the IP address of your device.');
  
  
  $form['content']['sync'] = array('#tree' => TRUE);
  $form['content']['sync']['help'] = array(
    '#type' => 'markup',
    '#markup' => theme('item_list', $items),
  );

  $form['content']['sync']['ip'] = array(
    '#type' => 'textfield',
    '#title' => t('IP address'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#attributes' => array(
        'class' => array('sg-element'),
        'placeholder' => t('IP address'),
    ),
    '#rules' => array('ipv4'),
  );

  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  $form['buttons']['buttons_left'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  $form['buttons']['buttons_left']['prev'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t('Done!'),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
      )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t("OK, I'm done!"),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
      )
  );

  $form['buttons']['buttons_right'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['sync'] = array(
      '#type' => 'button',
      '#value' => t('Sync'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-little-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );


  $form['buttons']['buttons_right']['sync_device'] = array(
      '#type' => 'button',
      '#value' => t('Sync device'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-lots-of-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @return multitype:string The multitype:string multitype:string
 */
//Bug #9554 - Wizard - Sync instructions for Cisco incorrect when adding ATA's
function hpbx_wizard_employee_form_step8($form, &$form_state) {	

  $form = hpbx_wizard_content_form(8, 5, t('Device successfully created'), 5, 5);

  $form['content']['title'] = array(
      '#markup' => '<h4 class="hpbx-wizard-content-title">'. t('Associate device') .'</h4>'
  );

  $items = array();
  $items['items'] = array();
  $items['items'][] = t(' Pick up the phone connected to the ATA and call * * * *');
  $items['items'][] = t('Enter 1 1 0 #');
  $items['items'][] = t('You will now hear a message giving you the IP address.');  
  
  $form['content']['sync'] = array('#tree' => TRUE);
  $form['content']['sync']['help'] = array(
    '#type' => 'markup',
    '#markup' => theme('item_list', $items),
  );

  $form['content']['sync']['ip'] = array(
    '#type' => 'textfield',
    '#title' => t('IP address'),
    '#theme_wrappers' => array('hpbx_wizard_form_element'),
    '#attributes' => array(
        'class' => array('sg-element'),
        'placeholder' => t('IP address'),
    ),
    '#rules' => array('ipv4'),
  );

  $form['buttons'] = array('#type' => 'container', '#attributes' => array('class' => array('hpbx-wizard-buttons')));
  $form['buttons']['buttons_left'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-left')),
  );
  $form['buttons']['buttons_left']['prev'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t('Done!'),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-little-space'),
      )
  );
  $form['buttons']['buttons_left']['prev_step'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/dashboard',
      '#title' => t("OK, I'm done!"),
      '#attributes' => array(
          'class' => array('hpbx-tab-form-link-button', 'hpbx-wizard-lots-of-space'),
      )
  );

  $form['buttons']['buttons_right'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-wizard-buttons-right')),
  );

  $form['buttons']['buttons_right']['sync'] = array(
      '#type' => 'button',
      '#value' => t('Sync'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-little-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );


  $form['buttons']['buttons_right']['sync_device'] = array(
      '#type' => 'button',
      '#value' => t('Sync device'),
      '#attributes' => array(
          'class' => array('sg-btn', 'sg-btn-primary', 'hpbx-wizard-lots-of-space'),
          'data-click-handler' => 'wizardSubmit',
          'data-click-handler-argument' => 'sync',
      ),
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>'
  );
  return $form;
}

/**
 * @param array $variables
 *
 * @return string
 */
function theme_hpbx_wizard_form_element($variables) {
  $element = &$variables['element'];

  $wrapper_attributes = array('class' => 'hpbx-form-login-row');
  if (isset($element['#wrapper_attributes'])) {
    $wrapper_attributes += $element['#wrapper_attributes'];
  }

  $output = '<div '.drupal_attributes($wrapper_attributes).'>';
  
  if (!empty($element['#title'])) {
    $output .= '<div class="hpbx-form-login-caption">'. $element['#title'] . '</div>';
  }
  $output .= $element['#children'];
  $output .= '</div>';

  return $output;
}

/**
 * @param array $variables
 *
 * @return string
 */
function theme_hpbx_wizard_progress($variables) {

  $step = $variables['step'];
  $total_steps = $variables['total_steps'];

  // Set all to completed.
  $steps = array_fill(1, $total_steps, 'completed');

  // Set current.
  $steps[$step] = 'current';

  $un = array();

  // Set next as ''.
  if ($step+1 <= $total_steps) {
    
    $steps[$step+1] = '';
  }

  if ($step+2 <= $total_steps) {
    // Set all other.
    $un = array_fill_keys(range($step+2, count($steps)), '');
  }

  $progress  = $un += $steps;

  ksort($progress, SORT_NUMERIC);

  $output = '<div class="hpbx-wizard-progress"><div class="hpbx-wizard-progress-inner">';

  foreach ($progress as $progres_step) {
    $class = '';
    if (!empty($progres_step)) {
      $class = 'hpbx-'. $progres_step;
    }
    $output .= '<div class="hpbx-wizard-progress-step '. $class .'"></div>' . PHP_EOL;
  }
  $output .= '</div></div>';

  return $output;
}















