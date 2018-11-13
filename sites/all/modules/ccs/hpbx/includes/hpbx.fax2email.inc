<?php

function hpbx_fax2email_delete_page($subscriber_id) {

  hpbx_subscriber_delete($subscriber_id);

  drupal_goto('hpbx/fax');
}


function hpbx_fax2email_edit_form($form, &$form_state, $subscriber_id = NULL) {

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js',
    array('scope' => 'footer'));

  $Customer = hpbx_get_active_customer();

  $FaxserverSettings = $SubscriberPreferences = $Subscriber = NULL;

  if (!is_null($subscriber_id) && $Subscriber = SipwiseEntity::load('subscribers', $subscriber_id)) {

    if ($Customer->id != $Subscriber->customer_id) {
      drupal_not_found();
      exit();
    }
    if (!$FaxserverSettings = SipwiseEntity::load('faxserversettings', $Subscriber->id)) {
      drupal_not_found();
      exit();
    }
    if (!$SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id)) {
      drupal_not_found();
      exit();
    }

    drupal_set_title('
            <h2 class="sg-header-2">'. hpbx_truncate_name_element($Subscriber->external_id, 20) .'</h2>
          </div>
          <div class="hpbx-page-subtitle">
            <span class="hpbx-hunting-group-user-count" data-default-label="'. t('This fax number has no members') .'" data-singular-label="'. t('{0} person is eligible to use this fax number'). '" data-plural-label="'. t('{0}  persons are eligible to use this fax number'). '">'. t('This Fax group is empty') .'</span>', PASS_THROUGH); //Modified for #9574 - PE DEV: Update Fax number - Shows "X persons in this Hunt Group"



  }
  else {
    // Set title.
    drupal_set_title(t('Create fax number'));
  }


  // Init form.
  $form = array(

  );

  // Init form.
  $form = array(

    '#prefix' => '<div class="hpbx-foldable-list-container">',
    '#suffix' => '</div>',
  );

  $form['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#title' => t('Settings'),
    '#tabform' => FALSE,
    '#theme_wrappers' => array('hpbx_foldable_list', 'container'),
    '#attributes' => array('class' => array('hpbx-tab-form-foldable-list', 'hpbx-tab-form')),
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

  // Name
  $form['table']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($Subscriber->external_id) ? trim($Subscriber->external_id) : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#title' => t('Name*'),
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
  // Email destination
  $form['table']['mail_destination']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($FaxserverSettings->destinations[0]->destination) ? $FaxserverSettings->destinations[0]->destination : '',
    '#access' => user_access('hpbx edit subscriber'),
    '#title' => t('Email destination for incoming faxes') . '*',
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Email destination')),
  );

  // Get alias_number and pbx_extension form elements.
  hpbx_subscriber_number_extension_elements($form, $form_state, $Customer, $Subscriber, 'dummy');
  $form['dummy']['alias_number']['value']['#attributes']['data-extension-id']  = '#edit-table-pbx-extension-value';
  $form['table'] += $form['dummy'];
  unset($form['dummy']);

  $form['table']['outgoing']['value'] = array(
    '#type' => 'checkbox',
    '#title' => t('Receive a copy of outgoing faxes'),
    '#default_value' => isset($FaxserverSettings->destinations[0]->outgoing) ? $FaxserverSettings->destinations[0]->outgoing : FALSE,
    '#title_display' => 'before',
  );

  $form['table']['status']['value'] = array(
    '#type' => 'checkbox',
    '#title' => t('Receive reports of outgoing and incoming faxes'),
    '#default_value' => isset($FaxserverSettings->destinations[0]->status) ? $FaxserverSettings->destinations[0]->status : FALSE,
    '#title_display' => 'before',
  );

  $form['table']['members'] = array(
    '#type' => 'container',
    '#title' => t('Members'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => FALSE,
    '#attributes' => array('class'=> array('hpbx-fax-members')),
  );

  $form['table']['members']['subscribers'] = array(
    '#type' => 'tableselect',
    '#tree' => TRUE,
    '#header' => array(),
    '#sortable' => FALSE,
    '#selectall' => TRUE,
    '#theme' => 'hpbx_huntgroup_user_tableselect',
    '#member_menu_title' => t('Select the subscribers that are eligable to use this fax number'),
    '#empty' => '<i>'. t('No subscribers found') . '</i>',
  );

  $form['table']['members']['subscribers']['#options'] = array();
  $form['table']['members']['subscribers']['#default_value'] = array();

  $options = array();
  foreach ($Customer->get_subscribers() as $Subscriber2) {

    $ad_entry = hpbx_users_get_user($Subscriber2->webusername);

    if (hpbx_attribute_allowed('sendfax', $Subscriber2->profile_id)) {
      if ($ad_entry) {
        $name = $ad_entry['sn'][0]. ', '. $ad_entry['givenname'][0];
      }
      elseif (!empty($Subscriber2->external_id)) {
        $name = $Subscriber2->external_id;
      }
      else {
        break;
      }

      $options[$Subscriber2->id]['#caption'] = '';

      if (!is_null($Subscriber) && $Subscriber->is_member($Subscriber2)) {
        // Checked/unchecked.

        $form['table']['members']['subscribers']['#default_value'][$Subscriber2->id] = $Subscriber2->id;
      }

      $options[$Subscriber2->id]['#caption'] .= '<input type="hidden" name="subscriber_weights['. $Subscriber2->id .']"/>';

      $alias = $Subscriber2->get_alias_number(HPBX_NUMBER_FORMAT_GSN) . ' ['. $Subscriber2->pbx_extension . ']';
      $options[$Subscriber2->id]['#caption'] .= <<<EOF
      <div class="hpbx-hunting-group-user-information">
        <div class="hpbx-hunting-group-user-name">{$name}</div>
        <div class="hpbx-hunting-group-user-phone">{$alias}</div>
      </div>
EOF;
    }
  }

  if (!is_null($Subscriber) && isset($Subscriber->pbx_groupmember_ids)) {
    $options = array_replace(array_flip($Subscriber->pbx_groupmember_ids), $options);
  }

  // Remove unwanted members.
  foreach ($options as $i => $option) {
    if (is_int($option)) {
      unset($options[$i]);
    }
  }

  $form['table']['members']['subscribers']['#options'] = $options;

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
    '#href' => 'hpbx/fax',
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

function hpbx_fax2email_edit_form_validate($form, $form_state) {

  global $ActiveReseller;

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  $Customer = hpbx_get_active_customer();


  $Subscriber = NULL;

  if (isset($form_state['values']['id'])) {
    $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
  }

  //Modified for Bug #9745 - Create/update Fax Number - Not allow semi colons (;) in Name field
  // Validate if the external_id is unique for this customer.
  if (hpbx_external_id_exists($Customer, $form_state['values']['table']['external_id']['value'], $Subscriber)) {
    form_set_error('table][external_id][value', t('Name must be unique'));
  }elseif(preg_match('/[;]/', $form_state['values']['table']['external_id']['value'])){
    form_set_error('table][external_id][value', t('Semicolon is not allowed in !name.', array('!name' => t('Name'))));
  }

  // Get all pbx_extensions.
  $extensions = hpbx_get_extensions($Customer);

  if ($form_state['values']['id']) {
    unset($extensions[$form_state['values']['id']]);
  }


  if (!$form_state['values']['id'] &&
    hpbx_bundle_settings_is_allowed($Customer, $ActiveReseller->_settings->fax_subscriber_profile_id, HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE_SET|HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE)<=0) {

    $SubscriberProfile = SipwiseEntity::load('subscriberprofiles',$ActiveReseller->_settings->fax_subscriber_profile_id );
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


function hpbx_fax2email_edit_form_submit($form, $form_state) {


  global $ActiveReseller;
  try {

    $Customer = hpbx_get_active_customer();

    if (!$CustomerPreferences = SipwiseEntity::load('customerpreferences', $Customer->id)) {
      drupal_set_message(t('Can\'t find customer preferences'));
      return;
    }

    if (!$PilotSubscriber = $Customer->get_pilot_subscriber()){
      drupal_set_message(t('Can\'t find the pilot subscriber'));
      return;
    }

    $Subscriber = NULL;

    // Verify an subscriber_id is set.
    if (!$form_state['values']['id']) {

      // New subscriber.
      $Subscriber = SipwiseEntity::create('subscribers');

      // Set values which will not change in the future.
      $Subscriber->webusername = uniqid((int)$Customer->id);
      $Subscriber->username = uniqid((int)$Customer->id);
      $Subscriber->password = hpbx_subscriber_password($Customer->id);
      $Subscriber->customer_id = (int)$Customer->id;
      $Subscriber->is_pbx_group = TRUE;
      $Subscriber->administrative = FALSE;
      $Subscriber->status = 'active';
      $Subscriber->domain_id = $PilotSubscriber->domain_id;
      $Subscriber->profile_set_id = $ActiveReseller->_settings->generic_profile_set_id;
      $Subscriber->profile_id = $ActiveReseller->_settings->fax_subscriber_profile_id;
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

    if ($form_state['values']['table']['alias_number']['value']!=$alias_number_original) {

      $an_change = TRUE;

      // Remove the newly set alias number from the pilot subscriber.
      $PilotSubscriber->alias_number_unset($Customer,$form_state['values']['table']['alias_number']['value']);

      // Set the newly set alias number on the subscriber, remove it when empty.
      $Subscriber->alias_number_set($Customer, $form_state['values']['table']['alias_number']['value']);
    }

    // Save the pilot subscriber and subscriber.
    $PilotSubscriber->save();

    $Subscriber->pbx_groupmember_ids = array();

    foreach ($form_state['values']['subscribers'] as $subscriber_id) {
      if ($subscriber_id) {
        $Subscriber->pbx_groupmember_ids[] = $subscriber_id;
      }
    }

    $Subscriber->save();

    // Add the original alias number to the pilot number again, so it will be available
    // to others subscribers to be used.
    if ($an_change && !empty($alias_number_original)) {
      $PilotSubscriber->alias_number_set($Customer, $alias_number_original, FALSE);
      $PilotSubscriber->save();
    }

    // Load the subscriberpreferences.
    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
	//Modified for Bug#9743 - BUILD: Change request - Set display_name for AA/CR/Fax for create/update use cases
    //$SubscriberPreferences->display_name = $Subscriber->pbx_extension;	
	$SubscriberPreferences->display_name = $Subscriber->external_id;
    $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;
    if ($area = $Customer->get_area_code($form_state['values']['table']['alias_number']['value'])) {
      $SubscriberPreferences->ac = $area;
    }

    // Set the gpp0 value equal to the aliasnumber for billing purposses.
    $SubscriberPreferences->cli = $SubscriberPreferences->gpp0 = $ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'];
    $SubscriberPreferences->gpp1 = $Subscriber->pbx_extension;

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
    
    if(isset($Customer->_settings->multi_site) && $Customer->_settings->multi_site){
      $SubscriberPreferences->gpp9 = $form_state['values']['table']['site']['value'];
    }

    $SubscriberPreferences->save();

    // Enable/Disable fax.
    if (!$FaxserverSettings = SipwiseEntity::load('faxserversettings', $Subscriber->id)) {
      drupal_set_message(t('Can\'t load faxserversettings'));
      return;
    }

    $destination = array();
    $destination['destination'] = $form_state['values']['table']['mail_destination']['value'];
    $destination['filetype'] = 'PDF';
    $destination['incoming'] = TRUE;
    $destination['outgoing'] = $form_state['values']['table']['outgoing']['value'];
    $destination['status'] = $form_state['values']['table']['status']['value'];

    $FaxserverSettings->destinations = array($destination);
    $FaxserverSettings->active = TRUE;

    $FaxserverSettings->save();

    // Set callforwarding unconditionaly to auto attendant.
    $form_state['values']['call-forwards']['cfu']['destination_type'] = 'fax2mail';
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

  foreach (array('subscribers', 'subscriberpreferences', 'faxserversettings', 'callforwards') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $Subscriber->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));

    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }

  if (!isset($catched)) {
    drupal_set_message(t('Fax number successfully saved')); //Modified for #9575 PE DEV: Create/update fax number: Success message says 'Faxnumber' instead of 'Fax number'
  }

  drupal_goto('hpbx/fax');
}
