<?php

/**
 * @param array $form
 * @param array $form_state
 * @param int $customer_id
 *
 * @return array
 */
function hpbx_autoattendants_overview_form($form, $form_state, $customer_id = NULL) {

  global $ActiveReseller;

  $Customer = hpbx_get_active_customer($customer_id);

  // Set title.
  drupal_set_title(t('Auto attendants'));

  // Add css.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  // Init form.
  $form = array();

  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create auto attendant') . '</h5>';
  $add = l($text, 'hpbx/auto-attendant/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form['table'] = array(
    '#add' => array('data' => $add, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-auto-attendants hpbx-table-sortable'),
    '#header' => array(
      'external_id' => array('data'=> t('Name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'number' => array('data'=> t('Number'), 'class' => array('hpbx-sortable', 'hpbx-show')),
    ),
    '#remove_title' => t('Delete auto attendant'),
  );

  if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
    $form['table']['#header']['pbx_extension'] = array('data'=> t('Extension'), 'class' => array('hpbx-sortable', 'hpbx-show-sm'));
  }

  if (!is_null($Customer->id)) {

    // Set customer_id as query.
    $query['customer_id'] = $Customer->id;
  }

  $query['profile_id'] = $ActiveReseller->_settings->auto_attendant_subscriber_profile_id;

  // Query subscribers.
  foreach (sipwise_api_get_all('subscribers', $query) as $Subscriber) {

    if (!empty($Subscriber->external_id) && $AutoAttendants = SipwiseEntity::load('autoattendants', $Subscriber->id)) {

      $form['table'][$Subscriber->id]['#url'] = 'auto-attendant/'. $Subscriber->id . '/edit';
      $form['table'][$Subscriber->id]['#attributes']['data-delete-url'] = 'auto-attendant/'. $Subscriber->id . '/delete';

      $form['table'][$Subscriber->id]['external_id']['data'] = array('#attributes' => array('title' => check_plain($Subscriber->external_id)), '#markup' => hpbx_truncate_name_element($Subscriber->external_id));
      $form['table'][$Subscriber->id]['external_id']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');

      if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
        $form['table'][$Subscriber->id]['number']['data'] = array('#markup' =>   $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN));
        $form['table'][$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');

        $form['table'][$Subscriber->id]['pbx_extension']['data'] = array('#markup' =>   !empty($Subscriber->pbx_extension) ? $Subscriber->pbx_extension : '');
        $form['table'][$Subscriber->id]['pbx_extension']['#attributes']['class'] = array('hpbx-show-sm hpbx-nowrap');
      }
      else {

        $number = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
        if(isset($number) && $number !=''){
			$number = substr($number, 0, $Customer->_settings->extension_length * -1) . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
		}else{
			$number = "<span style='padding-left:30px'>&nbsp</span>" . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
		}
        $form['table'][$Subscriber->id]['number']['data'] = array('#markup' =>  $number);
        $form['table'][$Subscriber->id]['number']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');
      }
    }
  }
  return $form;
}

function hpbx_autoattendant_edit_form($form, &$form_state, $subscriber_id = NULL) {


  global $ActiveReseller;

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  $Customer = hpbx_get_active_customer();

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

      if ($Subscriber->profile_id == $ActiveReseller->_settings->auto_attendant_subscriber_profile_id) {
        // Set title.
        drupal_set_title(t('!external_id', array('!external_id' => truncate_utf8(check_plain($Subscriber->external_id), 20, FALSE, TRUE))));
      }
      else {
        drupal_not_found();
        exit();
      }
      $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
    }
    else {
      drupal_not_found();
      exit();
    }
  }
  else {

    // Set title.
    drupal_set_title(t('Create Auto Attendant'));
  }

  // Add table styling.
  // Init form.
  $form = array(
    '#prefix' => '<div class="hpbx-foldable-list-container">',
    '#suffix' => '</div>',
  );

  $form['container'] = array(
    '#type' => 'container',
    '#theme_wrappers' => array('hpbx_foldable_list', 'container'),
    '#attributes' => array('class' => array('hpbx-tab-form-foldable-list', 'hpbx-tab-form')),
    '#tree' => FALSE,
  );


  $form['container']['table'] = array(
    '#type' => 'container',
    '#title' => t('Settings'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => TRUE,
    '#collapsed' => FALSE,
  );

  // Set the customer id.
  $form['container']['customer_id'] = array(
    '#type' => 'value',
    '#value' => !empty($Subscriber->customer_id) ? $Subscriber->customer_id : $Customer->id,
  );

  // Set the existing id.
  $form['container']['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Subscriber->id) ? $Subscriber->id : FALSE,
  );

  // Name.
  $form['container']['table']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($Subscriber->external_id) ? $Subscriber->external_id : NULL,
    '#required' => TRUE,
    '#title' => t('Name'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Name')),
  );

  hpbx_subscriber_number_extension_elements($form, $form_state, $Customer, $Subscriber, 'table');
	// Set default value based on user_cli setting.
    /*if ((string)$SubscriberPreferences->clir=='clir') {
      $default_value = 'clir';
    }else*/
    if (!isset($Subscriber->id) || isset($SubscriberPreferences->cli) && $SubscriberPreferences->cli == $ActiveReseller->_settings->country_code . substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls)))  {
      $default_value = 'default';
    }
    // Set default value based on cli setting.
    elseif (isset($SubscriberPreferences->cli)) {
      $default_value =  $ActiveReseller->_settings->digit_for_national_calls . substr($SubscriberPreferences->cli, strlen($ActiveReseller->_settings->country_code), -($Customer->_settings->extension_length));
    }
    else {
      $default_value = FALSE;
    }
  $form['container']['table'] += $form['table'];
  unset($form['table']);
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
  //	$Subscriber = SipwiseEntity::load('subscribers', $subscriber_id);
   	/*$subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
   		if(isset($Subscriber) && !empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	}else if(!isset($Subscriber) && isset($Customer->_settings->numberranges) && !empty($Customer->_settings->numberranges) && $form_state['input']['table']['alias_number']['value']=='0'){
    		$subscriber_options = $subscriber_options;
    	}else if(!isset($Subscriber) && isset($Customer->_settings->numberranges) && !empty($Customer->_settings->numberranges)){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	} */
    $subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, FALSE);
   	if(isset($Subscriber) && (($form_state['input']['table']['alias_number']['value']=='0') || (!isset($form_state['input']['table']['alias_number']['value']) && empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN))))){
      $subscriber_options = $subscriber_options;	
    }else {
      $subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    }
  
   	  // CLI.
	  $form['container']['table']['cli']['value'] = array(
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
   /*if(isset($Subscriber) && !empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
  		$defaultAliasNo = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN);
  		$form['container']['table']['cli']['value']['#disabled'] = TRUE;
   }if(isset($SubscriberPreferences->cli) && empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
   		$form['container']['table']['cli']['value']['#disabled'] = FALSE;
   }else if(isset($form_state['input']['table']['alias_number']['value']) && $form_state['input']['table']['alias_number']['value']==0){
   		$form['container']['table']['cli']['value']['#disabled'] = FALSE;
   }else{
  		$defaultAliasNo = 0;
  		$form['container']['table']['cli']['value']['#disabled'] = TRUE;
   }   */

  $soundfiles = array();
  $soundfiles['aa_welcome'] = NULL;
  $soundfiles['office_hours'] = NULL;

  // Get the sound file for this autoattendant subscriber.
  if (isset($Subscriber->id) && $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id)) {

    if (!empty($SubscriberPreferences->contract_sound_set)) {
      if ($SoundSet =  hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {

        foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
          if ($SoundFile->handle == 'aa_welcome') {
            $soundfiles['aa_welcome'] = $SoundFile;
          }
          elseif ($SoundFile->handle == 'office_hours') {
            $soundfiles['office_hours'] = $SoundFile;
          }
        }
      }
    }
  }

  foreach (array('aa_welcome' => 'menu', 'office_hours' => 'out of office') as $handle => $title) {

    $markup = '<div class="hpbx-tab-form-label-field-row hpbx-upload-audio-container">';

    if (!empty($soundfiles[$handle]->filename)) {
      $markup .= '<div class="col-xs-12">
                   <div class="hpbx-tab-form-label">'. drupal_ucfirst(t($title . ' audio')) .' </div>
                  </div>
                  <div class="col-xs-12 hpbx-tab-audio-container hpbx-audio-container hpbx-audio-container-singular" data-audio-url="/hpbx/audiorecording/'. $soundfiles[$handle]->id .'/play" data-audio-id="'.$soundfiles[$handle]->id.'">
                    <div class="hpbx-audio-background-active"></div>
                    <div class="hpbx-audio-progress-bar"></div>
                    <div class="hpbx-audio-content">
                      <div class="hpbx-audio-fallback">';

      $markup .= hpbx_audio_player('hpbx/audiorecording/'. $soundfiles[$handle]->id .'/play.wav');

      $markup .= '  </div>    
                       <div class="hpbx-audio-control-icon"></div>
                       <div class="hpbx-audio-title">'. $soundfiles[$handle]->filename .'</div>';

      if ($handle == 'office_hours' ) {
        $markup .= '<div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" title="'. t('Delete upload'). '"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
                       <input type="checkbox" class="hpbx-audio-is-replaced" name="hpbx-audio-is-replaced-office-hours" value="1" />';

      }
      $markup .= '</div>
                     </div>';
    }

    $markup .= '</div>';

    // Show audio player for handle.
    $form['container']['table'][$handle] = array(
      '#prefix' => '<div class="hpbx-tab-form-label-field-row hpbx-upload-audio-container">',
      '#suffix' => '</div>',
      '#type' => 'markup',
      '#markup' => $markup,
    );

    $form['container']['table'][$handle. '_upload']['value'] = array(
      '#title' => t('Upload @title', array('@title' => t(strtolower($title . ' audio')))),
      '#type' => 'file',
      '#name' => $handle,
      '#theme_wrappers' => array('hpbx_audio_upload', 'form_element'),
      '#attributes' => array(
        'class' => array('hpbx-click-with-alert-input'),
      ),
      '#wrapper_attributes' => array(
        'class' => array(
          'sg-btn',
          'sg-btn-file-input',
          'sg-btn-secondary',
          'hpbx-click-with-alert',
          'needsclick',
        ),
        'data-alert-title' => t('File requirements'),
        'data-alert-message' => t('If you upload an audio file, make sure you pay for or own the content rights for this audio file.</br></br>Supported file types: wav, mp3, flac, ogg, raw'),
        'data-alert-button-text' => t('OK'),
      ),
    );
  }
  // Add required in case no file is set for menu audio.
  if (empty($soundfiles['aa_welcome']->filename)) {
    $form['container']['table']['aa_welcome_upload']['value']['#title'] =$form['container']['table']['aa_welcome_upload']['value']['#title'] . '*';
  }
 

  $CFTimeSet = NULL;
  if (!empty($Subscriber->id)) {
    $CFTimeSets = sipwise_api_get_all('cftimesets', array('subscriber_id' => $Subscriber->id, 'name' => 'cfu_autoattendant'));
    if (count($CFTimeSets)) {
      $CFTimeSet = current($CFTimeSets);
    }
  }

  //table[times][value][cfu][times][2][ranges]
  $form['container']['call-forwards'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu']['times'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu']['times'] = hpbx_times_get_element($CFTimeSet, $add_parent_foldable_list =  FALSE, $form_state);


  // Speeddials
  if (!empty($Subscriber->id)) {

    $form['autoattendants'] = array(
      '#type' => 'container',
      '#title' => t('Menu options'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#tree' => TRUE,
      '#collapsed' => FALSE,
    );

    $AutoAttendants = SipwiseEntity::load('autoattendants', $Subscriber->id);

    $slot_options = drupal_map_assoc(range(0, 9));

    $form['autoattendants']['table'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('hpbx-slots-container')),
      '#tree' => TRUE,
    );

    $form['autoattendants']['table']['slots_header'] = array(
      '#type' => 'container',
      '#tree' => FALSE,
      '#attributes' => array('class' => array('hpbx-tab-form-label-field-row')),
    );
    $form['autoattendants']['table']['slots_header']['slot'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="col-xs-4"><div class="hpbx-tab-form-label hpbx-tab-form-label-checkbox">'. t('Option').'</div></div>',
    );
    $form['autoattendants']['table']['slots_header']['Destination'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="col-xs-8"><div class="hpbx-tab-form-label hpbx-tab-form-label-checkbox">'.t('Destination').'</div></div>',
    );

    $form['autoattendants']['table']['slots'] = array(
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

    // Define all 10 slots.
    for ($x=0;$x<=9;$x++) {

      $form['autoattendants']['table']['slots'][$x] = array(
        '#type' => 'container',
        '#deletable_row' => TRUE,
        '#attributes' => array(
          'class' => array('hpbx-slots-row', 'row', 'hpbx-deletable-row', 'hpbx-invisible'),
        ),
        '#tree' => TRUE,
      );
      $form['autoattendants']['table']['slots'][$x]['row'] = array(
        '#type' => 'container',
        '#tree' => TRUE,
        '#attributes' => array('class' => array('hpbx-tab-form-label-field-row')),
      );
      $form['autoattendants']['table']['slots'][$x]['row']['slot'] = array(
        '#type' => 'select',
        '#options' => $slot_options,
        '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
        '#field_size_class' => array('col-xs-4', 'col-sm-3'),
        '#row_wrapper' => FALSE,
        '#empty_option' => '-',
      );
      $form['autoattendants']['table']['slots'][$x]['row']['destination'] = array(
        '#type' => 'textfield',
        '#attributes' => array(
          'class' => array('sg-element', 'hpbx-autocomplete'),
          'data-values' => $datalist,
        ),
        '#field_size_class' => array('col-xs-6', 'col-sm-7', 'col-sm-offset-1'),
        '#row_wrapper' => FALSE,
      );
      $form['autoattendants']['table']['slots'][$x]['row']['delete'] = array(
        '#type' => 'markup',
        '#markup' => '<div class="col-xs-2 col-sm-1">
                    <div class="hpbx-tab-form-field">
                        <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" type="button" title="'. t('Delete speed dial') .'"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
                    </div>
                </div>',
      );
    }

    foreach ($AutoAttendants->slots as $key => $slot) {

      // Set default value for slot.
      $form['autoattendants']['table']['slots'][$key]['row']['slot']['#default_value'] = $slot->slot;
      $speeddial_no = hpbx_normalize_number_human_readable($Subscriber->id, trim($slot->destination));
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
       
      if(strlen($speeddial_no) >= $min_length && strlen($speeddial_no) <= $max_length){
        $destination = $speeddial_no;
      }else{
        $destination = substr($speeddial_no,-($Customer->_settings->extension_length));
      }
      // Normalize number.
     
      $form['autoattendants']['table']['slots'][$key]['row']['destination']['#default_value'] = $destination;

      // Remove the invisible class.
      array_pop($form['autoattendants']['table']['slots'][$key]['#attributes']['class']);
    }

    // Trimm fix to turn add more output into trimm usable output.
    unset($form['autoattendants']['table']['add']);

    $form['autoattendants']['table']['slots']['#suffix'] = '
    <div class="row">
      <div class="hpbx-tab-form-label-field-row">
        <div class="col-xs-12">
          <div class="hpbx-tab-form-field">
            <button class="sg-btn sg-btn-secondary hpbx-add-slot" type="button">'. t('Add') .'</button><span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span>
          </div>
        </div>
      </div>
    </div>';

    $form['container']['autoattendants'] = $form['autoattendants'];
    unset($form['autoattendants']);

    $form['container']['table']['#collapsed'] = (!empty($Subscriber->id)&& !count($AutoAttendants->slots));
  }

  // Submit button.
  $form['container']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );

  if (empty($Subscriber->id) || count($AutoAttendants->slots)) {
    $form['container']['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => 'hpbx/auto-attendants',
      '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
      '#prefix' => '<div class="col-xs-12">',
    );
  }
  $form['container']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => is_null($Subscriber) ? t('Next') : t('Save'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span></div>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  return $form;
}

/**
 * Form validate handler for hpbx_autoattendant_edit_form()
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_autoattendant_edit_form_validate($form, &$form_state) {
  global $ActiveReseller;

  // Add table styling.

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  if (isset($form_state['values']['autoattendants_autcomplete_subscriber_options'])) {
    hpbx_set_autocomplete('autoattendantsAddMore', '#hpbx-autoattendants-table-wrapper div.form-type-textfield input[type=text]', $form_state['values']['autoattendants_autcomplete_subscriber_options']);
  }

  if (!$Customer = SipwiseEntity::load('customers', $form_state['values']['customer_id'])) {
    drupal_set_message(t('Can\'t find customer'));
    return;
  }
  $Domain = SipwiseEntity::load('domains', $ActiveReseller->_settings->domain_id);

  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);

  $soundfiles = array();
  $soundfiles['aa_welcome'] = NULL;
  $soundfiles['office_hours'] = NULL;

  $Subscriber = NULL;

  // Get the soundfiles for this autoattendant subscriber.
  if (isset($form_state['values']['id'])) {
    if ($SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $form_state['values']['id'])) {

      if (!empty($SubscriberPreferences->contract_sound_set)) {
        if ($SoundSet =  hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {
          foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
            if (in_array($SoundFile->handle, array('aa_welcome', 'office_hours'))) {
              $soundfiles[$SoundFile->handle] = $SoundFile;
            }
          }
        }
      }
    }
    $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
  }

  if (empty($soundfiles['aa_welcome']) && empty($_FILES['aa_welcome']['name'])) {
    form_set_error('table][aa_welcome_upload][value', t('Menu audio file is required'));
  }


  if (!empty($form_state['values']['customer_id'])) {
    $customer_id = $form_state['values']['customer_id'];
  }
  else {
    drupal_set_message(t('Can\'t find the customer ID'), 'error');
    return;
  }

  // Get all pbx_extensions.
  $extensions = hpbx_get_extensions($Customer);

  if ($form_state['values']['id']) {
    unset($extensions[$form_state['values']['id']]);
  }

  if (!$form_state['values']['id'] &&
    hpbx_bundle_settings_is_allowed($Customer, $ActiveReseller->_settings->auto_attendant_subscriber_profile_id, HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE_SET|HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE)<=0) {

    // Load subscriberprofile
    $SubscriberProfile = SipwiseEntity::load('subscriberprofiles',$ActiveReseller->_settings->auto_attendant_subscriber_profile_id );
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


  // Validate files.
  if (isset($_FILES)) {
    $_FILES['aa_1_for']=NULL;
    $_FILES['aa_1_option'] = NULL;
    foreach ($_FILES as $handle => $sf) {
      if (!empty($sf['name'])) {
        if (empty($sf['tmp_name']) || !file_exists($sf['tmp_name']) || $sf['error']) {
          form_set_error('', t('Upload failed'));
        }

        //hpbx_audio_get_valid_file_extensions()
        $ext = pathinfo($sf['name'], PATHINFO_EXTENSION);

        if (!in_array(strtolower($ext), hpbx_audio_get_valid_file_extensions())) {
          form_set_error('table]['. $handle . '][value', t('The file format @ext is not supported', array('@ext' => $ext)));
        }
        if($handle == 'aa_welcome'){
          $_FILES['aa_1_for'] = $sf;
          $_FILES['aa_1_option'] = $sf;
        }
      }
    }
  }

  // Validate if the external_id is unique for this customer.
  if (hpbx_external_id_exists($Customer, $form_state['values']['table']['external_id']['value'], $Subscriber)) {
    form_set_error('table][external_id][value', t('Name must be unique'));
  }

  if(!is_numeric($form_state['input']['table']['alias_number']['value'])){
    form_set_error('table][alias_number][value', t('Number field is required'));
  }  

  $slot_count = 0;
  $slots_in_use = array();

  if (!empty($form_state['values']['id'])) {
    $normalized = sipwise_api_normalize_number($Subscriber->id, $ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'], 'callee_in');
    
  if(empty($form_state['values']['table']['alias_number']['value'])){
    $DDI = $form_state['values']['table']['cli']['value'];
  }else{
    $DDI = $form_state['values']['table']['alias_number']['value'];
  }
    // Validate auto attendants.
    if (isset($form_state['values']['autoattendants']['table']['slots'])) {
      foreach ($form_state['values']['autoattendants']['table']['slots'] as $slot_key => &$slot) {
        foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
          if($slot['row']['destination']==$SubscriberForCLI->pbx_extension){
            $slot['row']['destination'] = $DDI . $SubscriberForCLI->pbx_extension;
          }
       }
        if (empty($slot['row']['destination']) && $slot['row']['slot']=='') {
          continue;
        }

        $slots_in_use[] = $slot['row']['slot'];

        if (

          // Validateexentions against (extension length of) alias_number.
          (substr($normalized, $Customer->_settings->extension_length * -1) == $slot['row']['destination']) ||
          ($normalized == sipwise_api_normalize_number($Subscriber->id, $slot['row']['destination'], 'callee_in')) ||
          ($slot['row']['destination'] == $form_state['values']['table']['alias_number']['value'])) {

          form_set_error('autoattendants][table]['.$slot_key.'][destination', t('The destination of menu option @slot cannot be assigned to this auto attendant.', array('@slot' => $slot['row']['slot'])));
        }

        if(!empty($DomainPreferences->adm_cf_ncos) && !hpbx_ncos_is_allowed($DomainPreferences->adm_cf_ncos, $slot['row']['destination'], $form_state['values']['id'])) {
          form_set_error('autoattendants][table]['.$slot_key.'][destination', t('Not allowed to set this destination number'));
        }


        $slot_count++;
      }
    }

    if (count(array_diff_key( $slots_in_use , array_unique( $slots_in_use ) ))) {
      form_set_error('autoattendants][table', t('Please enter unique menu options'));
    }

    if (!$slot_count) {
      form_set_error('autoattendants][table', t('The Auto attendant must have been provided with at least one menu option.'));
    }
  }

  $form_state['values']['call-forwards']['cfu']['times_api'] = array();

  // Format the ranges set by the range sliders.
  if (isset($form_state['values']['call-forwards']['cfu']['times'])) {

    if (count(hpbx_api_times_get($form_state['values']['call-forwards']['cfu']['times']))) {

      if (isset($form_state['input']['hpbx-audio-is-replaced-office-hours']) && empty($_FILES['office_hours']['name'])) {
        form_set_error('table][office_hours_upload][value', t('Office hours audio file is required'));
      }
      // Verify if audio (already present or uploaded) is set in case officehours are defined.
      elseif (empty($soundfiles['office_hours']) && empty($_FILES['office_hours']['name'])) {

        form_set_error('table][office_hours_upload][value', t('Office hours audio file is required'));
      }
    }

    // Convert times to usable data, completely empty sets are converted to one week (1-7).
    $form_state['values']['call-forwards']['cfu']['times_api'] = hpbx_api_times_get($form_state['values']['call-forwards']['cfu']['times'], TRUE);
  }
}

/**
 * Form submit handler for hpbx_autoattendant_edit_form()
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_autoattendant_edit_form_submit($form, $form_state) {

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
      $Subscriber->webusername = uniqid((int)$customer_id);
      $Subscriber->username = uniqid((int)$customer_id);
      $Subscriber->password = hpbx_subscriber_password($customer_id);
      $Subscriber->customer_id = (int)$customer_id;
      $Subscriber->is_pbx_group = FALSE;
      $Subscriber->administrative = FALSE;
      $Subscriber->domain_id = $PilotSubscriber->domain_id;
      $Subscriber->profile_set_id = $ActiveReseller->_settings->generic_profile_set_id;
      $Subscriber->profile_id = $ActiveReseller->_settings->auto_attendant_subscriber_profile_id;
      $Subscriber->status = 'active';
    }
    else {
      // Existing subscriber.
      $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
    }

    // Set the auto attendant name.
    $Subscriber->external_id = trim($form_state['values']['table']['external_id']['value']);

    // Set the extension.
    $Subscriber->pbx_extension = trim($form_state['values']['table']['pbx_extension']['value']);

    // Store the current alias_number.
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
      
      
      // Remove the newly set alias number from the pilot subscriber.
     if(strlen($form_state['values']['table']['alias_number']['value']) >= $min_length && strlen($form_state['values']['table']['alias_number']['value']) <= $max_length && !empty($alias_no_original)){
      $PilotSubscriber->alias_number_unset($Customer,$form_state['values']['table']['alias_number']['value']);
     }
      // Set the newly set alias number on the subscriber, remove it when empty.
      $Subscriber->alias_number_set($Customer, $form_state['values']['table']['alias_number']['value']);
    }

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
    $SubscriberPreferences->display_name = $Subscriber->pbx_extension;
    $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;
    
    
    if ($area = $Customer->get_area_code($form_state['values']['table']['alias_number']['value'])) {
      $SubscriberPreferences->ac = $area;
    }

    // Check if the original office_hours file was deleted, we need to delete
    // it (and maybe upload a new file afterwords.
    if (isset($form_state['input']['hpbx-audio-is-replaced-office-hours']) &&
      $SoundSet =  hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {
  
      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
        if ($SoundFile->handle == 'office_hours') {
          $SoundFile->delete();
        }
      }
    }

    $soundset_name = 'AutoAttendant Subscriber '. $Subscriber->id;

    if (hpbx_soundset_upload_file($Subscriber,$soundset_name)) {
      $SubscriberPreferences->contract_sound_set = $soundset_name;
    }
    
  
  // CLI Restriction.
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
	}
    /*else {
     // Set the pilot subscriber primary number as cli.      
      $SubscriberPreferences->user_cli = $ActiveReseller->_settings->country_code . substr(hpbx_parse_primary_number($PilotSubscriber->primary_number), strlen($ActiveReseller->_settings->digit_for_national_calls));
    }*/
    
    $SubscriberPreferences->gpp1 = $Subscriber->pbx_extension;

    // Save the preferences.
    $SubscriberPreferences->save();

    hpbx_set_destination_sets($Subscriber, 'autoattendant', $form_state['values']['call-forwards']['cfu']['times_api']);
    
    // Save auto attendants.
    if (isset($form_state['values']['autoattendants']['table']['slots'])) {
      $AutoAttendants = SipwiseEntity::load('autoattendants', $Subscriber->id);
      $AutoAttendants->slots = array();
      $aa = &$AutoAttendants->slots;
      $aa = array();   
      foreach ($form_state['values']['autoattendants']['table']['slots'] as $slot) {

        if ($slot['row']['slot']!=='' && $slot['row']['destination']!=='') {
          $destination = sipwise_api_normalize_number($Subscriber->id, $slot['row']['destination']);       	
          $aa[] = array(
            'slot' => $slot['row']['slot'],
            'destination' => $destination,
          );
        }
      }

      // Save the attendants.
      $AutoAttendants->save();
    }
  }
  catch (Exception $e) {
    // Failed to create customer.
    drupal_set_message(t('Failed to update'), 'error');

    //
    drupal_set_message(t($e->getMessage()), 'error');

    $catched = TRUE;
  }

  foreach (array('subscribers', 'subscriberpreferences', 'autoattendants') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $Subscriber->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));
  
    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }
  
  if (!isset($catched)) {

    if ($form_state['values']['op'] == t('Save')) {
      drupal_set_message(t('Auto attendant successfully saved'));
    }
    elseif ($form_state['values']['op'] == t('Next')) {
      drupal_goto('hpbx/auto-attendant/'. $Subscriber->id .'/edit');
    }
  }

  drupal_goto('hpbx/auto-attendants');
}

/**
 * Callback for hpbx/auto-attendant/%/delete
 *
 * @param int $subscriber_id
 *
 * @return void
 */
function hpbx_autoattendant_delete_page($subscriber_id) {

  // @todo Delete the soundset and it's files.

  // Delete auto attendant subscriber.
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

  drupal_goto('hpbx/auto-attendants');
}

