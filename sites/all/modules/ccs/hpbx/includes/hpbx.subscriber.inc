<?php

/**
 * 
 * @param array $form
 * @param array $form_state
 * @param string $subscriber_id
 *
 * @return array $form
 */
function hpbx_subscriber_edit_form($form, $form_state, $subscriber_id = NULL) {

  global $ActiveReseller, $user;


  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js',
    array('scope' => 'footer'));
  
  $Customer = hpbx_get_active_customer();
  
  $PilotSubscriber = $Customer->get_pilot_subscriber();
  $Domain = SipwiseEntity::load('domains', $PilotSubscriber->domain_id);
  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);
  $Subscriber = NULL;
  $SubscriberPreferences = NULL;

  if (!is_null($subscriber_id) && $Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {

    // Verify if the subscriber to be deleted is owned by the active customer.
    if ($Customer->id != $Subscriber->customer_id) {
      drupal_not_found();
      exit();
    }
    
    // Get the Active Directory entry for this user.
    $entry = $Subscriber->get_active_directory_entry();
    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
  }
  else {
    // Set title.
    drupal_set_title(t('Create subscriber'));
  }

  // Verify if this is an admin user editting his/her own subscriber account.
  $self_admin = (isset($Subscriber->id) &&
    $user->data['sipwise']['subscriber_id'] == $Subscriber->id &&
    $Subscriber->is_admin()
  );

  // Init form.
  $form = array();
  
  if (isset($Subscriber->id)) {

    // Create fieldsets.
    $fieldsets = array(
      'general' =>  array('title' => t('General')),
      'call-forwards' => array('title' => t('Call forwards')),
      'voicemail' => array('title' => t('Voicemail')),
      'speeddials' => array('title' => t('Speed dials')),
    );

    // Create tabs.
    $items = array();
    foreach ($fieldsets as $key => $element) {
      $data = '<div class="hpbx-tab-item-title">
                  <a href="#tab-'. strtolower(str_replace(' ', '-', $key)) . '">'. $element['title'] .'</a>
              </div>';
      $data .= '<div class="hpbx-tab-item-background"></div>';
      $item = array('data' => $data);
      $items[] = $item;
    }
    
    // This is to specify the vertical tab holder.
    // One page can have two vertical tabs, but it's always one main tab with sub tabs.
    $form['tabs'] = array(
      '#type' => 'markup',
      '#markup' => substr(str_replace('<div class="item-list">', '', theme('item_list', array('items' => $items))), 0, -6),
      '#prefix' => '<div class="hpbx-tabs">',
    );
  }
  else {
    // Create fieldsets.
    $fieldsets = array(
      'general' =>  array('access' => 'access content'),
    );
    
    $form = array(
      '#prefix' => '<div class="hpbx-main-content-inner">',
      '#suffix' => '</div>',
    );
  }

  foreach ($fieldsets as $key => $fieldset) {
    $form[$key] = array(
      '#type' => 'container',
      '#tabform' => TRUE,
      '#title' => !empty($fieldset['title']) ? $fieldset['title'] : '',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#access' => isset($fieldset['access']) ? $fieldset['access'] : TRUE,
      '#attributes' => array(
        'id' => 'tab-'. strtolower(str_replace(' ', '-', $key)),
      ),
    );
  }
  
  $form['general']['#tabform'] = TRUE;
  $form['general']['settings'] = array(
    '#type' => 'container',
    '#title' => t('Subscriber settings'),
    '#tree' => TRUE,
  );
  
  // If this subscriber is allowed to use the callqueue, we need to make the
  // subscriber settings a foldable list item
  if (isset($Subscriber->profile_id) && hpbx_attribute_allowed('cloud_pbx_callqueue', $Subscriber->profile_id)) {
    
    $form['general']['#theme_wrappers'] = array('hpbx_foldable_list', 'container');
    $form['general']['#attributes']['class'] = array('hpbx-tab-form hpbx-tab-form-foldable-list');
    $form['general']['#tabform'] = FALSE;
    $form['general']['settings']['#theme_wrappers'] = array('hpbx_foldable_list_item');
    $form['general']['settings']['#collapsed'] = FALSE;
  }

  $soundfiles = array();
  if (!is_null($SubscriberPreferences) && !empty($SubscriberPreferences->contract_sound_set)) {
    if ($SoundSet = hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {

      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
        if ($SoundFile->handle == 'queue_greeting') {
          $soundfiles[$SoundFile->handle] = $SoundFile;
        }
      }
    }
  }
  
  // Add queue foldable list item.
  if (isset($Subscriber->profile_id) && hpbx_attribute_allowed('cloud_pbx_callqueue', $Subscriber->profile_id)) {
    $form['general']['queue'] = hpbx_queue_elements($SubscriberPreferences, $soundfiles);
  }
  
  // Last item place suffix.
  $form[$key]['#suffix'] = '</div>';
  
  $form['call-forwards']['#tabform'] = FALSE;
  $form['call-forwards']['#theme_wrappers'] = array('hpbx_foldable_list', 'container');
  $form['call-forwards']['#attributes']['class'][] = 'hpbx-tab-form hpbx-tab-form-foldable-list';
  
  $form['call-forwards']['#header'] = array(
    'type' => array('data'=> t('When')),
    'destination_type' => array('data'=> t('Destination type')),
    'times' => array('data'=> t('During')),
    'destination_uri' => array('data'=> t('Destination')),
    'ringtimeout' => array('data'=> t('Disconnect after')),
  );

  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Subscriber->id) ? $Subscriber->id : FALSE,
  );

  $form['customer_id'] = array(
    '#type' => 'value',
    '#value' => $Customer->id ? $Customer->id : $Subscriber->customer_id,
  );


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

  // Profile.
  $form['general']['settings']['profile_id']['value'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => !empty($Subscriber->profile_id) ? $Subscriber->profile_id : $default_profile_id,
    '#access' => user_access('hpbx edit subscriber'),
    '#title' => t('Subscriber profile'),
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-custom-change-event' => 'edit-subscriber-general-profile-id-change',
    )
  );

  // Allow the user to create a subcriber withouth login credentials. For
  // example for an meeting room or elevator.
  // If checked :
  //   Administrator will be disabled.
  //   Firstname will be disabled.
  //   Lastname will be disabled.
  //   E-mail address will be disabled.
  //   Won't create an active directory account.
  $form['general']['settings']['no_selfcare']['value'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable self care rights'),
    '#default_value' => FALSE,
    '#access' => user_access('hpbx edit subscriber')&& !$self_admin,
    '#attributes' => array(
      'data-show-hide-type' => 'chk-create-account',
      'class' => array('hpbx-subscriber-no-selfcare')
    ),
    '#title_display' => 'before',
  );
  $form['general']['settings']['no_selfcare_display']['value'] = array(
    '#type' => 'item',
    '#title' => t('Disable self care rights'),
    '#markup' => t('No'),
    '#access' => !user_access('hpbx edit subscriber') || $self_admin,
  );

  // In case the - by reseller customer manager - created admin  is still attached to the pilot
  // subscriber, allow the admin to move this admin account to a new subscriber
  // account.
  if (!isset($Subscriber->id) && 
    ($Customer->get_pilot_subscriber()->id == hpbx_get_active_subscriber()->id) &&
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
    $form['general']['settings']['move_from_pilot']['value'] = array(
      '#title' => t('Assign to current user'),
      '#type' => 'checkbox',
      '#data_hide_type' => 'chk-create-account',
      '#attributes' => array(
        'class' => array('sg-element', 'hpbx-subscriber-move-from-pilot'),
        'data-custom-change-event' => 'edit-general-move-from-pilot-change'
      ),
      '#description' => t('When you as an administrator would like to have a phone number created for this account, please check this box'),
      '#title_display' => 'before',
    );
  }

  // Firstname
  $form['general']['settings']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($Subscriber->external_id) ? trim($Subscriber->external_id) : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#maxlength' => 125, //#9593 Name should be limited 125 characters
    '#attributes' => array('autocomplete' => 'off'),
    '#title' => t('Name*'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Name')),
    '#data_show_type' => 'chk-create-account',
  );

  // External_id.
  $form['general']['settings']['external_id_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($Subscriber->external_id) ? trim($Subscriber->external_id) : '',
    '#access' => !user_access('hpbx edit subscriber') && !isset($entry[0]),
    '#title' => t('Name'),
  );
  
  // Firstname
  $form['general']['settings']['firstname']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['givenname'][0]) ? $entry['givenname'][0] : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#maxlength' => 125, //#9593 First name should be limited 125 characters
    '#attributes' => array('autocomplete' => 'off'),
    '#title' => t('First name*'),
    '#attributes' => array(
      'class' => array(
        'sg-element', 'hpbx-subscriber-firstname'
      ),
      'placeholder' => t('First name')
    ),
    '#data_hide_type' => 'chk-create-account',
  );
  $form['general']['settings']['firstname_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($entry['givenname'][0]) ? $entry['givenname'][0] : '',
    '#access' => !user_access('hpbx edit subscriber') && isset($entry[0]),
    '#title' => t('First name'),
  );
  
  // Lastname.
  $form['general']['settings']['lastname']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['sn'][0]) ? $entry['sn'][0] : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#maxlength' => 125, //#9593 Last name should be limited 125 characters
    '#title' => t('Last name*'),
    '#attributes' => array(
       'class' => array('sg-element', 'hpbx-subscriber-lastname'),
       'placeholder' => t('Last name')
    ),
    '#data_hide_type' => 'chk-create-account',
  );
  $form['general']['settings']['lastname_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($entry['sn'][0]) ? $entry['sn'][0] : '',
    '#access' => !user_access('hpbx edit subscriber') && isset($entry[0]),
    '#title' => t('Last name'),
  );
  
  // E-mail adres.
  $form['general']['settings']['email']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($entry['mail'][0]) ? $entry['mail'][0] : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#title' => t('Email*'),
    '#attributes' => array(
      'class' => array(
        'sg-element', 'hpbx-subscriber-email'
      ),
      'placeholder' => t('Email')
    ),
    '#data_hide_type' => 'chk-create-account',
  );

  // Email display.
  $form['general']['settings']['email_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($entry['mail'][0]) ? $entry['mail'][0] : '',
    '#access' => !user_access('hpbx edit subscriber') && isset($entry[0]),
    '#title' => t('Email'),
  );
  
  // Administrative rights.
  $form['general']['settings']['administrative']['value'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($Subscriber->administrative) ? $Subscriber->administrative : '',
    '#access' => user_access('hpbx edit subscriber') && !$self_admin,
    '#title' => t('Administrative'),
    '#attributes' => array('class' => array('sg-element')),
    '#data_hide_type' => 'chk-create-account',
    '#title_display' => 'before',
    '#attributes' => array('class' => 'hpbx-subscriber-administrative'), 
  );

  // Administrative display.
  $form['general']['settings']['administrative_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($Subscriber->administrative) ? t('Yes') : t('No'),
    '#access' => !user_access('hpbx edit subscriber') || $self_admin,
    '#title' => t('Administrative'),
  );
  
  if (isset($Subscriber->id) && !isset($entry[0])) {
    $form['general']['settings']['no_selfcare']['value']['#default_value'] = TRUE;
    $form['general']['settings']['no_selfcare_display']['value']['#markup'] = t('Yes');
  }
  

  if (isset($Subscriber->id) && $user->data['sipwise']['subscriber_id'] == $Subscriber->id && 
    $Subscriber->is_admin()) {

    $form['general']['settings']['administrative']['value']['#type'] = 'value';
    $form['general']['settings']['administrative']['value']['#value'] = 1;

    $form['general']['settings']['no_selfcare']['value']['#disabled'] = TRUE;
    $form['general']['settings']['external_id']['value']['#access'] = FALSE;
  }

  // Set the first and lastname as title.
  if (!empty($entry['givenname'][0]) && !empty($entry['sn'][0])) {
    drupal_set_title($entry['givenname'][0] . ' '. $entry['sn'][0] );
  }

  // Get alias_number and pbx_extension form elements.
  hpbx_subscriber_number_extension_elements($form, $form_state, $Customer, $Subscriber, 'dummy');
  $form['dummy']['alias_number']['value']['#attributes']['data-extension-id']  = '#edit-general-settings-pbx-extension-value';
  $form['general']['settings'] += $form['dummy'];
  unset($form['dummy']);
  
  if (!user_access('hpbx edit subscriber')) {
    
    if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
      $form['general']['settings']['alias_number_display']['value'] = array(
        '#type' => 'item',
        '#markup' => $form['general']['settings']['alias_number']['value']['#options'][$form['general']['settings']['alias_number']['value']['#default_value']],
        '#title' => t('Number'),
      );
      $form['general']['settings']['pbx_extension_display']['value'] = array(
        '#type' => 'item',
        '#markup' => $form['general']['settings']['pbx_extension']['value']['#default_value'],
        '#title' => t('Extension'),
      );
    }
    else {
       $number = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
	     if(isset($number) && !empty($number)){
		     $number = substr($number, 0, $Customer->_settings->extension_length * -1). '<span class="hpbx-pbx-extension" >'. $Subscriber->pbx_extension . '</span>';
	     }else{
		     $number='None';  
	     }
      
      $form['general']['settings']['alias_number_display']['value'] = array(
        '#type' => 'item',
        '#markup' => $number,
        '#title' => t('Number'),
      );
    }
  }
  $multisite = json_decode(json_encode($Customer->_settings->multisite),true);
  $site = array();
  foreach($multisite as $site){
    $sites[$site['site_name']] = $site['site_name'];
  }
  asort($sites);

  if($Customer->_settings->multi_site){
    $form['general']['settings']['site']['value'] = array(
      '#required' => TRUE,
      '#type' => 'select',
      '#options' => $sites,
      '#access' => user_access('hpbx edit subscriber'),
      '#default_value' => !empty($SubscriberPreferences->gpp9) ? $SubscriberPreferences->gpp9 : NULL,
      '#title' => t('Site'),
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element', 'no-search'),
      )
    );
}
  if (isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
    if (!user_access('hpbx edit subscriber') && isset($SubscriberPreferences->user_cli)){ 
    foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $Sub) {
      $alias_number = $Sub->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
      $cli = $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code));
    if($alias_number==$cli){
      $SubscriberForCLI = $Sub; 
    } 
    }
    $value = array();
    $value[] = '';
    $value[] = ' - '. hpbx_get_profile_name_from_profile_id($SubscriberForCLI->profile_set_id, $SubscriberForCLI->profile_id) . ' - ';
    $value[] = $SubscriberForCLI->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
    $value[] = ' ('. $SubscriberForCLI->pbx_extension . ')'; 
    if ($SubscriberForCLI->profile_id == $ActiveReseller->_settings->auto_attendant_subscriber_profile_id ||
    $SubscriberForCLI->profile_id == $ActiveReseller->_settings->conference_room_subscriber_profile_id ||
    $SubscriberForCLI->is_pbx_group) { 
      $value[0] = hpbx_truncate_name_element($SubscriberForCLI->external_id, 30);
      $subscriber_options[$ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code))] = implode('' , $value); 
    }else{ 
    // For subscriber with ad account.
    if ($ad_entry = hpbx_users_get_user($SubscriberForCLI->webusername)) { 
        $value[0] = $ad_entry['sn'][0]. ', '. $ad_entry['givenname'][0] . ' ';
        $subscriber_options[$ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code))] = implode('', $value);
    }
      // For subscribers with external_id
    elseif (!empty($SubscriberForCLI->external_id)) { 
        $value[0] = hpbx_truncate_name_element($SubscriberForCLI->external_id, 30);
        $subscriber_options[$ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code))] = implode('', $value);
    }
    }                            
    }else{
      // Get list of subscribers.
      $subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
    }
    
    //If Subscriber (alias_number) is "None" 
    if(isset($Subscriber) && (($form_state['input']['general']['settings']['alias_number']['value']=='0') || (!isset($form_state['input']['general']['settings']['alias_number']['value']) && empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN))))){
      $subscriber_options = $subscriber_options;	
    }else {
      $subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    }

    // Set default value based on user_cli setting.    
    if (!isset($Subscriber->id) || isset($SubscriberPreferences->user_cli) && $SubscriberPreferences->user_cli == $ActiveReseller->_settings->country_code . substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls)))  {
      $default_value = 'default';
    }
    // Set default value based on cli setting (other number).
    elseif (isset($SubscriberPreferences->user_cli)) {
    $default_value =  $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code));
    }else {
      $default_value = FALSE;
    }    
  }else{
     if (user_access('hpbx edit subscriber')) {

    // Get list of subscribers.
    $subscriber_options = hpbx_get_subscriber_options($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
    
    // Prepend the list with some default settings.
    $subscriber_options = array_merge(array('default' => t('Own number'), 'clir' =>  t('Restricted')), $subscriber_options);
    
    // Set default value based on clir setting.
    if (!empty($SubscriberPreferences->clir)) {
      $default_value = 'clir';
    }
    elseif (!isset($Subscriber->id) || isset($SubscriberPreferences->user_cli) && $SubscriberPreferences->user_cli == $ActiveReseller->_settings->country_code . substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls)))  {
      $default_value = 'default';
    }
    // Set default value based on cli setting.
    elseif (isset($SubscriberPreferences->user_cli)) {
      $default_value =  $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code));
    }
    else {
      $default_value = FALSE;
    }
    
    
  }
  else {

    $subscriber_options = array('default' => t('Own number'), 'clir' =>  t('Restricted'));
        
    // Set default value based on clir setting.
    if (!empty($SubscriberPreferences->clir)) {
      $default_value = 'clir';
    }
    elseif (isset($SubscriberPreferences->user_cli) && $SubscriberPreferences->user_cli == $ActiveReseller->_settings->country_code . substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls)))  {
      $default_value = 'default';
    }
    // Set default value based on cli setting (other number).
    elseif (isset($SubscriberPreferences->user_cli)) {
      
      $subscriber_options[$ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code))] = $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code));
      // Other number.
      $default_value =  $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->user_cli, strlen($ActiveReseller->_settings->country_code));
    }
    else {
      $default_value = FALSE;
    }
  }
  }
  // CLI.
  $form['general']['settings']['cli']['value'] = array(
    '#type' => 'select',
    '#options' => $subscriber_options,
    '#default_value' => $default_value,
    '#required' => TRUE,
    '#title' => t('Caller Line Identification'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
    '#access' => hpbx_attribute_allowed('cli'),

  );
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
  // CLI Restriction.
	  $form['general']['settings']['clir']['value'] = array(
	    '#type' => 'checkbox',
	    '#default_value' => !empty($SubscriberPreferences->clir) ?  $SubscriberPreferences->clir : '',
	    '#access' => user_access('hpbx edit subscriber'),
	    '#title' => t('Caller Line Identification Restriction'),
	    '#title_display' => 'before',
	  );
	
	  // CLI Restriction display.
	  $form['general']['settings']['clir_display']['value'] = array(
	    '#type' => 'item',
	    '#markup' => !empty($SubscriberPreferences->clir) ? t('Yes') : t('No'),
	    '#access' => !user_access('hpbx edit subscriber'),
	    '#title' => t('Caller Line Identification Restriction'),
	  );
  }
  $form['general']['settings']['profile_id_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($Subscriber->profile_id) ? $options[$Subscriber->profile_id] : '',
    '#access' => !user_access('hpbx edit subscriber'),
    '#title' => t('Subscriber profile'),
  );
  
  $form['profile_id_js'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($Subscriber->profile_id) ? $Subscriber->profile_id : $default_profile_id,
  );
  
  // Prevent this info beeing added on an #ajax call.
  if (empty($form_state['triggering_element'])) { 
    hpbx_subscriber_js_feature_add($options);
  }

  // Block in clir.
  $form['general']['settings']['block_in_clir']['value'] = array(
    '#type' => 'checkbox',
    '#default_value' => !empty($SubscriberPreferences->block_in_clir) ?  $SubscriberPreferences->block_in_clir : '',
    '#access' => user_access('hpbx edit subscriber') && hpbx_attribute_allowed('block_in_clir'),
    '#title' => t('Block anonymous inbound calls'),
    '#title_display' => 'before',
  );
  $form['general']['settings']['block_in_clir_display']['value'] = array(
    '#type' => 'item',
    '#markup' => !empty($SubscriberPreferences->block_in_clir) ?  t('Yes') : t('No'),
    '#access' => !user_access('hpbx edit subscriber') && hpbx_attribute_allowed('block_in_clir'),
    '#title' => t('Block anonymous inbound calls'),
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

  if (user_access('hpbx edit subscriber')) {
    $form['general']['settings']['ncos']['value'] = array(
      '#required' => FALSE,
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => !empty($SubscriberPreferences->ncos) ? $SubscriberPreferences->ncos : $ActiveReseller->_settings->default_subscriber_ncos_level,
      '#title' => t('Call barrings'),
      '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
      '#access' => hpbx_attribute_allowed('ncos'),
    );
  }
  else {
    $form['general']['settings']['ncos']['value'] = array(
      '#value' => !empty($SubscriberPreferences->ncos) ? $SubscriberPreferences->ncos : NULL,
    );
    
    $form['general']['settings']['ncos_display']['value'] = array(
      '#type' => 'item',
      '#markup' => !empty($SubscriberPreferences->ncos) ? $options[$SubscriberPreferences->ncos] : array_shift($options),
      '#title' => t('Call barrings'),
    );
  }

  $form['general']['settings']['block_out_override_pin']['value'] = array(
    '#required' => FALSE,
    '#type' => 'textfield',
    '#maxlength' => 4,
    '#default_value' => !empty($SubscriberPreferences->block_out_override_pin) ? $SubscriberPreferences->block_out_override_pin : NULL,
    '#access' => user_access('hpbx edit subscriber') && hpbx_attribute_allowed('block_out_override_pin'),
    '#title' => t('PIN for overriding (temporary) call barrings'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('PIN')),
  );

  // System wide announcements.
  $form['general']['settings']['language']['value'] = array(
    '#required' => FALSE,
    '#type' => 'select',
    '#title' => t('Language of Voicemail announcements'),
    '#access' => hpbx_attribute_allowed('language'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
    '#default_value' => $DomainPreferences->language,
  );
  
  $form['general']['settings']['language']['value']['#options']= array();
  if ($preferencedefs = sipwise_get_preferencedefs('subscriber')) {
    if (isset($preferencedefs->language->enum_values)) {
      foreach ($preferencedefs->language->enum_values as $enum) {
        if (!empty($enum->value)) {
          if ($enum->default_val) {
            $form['general']['settings']['language']['value']['#default_value'] = $enum->value; 
          }
          $form['general']['settings']['language']['value']['#options'][$enum->value] = t(drupal_ucfirst($enum->label));
        }
      }
    }
  }
  
  // Sort options list.
  asort($form['general']['settings']['language']['value']['#options']);
  
  if (!empty($SubscriberPreferences->language)) {
    $form['general']['settings']['language']['value']['#default_value'] = $SubscriberPreferences->language;
  }

  // Call forwards.
  if (!empty($Subscriber->id) && $Callforward = SipwiseEntity::load('callforwards', $Subscriber->id)) {
    hpbx_subscribers_callforwards_form($form, $form_state, $Customer, $Subscriber, $Callforward, $ActiveReseller);
  }

  // Speeddials
  if (!empty($Subscriber->id) && hpbx_attribute_allowed('speed_dial') && 
    $Speeddials = SipwiseEntity::load('speeddials', $Subscriber->id)) {
  }
  $slot_options = array(
    '*0' => '*0',
    '*1' => '*1',
    '*2' => '*2',
    '*3' => '*3',
    '*4' => '*4',
    '*5' => '*5',
    '*6' => '*6',
    '*7' => '*7',
    '*8' => '*8',
    '*9' => '*9'
  );
  
  $form['speeddials']['table'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('hpbx-slots-container')),
    '#tree' => TRUE,
  );
  
  $form['speeddials']['table']['slots_header'] = array(
      '#type' => 'container',
      '#tree' => FALSE,
      '#attributes' => array('class' => array('hpbx-tab-form-label-field-row')),
  );
  $form['speeddials']['table']['slots_header']['slot'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="col-xs-4"><div class="hpbx-tab-form-label hpbx-tab-form-label-checkbox">'. t('Option').'</div></div>',
  );
  $form['speeddials']['table']['slots_header']['Destination'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="col-xs-8"><div class="hpbx-tab-form-label hpbx-tab-form-label-checkbox">'.t('Destination').'</div></div>',
  );
  
  $form['speeddials']['table']['slots'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#attributes' => array('class' => array('hpbx-slots-rows')),
  );
  
  // Add datalist to the destination input.
  $subscriber_options = hpbx_get_subscriber_options($Customer, $Subscriber, 
    $number_key = TRUE,
    $is_pbx_pilot = FALSE,
    $is_pbx_group = TRUE,
    $autoattendant = TRUE,
    $conference = TRUE,
    $is_subscriber = TRUE,
    $must_be_pbx_group_member = FALSE
  );
  
  $datalist = array();
  foreach ($subscriber_options as $type => $options) {
    foreach ($options as $value => $option) {
      $datalist[] = array('value' => $value, 'label' => $option);
    }
  }

  $datalist = json_encode($datalist);
  
  // Define all 10 slots.
  for ($x=0;$x<=9;$x++) {

    $form['speeddials']['table']['slots'][$x] = array(
      '#type' => 'container',
      '#deletable_row' => TRUE,
      '#attributes' => array(
        'class' => array('hpbx-slots-row', 'row', 'hpbx-deletable-row'),
      ),
      '#tree' => TRUE,
    );
    
    if (empty($form_state['input']['speeddials']['table']['slots'][$x]['row']['slot']) && 
      empty($form_state['input']['speeddials']['table']['slots'][$x]['row']['destination'])) {
        $form['speeddials']['table']['slots'][$x]['#attributes']['class'][] = 'hpbx-invisible';
    }
    $form['speeddials']['table']['slots'][$x]['row'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#attributes' => array('class' => array('hpbx-tab-form-label-field-row')),
    );
    $form['speeddials']['table']['slots'][$x]['row']['slot'] = array(
      '#type' => 'select',
      '#options' => $slot_options,
      '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
      '#field_size_class' => array('col-xs-4', 'col-sm-3'),
      '#row_wrapper' => FALSE,
      '#empty_option' => '-',
    );
    $form['speeddials']['table']['slots'][$x]['row']['destination'] = array(
      '#type' => 'textfield',
      '#attributes' => array(
        'class' => array('sg-element', 'hpbx-autocomplete'),
        'data-values' => $datalist,
      ),
      '#field_size_class' => array('col-xs-6', 'col-sm-7', 'col-sm-offset-1'),
      '#row_wrapper' => FALSE,
    );
    $form['speeddials']['table']['slots'][$x]['row']['delete'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="col-xs-2 col-sm-1">
            <div class="hpbx-tab-form-field">
                <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" type="button" title="'. t('Delete speed dial') .'"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
            </div>
        </div>',
    );
  }

  foreach ($Speeddials->speeddials as $key => $slot) {
    
    // Set default value for slot.
    $form['speeddials']['table']['slots'][$key]['row']['slot']['#default_value'] = $slot->slot;
    $speeddial_no = hpbx_normalize_number_human_readable($Subscriber->id, trim($slot->destination));
    $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
    $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
       
    if(strlen($speeddial_no) >= $min_length && strlen($speeddial_no) <= $max_length){
      $destination = $speeddial_no;
    }else{
      $destination = substr($speeddial_no,-($Customer->_settings->extension_length));
    }
    // Normalize number.
    
    $form['speeddials']['table']['slots'][$key]['row']['destination']['#default_value'] = $destination;
    
    // Remove the invisible class.
    array_pop($form['speeddials']['table']['slots'][$key]['#attributes']['class']);
  }

  // Trimm fix to turn add more output into trimm usable output.
  unset($form['speeddials']['table']['add']);

  $form['speeddials']['table']['slots']['#suffix'] = '
    <div class="row">
      <div class="hpbx-tab-form-label-field-row">
        <div class="col-xs-12">
          <div class="hpbx-tab-form-field">
            <button class="sg-btn sg-btn-secondary hpbx-add-slot" type="button">'. t('Add') .'</button><span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>
          </div>
        </div>
      </div>
    </div>';

  // Voicemail
  if (!empty($Subscriber->id) && $VoicemailSettings = SipwiseEntity::load('voicemailsettings', $Subscriber->id)) {
    hpbx_voicemail_elements($form['voicemail'], $VoicemailSettings);
  }

  // Submit button.
  $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );
  
  if (user_access('hpbx edit subscriber')) {
    $form['general']['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'hpbx/subscribers',
      '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
    );
  }
  $form['general']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );

  
  $form['call-forwards']['actions'] = $form['voicemail']['actions'] = $form['speeddials']['actions'] = $form['general']['actions'];

  // If this subscriber is allowed to use the callqueue, we need to make the subscriber settings a foldable list item
  if (isset($Subscriber->profile_id) && hpbx_attribute_allowed('cloud_pbx_callqueue', $Subscriber->profile_id)) {
    
    // Add extra div for alignment of buttons in case of a foldable list.
    $form['general']['actions']['#prefix'] = '<div class="hpbx-tab-form">'. $form['general']['actions']['#prefix'];
    $form['general']['actions']['#suffix'] .= '</div>';
  }
  
  $form['call-forwards']['actions']['#prefix'] = '<div class="hpbx-tab-form">'. $form['call-forwards']['actions']['#prefix'];
  $form['call-forwards']['actions']['#suffix'] .= '</div>';
  
  if (empty($Subscriber->id)) {
    unset($form['call-forwards']);
    unset($form['voicemail']);
    unset($form['speeddials']);
  }

  // Set the foldable states, based on error state.
  if (!isset($form_state['storage']['errors'])) {

    // By default first foldable item will not be collapsed.
    $form['call-forwards']['cfu']['#collapsed'] = FALSE;
    $form['general']['settings']['#collapsed'] = FALSE;
  }
  
  // In case any error is passed from the any validation handler, the foldable
  // state will be decided based on the fields which caused an error.
  else {
    foreach ($form_state['storage']['errors'] as $error) {
      $keys = explode('][', $error);
      
      if (array_count($keys)>=2 && isset($form[$keys[0]][$keys[1]])) {
        $form[$keys[0]][$keys[1]]['#collapsed'] = FALSE;
      }
    } 
  }
  
  return $form;
}

/**
 * @param array $form
 * @param array $form_state
 * @param SipwiseCustomer $Customer
 * @param SipwiseSubscriber $Subscriber
 * @param SipwiseEntity $Callforward
 */
function hpbx_subscribers_callforwards_form(&$form, &$form_state, SipwiseCustomer $Customer, SipwiseSubscriber $Subscriber = NULL, SipwiseEntity $Callforward = NULL, $ActiveReseller) {

  $cf_types = array(
    'cfu' => t('Unconditional'),
    'cfb' => t('Busy'),
    'cfna' => t('Unavailable'),
    'cft' => t('Timeout'),
  );

  $time_options = array();
  for ($h=0;$h<=23;$h++) {
    for ($i=0;$i<=59;$i+=5) {
      $option = str_pad($h, 2, '0', STR_PAD_LEFT) . ':'. str_pad($i, 2, '0', STR_PAD_LEFT);
      $time_options[$option] = $option;
    }
  }

  // Walk each type.
  foreach ($cf_types as $cf_type => $cf_title) {
    $options = array(
      'voicebox' => t('Voicemail'),
      'uri' => t('Number'),
    );

    // Sort the option list.
    asort($options);

    // In case a destination is set.
    if (!empty($Callforward->$cf_type->destinations[0]->destination)) {

     if (substr($Callforward->$cf_type->destinations[0]->destination, 0, 4) == 'sip:') { 
        $type_default_value = 'uri';
        $cf_number = hpbx_normalize_number_human_readable($Subscriber->id, trim($Callforward->$cf_type->destinations[0]->destination));
        $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
        $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
       
        if(strlen($cf_number) >= $min_length && strlen($cf_number) <= $max_length){
           $uri_default_value = $cf_number;
        }else{
           $uri_default_value = substr($cf_number,-($Customer->_settings->extension_length));
        }
        $cf_title .= ': '. $uri_default_value; 
        
      }
      else {
        $type_default_value = !empty($Callforward->$cf_type->destinations[0]->destination) ? $Callforward->$cf_type->destinations[0]->destination : FALSE;

        if ($type_default_value == 'voicebox') {
          $cf_title .= ': '. t('voicemail');
        }
        else {
          $cf_title .= ': '. $type_default_value;
        }
        $uri_default_value = '';
      }
    }
    else {
      $type_default_value = FALSE;
      $uri_default_value = '';
      $cf_title .= ': '. t('Not active');
    }
    
    $form['call-forwards'][$cf_type] = array(
      '#type' => 'container',
      '#title' => t('@cf_title', array('@cf_title' => $cf_title)),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
    );
    
    $form['call-forwards'][$cf_type]['destination_type'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $type_default_value,
      '#empty_option' => t('Choose an option'),
      '#required' => FALSE,
      '#attributes' => array('cf-type' => $cf_type, 
        'class' => array('chzn-select', 'sg-element', 'no-search', 'hpbx-enable-target-when-value', 'form-select'),
        'data-target' => 'hpbx-'. $cf_type .'-destination',
        'data-value' => 'uri',
      ),
      '#title' => t('Destination type'),
    );

    $subscriber_options = hpbx_get_subscriber_options($Customer, $Subscriber,
      $number_key=TRUE,
      $is_pbx_pilot=FALSE,
      $is_pbx_group= TRUE,
      $autoattendant=TRUE,
      $conference = FALSE,
      $is_subscriber = TRUE,
      $must_be_pbx_group_member = FALSE
    );

    $datalist = array();
    foreach ($subscriber_options as $type => $options) {
      foreach ($options as $value => $option) {
        $datalist[] = array('value' => $value, 'label' => $option);
      }
    }
    
    $datalist = json_encode($datalist);
    
    $form['call-forwards'][$cf_type]['destination_uri'] = array(
      '#type' => 'textfield',
      '#default_value' => $uri_default_value,
      '#title' => t('Destination'),
      '#attributes'=> array(
        'class' => array('sg-element', 'hpbx-autocomplete', 'hpbx-'. $cf_type .'-destination'),
        'data-values' => $datalist,
        'placeholder' => '',
      ),
    );

    if ($cf_type == 'cft') {

      // Only applies to CFT (Call Forward Timeout).
      $form['call-forwards'][$cf_type]['ringtimeout'] = array(
        '#type' => 'textfield',
        '#default_value' => !empty($Callforward->$cf_type->ringtimeout) ? $Callforward->$cf_type->ringtimeout : '',
        '#maxlength' => 3,
        '#size' => 3,
        '#title' => t('After (sec)'),
      );
    }
    else {
      $form['call-forwards'][$cf_type]['ringtimeout'] = array('#markup' => '');
    }
    
    $form['call-forwards'][$cf_type]['times'] = hpbx_get_simple_times_element($Callforward, $cf_type, $add_parent_foldable_list = TRUE);
    
    $form['call-forwards'][$cf_type]['times']['#title'] = t('Call forward active during');
  }
}


function hpbx_subscriber_js_feature_add($profiles) {

  $js_profile_features_toggle = array();
  foreach (array_keys($profiles) as $profile_id) {
    $js_profile_features_toggle[$profile_id] = array();
    foreach (array('cloud_pbx_callqueue', 'cfu', 'cft', 'cfb', 'cft', 'voice_mail', 'speed_dial', 'block_out_override_pin', 'block_in_clir', 'ncos', 'language', 'cli') as $attribute) {

      if (hpbx_attribute_allowed($attribute) === HPBX_SUBSCRIBER_FEATURE_TOGGLE) {
        if (hpbx_attribute_allowed($attribute, $profile_id) === TRUE) {
          $js_profile_features_toggle[$profile_id][] = $attribute;
        }
      }
    }

    if (count(array_intersect(array('cfu', 'cft', 'cfb', 'cft'), $js_profile_features_toggle[$profile_id])) == 4) {
      $js_profile_features_toggle[$profile_id][] = 'callforward';
    }
  }
  drupal_add_js(array('hpbx' => array(
  'profile_features_toggle' => $js_profile_features_toggle,
  )), 'setting');

}

/**
 * 
 * @param array $form
 * @param array $form_state
 *
 * @return array $form
 */
function hpbx_subscriber_edit_form_validate(&$form, &$form_state) {
  global $ActiveReseller, $user;

  // Add custom js.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  // Load sipwise entities.
  //$Customer = SipwiseEntity::load('customers', $form_state['values']['customer_id']);
  

  foreach (array('cfu', 'cfb', 'cfna', 'cft') as $cf_type) {
    if (isset($form_state['values']['callforwards_autcomplete_subscriber_options_'. $cf_type])) {
      hpbx_set_autocomplete('callforwards', "#edit-call-forwards-".$cf_type."-destination-uri", $form_state['values']['callforwards_autcomplete_subscriber_options_'. $cf_type]);
    }
  }

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

  //Modified for #9730 - BUILD: Multi-domain support
  //$Customer = hpbx_get_active_customer();
  $PilotSubscriber = $Customer->get_pilot_subscriber();
  $Domain = SipwiseEntity::load('domains', $PilotSubscriber->domain_id);
  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);
  // Get all pbx_extensions.
  $extensions = hpbx_get_extensions($Customer);

  $Subscriber = NULL;
  if ($form_state['values']['id']) {
    unset($extensions[$form_state['values']['id']]);

    // Existing subscriber.
    $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
  }

  if (!$form_state['values']['id']) {
    // Create new subscriber; incease profile_set_id and profile_id counter.
    if (hpbx_bundle_settings_is_allowed($Customer, $form_state['values']['general']['settings']['profile_id']['value'],
        HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE_SET|HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE) <= 0) {

      $SubscriberProfile = SipwiseEntity::load('subscriberprofiles', $form_state['values']['general']['settings']['profile_id']['value']);
      form_set_error('general][settings][profile_id][value', t('You have used up all your available @profile_name subscribers', array('@profile_name' => $SubscriberProfile->name)));
    }
  }
  else {
    // Update existing subscriber; keep profile_set_id counter and increase (new) profile_id counter.
    if ($Subscriber->profile_id!=$form_state['values']['general']['settings']['profile_id']['value']) {
      // Profile change; Profile counter will change; Profile set counter will not change.
      if (hpbx_bundle_settings_is_allowed($Customer, $form_state['values']['general']['settings']['profile_id']['value'], HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE) <= 0) {
        $SubscriberProfile = SipwiseEntity::load('subscriberprofiles', $form_state['values']['general']['settings']['profile_id']['value']);
        form_set_error('general][settings][profile_id][value', t('You have used up all your available @profile_name subscribers', array('@profile_name' => $SubscriberProfile->name)));
      }
    }
  }

//VTR extension validate
  if(empty($form_state['values']['general']['settings']['pbx_extension']['value'])){
     form_set_error('general][settings][pbx_extension][value', t('!name field is required.', array('!name' => t('Extension'))));	
  }
  
  if(!is_numeric($form_state['values']['general']['settings']['pbx_extension']['value'])){
     form_set_error('general][settings][pbx_extension][value', t('Extension field cannot have string values.'));	
  }
  // Modified for Bug #9746 - PE DEV: Create/Update subscriber/CR/HG/AA/fax - Extension should not be able to start with the digit for national calls
  // Verify PBX Extension.
   if (!empty($form_state['values']['general']['settings']['pbx_extension']['value'])) {
    hpbx_extension_validate('general][settings', $form_state['values']['general']['settings'], $Customer);
  }elseif (!empty($form_state['values']['general']['settings']['pbx_extension']['value']) &&
      in_array($form_state['values']['general']['settings']['pbx_extension']['value'], $extensions)) {
    form_set_error('general][settings][pbx_extension', t('Extension is already in use. Please select another extension.'));
  }

 

  if (empty($form_state['values']['general']['settings']['move_from_pilot']['value']) &&
      empty($form_state['values']['general']['settings']['no_selfcare']['value'])) {

	if (empty($form_state['values']['general']['settings']['firstname']['value'])) {
      form_set_error('general][settings][firstname][value', t('!name field is required.', array('!name' => t('First name'))));
    }
	elseif(preg_match('/[;]/', $form_state['values']['general']['settings']['firstname']['value'])){
	  form_set_error('general][settings][firstname][value', t('Semicolon is not allowed in !name.', array('!name' => t('First name'))));
	}
	//#9593 First name should be in UTF-8 charset
	elseif (!mb_check_encoding($form_state['values']['general']['settings']['firstname']['value'], 'UTF-8')) {
      form_set_error('general][settings][firstname][value', t('Only UTF-8 characters are allowed'));  
    }
    if (empty($form_state['values']['general']['settings']['lastname']['value'])) {
      form_set_error('general][settings][lastname][value', t('!name field is required.', array('!name' => t('Last name'))));
    }
	elseif(preg_match('/[;]/', $form_state['values']['general']['settings']['lastname']['value'])){
	  form_set_error('general][settings][lastname][value', t('Semicolon is not allowed in !name.', array('!name' => t('Last name'))));
	}
	//#9593 Last name should be in UTF-8 charset
	elseif (!mb_check_encoding($form_state['values']['general']['settings']['lastname']['value'], 'UTF-8')) {
      form_set_error('general][settings][lastname][value', t('Only UTF-8 characters are allowed')); 
    }
    if (empty($form_state['values']['general']['settings']['email']['value'])) {
      form_set_error('general][settings][email][value', t('!name field is required.', array('!name' => t('Email'))));
    }
    elseif (!valid_email_address($form_state['values']['general']['settings']['email']['value'])) {
      form_set_error('general][settings][email][value', t('The email address you specified is not valid.'));
    }
	//#9593 The whole display name may not exceed 125 characters
  	elseif ((strlen($form_state['values']['general']['settings']['firstname']['value']) + strlen($form_state['values']['general']['settings']['lastname']['value'])) > 123) {
      form_set_error('', t('The amount of characters entered for the First name and Last name exceeds the limit.'));
    }
    else {
      // Set lower case.
      $form_state['values']['general']['settings']['email']['value'] = strtolower($form_state['values']['general']['settings']['email']['value']);
    }
  }

  // Validate if the user is not already known.
  if (empty($form_state['values']['general']['settings']['no_selfcare']['value']) && 
    empty($form_state['values']['general']['settings']['move_from_pilot']['value']) && 
    isset($form_state['values']['general']['settings']) && !hpbx_users_validate_customer_user(
    $Customer,
    trim($form_state['values']['general']['settings']['firstname']['value']),
    trim($form_state['values']['general']['settings']['lastname']['value']),
    trim($form_state['values']['general']['settings']['email']['value']), $form_state['values']['id'])) {
  }

  if (!empty($form_state['values']['general']['settings']['no_selfcare']['value'])) {
    if (empty($form_state['values']['general']['settings']['external_id']['value'])) {
      form_set_error('general][settings][external_id][value', t('!name field is required.', array('!name' => t('Name'))));
    }
	elseif(preg_match('/[;]/', $form_state['values']['general']['settings']['external_id']['value'])){
	  form_set_error('general][settings][external_id][value', t('Semicolon is not allowed in !name.', array('!name' => t('Name'))));
	}
  //#9593 Name should not exceed 125 characters
    elseif (strlen($form_state['values']['general']['settings']['external_id']['value']) > 125) {
      form_set_error('general][settings][external_id][value', t('The entered Name may not exceed 125 characters.'));
    }
	//#9593 Name should be in UTF-8 charset
	  elseif (!mb_check_encoding($form_state['values']['general']['settings']['external_id']['value'], 'UTF-8')) {
      form_set_error('general][settings][external_id][value', t('Only UTF-8 characters are allowed'));
    }
  }
  
  // Remove values for properties not allowed within selected profile.
  foreach (array('clir', 'block_in_clir', 'block_out_override_pin', 'ncos', 'language', 'cli') as $element) {
    if (!hpbx_attribute_allowed($element, $form_state['values']['general']['settings']['profile_id']['value'])) {

      if (isset($form_state['values']['general']['settings'][$element])) {
        unset($form_state['values']['general']['settings'][$element]);
      }
    }
  }

  if (!empty($form_state['values']['general']['settings']['alias_number']['value'])) {

    // If passed remove the leading national digit.
    $ActiveReseller->remove_national_digits($form_state['values']['general']['settings']['alias_number']['value']);

    // Extra check to validate the passed aliasnumber is owned by this customer.
    if (!array_key_exists($form_state['values']['general']['settings']['alias_number']['value'], $Customer->get_free_numbers($Subscriber))) {
      form_set_error('general][settings][alias_number][value', t('Invalid number'));
    }

    // Verify the area code.
    $area = $Customer->get_area_code($form_state['values']['general']['settings']['alias_number']['value']);

    if ($ActiveReseller->_settings->execute_logic_for_area_code_dependant_emergency_suffix) {
      if (!hpbx_get_emergency_index($area)) {
        form_set_error('', t('Invalid area code'));
      }
    }
  }
 
  if(!is_numeric($form_state['values']['general']['settings']['alias_number']['value'])){
    form_set_error('general][settings][alias_number][value', t('Number field is required'));
  }
  
  if(empty($form_state['values']['general']['settings']['alias_number']['value']) && $form_state['values']['general']['settings']['cli']['value']== 'default'){
      form_set_error('general][settings][cli][value', t('Invalid CLI'));
  }
  
   //  Validate callforwards.
  if(empty($form_state['values']['general']['settings']['alias_number']['value'])){
    $DDI = $form_state['values']['general']['settings']['cli']['value'];
  }else{
    $DDI = $form_state['values']['general']['settings']['alias_number']['value'];
  }
 
  foreach (array('cfu', 'cfb', 'cfna', 'cft') as $type) {
    foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
      if($form_state['values']['call-forwards'][$type]['destination_uri']==$SubscriberForCLI->pbx_extension){
        $form_state['values']['call-forwards'][$type]['destination_uri'] = $DDI . $SubscriberForCLI->pbx_extension;
      }
    }
  }
  foreach (array('cfu', 'cfb', 'cfna', 'cft') as $type) {

    if (isset($form_state['values']['call-forwards'][$type])) {
      if ($form_state['values']['call-forwards'][$type]['destination_type'] == 'uri') {
        
        if (empty($form_state['values']['call-forwards'][$type]['destination_uri'])) {
          form_set_error('call-forwards]['.$type.'][destination_uri', t('Destination number is required'));
        }
        elseif(!empty($DomainPreferences->adm_cf_ncos) && !hpbx_ncos_is_allowed($DomainPreferences->adm_cf_ncos, $form_state['values']['call-forwards'][$type]['destination_uri'], $form_state['values']['id'])) {
          form_set_error('call-forwards]['.$type.'][destination_uri', t('Not allowed to set this destination number'));
        } 
        elseif(!empty($form_state['values']['general']['settings']['ncos']['value']) && !hpbx_ncos_is_allowed($form_state['values']['general']['settings']['ncos']['value'], $form_state['values']['call-forwards'][$type]['destination_uri'], $form_state['values']['id'])) {
          form_set_error('call-forwards]['.$type.'][destination_uri', t('Not allowed to set this destination number'));
        }
        elseif(!preg_match('/^\+?[0-9]+$/', $form_state['values']['call-forwards'][$type]['destination_uri'])) {
          form_set_error('call-forwards]['.$type.'][destination_uri', t('Please enter a valid destination number'));
        }
        else {

          // Replace '+' sign with 00.
          $form_state['values']['call-forwards'][$type]['destination_uri'] = str_replace('+', '00', $form_state['values']['call-forwards'][$type]['destination_uri']);
        }
      }
      if ($type == 'cft' && !empty($form_state['values']['call-forwards'][$type]['destination_type']) && empty($form_state['values']['call-forwards']['cft']['ringtimeout'])) {
        form_set_error('call-forwards][cft][ringtimeout', t('Please enter a valid ring time out'));
      }
      
      $form_state['values']['call-forwards'][$type]['times_api'] = array();
      
      // Format the ranges set by the range sliders.
      if (isset($form_state['values']['call-forwards'][$type]['times'])) {
        
        $time_ranges = $form_state['values']['call-forwards'][$type]['times'];
  
        $form_state['values']['call-forwards'][$type]['times_api'] = hpbx_api_times_get($form_state['values']['call-forwards'][$type]['times']);
      }
    }
  }
  
  // Verify the speeddials.
  if (isset($form_state['values']['speeddials']['table']['slots'])) {

    $slots = &$form_state['values']['speeddials']['table']['slots'];
    
    $used_slots = array();
    
    // Walk all speeddials.
    foreach ($slots as $id => &$speeddial) {
      foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
       if($speeddial['row']['destination']==$SubscriberForCLI->pbx_extension){
        $speeddial['row']['destination'] = $DDI . $SubscriberForCLI->pbx_extension;
       }
      }
      if ($speeddial['row']['destination']!='' && $speeddial['row']['slot']=='') {
        form_set_error('speeddials][table][slots]['.$id.'][row][destination', t('Please choose a slot for the speed dial destination.'));
        $form['#rebuild'] = TRUE;
      }
      
      if ($speeddial['row']['destination'] !='') {
        
        $used_slots[$id] = $speeddial['row']['slot'];
        
        // Replace '+' sign with 00.
        $speeddial['row']['destination'] = str_replace('+', '00', $speeddial['row']['destination']);
        
        if (preg_match('/\D/', $speeddial['row']['destination'])) {
          form_set_error('speeddials][table][slots]['.$id.'][row][destination', t('Only numeric characters are allowed as destination number.'));
          $form['#rebuild'] = TRUE;
        }
        elseif(!empty($form_state['values']['general']['settings']['ncos']['value']) && !hpbx_ncos_is_allowed($form_state['values']['general']['settings']['ncos']['value'], $speeddial['row']['destination'], $form_state['values']['id'])) {
          form_set_error('speeddials][table][slots]['.$id.'][row][destination', t('Not allowed to set this destination number'));
          $form['#rebuild'] = TRUE;
        }
      }
    }
    foreach (array_keys(array_diff_key($used_slots, array_unique($used_slots))) as $id) {
      form_set_error('speeddials][table][slots]['.$id.'][row][destination', t('Options need to be unique'));
      $form['#rebuild'] = TRUE;
    }
  }

  if (!empty($form_state['values']['voicemail']['pin']['value']) && (!ctype_digit($form_state['values']['voicemail']['pin']['value']) || strlen($form_state['values']['voicemail']['pin']['value'])!=4)) {
    form_set_error('voicemail][pin][value', t('The voicemail PIN should consist of exactly 4 numbers'));
  }

  // Verify the voicemail settings.
  if (!empty($form_state['values']['voicemail']['email']['value'])) {

    if (!valid_email_address($form_state['values']['voicemail']['email']['value'])) {
      form_set_error('voicemail][email][value', t('Please provide a valid email address'));
    }
    
    // In case the delete checkbox is unchecked and the attachment must be sent.
    if (!empty($form_state['values']['voicemail']['delete']['value'])) {
      $form_state['values']['voicemail']['attach']['value'] = TRUE;
    }
  }
  else {
    $form_state['values']['voicemail']['delete']['value'] = FALSE;
    $form_state['values']['voicemail']['attach']['value'] = FALSE;
  }


  // Set location id in case polish emergency routing is active for this customer / reseller.
  if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing) {

    $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
    if (empty($emergency_indexes[$Customer->_settings->region][$Customer->_settings->township][$Customer->_settings->city][$Customer->_settings->z_gm])) {
      form_set_error(NULL, t('An internal error occurred'));
      watchdog('hpbx_subscriber', t('Could not find location id for passed polish emergency routing arguments'));
    }
  }
}


/**
 * Helper function to determine the first usable private linerange. Will be
 * used at the wizard and import.
 *
 * @param integer $profile_id
 *
 * @return object|boolean
 */
function hpbx_subscriber_get_private_linerange($profile_id) {

  $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($profile_id);
  if (isset($PBXDeviceModel->linerange)) {
    foreach ($PBXDeviceModel->linerange as $range) {
      if ($range->can_private && $range->num_lines) {
        return $range;
      }
    }
  }
  return FALSE;
}


/**
 *
 * @param array $form
 * @param array $form_state
 *
 * @return array $form
 */
function hpbx_subscriber_edit_form_submit($form, $form_state) {

  global $ActiveReseller;

  try {

    $values = $form_state['values']['general']['settings'];

    // Get customer id.
    if (!empty($values['customer_id']['value'])) {
      $customer_id = $values['customer_id']['value'];
    }
    elseif (!empty($form_state['values']['customer_id'])) {
      $customer_id = $form_state['values']['customer_id'];
    }
    else {
      drupal_set_message(t('Can\'t find the customer ID'), 'error');
      return;
    }

    // Load customer.
    if (!$Customer = SipwiseEntity::load('customers', $customer_id)) {
      drupal_set_message(t('Can\'t find customer'));
      return;
    }

    if (!$CustomerPreferences = SipwiseEntity::load('customerpreferences', $Customer->id)) {
      drupal_set_message(t('Can\'t find customer preferences'));
      return;
    }

    // Load Pilot Subscriber.
    if (!$PilotSubscriber = $Customer->get_pilot_subscriber()) {
      drupal_set_message(t('Can\'t find the pilot subscriber'));
      return;
    }

    // Load Pilot Subscriber.
    if (!$PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id)) {
      drupal_set_message(t('Can\'t find the pilot subscriber preferences'));
      return;
    }

    // Verify an subscriber_id is set.
    if (!isset($form_state['values']['id']) || !$form_state['values']['id']) {

      // New subscriber.
      $Subscriber = SipwiseEntity::create('subscribers');
      $Subscriber->webusername = uniqid((int) $Customer->ids);
      $Subscriber->password = hpbx_subscriber_password($Customer->id);
      $Subscriber->customer_id = $Customer->id;
      $Subscriber->domain_id = $PilotSubscriber->domain_id;

      // Set the Subscriber username to a random value.
      $Subscriber->username = uniqid((int) $Customer->id);
	  
	  //Modified for Bug #9748 - BUILD: Change request - Set web password during create/update subscriber
	  //set the random id length 
	  $rnd_id_length = 10; 

	  //generate a random id encrypt it and store it in $rnd_id 
	  $rnd_id = crypt(uniqid(rand(),1),''); 

	  //to remove any slashes that might have come 
	  $rnd_id = strip_tags(stripslashes($rnd_id)); 

	  //Removing any . or / and reversing the string 
	  $rnd_id = str_replace(".","",$rnd_id); 
	  $rnd_id = strrev(str_replace("/","",$rnd_id)); 

	  //finally I take the first 10 characters from the $rnd_id 
	  $rnd_id = strtolower(substr($rnd_id,0,$rnd_id_length));
	  
	  // Set the Subscriber webpassword to a random value.
	  $Subscriber->webpassword = $rnd_id;		

    }
    else {

      // Existing subscriber.
      $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
    }
	//Modified for Bug #9744 - PE DEV: Edit subscriber - When changing from a subscriber without an account to a subscriber with an account, clear the External # field in the NGCP
	$Subscriber->external_id = '';
    $Subscriber->status = 'active';
    $Subscriber->profile_set_id = $Customer->_settings->profile_set_id;

    $profile_changed = FALSE;
    if ((int) $values['profile_id']['value'] != $Subscriber->profile_id) {
      $profile_changed = TRUE;
    }
    $Subscriber->profile_id = (int) $values['profile_id']['value'];
    $Subscriber->pbx_extension = trim($values['pbx_extension']['value']);

    if (!hpbx_attribute_allowed('huntgroup_member', $Subscriber->profile_id)) {
      unset($Subscriber->pbx_group_ids);
    }

    // If no account needs to be created, we will set the subscriber reference
    // to the firstname (renamed in GUI to name).
    if (!empty($form_state['values']['general']['settings']['no_selfcare']['value']) && !empty($values['external_id']['value'])) {
      $Subscriber->external_id = trim($values['external_id']['value']);
    }

    $Subscriber->primary_number = new stdClass();
    $Subscriber->primary_number->ac = $PilotSubscriber->primary_number->ac;
    $Subscriber->primary_number->cc = $PilotSubscriber->primary_number->cc;
    $Subscriber->primary_number->sn = $PilotSubscriber->primary_number->sn . $Subscriber->pbx_extension;

    if (!empty($values['administrative']['value'])) {
      $Subscriber->administrative = boolval($values['administrative']['value']);
    }
    else {
      unset($Subscriber->administrative);
    }

    $an_change = FALSE;
    $alias_number_original = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
     $alias_no_original = $values['alias_number']['value'];
    if(empty($values['alias_number']['value']) && isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
      $values['alias_number']['value'] = $values['cli']['value'].$values['pbx_extension']['value'];
    } 
    if(isset($ActiveReseller->_settings->digit_for_national_calls)){
      $values['alias_number']['value'] = $ActiveReseller->_settings->digit_for_national_calls . $values['alias_number']['value'] ;
    }

    // Verify if the alias number has changed.
    if ($values['alias_number']['value'] != $alias_number_original) {

      // It has changed.
      $an_change = TRUE;

      // Remove the new alias number from the pilot subscriber.
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
      
      if(strlen($values['alias_number']['value']) >= $min_length && strlen($values['alias_number']['value']) <= $max_length && !empty($alias_no_original)){
        $PilotSubscriber->alias_number_unset($Customer, $values['alias_number']['value']);
      }

      // Set the new alias number on the subscriber.
      $Subscriber->alias_number_set($Customer, $values['alias_number']['value']);
    }
	$ActiveReseller->remove_national_digits($values['alias_number']['value']);
    // If the  move-to-pilot flag  is set,  clear the webusername from  the 
    // pilot subscriber and move that value to the  created subscriber. Now 
    // the currently logged in user will be assigned to the subscriber that
    // is created.
    if (!$values['no_selfcare']['value'] && !empty($values['move_from_pilot']['value'])) {
      $Subscriber->administrative = TRUE;
      $Subscriber->webusername = $PilotSubscriber->webusername;
      unset($PilotSubscriber->webusername);
    }

    // Save the pilot subscriber and subscriber.
    $PilotSubscriber->save();

    // In case storing the Subscriber fails we need to restore the alias
    // number within the catched exception.
    $restore_alias = TRUE;

    if ($values['no_selfcare']['value']) {
      $Subscriber->administrative = FALSE;
    }

    $Subscriber->save();

    $_SESSION['ccs']['hpbx']['wizard']['subscriber_id'] = $Subscriber->id;

    // Saving the subscriber succeeded, so don't restore the alias number.
    $restore_alias = FALSE;

    // Add the original alias number to the pilot number again, so it will
    // be available to others subscribers to be used.
    if ($an_change && !empty($alias_number_original)) {

      $PilotSubscriber->alias_number_set($Customer, $alias_number_original, FALSE);
      $PilotSubscriber->save();
    }

    // Load the subscriberpreferences.
    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
    $SubscriberPreferences->concurrent_max_out = 2;
    $SubscriberPreferences->concurrent_max = 3;

    // Set callbarring level.
    if (!empty($values['ncos']['value'])) {

      $SubscriberPreferences->ncos = $values['ncos']['value'];
    }
    else {
      unset($SubscriberPreferences->ncos);
    }

    // Set call barring override pin.
    if (!empty($values['block_out_override_pin']['value'])) {

      $SubscriberPreferences->block_out_override_pin = trim($values['block_out_override_pin']['value']);
    }
    else {
      unset($SubscriberPreferences->block_out_override_pin);
    }

   if (isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
     // CLI Restriction.
    if (isset($values['cli']['value']) && is_numeric($alias_no_original) && $alias_no_original!=0) {
      
      	if ($values['cli']['value'] == 'default') {

        // Set the CLI based on own alias_number.
          $SubscriberPreferences->user_cli = $SubscriberPreferences->cli = $SubscriberPreferences->gpp0 = $ActiveReseller->_settings->country_code . $values['alias_number']['value'];
        }

      // Other subscriber.
        elseif (is_numeric($values['cli']['value'])) {
          $SubscriberPreferences->cli = $SubscriberPreferences->gpp0 = $ActiveReseller->_settings->country_code . $values['alias_number']['value'];
          $SubscriberPreferences->user_cli =  $ActiveReseller->_settings->country_code . substr($values['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
        }
        
    	$SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;

        // Get area number
        $area = $Customer->get_area_code($values['alias_number']['value']);

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
      
      if (in_array($ActiveReseller->_settings->country_code . $values['alias_number']['value'],
        $CustomerPreferences->allowed_clis)) {
        $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
      }
      else {
        $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
      }	
	      
    }// && $values['alias_number']['value']==0
    elseif(isset($values['cli']['value']) && is_numeric($values['cli']['value'])){
    	
      $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr($values['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
      
      //Set the gpp0 value equal to the aliasnumber for billing purposses.
      $SubscriberPreferences->gpp0  = $ActiveReseller->_settings->country_code . substr($values['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
      $SubscriberPreferences->cli =  $ActiveReseller->_settings->country_code . substr($values['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls)) . $values['pbx_extension']['value'];
      $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;

      // Get area number
      $area = $Customer->get_area_code($values['cli']['value']);

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
      if (in_array($ActiveReseller->_settings->country_code . $values['cli']['value'],
        $CustomerPreferences->allowed_clis)) {

        $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
      }
      else {
        $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
      }		
      
	}
    else {
     // Set the pilot subscriber primary number as cli.      
      $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr(hpbx_parse_primary_number($PilotSubscriber->primary_number), strlen($ActiveReseller->_settings->digit_for_national_calls));
    }
    if (!empty($values['clir']['value'])) {
      $SubscriberPreferences->clir = (bool) $values['clir']['value'];
    }
    else {
      $SubscriberPreferences->clir = FALSE;
    }
    $SubscriberPreferences->display_name = trim($Subscriber->pbx_extension);
   }else{
      if (!empty($values['alias_number']['value'])) {

      $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;

      // Get area number
      $area = $Customer->get_area_code($values['alias_number']['value']);

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
    }
    // CLI Restriction.
    if (isset($values['cli']['value'])) {

      if ($values['cli']['value'] == 'default' || $values['cli']['value'] == 'clir') {

        // Set the CLI based on own alias_number.
        $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . $values['alias_number']['value'];
      }

      // Other subscriber.
      elseif (is_numeric($values['cli']['value'])) {
        $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr($values['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));
      }

      // Set clir based on value == clir.
      $SubscriberPreferences->clir = (bool) ($values['cli']['value'] === 'clir');
    }
    elseif (!empty($values['alias_number']['value'])) {

      // Set the own (alias_number) as cli.
      $SubscriberPreferences->clir = FALSE;
      $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . $values['alias_number']['value'];
    }
    else {
      // Set the pilot subscriber primary number as cli.
      $SubscriberPreferences->clir = FALSE;
      $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr(hpbx_parse_primary_number($PilotSubscriber->primary_number), strlen($ActiveReseller->_settings->digit_for_national_calls));
    }
     $SubscriberPreferences->cli = $SubscriberPreferences->gpp0 = $ActiveReseller->_settings->country_code . $values['alias_number']['value'];

    // To support routing for non-ported numbers to peer, which is necessary
    // to ensure  that calling  non-ported numbers  from HPBX will be routed
    // correctly  (to the peer), we  will set force_inbound_calls_to_peer to
    // TRUE in case the number is not yet ported or to FALSE when the number
    // is ported. To verify if the number is ported, we will use the allowed
    // -clis variable, which is filled based on the inported checkbox at the
    // customer edit form.
    if (in_array($ActiveReseller->_settings->country_code . $values['alias_number']['value'],
      $CustomerPreferences->allowed_clis)) {

      $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
    }
    else {
      $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
    }
    if ((empty($values['no_selfcare']['value'])) && (!empty($values['firstname']['value'])) && (!empty($values['lastname']['value']))) {
            $SubscriberPreferences->display_name = trim($values['lastname']['value'].", ".$values['firstname']['value']);
        } elseif ((!empty($values['no_selfcare']['value'])) && (!empty($values['external_id']['value']))) {
      //Modified for Bug#9743 - BUILD: Change request - Set display_name for AA/CR/Fax for create/update use cases
            $SubscriberPreferences->display_name = $Subscriber->external_id;
        }
   }
    // Anonymouse call reject.
    if (!empty($values['block_in_clir']['value'])) {
      $SubscriberPreferences->block_in_clir = (bool) $values['block_in_clir']['value'];
    }
    else {
      $SubscriberPreferences->block_in_clir = FALSE;
    }

    // Set the gpp0 value equal to the aliasnumber for billing purposses.
   
    $SubscriberPreferences->gpp1 = $Subscriber->pbx_extension;

    // Set announcement language.
    if (!empty($values['language']['value'])) {
      $SubscriberPreferences->language = $values['language']['value'];
    }
    else {
      unset($SubscriberPreferences->language);
    }

    // Try to retrieve music-on-hold sound file from the pilot subscriber.
    $pilot_soundfiles = hpbx_get_soundfiles_from_soundset($HuntGroup->customer_id, $PilotSubscriberPreferences->contract_sound_set, 'music_on_hold');

    // Set the subscriber soundset.
    $SubscriberPreferences->contract_sound_set = 'Subscriber ' . $Subscriber->id;

    // Get or create the subscriber SoundSet.
    $SoundSet = hpbx_soundset_get_subscriber_soundset($Subscriber,
      $SubscriberPreferences->contract_sound_set);

    // Verify if callqueue is allowed.
    if (hpbx_attribute_allowed('cloud_pbx_callqueue', $Subscriber->profile_id)) {

      // Verify if the profile has changed and on this new profile and if a
      // music on hold soundfile is available.
      if ($profile_changed && isset($Subscriber->profile_id) &&
        isset($pilot_soundfiles['music_on_hold']) && $SoundSet
      ) {

        // Copy the music-on-hold soundfile from pilot subscriber soundfile.
        // to the subscriber soundset (as queue_waiting_music).
        hpbx_soundfile_copy($pilot_soundfiles['music_on_hold'], $SoundSet, 'queue_waiting_music');
      }

      // Queue (we need to check on the input array, as the elements are
      // disabled using custom Trimm js) and due to that set on values
      // and not set on input.
      if (isset($form_state['input']['general']['queue']['cloud_pbx_callqueue']['value'])
        && !is_null($form_state['input']['general']['queue']['cloud_pbx_callqueue']['value'])
      ) {

        $SubscriberPreferences->max_queue_length = (int) $form_state['values']['general']['queue']['max_queue_length']['value'];
        $SubscriberPreferences->queue_wrap_up_time = (int) $form_state['values']['general']['queue']['queue_wrap_up_time']['value'];
        $SubscriberPreferences->cloud_pbx_callqueue = TRUE;
      }
      else {
        $SubscriberPreferences->cloud_pbx_callqueue = FALSE;
        unset($SubscriberPreferences->max_queue_length,
          $SubscriberPreferences->queue_wrap_up_time);
      }

      // Check if the original queue_greeting file was deleted, we need to delete
      // it (and maybe upload a new file afterwords. 
      if (isset($form_state['input']['hpbx-audio-is-replaced-queue-greeting'])) {

        foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
          if ($SoundFile->handle == 'queue_greeting') {
            $SoundFile->delete();
          }
        }
      }

      // Upload the queue_greeting soundfile upload.
      if ($SubscriberPreferences->cloud_pbx_callqueue && isset($_FILES['queue_greeting'])) {
        $soundset_name = 'Subscriber ' . $Subscriber->id;

        if (hpbx_soundset_upload_file($Subscriber, $soundset_name)) {
          $SubscriberPreferences->contract_sound_set = $soundset_name;
        }
      }
    }
    else {
      $SubscriberPreferences->cloud_pbx_callqueue = FALSE;
      unset($SubscriberPreferences->max_queue_length,
        $SubscriberPreferences->queue_wrap_up_time);
    }

    if (hpbx_attribute_allowed('music_on_hold', $Subscriber->profile_id) === TRUE) {

      // Clone music-on-hold sound file assigned to the soundset for  the
      // pilotsubscriber to a new soundfile which will be attached to the
      // soundset for this subscriber.

      // Verify if the music_on_hold sound file is present.
      if (isset($pilot_soundfiles['music_on_hold'])) {

        // Copy the music_on_hold soundfile to this subscriber contract_sound_set.
        if (hpbx_soundfile_copy($pilot_soundfiles['music_on_hold'], $SoundSet)) {
          $SubscriberPreferences->music_on_hold = TRUE;
        }
      }
    }
    else {
      $SubscriberPreferences->music_on_hold = FALSE;
    }
	
	//Modified for #9735 - BUILD: Change request - Set softphone_autoprov based on subscriber profile
	// Verify if softphone_autoprov is allowed.
	if (hpbx_attribute_allowed('softphone_autoprov', $Subscriber->profile_id) === TRUE) {      
      $SubscriberPreferences->softphone_autoprov = TRUE;       
    }
    else {
      $SubscriberPreferences->softphone_autoprov = FALSE;
    }


    $allsite = json_decode(json_encode($Customer->_settings->multisite),true);
    foreach($allsite as $key=>$sites){
      if($sites['site_name'] == $form_state['values']['general']['settings']['site']['value']){
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
    
    // Save the subscriber preferences.
    $SubscriberPreferences->save();

    if ($Subscriber->administrative) {
      $role = 'HostedPBX Customer Administrator';
    }
    else {
      $role = 'HostedPBX Subscriber';
    }

    // When the move-from-pilot flag is NOT set
    if (empty($values['move_from_pilot']['value'])) {

      // If no_selfcare flag IS NOT set.
      if (empty($form_state['values']['general']['settings']['no_selfcare']['value'])) {

        // ... and if this is a new user, create the AD user as well.
        if (!$form_state['values']['id']) {

          if ($account = hpbx_users_create_customer_user($Customer, $Subscriber->id,
            $values['email']['value'], $values['firstname']['value'], $values['lastname']['value'], $role)
          ) {

            // Now find the uid (==username) of the AD and Drupal user.
            $Subscriber->webusername = $account->uid;
            $Subscriber->save();
          }
          else {
            drupal_set_message(t('Failed to create user'), 'error');

            // Rollback subscriber.
            $Subscriber->delete();
            unset($Subscriber);
            return;
          }
        }
        // ...update the existing ad user.
        else {
          // Existing user, we have to update the AD entry.
          if (!hpbx_users_update_user($Subscriber->webusername, $values['email']['value'], $values['firstname']['value'], $values['lastname']['value'], $role)) {

            // Update failed; Will create the user account within AD and Drupal.
            if ($account = hpbx_users_create_customer_user($Customer, $Subscriber->id, $values['email']['value'], $values['firstname']['value'], $values['lastname']['value'], $role)) {
              $Subscriber->webusername = $account->uid;
              $Subscriber->save();
            }
            else {
              drupal_set_message(t('Failed to update user'), 'error');
            }
          }
        }
      }
      else {
        // No account was required for this subscriber, so simply we won't
        // create a new ad account.

        if (!empty($Subscriber->webusername)) {

          // Remove the active directory account in case it exists.
          if ($Subscriber->get_active_directory_entry()) {
            hpbx_users_remove($Subscriber);
          }
        }
      }
    }
    elseif (!$values['no_selfcare']['value']) {
      // In case the no selfcare account checkbox IS NOT checked; AD account is
      // moved from pilot subscriber to regular subscriber.
      global $user;
      $user->data['sipwise']['subscriber_id'] = $Subscriber->id;
      user_save($user);
    }

    // Set default callforwards when creating an new subscriber.
    if (empty($form_state['values']['id'])) {

      // Set default values for callfowords.
      $form_state['values']['call-forwards']['cfu']['destination_type'] = '';
      $form_state['values']['call-forwards']['cfu']['destination_timeout'] = '';

      $form_state['values']['call-forwards']['cfb']['destination_type'] = 'voicebox';
      $form_state['values']['call-forwards']['cfb']['destination_timeout'] = 300;

      $form_state['values']['call-forwards']['cfna']['destination_type'] = 'voicebox';
      $form_state['values']['call-forwards']['cfna']['destination_timeout'] = 300;

      $form_state['values']['call-forwards']['cft']['destination_type'] = 'voicebox';
      $form_state['values']['call-forwards']['cft']['destination_timeout'] = 300;
    }

    // Load Subscriber.
    $Subscriber = SipwiseEntity::load('subscribers', $Subscriber->id);

    // Load CallForwards.
    $Callforwards = SipwiseEntity::load('callforwards', $Subscriber->id);

    // Walk each call forward type and set the objects' values.
    foreach (array('cfu', 'cfb', 'cfna', 'cft') as $type) {

      hpbx_set_simple_callforward_values($type, $form_state, $Callforwards, $Subscriber);
    }

    // Save the callforwards.
    $Callforwards->save();

    // Set settings for an existing subscribers.
    if (!empty($form_state['values']['id'])) {

      // Save voicemail settings.
      if ($VoicemailSettings = SipwiseEntity::load('voicemailsettings', $form_state['values']['id'])) {
        
        $VoicemailSettings->pin = trim($form_state['values']['voicemail']['pin']['value']);
        $VoicemailSettings->email = trim($form_state['values']['voicemail']['email']['value']);
        $VoicemailSettings->delete = boolval($form_state['values']['voicemail']['delete']['value']);
        $VoicemailSettings->attach = boolval($form_state['values']['voicemail']['attach']['value']);
        $VoicemailSettings->save();
      }

      // Only save the speedials in case it is allowed by the profile
      // configuration to use speed dials.
      if (!hpbx_attribute_allowed('speed_dial', $Subscriber->profile_id) === TRUE) {

        // Empty the speed dials. As this use is not allowed to use them.
        $SpeedDials = SipwiseEntity::load('speeddials', $form_state['values']['id']);
        $SpeedDials->speeddials = array();
        $SpeedDials->save();
      }
      // Save speed dials settings.
      elseif (isset($form_state['values']['speeddials']['table']['slots'])) {

        $slots = &$form_state['values']['speeddials']['table']['slots'];
        
        $SpeedDials = SipwiseEntity::load('speeddials', $form_state['values']['id']);
        $SpeedDials->speeddials = array();
        $sd = &$SpeedDials->speeddials;

        foreach ($slots as $speeddial) {
          if ($speeddial['row']['slot']!=='' && $speeddial['row']['destination']!='') {
            $sd[] = array(
              'slot' => $speeddial['row']['slot'],
              'destination' => sipwise_api_normalize_number($Subscriber->id, $speeddial['row']['destination']),
            );
          }
        }
        $SpeedDials->save();
      }
      
      
      
      // If the subscriber profile changed from profile X to profile Y, verify if this
      // specific  user is  assigned to BLF key  on one or  more devices. If  this  is
      // the case, remove that specific line from the device.
      if (!hpbx_attribute_allowed('blf', $Subscriber->profile_id)) {
        foreach (sipwise_api_get_all('pbxdevices', array('customer_id' => $Customer->id)) as $PBXDevice) {
          $save_pbxdevice = FALSE;
          foreach ($PBXDevice->lines as $line_key => $line) {
            if ($Subscriber->id == $line->subscriber_id && $line->type == 'blf') {
              watchdog('hpbx_subscriber', 'Subscriber '. $Subscriber->id .  ' removed from blf line on pbxdevice id '. $PBXDevice->id);
              unset($PBXDevice->lines[$line_key]);
              $save_pbxdevice = TRUE;
            }
          }
          
          if ($save_pbxdevice) {
            $PBXDevice->lines = array_values($PBXDevice->lines);
            $PBXDevice->save();
          }
        }
        
      }
    }
  }
  catch (Exception $e) {

    watchdog('hpbx_subscriber', $e->getMessage());
    
    // Failed to save subscriber.
    drupal_set_message(t('Failed to update'), 'error');


    if ($restore_alias) {
      
      try {
        // Something went wrong, so add the alias number to the PilotSubscriber again.
        $PilotSubscriber->alias_number_set($Customer, $alias_no_original, FALSE);
        $PilotSubscriber->save();
      }
      catch (Exception $ee) {
        
        watchdog('hpbx', t('Failed to restore the alias_number on the PilotSubscriber. Exception: @e', array('@e' => $ee->getMessage())));
        drupal_set_message(t('Failed to restore the number'), 'error');
      }
    }
    $catched = TRUE;
  }
  
  
  foreach (array('subscribers', 'subscriberpreferences', 'speeddials', 'callforwards', 'voicemailsettings') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $Subscriber->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));

    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }
  
  if (!isset($catched)) {
    drupal_set_message(t('User successfully saved'));
  }

  if (empty($form_state['values']['no_redirect'])) {

    if (user_access('hpbx edit own subscriber settings') && arg(1) == 'settings') {
      drupal_goto('hpbx/settings');
    }
    elseif (user_access('hpbx edit subscriber')) {
      drupal_goto('hpbx/subscribers');
    }
    else {
      drupal_goto('hpbx');
    }
  }
}

/**
 * 
 * @param array $form
 * @param array $form_state
 */
function hpbx_subscriber_phonenumber_callback($form, $form_state) {
  return $form['general']['extension']['value'];
}

/**
 * 
 * @param string $subscriber_id
 * @param string $remove_ad
 */
function hpbx_subscriber_delete($subscriber_id, $remove_ad = FALSE) {

  global $ActiveReseller;
  try {
    // Verify if the subscriber_id does exist.
    if ($Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {

      // Get the active customer.
      $Customer = hpbx_get_active_customer();

      // Verify if the subscriber to be deleted is owned by the active customer.
      if ($Customer->id != $Subscriber->customer_id) {
        drupal_not_found();
        exit();
      }

      // Load the pilot subscriber.
      $PilotSubscriber = $Customer->get_pilot_subscriber();
      $PilotSubscriberPreferences =  SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id);
      $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);

      // Get the subscriber alias numbers, as it has to be moved back to the
      // pilot subscriber.
      $alias_number_original = FALSE;
      if (!is_null($Subscriber)) {
        $alias_number_original = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
      }

      if ($Subscriber->delete()) {

        // Verify if the subscriber soundset is not 'owned' by the pilotsubscriber (in case of 'old' moh soundsets).
        if ($PilotSubscriberPreferences->contract_sound_set != $SubscriberPreferences->contract_sound_set) {
          if ($SoundSet = hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {
            
            // Remove the SoundSet.
            watchdog('hpbx_subscriber_delete', 'Will remove soundset '. $SubscriberPreferences->contract_sound_set);
            
            if (!$SoundSet->delete()) {
              watchdog('hpbx_subscriber_delete', 'Failed to remove soundset');
            }
          }
          else {
            watchdog('hpbx_subscriber_delete', 'Soundset '. $SubscriberPreferences->contract_sound_set . ' not found');
          }
        }
        else {
          watchdog('hpbx_subscriber_delete', 'Will not remove pilotsubscriber soundset');
        }

        if ($remove_ad) {

          // Delete the subscriber's AD account.
          hpbx_users_remove($Subscriber); 
        }

        // if (!empty($alias_number_original)) {
        //   $PilotSubscriber->alias_number_set($Customer, $alias_number_original, FALSE);
        //   $PilotSubscriber->save();
        // }
      }
      else {
        drupal_set_message(t('Failed to remove the subscriber'));
      }

      foreach (array('subscribers', 'subscriberpreferences', 'speeddials', 'callforwards', 'voicemailsettings') as $type) {
        // Clear the Drupal cache for this API type.
        $cid = $type . '_'. $ActiveReseller->id . '_'. $Subscriber->id.'_response';
        cache_clear_all($cid, 'cache_sipwise_api');
        watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));
      
        $cid = $type . '_list_'. $ActiveReseller->id.'_';
        cache_clear_all($cid, 'cache_sipwise_api', TRUE);
        watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
      }

      // Set customer_id.
      $query = array('customer_id' => $Customer->id);
      foreach (sipwise_api_get_all('pbxdevices', $query, TRUE, TRUE) as $PBXDevice) {

        foreach ($PBXDevice->lines as $line) {
          if ($line->subscriber_id == $subscriber_id) {
            // Clear the Drupal cache for this API type.
            $type = 'pbxdevice';
            $cid = $type . '_'. $ActiveReseller->id . '_'. $PBXDevice->id.'_response';
            cache_clear_all($cid, 'cache_sipwise_api');
            watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));
            break;
          }
        }
      }

      // Verify if the subscriber is assigned to a device, clear the cache for
      // that device.
    }
  }
  catch (Exception $e) {
    drupal_set_message(t($e->getMessage()), 'error');
  }
}


/**
 * 
 * @param string $subscriber_id
 */
function hpbx_subscriber_delete_page($subscriber_id) {
  global $user;
  if ($Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {
    if ($user->data['sipwise']['subscriber_id'] == $Subscriber->id) {
      drupal_not_found();
      die();
    } 
    hpbx_subscriber_delete($subscriber_id, TRUE);
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
  drupal_goto('hpbx/subscribers');
}


/**
 * @param $add_link
 * @param boolean $fax
 *
 * @return array
 */
function hpbx_subscribers_get_subscribers_table($add_link = NULL, $fax = FALSE) {

  global $user, $ActiveReseller;
  $Customer = hpbx_get_active_customer();

  $url_prefix = ($fax) ? 'fax2email/' : 'subscriber/';

  $table = array(
    '#add' => !is_null($add_link) ? array('data' => $add_link, 'attributes' => array('class' => 'hpbx-table-caption')) : NULL,
    '#empty' => t('No entries found'),
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-subscribers hpbx-table-sortable'),
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
  );

  $table['#header'] = array(
    'name' => array('data'=> t('Name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
    'number' => array('data'=> t('Number'), 'class' => array('hpbx-sortable', 'hpbx-show')),
  );

  if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
    $table['#header']['pbx_extension'] = array('data'=> t('Extension'), 'class' => array('hpbx-sortable', 'hpbx-show-sm'));
  }

  if ($fax) {
    $table['#header']['mail_destination'] = array(
      'data' => t('Email destination'),
      'class' => array('hpbx-sortable', 'hpbx-show-xs')
    );
  }
  if (!$fax) {
    $table['#header']['mail'] = array(
      'data' => t('Login name'),
      'class' => array('hpbx-sortable', 'hpbx-show-xs')
    );
  }
  if (!$fax) {
    $table['#header']['admin'] = array(
      'data' => t('Admin'),
      'class' => array('hpbx-show-sm')
    );
  }
  if (!$fax) {
    $table['#header']['profile'] = array(
      'data' => t('Subscriber profile'),
      'class' => array('hpbx-sortable', 'hpbx-show-sm')
    );
  }

  $table['#remove_title'] = $fax ? t('Delete fax number') : t('Delete subscriber');

  $ProfileSets = array();
  foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
    $ProfileSets[$SubscriberProfileSet->id] = $SubscriberProfileSet->name;
  }

  $Profiles = array();
  foreach (sipwise_api_get_all('subscriberprofiles') as $SubscriberProfile) {
    $Profiles[$SubscriberProfile->id] = $SubscriberProfile->description;
  }

  $sort_names = array();
  $subscribers_table = array();

  foreach ($Customer->get_subscribers(FALSE, FALSE, FALSE, FALSE, boolval($fax), !$fax) as $Subscriber) {

    $ad_entry = $Subscriber->get_active_directory_entry();

    // Set tr data-url.
    $subscribers_table[$Subscriber->id]['#url'] = url('hpbx/'.      $url_prefix . $Subscriber->id . '/edit');
    $subscribers_table[$Subscriber->id]['#attributes']['data-delete-url'] = url('hpbx/'. $url_prefix . $Subscriber->id . '/delete');

    if ($user->data['sipwise']['subscriber_id'] == $Subscriber->id) {
      $subscribers_table[$Subscriber->id]['#attributes']['delete'] = FALSE;
    }

    if ($ad_entry) {
      $name_markup = array('#markup' => check_plain($ad_entry['sn'][0] . ', ' . $ad_entry['givenname'][0]));
      $mail_markup = array('#markup' => $ad_entry['mail'][0]);
    }
    elseif (!empty($Subscriber->external_id)) {
      $name_markup = array(
        '#markup' => hpbx_truncate_name_element($Subscriber->external_id),
        '#attributes' => array('title' => check_plain($Subscriber->external_id))
      );
      $mail_markup = array('#markup' => '');
    }
    else {
      $name_markup = array('#markup' => '');
      $mail_markup = array('#markup' => '');
    }

    $subscribers_table[$Subscriber->id]['name']['data'] = $name_markup;
    $subscribers_table[$Subscriber->id]['name']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');
    $sort_names[$Subscriber->id] = $subscribers_table[$Subscriber->id]['name']['data']['#markup'];

    if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
      $subscribers_table[$Subscriber->id]['number']['data'] = array('#markup' => $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN));
      $subscribers_table[$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');

      $subscribers_table[$Subscriber->id]['pbx_extension']['data'] = array('#markup' => !empty($Subscriber->pbx_extension) ? $Subscriber->pbx_extension : '');
      $subscribers_table[$Subscriber->id]['pbx_extension']['#attributes']['class'] = array('hpbx-show-sm hpbx-nowrap');
    }
    else {

       $number = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
        if(isset($number) && $number !=''){
        $number = substr($number, 0, $Customer->_settings->extension_length * -1) . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
        }else{
        $number = "<span style='padding-left:30px'>&nbsp</span>" . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
        }
      $subscribers_table[$Subscriber->id]['number']['data'] = array('#markup' => $number);
      $subscribers_table[$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');
    }

    if ($fax) {
      $mail_destination = '';
      if ($Subscriber->profile_id == $ActiveReseller->_settings->fax_subscriber_profile_id  && $FaxserverSettings = SipwiseEntity::load('faxserversettings', $Subscriber->id)) {
        $mail_destination = $FaxserverSettings->destinations[0]->destination;
      }
      $subscribers_table[$Subscriber->id]['mail_destination']['data'] = array('#markup' => $mail_destination);
      $subscribers_table[$Subscriber->id]['mail_destination']['#attributes']['class'] = array('hpbx-show-xs hpbx-nowrap');
    }
    if (!$fax) {
      $subscribers_table[$Subscriber->id]['mail']['data'] = $mail_markup;
      $subscribers_table[$Subscriber->id]['mail']['#attributes']['class'] = array('hpbx-show-xs hpbx-nowrap');
    }
    if (!$fax) {
      $subscribers_table[$Subscriber->id]['admin']['data'] = array('#markup' => $Subscriber->administrative ? '<span class="LGI-iconcheck"/>' : '');
      $subscribers_table[$Subscriber->id]['admin']['#attributes']['class'] = array('hpbx-show-sm');
    }
    if (!$fax) {
      $subscribers_table[$Subscriber->id]['profile']['data'] = array('#markup' => !empty($Subscriber->profile_id) ? t($Profiles[$Subscriber->profile_id]) : '');
      $subscribers_table[$Subscriber->id]['profile']['#attributes']['class'] = array('hpbx-show-sm');
    }
  }

  // Sort based on $sort_names.
  $a = array($subscribers_table, $sort_names);

  array_multisort($a[0], SORT_ASC, SORT_NUMERIC,
    $a[1], SORT_ASC, SORT_NATURAL|SORT_FLAG_CASE);
  $table= array_merge($table, $a[0]);

  return $table;
}


/**
 *
 * @param array $form
 * @param array $form_state
 * @param int $customer_id
 *
 * @return array $form
 */
function hpbx_subscribers_overview_form($form, $form_state) {

  drupal_set_title(t('Subscribers'));
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create subscriber') . '</h5>';
  $add_link = l($text, 'hpbx/subscriber/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form = array();
  $form['create-link']['#href'] = 'hpbx/subscriber/add';
  $form['table'] = hpbx_subscribers_get_subscribers_table($add_link);

  return $form;
}

/**
 * 
 * @return unknown|multitype:multitype:NULL
 */
function hpbx_subscriber_own_settings_page() {
  global $user;

  if (($Customer = hpbx_get_active_customer()) &&  ($Subscriber = hpbx_get_active_subscriber())) {

    if ($PilotSubscriber = $Customer->get_pilot_subscriber()) {
      if ($Subscriber->id != $PilotSubscriber->id) {
        $form = drupal_get_form('hpbx_subscriber_edit_form', $Subscriber->id);

        menu_set_active_item('hpbx/settings');

        return $form;
      }
      else {
        drupal_set_message(t('Create an administrative subscriber first'), 'error');
        drupal_goto('hpbx/subscribers');
      }
    }
    else {
      drupal_set_message(t('Create an administrative subscriber first'), 'error');
      drupal_goto('hpbx/subscribers');
    }
  }
  else {
    drupal_not_found();
    exit();
  }
}

/**
 * 
 * @param array $form
 * @param array $form_state
 */
function hpbx_subscriber_pbx_extension_callback($form, &$form_state) {
  return $form[$form_state['values']['alias_key']]['pbx_extension']['value'];
}

/**
 * 
 * @param array $form
 * @param array $form_state
 */
function hpbx_subscriber_move_from_pilot_callback(&$form, &$form_state) {

  // Validate the checkbox checked.
  if (isset($form_state['triggering_element']['#value']) &&
      $form_state['triggering_element']['#value']) {

    // Get customer.
    $Customer = hpbx_get_active_customer();

    // Get pilot subscriber.
    $PilotSubscriber = $Customer->get_pilot_subscriber();

    if ($ad_entry = hpbx_users_get_user($PilotSubscriber->webusername)) {
      $form['general']['firstname']['value']['#value'] = $ad_entry['givenname'][0];
      $form['general']['firstname']['value']['#attributes']['disabled'] = 'disabled';

      $form['general']['lastname']['value']['#value'] = $ad_entry['sn'][0];
      $form['general']['lastname']['value']['#attributes']['disabled'] = 'disabled';

      $form['general']['email']['value']['#value'] = $ad_entry['mail'][0];
      $form['general']['email']['value']['#attributes']['disabled'] = 'disabled';

      $form['general']['administrative']['value']['#attributes']['checked'] = 'checked';
      $form['general']['administrative']['value']['#attributes']['disabled'] = 'disabled';
    }
  }
  else {
    unset($form['general']['firstname']['value']['#value']);
    unset($form['general']['firstname']['value']['#attributes']);

    unset($form['general']['lastname']['value']['#value']);
    unset($form['general']['lastname']['value']['#attributes']);

    unset($form['general']['email']['value']['#value']);
    unset($form['general']['email']['value']['#attributes']);

    unset($form['general']['administrative']['value']['#attributes']);
  }

  //form_set_cache($form['#build_id'], $form, $form_state);

  $commands = array();
  $commands[] = ajax_command_replace('.form-item-general-firstname-value', drupal_render($form['general']['firstname']['value']));
  $commands[] = ajax_command_replace('.form-item-general-lastname-value', drupal_render($form['general']['lastname']['value']));
  $commands[] = ajax_command_replace('.form-item-general-email-value', drupal_render($form['general']['email']['value']));
  $commands[] = ajax_command_replace('.form-item-general-administrative-value', drupal_render($form['general']['administrative']['value']));
  
  return array('#type' => 'ajax', '#commands' => $commands);
}
