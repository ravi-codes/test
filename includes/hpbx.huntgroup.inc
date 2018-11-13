<?php


function hpbx_huntgroups_overview_form($form, $form_state, $customer_id = NULL) {

  global $user;

  $Customer = hpbx_get_active_customer($customer_id);
  $PilotSubscriber = $Customer->get_pilot_subscriber();

  drupal_set_title(t('Hunt groups'));
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');
  
  // Init form.
  $form = array();
  
  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create hunt group') . '</h5>';
  $add = l($text, 'hpbx/huntinggroup/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));
  
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
    '#remove_title' => t('Delete hunt group'),
  );
  
  if (!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer)) {
    $form['table']['#header']['pbx_extension'] = array('data'=> t('Extension'), 'class' => array('hpbx-sortable', 'hpbx-show-sm'));
  }
  
  $form['table']['#header']['pbx_hunt_policy'] = array('data'=> t('Hunting Policy'), 'class' => array('hpbx-sortable', 'hpbx-show-xs'));
  $form['table']['#header']['pbx_hunt_timeout'] = array('data'=> t('Time out'), 'class' => array('hpbx-sortable', 'hpbx-show-sm'));
  
  foreach ($Customer->get_huntgroups() as $Subscriber) {

    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
    
    if (!empty($Subscriber->external_id)) {

      $form['table'][$Subscriber->id]['#url'] = 'huntinggroup/'. $Subscriber->id . '/edit';
      $form['table'][$Subscriber->id]['#attributes']['data-delete-url'] = 'huntinggroup/'. $Subscriber->id . '/delete';
      
      $form['table'][$Subscriber->id]['external_id']['data'] = array('#markup' => hpbx_truncate_name_element($Subscriber->external_id), '#attributes' => array('title' => check_plain($Subscriber->external_id)));
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

      $form['table'][$Subscriber->id]['pbx_hunt_policy']['data'] = array('#markup' => t(drupal_ucfirst($Subscriber->pbx_hunt_policy)));
      $form['table'][$Subscriber->id]['pbx_hunt_policy']['#attributes']['class'] = array('hpbx-show-xs');

      if ($Subscriber->pbx_hunt_policy == 'parallel') {
        $form['table'][$Subscriber->id]['pbx_hunt_timeout']['data'] = array('#markup' => $SubscriberPreferences->ringtimeout . ' sec');
      }
      else {
        $form['table'][$Subscriber->id]['pbx_hunt_timeout']['data'] = array('#markup' => $Subscriber->pbx_hunt_timeout . ' sec');
      }
      $form['table'][$Subscriber->id]['pbx_hunt_timeout']['#attributes']['class'] = array('hpbx-show-sm');
    }
  }
  return $form;
}

/**
 * @param int $subscriber_id
 */
function hpbx_huntgroup_delete_page($subscriber_id) {

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
  drupal_goto('hpbx/huntinggroups');
}

/**
 * @param array $form
 * @param array $form_state
 * @param int $subscriber_id
 *
 * @return array
 */
function hpbx_huntgroup_edit_form($form, &$form_state, $subscriber_id = NULL) {

  global $ActiveReseller;

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));

  // Load the customer.
  $Customer = hpbx_get_active_customer();

  // Load domain settings.
  $Domain = SipwiseEntity::load('domains', $ActiveReseller->_settings->domain_id);
  $DomainPreferences = SipwiseEntity::load('domainpreferences', $Domain->id);

  $Callforwards = $SubscriberPreferences = $VoicemailSettings = $HuntGroup = NULL;

  // Verify if the customer_id is passed.
  if (!is_null($subscriber_id)) {
    if ($HuntGroup = SipwiseEntity::load('subscribers', $subscriber_id)) {
      if ($HuntGroup->profile_id == $ActiveReseller->_settings->hunt_group_subscriber_profile_id) {

        // Verify if the subscriber to be deleted is owned by the active customer.
        if ($Customer->id != $HuntGroup->customer_id) {
          drupal_not_found();
          exit();
        }

        $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $HuntGroup->id);
        $VoicemailSettings = SipwiseEntity::load('voicemailsettings', $HuntGroup->id);
        $Callforwards = SipwiseEntity::load('callforwards', $HuntGroup->id);
        drupal_set_title('
            <h2 class="sg-header-2">' . hpbx_truncate_name_element($HuntGroup->external_id, 20) . '</h2>
          </div>
          <div class="hpbx-page-subtitle">
            <span class="hpbx-hunting-group-user-count" data-default-label="' . t('This Hunt group is empty') . '" data-singular-label="' . t('{0} person in this Hunt group') . '" data-plural-label="' . t('{0} persons in this Hunt group') . '">' . t('This Hunt group is empty') . '</span>', PASS_THROUGH);

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
    drupal_set_title(t('Create Hunt group'));
  }

  // Init form.
  $form = array(
    '#prefix' => '<div class="hpbx-foldable-list-container">',
    '#suffix' => '</div>',
  );

  $form['container'] = array(
    '#type' => 'container',
    '#theme_wrappers' => array(
      'hpbx_foldable_list',
      'container'
    ),
    '#attributes' => array(
      'class' => array(
        'hpbx-tab-form-foldable-list',
        'hpbx-tab-form'
      )
    ),
    '#tree' => FALSE,
  );

  $form['container']['table'] = array(
    '#type' => 'container',
    '#title' => t('Settings'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['container']['customer_id'] = array(
    '#type' => 'value',
    '#value' => !empty($HuntGroup->customer_id) ? $HuntGroup->customer_id : $Customer->id,
  );

  $form['container']['id'] = array(
    '#type' => 'value',
    '#value' => !empty($HuntGroup->id) ? $HuntGroup->id : FALSE,
  );

  $form['container']['table']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($HuntGroup->external_id) ? $HuntGroup->external_id : NULL,
    '#required' => TRUE,
    '#title' => t('Name*'),
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('Name')
    ),
  );

  // Set number + extension form elements.
  hpbx_subscriber_number_extension_elements($form, $form_state, $Customer, $HuntGroup, 'table');
  // Set default value based on cli setting.
    if (!isset($HuntGroup->id) || isset($SubscriberPreferences->cli) && $SubscriberPreferences->cli == $ActiveReseller->_settings->country_code . substr($HuntGroup->get_alias_number(HPBX_NUMBER_FORMAT_GSN), strlen($ActiveReseller->_settings->digit_for_national_calls)))  {
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
  $Subscriber = SipwiseEntity::load('subscribers', $subscriber_id);
 
 if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
  	
   	$subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, FALSE);
   		/*if(isset($Subscriber) && !empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	}else if(!isset($Subscriber) && isset($Customer->_settings->numberranges) && !empty($Customer->_settings->numberranges) && $form_state['input']['table']['alias_number']['value']=='0'){
    		$subscriber_options = $subscriber_options;
    	}else if(!isset($Subscriber) && isset($Customer->_settings->numberranges) && !empty($Customer->_settings->numberranges) && $form_state['input']['table']['alias_number']['value']!=''){
    		$subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	}*/
      //$subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, TRUE);
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
   if(isset($Subscriber) && !empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
  		$defaultAliasNo = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN);
  		$form['container']['table']['cli']['value']['#disabled'] = TRUE;
   }if(isset($SubscriberPreferences->cli) && empty($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN))){
   		$form['container']['table']['cli']['value']['#disabled'] = FALSE;
   }else if(isset($form_state['input']['table']['alias_number']['value']) && $form_state['input']['table']['alias_number']['value']==0){
   		$form['container']['table']['cli']['value']['#disabled'] = FALSE;
   }else{
  		$defaultAliasNo = 0;
  		$form['container']['table']['cli']['value']['#disabled'] = TRUE;
   }   

  $options = array(
    'serial' => t('Serial Ringing'),
    'parallel' => t('Parallel Ringing'),
    'random' => t('Random'),
    'circular' => t('Circular'),
  );

  $option_attributes = array(
    'parallel' => array('data-help-text' => t('The selected policy does not support ordering')),
    'random' => array('data-help-text' => t('The selected policy does not support ordering')),
  );

  // Policy.
  $form['container']['table']['pbx_hunt_policy']['value'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#option_attributes' => $option_attributes,
    '#default_value' => !empty($HuntGroup->pbx_hunt_policy) ? $HuntGroup->pbx_hunt_policy : NULL,
    '#empty_option' => t('Choose an option'),
    '#required' => TRUE,
    '#title' => t('Hunting Policy*'),
    '#attributes' => array(
      'class' => array(
        'chzn-select',
        'sg-element',
        'no-search'
      ),
      'data-placeholder' => t('Choose an option'),
      'id' => 'hpbx-huntgroup-settings-how',
    ),
  );

  // Time-out
  $form['container']['table']['pbx_hunt_timeout']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => (!empty($HuntGroup->pbx_hunt_timeout) || $HuntGroup->pbx_hunt_timeout == '') ? $HuntGroup->pbx_hunt_timeout : 10,
    '#size' => 5,
    '#maxlength' => 3,
    '#title' => t('Time out (sec)'),
    '#attributes' => array(
      'class' => array('sg-element', 'hpbx-hunt-timeout'),
      'placeholder' => ''
    ),
  );

  if ($HuntGroup->pbx_hunt_policy == 'parallel') {
    $form['container']['table']['pbx_hunt_timeout']['value']['#default_value'] = (int) $SubscriberPreferences->ringtimeout;
  }

  // System wide announcements.
  $form['container']['table']['language']['value'] = array(
    '#required' => TRUE,
    '#type' => 'select',
    '#title' => t('Language of Voicemail announcements'),
    '#attributes' => array(
      'class' => array(
        'chzn-select',
        'sg-element',
        'no-search'
      )
    ),
    '#default_value' => $DomainPreferences->language,
  );

  $form['container']['table']['language']['value']['#options'] = array();
  if ($preferencedefs = sipwise_get_preferencedefs('subscriber')) {
    if (isset($preferencedefs->language->enum_values)) {
      foreach ($preferencedefs->language->enum_values as $enum) {

        if (!empty($enum->value)) {
          if ($enum->default_val) {
            $form['container']['table']['language']['value']['#default_value'] = $enum->value;
          }
          $form['container']['table']['language']['value']['#options'][$enum->value] = t(drupal_ucfirst($enum->label));
        }
      }
    }
  }

  if (!empty($SubscriberPreferences->language)) {
    // Set default value.
    $form['container']['table']['language']['value']['#default_value'] = $SubscriberPreferences->language;
  }

  // Sort options list.
  asort($form['container']['table']['language']['value']['#options']);

  $CFTimeSet = NULL;
  if (!empty($HuntGroup->id)) {
    if ($CFMapping = SipwiseEntity::load('cfmappings', $HuntGroup->id)) {
      if (count($CFMapping->cfu)) {

        $CFTimeSets = sipwise_api_get_all('cftimesets', array(
          'subscriber_id' => $HuntGroup->id,
          'name' => $CFMapping->cfu[0]->timeset
        ));
        if (count($CFTimeSets)) {
          $CFTimeSet = current($CFTimeSets);
        }

        $CFDestinationSets = sipwise_api_get_all('cfdestinationsets', array(
          'subscriber_id' => $HuntGroup->id,
          'name' => $CFMapping->cfu[0]->destinationset
        ));
        if (count($CFDestinationSets)) {
          $CFDestinationSet = current($CFDestinationSets);
        }
      }
    }
  }

  $form['container']['call-forwards'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu']['times'] = array('#tree' => TRUE);
  $form['container']['call-forwards']['cfu']['times'] = hpbx_times_get_element($CFTimeSet, FALSE);

  $form['container']['call-forwards']['cfu']['times']['last_resort'] = array(
    '#title' => t('Out of office destination'),
    '#type' => 'item',
    '#field_size_class' => array('col-xs-12', 'col-md-9'),
    '#label_row_attributes' => array(
      'class' => array(
        'hpbx-tab-form-label-field-row',
        'hpbx-last-resort-row'
      )
    ),
  );

  $form['container']['call-forwards']['cfu']['times']['last_resort']['nls'] = array(
    '#type' => 'radio',
    '#caption' => t('No out of office destination'),
    '#title' => '',
    '#return_value' => 'nls',
    '#parents' => array('last_resort'),
    '#attributes' => array('data-show-type' => '', 'data-hide-type' => 'oo'),
    '#field_size_class' => array('col-xs-12', 'col-md-9'),

  );
  $form['container']['call-forwards']['cfu']['times']['last_resort']['ooa'] = array(
    '#type' => 'radio',
    '#caption' => t('Announcement'),
    '#title' => '',
    '#return_value' => 'ooa',
    '#parents' => array('last_resort'),
    '#attributes' => array('data-show-type' => 'ooa', 'data-hide-type' => 'oo'),
    '#field_size_class' => array('col-xs-12', 'col-md-9'),
  );
  $form['container']['call-forwards']['cfu']['times']['last_resort']['ood'] = array(
    '#type' => 'radio',
    '#caption' => t('Destination (Voicemail or Number)'),
    '#title' => '',
    '#return_value' => 'ood',
    '#parents' => array('last_resort'),
    '#attributes' => array('data-show-type' => 'ood', 'data-hide-type' => 'oo'),
    '#field_size_class' => array('col-xs-12', 'col-md-9'),
  );

  $soundfiles = hpbx_get_soundfiles_from_soundset($HuntGroup->customer_id, $SubscriberPreferences->contract_sound_set, array(
    'office_hours',
    'queue_greeting'
  ));

  $handle = 'office_hours';
  $title = 'out of office';

  $markup = ' <div class="row" data-show-type="ooa" data-hide-type="oo">
                <div class="hpbx-tab-form-label-field-row hpbx-upload-audio-container">';

  if (!empty($soundfiles[$handle]->filename)) {
    $markup .= '<div class="col-xs-12">
                   <div class="hpbx-tab-form-label">' . t(drupal_ucfirst($title)) . ' audio</div>
                </div>
                <div class="col-xs-12 hpbx-tab-audio-container hpbx-audio-container hpbx-audio-container-singular" data-audio-url="/hpbx/audiorecording/' . $soundfiles[$handle]->id . '/play" data-audio-id="' . $soundfiles[$handle]->id . '">
                   <div class="hpbx-audio-background-active"></div>
                   <div class="hpbx-audio-progress-bar"></div>
                   <div class="hpbx-audio-content">
                      <div class="hpbx-audio-fallback">';

    $markup .= hpbx_audio_player('hpbx/audiorecording/' . $soundfiles[$handle]->id . '/play.wav');

    $markup .= '      </div>
                      <div class="hpbx-audio-control-icon"></div>
                      <div class="hpbx-audio-title">' . $soundfiles[$handle]->filename . '</div>
                      <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" title="' . t('Delete upload') . '"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
                          <input type="checkbox" class="hpbx-audio-is-replaced" name="hpbx-audio-is-replaced-office-hours" value="1" />
                   </div>
                 </div>';
  }

  $markup .= '</div></div>';

  // Show audio player for handle.
  $form['container']['call-forwards']['cfu']['times'][$handle] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['container']['call-forwards']['cfu']['times'][$handle . '_upload']['value'] = array(
    '#title' => t('Upload @title audio', array('@title' => $title)),
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
    '#data_show_type' => 'ooa',
    '#data_hide_type' => 'oo',
  );

  $form_cfu = $form_state_resort = array();

  hpbx_subscribers_callforwards_form($form_cfu, $form_state_resort, $Customer, $HuntGroup, $Callforwards,$ActiveReseller);

  // Some conversion.
  unset(
    $form_cfu['call-forwards']['cfb'],
    $form_cfu['call-forwards']['cfna'],
    $form_cfu['call-forwards']['cft'],
    $form_cfu['call-forwards']['cfu']['times']
  );
  
  $form_cfu['call-forwards']['cfu']['#theme_wrappers'] = array();

  $form_cfu['call-forwards']['cfu']['destination_type']['#data_show_type'] = 'ood';
  $form_cfu['call-forwards']['cfu']['destination_type']['#data_hide_type'] = 'oo';
  $form_cfu['call-forwards']['cfu']['destination_uri']['#data_show_type'] = 'ood';
  $form_cfu['call-forwards']['cfu']['destination_uri']['#data_hide_type'] = 'oo';
  
  $form['container']['call-forwards']['cfu']['times'] += $form_cfu['call-forwards'];
  
  
  if (isset($CFDestinationSet->destinations[0]->destination) && $CFDestinationSet->destinations[0]->destination == 'sip:office-hours@app.local') {
  
    // Announcement.
    $form['container']['call-forwards']['cfu']['times']['last_resort']['ooa']['#default_value'] = 'ooa';
  }
  elseif (isset($CFDestinationSet->destinations[0]->simple_destination) &&
      is_numeric($CFDestinationSet->destinations[0]->simple_destination)) {
  
    // Number.
    $form['container']['call-forwards']['cfu']['times']['last_resort']['ood']['#default_value'] = 'ood';
    
    $form['container']['call-forwards']['cfu']['times']['call-forwards']['cfu']['destination_type']['#default_value'] = 'uri';
    $form['container']['call-forwards']['cfu']['times']['call-forwards']['cfu']['destination_uri']['#default_value'] =
    hpbx_normalize_number_human_readable($HuntGroup->id, $CFDestinationSet->destinations[0]->simple_destination);
  }
  elseif (!empty($CFDestinationSet->destinations[0]->destination) &&
      preg_match('/^sip\:vmu/', $CFDestinationSet->destinations[0]->destination)) {
  
    // Voicebox.
    $form['container']['call-forwards']['cfu']['times']['last_resort']['ood']['#default_value'] = 'ood';
    $form['container']['call-forwards']['cfu']['times']['call-forwards']['cfu']['destination_type']['#default_value'] = 'voicebox';
  }
  else {
    $form['container']['call-forwards']['cfu']['times']['last_resort']['nls']['#default_value'] = 'nls';
  }

  if (!is_null($CFTimeSet)) {
    hpbx_api_times_inverse($form['container']['call-forwards']['cfu']['times']);
  }


  if (isset($HuntGroup->profile_id) && hpbx_attribute_allowed('cloud_pbx_callqueue', $HuntGroup->profile_id, $ActiveReseller->_settings->generic_profile_set_id)) {
    $form['container']['queue'] = hpbx_queue_elements($SubscriberPreferences, $soundfiles);
  }
    
  $form['container']['voicemail'] = array(
      '#type' => 'container',
      '#title' => t('Voicemail'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#tree' => TRUE,
  );
  
  hpbx_voicemail_elements($form['container']['voicemail'], $VoicemailSettings);
  
  $form['container']['voicemail']['pin']['value']['#title'] = t('PIN*');
  
  $form['container']['no_answer_settings'] = array(
      '#type' => 'container',
      '#title' => t('No Answer Settings'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#tree' => TRUE,
  );

  $form_cfna = $form_state_resort = array();
  $Callforwards = SipwiseEntity::load('callforwards', $HuntGroup->id);
  hpbx_subscribers_callforwards_form($form_cfna, $form_state_resort, $Customer, $HuntGroup, $Callforwards,$ActiveReseller);

  // Some conversion.
  unset(
    $form_cfna['call-forwards']['cfb'],
    $form_cfna['call-forwards']['cfu'],
    $form_cfna['call-forwards']['cft'],
    $form_cfna['call-forwards']['cfna']['times']
  );

  $form_cfna['call-forwards']['cfna']['#theme_wrappers'] = array();
  $form['container']['no_answer_settings'] += $form_cfna['call-forwards'];

  $form['container']['members'] = array(
    '#type' => 'container',
    '#title' => t('Members'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => FALSE,
    '#attributes' => array('class'=> array('hpbx-hunting-group-members')),
  );

  $form['container']['members']['subscribers'] = array(
    '#type' => 'tableselect',
    '#tree' => TRUE,
    '#header' => array(),
    '#sortable' => TRUE,
    '#theme' => 'hpbx_huntgroup_user_tableselect',
    '#member_menu_title' => t('Drag members in the correct dialing sequence'),
    '#empty' => '<i>'. t('No subscribers found') . '</i>',
  );

  $form['container']['members']['subscribers']['#options'] = array();
  $form['container']['members']['subscribers']['#default_value'] = array();

  $options = array();

  foreach ($Customer->get_subscribers() as $Subscriber) {

    $ad_entry = hpbx_users_get_user($Subscriber->webusername);
    
    if (hpbx_attribute_allowed('huntgroup_member', $Subscriber->profile_id)) {
      if ($ad_entry) {
         $name = $ad_entry['sn'][0]. ', '. $ad_entry['givenname'][0];
      }
      elseif (!empty($Subscriber->external_id)) {
        $name = $Subscriber->external_id;
      }
      else {
        break;
      }

      $options[$Subscriber->id]['#caption'] = '';
      
      if (in_array($Subscriber->id, $HuntGroup->pbx_groupmember_ids)) {
        // Checked/unchecked.
        $form['container']['members']['subscribers']['#default_value'][$Subscriber->id] = $Subscriber->id;
      }

      $options[$Subscriber->id]['#caption'] .= '<input type="hidden" name="subscriber_weights['. $Subscriber->id .']"/>';
      
      $alias = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN) . ' ['. $Subscriber->pbx_extension . ']';
      $options[$Subscriber->id]['#caption'] .= <<<EOF
      <div class="hpbx-hunting-group-user-information">
        <div class="hpbx-hunting-group-user-name">{$name}</div>
        <div class="hpbx-hunting-group-user-phone">{$alias}</div>
      </div>
EOF;
    }
  }
  
  if (isset($HuntGroup->pbx_groupmember_ids)) {
    $options = array_replace(array_flip($HuntGroup->pbx_groupmember_ids), $options);
  }
  
  // Remove unwanted members.
  foreach ($options as $i => $option) {
    if (is_int($option)) {
      unset($options[$i]);
    }
  }

  $form['container']['members']['subscribers']['#options'] = $options;

  // Submit button.
  $form['container']['actions'] = array(
    '#prefix' => '<div class="row">',
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#suffix' => '</div>',
  );
  $form['container']['actions']['cancel'] = array(
    '#prefix' => '<div class="col-xs-12">',
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'hpbx/huntinggroups',
    '#attributes' => array('class' => array('hpbx-tab-form-link-button')),
  );
  $form['container']['actions']['submit'] = array(
    '#prefix' => '<span class="submit-button">',
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array(
      'class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')
    ),
    '#suffix' => '</span></div>',
  );
  return $form;
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_huntgroup_edit_form_validate($form, &$form_state) {

  global $ActiveReseller;

  $members = array();
  foreach ($form_state['input']['subscriber_weights'] as $subscriber_id => $weight) {
    if (is_numeric($weight) && isset($form_state['values']['subscribers'][$subscriber_id]) &&
      $form_state['values']['subscribers'][$subscriber_id] == $subscriber_id) {
      
      $members[$weight] = $subscriber_id;
    }
  }
  ksort($members);
  
  $form_state['values']['subscribers'] = $members;
  
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

  if (!$form_state['values']['id'] &&
    hpbx_bundle_settings_is_allowed($Customer, $ActiveReseller->_settings->hunt_group_subscriber_profile_id, HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE_SET|HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_PROFILE)<=0) {

    $SubscriberProfile = SipwiseEntity::load('subscriberprofiles',$ActiveReseller->_settings->hunt_group_subscriber_profile_id );
    form_set_error('', t('You have used up all your available @profile_names', array(
      '@profile_name' => drupal_strtolower($SubscriberProfile->name))
    ));
  }

  $Subscriber = NULL;
  if (isset($form_state['values']['id'])) {
    
    if ($SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $form_state['values']['id'])) {
  
      if (!empty($SubscriberPreferences->contract_sound_set)) {
        if ($SoundSet =  hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {
  
          foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
            if ($SoundFile->handle == 'office_hours') {
              $soundfiles[$SoundFile->handle] = $SoundFile;
            }
          }
        }
      }
    }
    $Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['id']);
  }
    
  // Validate if the external_id is unique for this customer.
  if (hpbx_external_id_exists($Customer, $form_state['values']['table']['external_id']['value'], $Subscriber)) {
    form_set_error('table][external_id][value', t('Name must be unique'));
  }
  
  // Validate extension.
  $extensions = hpbx_get_extensions($Customer);
  if ($form_state['values']['id']) {
    unset($extensions[$form_state['values']['id']]);
  }
  if (!empty($form_state['values']['table']['pbx_extension']['value']) &&
      in_array($form_state['values']['table']['pbx_extension']['value'], $extensions)) {
    form_set_error('table][pbx_extension', t('Extension is already in use. Please select another extension.'));
  }

  if ($form_state['values']['table']['pbx_extension']['value']!='') {
    
    hpbx_extension_validate('table', $form_state['values']['table'], $Customer);
  }

  $subscribers_set = FALSE;
  foreach ($form_state['values']['subscribers'] as $subscriber_id) {
    if (!empty($subscriber_id)) {
      $subscribers_set = TRUE;
    }
  }

  if (!$subscribers_set) {
    form_set_error('', t('Please select one or more members for this hunt group.'));
  }
  
  // Validate files.
  if (isset($_FILES)) {
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
      }
    }
  }

  // Unfortunatly we have to inverse the times for huntgroup, as the huntgroup is not processed
  // by assigned callforwards. Only the out of office announcement itself.
  $type = 'cfu';
  
  $form_state['values']['call-forwards'][$type]['times_api'] = array();

  if(!is_numeric($form_state['input']['table']['alias_number']['value'])){
    form_set_error('table][alias_number][value', t('Number field is required'));
  }


  if (!empty($form_state['values']['voicemail']['pin']['value']) && !ctype_digit($form_state['values']['voicemail']['pin']['value'])) {
    form_set_error('voicemail][pin][value', t('Please provide a valid numeric voicemail pin'));
  }

  
  // Format the ranges set by the range sliders.
  if (isset($form_state['values']['call-forwards']['cfu']['times'])) {
    
    $form_state['values']['call-forwards']['cfu']['times_orig'] = $form_state['values']['call-forwards']['cfu']['times'];

    // Inverse the input from slider. on Validate().
    hpbx_api_times_inverse($form_state['values']['call-forwards']['cfu']['times']);
    $form_state['values']['call-forwards'][$type]['times_api'] = hpbx_api_times_get($form_state['values']['call-forwards']['cfu']['times']);
    
    $form_state['values']['call-forwards'][$type]['times_api_orig'] = hpbx_api_times_get($form_state['values']['call-forwards']['cfu']['times_orig']);
    
    // In case ooa (announcement) is set, verify if audio (already present or uploaded) is set in case officehours are defined.
    if ($form_state['values']['last_resort'] == 'ooa' && count($form_state['values']['call-forwards'][$type]['times_api'])) {

      if (isset($form_state['input']['hpbx-audio-is-replaced-office-hours']) && empty($_FILES['office_hours']['name'])) {

        // Old file deleted and no new file uploaded.
        form_set_error('container][call-forwards][cfu][times][office_hours_upload][value', t('Office hours audio file is required'));
      }
      elseif (empty($soundfiles['office_hours']) && empty($_FILES['office_hours']['name'])) {
        // Nu current file and no new file uploaded.
        form_set_error('container][call-forwards][cfu][times][office_hours_upload][value', t('Office hours audio file is required'));
      }
    }
  }
  
}

function hpbx_huntgroup_edit_form_submit($form, $form_state) {
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

    if (!$PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id)) {
      drupal_set_message(t('Can\'t find the pilot subscriber preferences'));
      return;
    }

    // Verify an subscriber_id is set.
    if (!$form_state['values']['id']) {

      // New subscriber.
      $HuntGroup = SipwiseEntity::create('subscribers');

      // Set values which will not change in the future.
      $HuntGroup->webusername = uniqid((int)$customer_id);
      $HuntGroup->username = uniqid((int)$customer_id);
      $HuntGroup->password = hpbx_subscriber_password($customer_id);
      $HuntGroup->customer_id = (int)$customer_id;
      $HuntGroup->is_pbx_group = TRUE;
      $HuntGroup->administrative = FALSE;
      $HuntGroup->profile_set_id = $ActiveReseller->_settings->generic_profile_set_id;
      $HuntGroup->profile_id = $ActiveReseller->_settings->hunt_group_subscriber_profile_id;
      $HuntGroup->domain_id = $PilotSubscriber->domain_id;
      $HuntGroup->status = 'active';
    }
    else {

      // Existing subscriber.
      $HuntGroup = SipwiseEntity::load('subscribers', $form_state['values']['id']);
    }

    $HuntGroup->pbx_hunt_policy = trim($form_state['values']['table']['pbx_hunt_policy']['value']);
    
    if (!isset($form_state['values']['table']['pbx_hunt_timeout']['value']) || $form_state['values']['table']['pbx_hunt_timeout']['value'] == '') {
      $form_state['values']['table']['pbx_hunt_timeout']['value'] = 10;
    }
    
    // For parallel huntgroups set the timeout on 0.
    if ($HuntGroup->pbx_hunt_policy == 'parallel') {
      $HuntGroup->pbx_hunt_timeout = 0;
    }
    else {
      $HuntGroup->pbx_hunt_timeout = (int)trim($form_state['values']['table']['pbx_hunt_timeout']['value']);
    }

    $HuntGroup->external_id = trim($form_state['values']['table']['external_id']['value']);
    $HuntGroup->pbx_extension = trim($form_state['values']['table']['pbx_extension']['value']);
    $HuntGroup->pbx_groupmember_ids = array_values($form_state['values']['subscribers']);

    $alias_number_original = $HuntGroup->get_alias_number(HPBX_NUMBER_FORMAT_GSN);

    $an_change = FALSE;
  
    $alias_no_original = $form_state['values']['table']['alias_number']['value'];
    
    if(empty($form_state['values']['table']['alias_number']['value']) && isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){
      $form_state['values']['table']['alias_number']['value'] = $form_state['values']['table']['cli']['value'] . $form_state['values']['table']['pbx_extension']['value'];
    } 

    if ($form_state['values']['table']['alias_number']['value']!=$alias_number_original) {

      $an_change = TRUE;
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
      
      // Remove the newly set alias number from the pilot subscriber.
       if(strlen($form_state['values']['table']['alias_number']['value']) >= $min_length && strlen($form_state['values']['table']['alias_number']['value']) <= $max_length && !empty($alias_no_original)){
         $PilotSubscriber->alias_number_unset($Customer, $form_state['values']['table']['alias_number']['value']);
       }
      // Set the newly set alias number on the subscriber, remove it when empty.
      $HuntGroup->alias_number_set($Customer, $form_state['values']['table']['alias_number']['value']);
    }

    $PilotSubscriber->save();
    $HuntGroup->save();
    
    // Add the original alias number to the pilot number again, so it will be available
    // to others subscribers to be used.
    if ($an_change && !empty($alias_number_original)) {
      $PilotSubscriber->alias_number_set($Customer, $alias_number_original, FALSE);
      $PilotSubscriber->save();
    }

    // Load the subscriberpreferences.
    $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $HuntGroup->id);
    $SubscriberPreferences->display_name = $HuntGroup->pbx_extension;
    $SubscriberPreferences->cc = $ActiveReseller->_settings->country_code;
    if ($area =$Customer->get_area_code($form_state['values']['table']['alias_number']['value'])) {
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
    
    // Check if the original queue_greeting file was deleted, we need to delete
    // it (and maybe upload a new file afterwords.
    if (isset($form_state['input']['hpbx-audio-is-replaced-queue-greeting']) &&
      $SoundSet =  hpbx_get_soundset_on_name($SubscriberPreferences->contract_sound_set, $Customer->id)) {
    
      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
        if ($SoundFile->handle == 'queue_greeting') {
          $SoundFile->delete();
        }
      }
    }    
    
    // Upload soundfiles (queue_greeting, office_hours).
    $soundset_name = 'Huntgroup Subscriber '. $HuntGroup->id;

    if (hpbx_soundset_upload_file($HuntGroup, $soundset_name)) {
      $SubscriberPreferences->contract_sound_set = $soundset_name;
    }

    // Load soundset.
    $SoundSet = hpbx_get_soundset_on_name($soundset_name);
    
    // In case the music-on-hold file is uploaded for the pilot subscriber: 
    // - Copy music-on-hold soundfile from pilot subscriber to huntgroup soundset.
    // - Add the soundfile to the huntgroup soundset
    // - Enable music-on-hold for this huntgroup.
    
    // Initialy disable music-on-hold.
    $SubscriberPreferences->music_on_hold = FALSE;
    
    // Try to retrieve music-on-hold sound file from the pilot subscriber. 
    $pilot_soundfiles = hpbx_get_soundfiles_from_soundset(
      $HuntGroup->customer_id, $PilotSubscriberPreferences->contract_sound_set, 'music_on_hold');
    
    if (hpbx_attribute_allowed('music_on_hold', $HuntGroup->profile_id, $HuntGroup->profile_set_id) === TRUE) {

      // Enable music-on-hold in the preferences.
      $SubscriberPreferences->music_on_hold = TRUE;

      // Verify if the pilot subscriber does have a music on hold file and try
      // to retrieve the huntgroup soundset.
      if (isset($pilot_soundfiles['music_on_hold']) && $SoundSet) {
      
        // Copy the music-on-hold soundfile from pilot subscriber soundfile
        // to the huntgroup soundset.
        hpbx_soundfile_copy($pilot_soundfiles['music_on_hold'], $SoundSet);
      }      
    }
    else {
      $SubscriberPreferences->music_on_hold = FALSE;
    }

    
  //CLI Restriction.
    if (isset($alias_no_original) && is_numeric($alias_no_original) && $alias_no_original!=0) {
      	//Set the gpp0 value equal to the aliasnumber for billing purposses.
        $SubscriberPreferences->cli = $SubscriberPreferences->gpp0  = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['alias_number']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls));

        if (in_array($ActiveReseller->_settings->country_code . $form_state['values']['table']['alias_number']['value'],
	        $CustomerPreferences->allowed_clis)) {
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
      $SubscriberPreferences->cli = $ActiveReseller->_settings->country_code . substr($form_state['values']['table']['cli']['value'], strlen($ActiveReseller->_settings->digit_for_national_calls)).$HuntGroup->pbx_extension;
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
    $SubscriberPreferences->gpp1 = $HuntGroup->pbx_extension;
    
    // Set announcement language.
    if (!empty($form_state['values']['table']['language']['value'])) {
      $SubscriberPreferences->language = $form_state['values']['table']['language']['value'];
    }
    else {
      unset($SubscriberPreferences->language);
    }

    // Queue (we need to check on the input array, as the elements are
    // disabled using custom Trimm js) and due to that set on values
    // and not set on input.
    if (!is_null($form_state['input']['queue']['cloud_pbx_callqueue']['value'])) {

      $SubscriberPreferences->max_queue_length = (int)$form_state['values']['queue']['max_queue_length']['value'];
      $SubscriberPreferences->queue_wrap_up_time = (int)$form_state['values']['queue']['queue_wrap_up_time']['value'];
      $SubscriberPreferences->cloud_pbx_callqueue = TRUE;

      // Copy queue_waiting_music from  
      
      // Verify if the pilot subscriber does have a music on hold file and try
      // to retrieve the huntgroup soundset.
      if (isset($pilot_soundfiles['music_on_hold']) && $SoundSet) {
      
        // Copy the music-on-hold soundfile from pilot subscriber soundfile.
        // to the huntgroup soundset (as queue_waiting_music).
        hpbx_soundfile_copy($pilot_soundfiles['music_on_hold'], $SoundSet, 'queue_waiting_music');
      }
    }
    else {
      $SubscriberPreferences->cloud_pbx_callqueue = FALSE;
      unset($SubscriberPreferences->max_queue_length, $SubscriberPreferences->queue_wrap_up_time);
    }

    if ($HuntGroup->pbx_hunt_policy == 'parallel') {
      
      if (!isset($form_state['values']['table']['pbx_hunt_timeout']['value']) || $form_state['values']['table']['pbx_hunt_timeout']['value'] == '') {
        $SubscriberPreferences->ringtimeout = 30;
      }
      else {
        $SubscriberPreferences->ringtimeout = (int)$form_state['values']['table']['pbx_hunt_timeout']['value'];
      }
    }
    else {
      $SubscriberPreferences->ringtimeout = $HuntGroup->pbx_hunt_timeout * count($HuntGroup->pbx_groupmember_ids);
    }
    
    // Set allowed ip's in case the settings is set.
    if (isset($ActiveReseller->_settings->enable_restricted_access) &&
      $ActiveReseller->_settings->enable_restricted_access &&
      isset($Customer->_settings->allowed_ips)
    ) {

      $SubscriberPreferences->allowed_ips = $Customer->_settings->allowed_ips;
    }
    elseif (isset($SubscriberPreferences->allowed_ips)) {

      // Currently set and not set on customer level, so unset the variable.
      unset($SubscriberPreferences->allowed_ips);
    }


    $SubscriberPreferences->save();

    // Office hours.
    $out_of_office_hours_destination = FALSE;
    if(empty($alias_no_original)){
     $DDI = $form_state['values']['table']['cli']['value'];
    }else{
     $DDI = $form_state['values']['table']['alias_number']['value'];
    }
    
    if ($form_state['values']['last_resort'] == 'ooa') {
      $out_of_office_hours_destination = 'officehours';
    }
    elseif ($form_state['values']['last_resort'] == 'ood') {
      if ($form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_type'] == 'voicebox') {
        $out_of_office_hours_destination = 'voicebox';
      }
      elseif ($form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_type'] == 'uri') {
         foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
      if($form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri']==$SubscriberForCLI->pbx_extension){
        $form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri'] = $DDI . $SubscriberForCLI->pbx_extension;
      }
    }

        /*foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
          //$alias_number[] = $SubscriberForCLI->get_alias_number(HPBX_NUMBER_FORMAT_SN);
          $primary_numbers[] = (array) $SubscriberForCLI->primary_number; 
        }
        foreach($primary_numbers as $pn){
          $primary_number[] = $pn['ac'].$pn['sn'];
        }
         
       if(in_array($form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri'],$primary_number)){
          $out_of_office_hours_destination = $ActiveReseller->_settings->country_code . $form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri'];
       
       }else{
         $out_of_office_hours_destination = sipwise_api_normalize_number($HuntGroup->id, $form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri']);
       }*/
       $out_of_office_hours_destination = sipwise_api_normalize_number($HuntGroup->id, $form_state['values']['call-forwards']['cfu']['times']['cfu']['destination_uri']);
      }
    }
    
    if ($out_of_office_hours_destination) {

      // Set out-of-office and timeranges.
      hpbx_set_destination_sets($HuntGroup, $out_of_office_hours_destination, $form_state['values']['call-forwards']['cfu']['times_api'], FALSE);
    }
    else {
      $CFMapping = SipwiseEntity::load('cfmappings', $HuntGroup->id, TRUE);
      $CFMapping->cfu = array();
      $CFMapping->save();
    }

    // No answer (cfna, cfb,cft IN office hours, so not the inversed (out of office) hours.
    $no_answer_destination = FALSE; 
    if ($form_state['values']['no_answer_settings']['cfna']['destination_type'] == 'voicebox') {
      $no_answer_destination = 'voicebox';
    }
    elseif($form_state['values']['no_answer_settings']['cfna']['destination_type'] == 'uri') {
       foreach ($Customer->get_subscribers(FALSE,TRUE,TRUE,TRUE) as $SubscriberForCLI) {
      if($form_state['values']['no_answer_settings']['cfna']['destination_uri']==$SubscriberForCLI->pbx_extension){
        $form_state['values']['no_answer_settings']['cfna']['destination_uri'] = $DDI . $SubscriberForCLI->pbx_extension;
      }
    }

      $no_answer_destination = $form_state['values']['no_answer_settings']['cfna']['destination_uri'];
    }
    
    if ($no_answer_destination) {
        
      // Load CallForwards.
      $Callforwards = SipwiseEntity::load('callforwards', $HuntGroup->id, TRUE);
      
      foreach (array('cfb', 'cfna', 'cft') as $cf_type) {
        $form_state_no_answer['values']['call-forwards'][$cf_type] = array(
            'destination_type' => $form_state['values']['no_answer_settings']['cfna']['destination_type'],
            'destination_timeout' => 300,
            'destination_uri' => $no_answer_destination
        );
        
        if ($cf_type == 'cft' && !empty($SubscriberPreferences->ringtimeout)) {
          $form_state_no_answer['values']['call-forwards']['cft']['ringtimeout'] = $SubscriberPreferences->ringtimeout;
        }

        hpbx_set_simple_callforward_values($cf_type, $form_state_no_answer, $Callforwards, $HuntGroup,$ActiveReseller, $Customer);
      }
      $Callforwards->save();
    }
    else {
      // Empty cfna, cfb and cft for this huntgroup.
      // Load CallForwards.
      $Callforwards = SipwiseEntity::load('callforwards', $HuntGroup->id, TRUE);
      unset($Callforwards->cfb, $Callforwards->cfna, $Callforwards->cft);
      $Callforwards->save();
    }

    // Save voicemail settings.
    if ($VoicemailSettings = SipwiseEntity::load('voicemailsettings', $HuntGroup->id)) {
    
      $VoicemailSettings->pin = trim($form_state['values']['voicemail']['pin']['value']);
      $VoicemailSettings->email = trim($form_state['values']['voicemail']['email']['value']);
      $VoicemailSettings->delete = boolval($form_state['values']['voicemail']['delete']['value']);
      $VoicemailSettings->attach = boolval($form_state['values']['voicemail']['attach']['value']);
      $VoicemailSettings->save();
    }
  }
  catch (Exception $e) {

    // Failed to create customer.
    drupal_set_message(t('Failed to update'), 'error');

    $catched = TRUE;
    //drupal_set_message(t($e->getMessage()), 'error');
  }

  foreach (array('subscribers', 'subscriberpreferences') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $HuntGroup->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));
  
    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared subscribers list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }
  
  if (!isset($catched)) {
    drupal_set_message(t('Hunt group successfully saved'));
  }

  drupal_goto('hpbx/huntinggroups');
}

