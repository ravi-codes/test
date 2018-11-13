<?php

/**
 * Callback for hpbx/customer/location.
 *
 * @param int $reseller_id
 */
function hpbx_customer_location_callback($reseller_id = NULL) {

  if (is_null($reseller_id)) {
    $reseller_id = sipwise_api_get_active_reseller();
  }

  try {

    $result = array();

    foreach (array('region', 'township', 'city', 'zgm') as $field) {
      $query = db_select('hpbx_pl_emergency_indexes', 'plei');
      $query->condition('reseller_id', $reseller_id);

      if (!empty($_GET['region'])) {
        $query->condition('plei.region', $_GET['region']);
      }
      if (!empty($_GET['township'])) {
        $query->condition('plei.township', $_GET['township']);
      }
      if (!empty($_GET['city'])) {
        $query->condition('plei.city', $_GET['city']);
      }
      if (!empty($_GET['zgm'])) {
        $query->condition('plei.zgm', $_GET['zgm']);
      }
      $query->distinct();

      $query->fields('plei', array($field));

      $result[$field == 'zgm' ? 'z-gm' : $field] = array_keys($query->execute()->fetchAllKeyed());
    }
    drupal_json_output($result);
    die();
  }
  catch (Exception $e) {
    echo $e->getMessage();
    die();
  }
}

/**
 * Callback for hpbx/customers.
 *
 * @param int $reseller_id
 *
 * @return string
 */
function hpbx_customers_overview_page($reseller_id = NULL) {
    
  if(!isset($_GET['search'])) {
    unset($_SESSION['ccs']['hpbx']['customer_search']['customers']);
  }  
  global $ActiveReseller, $conf, $user;
  $domain = NULL;
  $add = FALSE;
  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }
  else {
    $reseller_id = $ActiveReseller->id;
  }
  // Add css.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  // Set header text/link.
  if (user_access('hpbx edit customer')) {
    $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create customer') . '</h5>';
    if (!is_null($reseller_id) && user_access('hpbx view reseller')) {
      $add = l($text, 'hpbx/affiliate/'. $reseller_id . '/customer/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));
    }
    else {
      $add = l($text, 'hpbx/customer/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));
    }
  }

  $search_form = drupal_get_form('hpbx_customer_search_form', $reseller_id);
  $form = array(
    'table' => array(
      '#add' => array('data' => $add . render($search_form), 'attributes' => array('class' => 'hpbx-table-caption')),
      '#type' => 'table',
      '#tree' => TRUE,
      '#theme' => 'hpbx_table',
      '#class' => array('hpbx-table', 'hpbx-table-fullwidth', 'hpbx-table-customers', 'hpbx-table-sortable'),
      )
  );
  $form['table']['#header'] = array(
    'company' => array('data'=> t('Company name'), 'class' => array('hpbx-show'), 'field' => 'company', 'sort' => 'asc'),    
  );
  if((user_access('hpbx view business partner customer')|| user_access('hpbx view reseller')) && isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
  	$form['table']['#header'] += array('business_partner' => array('data'=> t('Business Partner'), 'class' => array('hpbx-show-xs'), 'field' => 'bp_name'));
  }   
  $form['table']['#header']['external_id'] = array('data' => t('Customer Id'), 'class' => array('hpbx-show-sm'), 'field' => 'external_id');
   
  if(isset($ActiveReseller->_settings->enable_multisite) && !empty($ActiveReseller->_settings->enable_multisite)){
 	$form['table']['#header'] += array('multisite' => array('data'=> t('Multisite'), 'class' => array('hpbx-show-xs'), 'field' => 'multisite'));
  } 
   
  $form['table']['#header'] += array(    
    'status' => array('data'=> t('Status'), 'class' => array('hpbx-show-xs'), 'field' => 'status'),
    'operations' => array('data'=> '', 'class' => array('hpbx-show')),    
  );
      
  
  
  $table_order = tablesort_get_order($form['table']['#header']);
  $table_sort = tablesort_get_sort($form['table']['#header']);

  $query = array();

  $query['reseller_id'] = $reseller_id;

  if (!hpbx_customer_search_state($reseller_id)) {

    // Remove all entries for this reseller.
    hpbx_customer_search_empty($reseller_id);

    // Rebuild local customer search table.
    sipwise_api_get_all('customers', $query, FALSE, FALSE, FALSE);
  }

  $sort_names = array();

  $customers_table = array();
  
  if(isset($user->data['sipwise']['bp_name']) && !empty($user->data['sipwise']['bp_name'])){
  	$bp_name = $user->data['sipwise']['bp_name'];
  }
  
  $customers = hpbx_customer_get_local($query['reseller_id'], $bp_name);
  
// Modified for #9759 - PE DEV: Customer overview - Improved caching of customer information
  foreach ($customers as $cust) {

    if (empty($cust->customer_id)) continue;
    //if (!$Customer = SipwiseEntity::load('customers', $cust->customer_id)) continue;

    //$Customer = SipwiseEntity::load('customers', $Customer->id);
    if (isset($_SESSION['ccs']['hpbx']['customer_search']['customers']) &&
      !in_array($cust->customer_id, $_SESSION['ccs']['hpbx']['customer_search']['customers'])) {
      continue;
    }

    // Verify if the external ID is set.
    if (empty($cust->external_id)) continue;

    $customers_table[$cust->customer_id]['#url'] = 'customer/'. $cust->customer_id . '/edit';

    $customers_table[$cust->customer_id]['company']['data'] = array('#markup' => !empty($cust->company) ? truncate_utf8($cust->company, 55, FALSE, TRUE) : '');
    $customers_table[$cust->customer_id]['company']['#attributes']['class'] = array('hpbx-show');
    $sort_names['company'][$cust->customer_id] = !empty($cust->company) ? $cust->company : '';
    
    if((user_access('hpbx view business partner customer')|| user_access('hpbx view reseller')) && isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
	  $customers_table[$cust->customer_id]['bp_name']['data'] = array('#markup' => !empty($cust->bp_name) ? truncate_utf8($cust->bp_name, 55, FALSE, TRUE) : '');
      $customers_table[$cust->customer_id]['bp_name']['#attributes']['class'] = array('hpbx-show');
      $sort_names['bp_name'][$cust->customer_id] = !empty($cust->bp_name) ? $cust->bp_name : '';
	} 

    $customers_table[$cust->customer_id]['external_id']['data'] = array('#markup' => hpbx_truncate_name_element($cust->external_id), '#attributes' => array('title' => check_plain($cust->external_id)));
    $customers_table[$cust->customer_id]['external_id']['#attributes']['class'] = array('hpbx-show-sm');
    $sort_names['external_id'][$cust->customer_id] = $cust->external_id;     
    
    if(isset($ActiveReseller->_settings->enable_multisite) && !empty($ActiveReseller->_settings->enable_multisite)){
      if($cust->multisite){
        $customers_table[$cust->customer_id]['multisite']['data'] = array('#markup' => 'Yes');
        $customers_table[$cust->customer_id]['multisite']['#attributes']['class'] = array('hpbx-show');            
      }
      else{
        $customers_table[$cust->customer_id]['multisite']['data'] = array('#markup' => 'No');
        $customers_table[$cust->customer_id]['multisite']['#attributes']['class'] = array('hpbx-show','hpbx-tab-item','r-tabs-state-active');      
      }      
      $sort_names['multisite'][$cust->customer_id] = !empty($cust->multisite) ? $cust->multisite : ''; 
    }  
    
    
    $customers_table[$cust->customer_id]['status']['data'] = array('#markup' => (!empty($ActiveReseller->_settings->suspend_ncos_level) && $cust->ncos == $ActiveReseller->_settings->suspend_ncos_level) ? t('Suspended') : t(drupal_ucfirst($cust->status)));
    $customers_table[$cust->customer_id]['status']['#attributes']['class'] = array('hpbx-show-xs');
    $sort_names['status'][$cust->customer_id] = $customers_table[$cust->customer_id]['status']['data']['#markup'];

    $op = array();

    if (user_access('hpbx delete customer') || user_access('hpbx delete reseller customer')) {
      if (SipwiseCustomer::deleted_allowed_on_date($cust->suspend_date) == HPBX_CUSTOMER_SUSPENDED_DELETE_ALLOWED || variable_get('hpbx_sipwise_delete_allowed', FALSE)) {

        if (!is_null($reseller_id) && user_access('hpbx view reseller')) {
          $op[] = l(t('Remove'), 'hpbx/affiliate/'. $reseller_id . '/customer/'. $cust->customer_id . '/delete', array('attributes' => array('class' => array('js-delete', 'sg-btn', 'sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
        }
        else {
          $op[] = l(t('Remove'), 'hpbx/customer/'. $cust->customer_id . '/delete', array('attributes' => array('class' => array('js-delete', 'sg-btn', 'sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
        }

        $customers_table[$cust->customer_id]['#attributes']['data-delete-url'] = 'customer/'. $cust->customer_id . '/delete';
      }
      elseif (SipwiseCustomer::deleted_allowed_on_date($cust->suspend_date) == HPBX_CUSTOMER_SUSPENDED_DELETE_NOT_ALLOWED) {
        $op[] = '<span title="'. t('Customer is suspended since @suspend_date and can be remove from @remove_date.', array('@suspend_date' => date('Y-m-d H:i', strtotime(hpbx_convert_to_local_datetime($cust->suspend_date, TRUE))), '@remove_date' => date('Y-m-d H:i', strtotime('+7 days', strtotime(hpbx_convert_to_local_datetime($cust->suspend_date, TRUE)))))) .'" class="sg-btn-small sg-btn sg-without-icon sg-disabled">'. t('Remove') . '</span>';
      }
      else {
        $op[] = '<span title="'. t('Customer needs to be suspended before it can be removed') .'" class="sg-btn-small sg-btn sg-without-icon sg-disabled">'. t('Remove') . '</span>';
      }
    }
    
    $subadmin = sipwise_api_get_all('subscribers', array('customer_id' => $cust->customer_id, 'is_admin' => 1, 'is_pbx_pilot' =>0));
    
    if (user_access('hpbx masquerade customer')) {
      if (!is_null($reseller_id)) {

        if (variable_get('hpbx_verify_masquerade_login_btn', FALSE)) {
          $can_login = FALSE;
          foreach (sipwise_api_get_all('subscribers', array('customer_id' => $cust->customer_id, 'is_admin' => 1)) as $Subscriber) {

            if (!empty($Subscriber->webusername)) {
              if ($Subscriber->get_active_directory_entry()) {
                if (user_load($Subscriber->webusername)) {
                  $can_login = TRUE;
                  break;
                }
              }
            }
          }
        }
        else {
          $can_login = TRUE;
        }
		
      // if more than one subadmins are the for a Customer- Riderect to pre-masquerade page
        if ($can_login) {
        	if(count($subadmin) <= 1){
					$op[] = l(t('Login'), 'hpbx/affiliate/'. $reseller_id . '/customer/'. $cust->customer_id . '/masquerade', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
			}else{
				$op[] = l(t('Login'), 'hpbx/affiliate/'. $reseller_id . '/customer/'. $cust->customer_id . '/pre-masquerade', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
			}
          
        }
        else {
          $op[] = '<span title="'. t('Login disabled. This customer is created on another environment') .'" class="sg-btn-small sg-btn sg-without-icon sg-disabled">'. t('Login') . '</span>';
        }
      }
      else {
        $op[] = l(t('Login'), 'hpbx/customer/'. $cust->customer_id . '/masquerade', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
      }
    }

    $customers_table[$cust->customer_id]['operations']['data'] = array('#markup' => implode(' ', $op));
    $customers_table[$cust->customer_id]['operations']['#attributes']['class'] = array('hpbx-show');  
      
  }



  $page = pager_find_page() + 1;
  $rows = 20;

  $order = $sort_names['company'];

  if (isset($table_order['sql']) && isset($sort_names[$table_order['sql']])) {
    $order = $sort_names[$table_order['sql']];
  }
  $direction = SORT_ASC;
  if ($table_sort == 'desc') {
    $direction = SORT_DESC;
  }

  // Sort based on $sort_names.
  $a = array($customers_table, $order);

  array_multisort($a[0], $direction, SORT_NUMERIC,
    $a[1], $direction, SORT_NATURAL|SORT_FLAG_CASE);


  $a[0] = array_splice($a[0], ($page-1)* $rows, $rows);

  $form['table'] = array_merge($form['table'], $a[0]);

  $pager = '';

  $total_count = count($customers_table);

  if ($total_count > $rows) {
    $total_pages = ceil($total_count / $rows);

    // Set pager options.
    pager_default_initialize($total_count, $rows);

    // Render pager.
    $pager = theme('pager', array('element' => 0, 'quantity' => 8, 'parameters' => array(), 'tags' => ''));
  }

  return drupal_render($form) . '<div class="'. 'hpbx-pagination-container' .'">'. $pager . '</div>';
}

/**
 *
 * @param array $form
 * @param array $form_state
 * @param integer $customer_id
 * @param integer $reseller_id
 *
 * @return array $form
 */
function hpbx_customer_edit_form($form, $form_state, $customer_id = NULL, $reseller_id = NULL) {

  global $ActiveReseller, $user;

  // Temporary set the reseller_id.
  if (!is_null($reseller_id)) {
  	sipwise_api_activate_reseller($reseller_id);
  }else{
  	$reseller_id = sipwise_api_get_active_reseller();
  }

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_customers.js', array('scope' => 'footer'));

  $CustomerPreferences = $PilotSubscriber = $CustomerContact = $Customer = NULL;

  // Verify if the customer_id is passed.
  if (!is_null($customer_id)) {

    if (!is_null($reseller_id)) {
      sipwise_api_activate_reseller((int)$reseller_id);
    }

    if ($Customer = SipwiseEntity::load('customers', $customer_id)) {
      // Get customer contact.
      $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);

      // Validate reseller_id.
      if ($CustomerContact->reseller_id!= $ActiveReseller->id) {
        drupal_not_found();
        exit();
      }

      // Get customer preferences.
      $CustomerPreferences = SipwiseEntity::load('customerpreferences', $Customer->id);

      if (!$Customer->_settings) {
        $Customer->_settings = new stdClass();
      }

      $PilotSubscriber = $Customer->get_pilot_subscriber();

      drupal_set_title(t('!name', array('!name' => $CustomerContact->company)));
    }
    else {
      drupal_not_found();
      exit();
    }
  }
  else {
    drupal_set_title(t('Create customer'));

    // Dummy settings class.
    $Customer = new stdClass();
    $Customer->_settings = new stdClass();
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
    '#access' => user_access('hpbx edit customer'),
    '#title' => t('Customer settings'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#collapsed' => FALSE,
  );

  // Set the existing id.
  $form['reseller_id'] = array(
    '#type' => 'value',
    '#value' => !is_null($reseller_id) ? $reseller_id : FALSE,
  );

  // Set the existing id.
  $form['customer_id'] = array(
    '#type' => 'hidden',
    '#value' => !is_null($customer_id) ? $customer_id : FALSE,
  ); 

  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Customer->id) ? $Customer->id : FALSE,
  );

  // Set the existing id.
  $form['subscriber_id'] = array(
    '#type' => 'value',
    '#value' => !empty($PilotSubscriber->id) ? $PilotSubscriber->id : FALSE,
  );

  // External ID.
  $form['general']['table']['company']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($CustomerContact->company) ? $CustomerContact->company : '',
    '#maxlength' => 255,
    '#required' => TRUE,
    '#title' => t('Company Name'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Company Name')),
  );

  // External ID.
  $form['general']['table']['external_id']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($Customer->external_id) ? $Customer->external_id : '',
    '#maxlength' => $ActiveReseller->_settings->customer_id_length,
    '#required' => TRUE,
    '#title' => t('Customer Id'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Customer Id')),
  );
 
  if (isset($ActiveReseller->_settings->domain_id) && count((array)$ActiveReseller->_settings->domain_id) > 1) {
    $options = array();
      foreach ($ActiveReseller->_settings->domain_id as $key => $value) {
        if (!empty($key)) {
		$Domain = SipwiseEntity::load('domains', $value);
          $options[$Domain->id] = $Domain->domain;
        }     
      }
  }else{
  	$options = (array)$ActiveReseller->_settings->domain_id;
  }
	
// Profile Set for this customer. 
  if (count($options) > 1){
    $form['general']['table']['domain_id']['value'] = array(
	  '#type' => 'select',
	  '#default_value' => $PilotSubscriber->domain_id,        
	  '#options' => $options,
	  '#title' => t('Domain'),
	  '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
   ); 

    if(arg(3)=='customer' && arg(5)=='edit'){
      $form['general']['table']['domain_id']['value']['#disabled'] = TRUE;
    }  
  }else{
  	$form['general']['table']['domain_id']['value'] = array(
        '#type' => 'value',
        '#value' => current($options),
    );
  }
    
  if (isset($ActiveReseller->_settings->customer_concurrent_max)) {
    $options = array();
    foreach ($ActiveReseller->_settings->customer_concurrent_max as $m) {

      if (!empty($m)) {
        $options[$m] = (int)$m;
      }
      elseif($m == '0') {
        $options[0] = t('Unlimited');
      }
    }

    // In case onlu one option is available, set the value as a static value.
    if (count($options) == 1) {
      $form['general']['table']['concurrent_max']['value'] = array(
        '#type' => 'value',
        '#value' => current($options),
      );
    }

    // Offer the user een dropdown.
    elseif (count($options) > 1) {
      // Profile Set for this customer.
      $form['general']['table']['concurrent_max']['value'] = array(
        '#type' => 'select',
        '#default_value' => $CustomerPreferences->concurrent_max,
        '#required' => TRUE,
        '#options' => $options,
        '#title' => t('Maximum total concurrent calls'),
        '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
      );
    }
  }

  // Get all reseller profile sets.
  $options = array();
  foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
    if ($SubscriberProfileSet->name != 'Generic') {
      $options[$SubscriberProfileSet->id] = t($SubscriberProfileSet->name);
    }
  }

  // Profile Set for this customer.
  $form['general']['table']['profile_set_id']['value'] = array(
    '#type' => 'select',
    '#default_value' => !empty($Customer->_settings->profile_set_id) ? $Customer->_settings->profile_set_id : NULL,
    '#required' => TRUE,
    '#options' => $options,
    '#title' => t('Profile Set'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  if (isset($Customer->id)) {

    // Status
    $form['general']['table']['suspended']['value'] = array(
      '#type' => 'checkbox',
      '#title' => t('Suspended'),
      '#title_display' => 'before',
    );

    if (isset($CustomerPreferences->ncos) && $CustomerPreferences->ncos == $ActiveReseller->_settings->suspend_ncos_level) {
      $form['general']['table']['suspended']['value']['#default_value'] = TRUE;
      $form['general']['table']['suspended']['value']['#description'] = t('Since @suspend_date', array('@suspend_date' => date('Y-m-d H:i', strtotime(hpbx_convert_to_local_datetime($Customer->_settings->suspend_date, TRUE)))));
    }
    else {
      $form['general']['table']['suspended']['value']['#default_value'] = FALSE;
    }
  }

  /////////////
  // Derive Extension From DID
  $form['general']['table']['derive_extension_from_did']['value'] = array(
    '#title' => t('Derive extension from DID'),
    '#type' => 'select',
    '#options' => array(
      0 => t('No'),
      1 => t('Yes'),
    ),
    '#empty_option' => t('Choose an option'),
    '#required' => TRUE,
    '#default_value' => isset($Customer->_settings->derive_extension_from_did) ? (int)$Customer->_settings->derive_extension_from_did : NULL,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  // Extension Length.
  $form['general']['table']['extension_length']['value'] = array(
    '#title' => t('Extension Length'),
    '#type' => 'select',
    '#options' => drupal_map_assoc(range(2, $ActiveReseller->_settings->customer_max_extension_length)),
    '#empty_option' => t('Choose an option'),
    '#required' => TRUE,
    '#default_value' => !empty($Customer->_settings->extension_length) ? $Customer->_settings->extension_length: NULL,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  if (isset($Customer->id) && count($Customer->get_subscribers(FALSE, FALSE, TRUE, TRUE)) > 0) {
    $form['general']['table']['derive_extension_from_did']['value']['#disabled'] = TRUE;
    $form['general']['table']['extension_length']['value']['#disabled'] = TRUE;
  }

  if (!isset($ActiveReseller->_settings->automatic_pilot_number_generation) || !$ActiveReseller->_settings->automatic_pilot_number_generation) {
    $form['general']['table']['pilot_number']['value'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($PilotSubscriber->primary_number) ? hpbx_parse_primary_number($PilotSubscriber->primary_number): '',
      '#required' => TRUE,
      '#size' => 30,
      '#title'=> t('Pilot number'),
      '#maxlength' => $ActiveReseller->_settings->pilot_subscriber_number_max_digits,
      '#rules' => array(
        array(
          'rule' => 'length['. $ActiveReseller->_settings->pilot_subscriber_number_min_digits .','. $ActiveReseller->_settings->pilot_subscriber_number_max_digits . ']',
          'error' => t('Invalid pilotnumber size'),
        )
      ),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Pilot number')),
    );
  }

  if (!empty($ActiveReseller->_settings->enable_restricted_access)) {
    $form['general']['table']['allowed_ips']['value'] = array(
      '#type' => 'textarea',
      '#default_value' => isset($Customer->_settings->allowed_ips) ? str_replace("/32", "", implode(PHP_EOL, $Customer->_settings->allowed_ips)) : '',
      '#required' => FALSE,
      '#rows' => 3,
      '#title' => t('Allowed IPs'),
      '#attributes' => array('class' => array('sg-element')),
    );
  }
  
  if (!empty($ActiveReseller->_settings->enable_multisite)) {
    $form['general']['table']['multi_site']['value'] = array(
      '#title' => t('Multisite'),
      '#type' => 'select',
      '#options' => array(
        0 => t('No'),
        1 => t('Yes'),
      ),
      //'#empty_option' => t('Choose an option'),
      '#required' => TRUE,
      '#default_value' => isset($Customer->_settings->multi_site) ? (int)$Customer->_settings->multi_site : 0,
      '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
    );
    
    $Customer->_settings->table = 'multisite';
    
    $form['general']['multisite'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#title' => t('Site Settings'),
      '#access' => user_access('hpbx edit customer'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#collapsed' => FALSE,
    );    
    
    hpbx_ajax_add_more_sites($form['general'], $form_state, $Customer->_settings);
     
    if (isset($Customer->id)) {
      foreach (element_children($form['general']['multisite']['table']) as $element) {        
        $el = &$form['general']['multisite']['table'][$element];  
        foreach (json_decode(json_encode($Customer->_settings->multisite), TRUE) as $id => $values){
          foreach ($values as $key => $value){
          	if($el['id']['data']['#default_value'] == $id){
          	  if($key == 'ips'){
                $el['ips']['data']['#default_value'] = str_replace("/32", "", implode(PHP_EOL, $value));
              }	
          	}            
          }        
        }
      }
    }         
  }

  if((user_access('hpbx view business partner customer')|| user_access('hpbx view reseller')) && isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
  	$query = db_select('hpbx_business_partners','bp')->fields('bp')->condition('reseller_id', $reseller_id)->execute();  
    $results = $query->fetchAll();
  
    foreach ($results as $res){
  	  $bpoptions[$res->bp_name] = t($res->bp_name);
    }
  	if(count($bpoptions)>= 1){
      $form['general']['table']['bp_name']['value'] = array(
	    '#type' => 'select',
	    '#default_value' => !empty($CustomerContact->gpp9) ? $CustomerContact->gpp9 : FALSE,        
	    '#options' => $bpoptions,
	    '#empty_option' => t('None'),
	    '#title' => t('Business Partner'),
	    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
      );	
	}else{
	  $form['general']['table']['bp_name']['value'] = array(
	    '#type' => 'select',	    
	    '#options' => array(t('None')),	    	    
	    '#title' => t('Business Partner'),
	    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
      );	
	}
  	
  }else{
  	if(isset($user->data['sipwise']['bp_name']) && !empty($user->data['sipwise']['bp_name'])){
  	  $bp_name = $user->data['sipwise']['bp_name'];
    }
    $form['bp_name'] = array(
      '#type' => 'value',
      '#value' => !is_null($bp_name) ? $bp_name : '',
    );    
  }

  if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing) {
    $form['general']['emergency'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#title' => t('Customer Location'),
      '#access' => user_access('hpbx edit customer'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#collapsed' => empty($Customer->id) ? FALSE : TRUE,
    );

    $emergency_indexes = variable_get('hpbx_emergency_indexes_'. $ActiveReseller->id, array());

    unset($emergency_indexes['']);

    drupal_add_js(array('hpbx_emergency_indexes' => $emergency_indexes), 'setting');
    if (arg(1)== 'affiliate') {
      drupal_add_js(array('reseller_id' => $reseller_id), 'setting');
    }

    $options = array();
    foreach ($emergency_indexes as $region => $townships) {
      foreach ($townships as $township => $cities) {
        foreach ($cities as $city => $z_gms) {
          foreach ($z_gms as $z_gm => $location_id) {
            $options['cities'][] = $city;
            $options['townships'][] = $township;
            $options['z_gms'][] = $z_gm;
            $options['regions'][] = $region;
          }
        }
      }
    }

    natcasesort($options['cities']);
    natcasesort($options['townships']);
    natcasesort($options['z_gms']);
    natcasesort($options['regions']);

    // Extension Length.
    $form['general']['emergency']['table']['region']['value'] = array(
      '#title' => t('Region'),
      '#type' => 'select',
      '#options' => drupal_map_assoc($options['regions']),
      '#empty_option' => t('Choose an option'),
      '#required' => TRUE,
      '#default_value' => !empty($Customer->_settings->region) ? $Customer->_settings->region: NULL,
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element'),

      ),
    );

    // Extension Length.
    $form['general']['emergency']['table']['township']['value'] = array(
      '#title' => t('Township'),
      '#type' => 'select',
      '#options' => drupal_map_assoc($options['townships']),
      '#empty_option' => t('Choose an option'),
      '#required' => TRUE,
      '#default_value' => !empty($Customer->_settings->township) ? $Customer->_settings->township: NULL,
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element'),

      ),
    );
    // Extension Length.
    $form['general']['emergency']['table']['city']['value'] = array(
      '#title' => t('City'),
      '#type' => 'select',
      '#options' => drupal_map_assoc($options['cities']),
      '#empty_option' => t('Choose an option'),
      '#required' => TRUE,
      '#default_value' => !empty($Customer->_settings->city) ? $Customer->_settings->city: NULL,
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element'),
      ),
    );
    // Extension Length.
    $form['general']['emergency']['table']['z_gm']['value'] = array(
      '#title' => t('Z gm.'),
      '#type' => 'select',
      '#options' => drupal_map_assoc($options['z_gms']),
      '#empty_option' => t('Choose an option'),
      '#required' => TRUE,
      '#default_value' => !empty($Customer->_settings->z_gm) ? $Customer->_settings->z_gm: NULL,
      '#attributes' => array(
        'class' => array('chzn-select', 'sg-element'),
      ),
    );
  }

  if (!empty($ActiveReseller->_settings->bundle_settings)) {

    $bundle_settings = $Customer->_settings->bundle_settings;

    // Bundle settings.
    $form['general']['bundle_settings'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#theme' => 'hpbx_table',
      '#title' => t('Bundle settings'),
      '#access' => user_access('hpbx edit customer'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#attributes' => array('class' => array('hpbx-bundle-settings')),
      '#collapsed' => FALSE,
      '#header' => array(
        t('Profile set'),
        t('Profile'),
        '',
        t('Amount')
      ),
    );

    foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {

      $form['general']['bundle_settings']['profile_set_' . $SubscriberProfileSet->id]['label']['value'] = array(
        '#type' => 'item',
        '#markup' => $SubscriberProfileSet->name,
        '#prefix' => '<span id = profile_set_' . $SubscriberProfileSet->id .' profileSet_id ='.$SubscriberProfileSet->id.' >',
      );

      // Profile Set for this customer.
      $form['general']['bundle_settings']['profile_set_' . $SubscriberProfileSet->id]['setting']['value'] = array(
        '#type' => 'select',
        '#default_value' => isset($bundle_settings->{$SubscriberProfileSet->id}->value) ? $bundle_settings->{$SubscriberProfileSet->id}->value : 'unlimited',
        '#required' => TRUE,
        '#options' => array(
          'unlimited' => t('Unlimited'),
          'limit_total' => t('Limit total'),
          'limit_per_profile' => t('Limit per profile')
        ),
        '#field_size_class' => array('col-xs-12', 'col-sm-12', 'col-md-12'),
        '#attributes' => array(
          'class' => array('chzn-select', 'sg-element', 'no-search'),
          'data-custom-change-event' => 'edit-bundle-settings-profileset-change',
          'data-profileset-id' => $SubscriberProfileSet->id,
        ),
      );

      $form['general']['bundle_settings']['profile_set_' . $SubscriberProfileSet->id]['empty']['value'] = array(
        '#type' => 'item',
        '#markup' => '',
      );

      $form['general']['bundle_settings']['profile_set_' . $SubscriberProfileSet->id]['amount']['value'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($bundle_settings->{$SubscriberProfileSet->id}->total_amount) ? $bundle_settings->{$SubscriberProfileSet->id}->total_amount : '',
        '#size' => 4,
        '#suffix' =>'</span>',
        '#attributes' => array(
          'data-profileset-id' => $SubscriberProfileSet->id,
          'class' => array('hpbx-bundle-settings-profileset-total-amount')
        ),
      );

      foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {
        if ($SubscriberProfile->id == $ActiveReseller->_settings->pilot_subscriber_profile_id) {
          continue;
        }

        $form['general']['bundle_settings']['profile_' . $SubscriberProfile->id] = array(
          '#attributes' => array('data-profileset-id' => $SubscriberProfileSet->id),
        );

        $form['general']['bundle_settings']['profile_' . $SubscriberProfile->id]['empty']['value'] = array(
          '#type' => 'item',
          '#markup' => '',
        );
		//Bug #9564 - Create/update Customer - Bundle settings: English subscriber profile names not translated
        $form['general']['bundle_settings']['profile_' . $SubscriberProfile->id]['label']['value'] = array(
          '#type' => 'item',
          '#markup' => t($SubscriberProfile->description),
          '#field_size_class' => array('col-xs-12', 'col-sm-12', 'col-md-12'),
        );

        // Profile Set for this customer.
        $form['general']['bundle_settings']['profile_' . $SubscriberProfile->id]['setting']['value'] = array(
          '#type' => 'select',
          '#default_value' => isset($bundle_settings->{$SubscriberProfileSet->id}->profiles->{$SubscriberProfile->id}->value) ? $bundle_settings->{$SubscriberProfileSet->id}->profiles->{$SubscriberProfile->id}->value : NULL,
          //'#required' => TRUE,
          '#options' => array(
            'unlimited' => t('Unlimited'),
            'none' => t('None'),
            'input' => t('Input')
          ),
          '#field_size_class' => array('col-xs-12', 'col-sm-12', 'col-md-12'),
          '#attributes' => array(
            'class' => array('chzn-select', 'sg-element', 'no-search'),
            'data-custom-change-event' => 'edit-bundle-settings-profile-change',
            'data-profile-id' => $SubscriberProfile->id,
          ),
        );

        $form['general']['bundle_settings']['profile_' . $SubscriberProfile->id]['profile_amount']['value'] = array(
          '#type' => 'textfield',
          '#default_value' => isset($bundle_settings->{$SubscriberProfileSet->id}->profiles->{$SubscriberProfile->id}->amount) ? $bundle_settings->{$SubscriberProfileSet->id}->profiles->{$SubscriberProfile->id}->amount : '',
          '#title' => '',
          '#size' => 4,
          '#attributes' => array(
            'class' => array('hpbx-bundle-settings-profile-total-amount'),
            'data-profile-id' => $SubscriberProfile->id
          ),
        );
      }
    }
  }

  // Numberranges.
  $Customer->_settings->table = 'numberranges';

  $form['general']['numberranges'] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#title' => t('Number ranges'),
    '#access' => user_access('hpbx edit customer'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#collapsed' => FALSE,
  );

  // Add a speeddials slots/destination table.
  hpbx_add_more_table($form['general'], $form_state, $Customer->_settings);

  if (isset($Customer->id)) {

    // Make sure numberranges that have subscribers attached can't be updated or remove.
    foreach ($Customer->get_subscribers(FALSE, TRUE, TRUE, TRUE) as $Subscriber) {

      $nr = hpbx_get_numberrange($Customer, $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN));

      foreach (element_children($form['general']['numberranges']['table']) as $element) {

        $el = &$form['general']['numberranges']['table'][$element];

        if (!empty($el['area']['data']['#default_value']) && $el['area']['data']['#default_value'] == $nr->area && !empty($el['start']['data']['#default_value']) && $el['start']['data']['#default_value'] == $nr->start) {
          // Can't disable the fields, set to disabled.
          $el['area']['data']['#attributes']['readonly'] = 'readonly';
          $el['start']['data']['#attributes']['readonly'] = 'readonly';
          $el['length']['data']['#attributes']['onfocus'] = 'this.defaultIndex=this.selectedIndex;';
          $el['length']['data']['#attributes']['onchange'] = 'this.selectedIndex=this.defaultIndex;';

          //$el['prefix']['data']['#attributes']['readonly'] = 'readonly';

          // Remove the (remove) button.
          unset($el['op']['data']);
        }
      }
    }
  }

  if (!$customer_id) {

    // When adding a new customer. We will create an (pilot) subscriber as well.
    $form['general']['subscriber'] = array(
      '#type' => 'container',
      '#title' => t('Customer administrator'),
      '#tree' => TRUE,
      '#access' => user_access('hpbx edit customer'),
      '#theme_wrappers' => array('hpbx_foldable_list_item'),
      '#collapsed' => FALSE,
    );

    $form['general']['subscriber']['table'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
      '#theme' => 'hpbx_table',
      '#title' => t('Administrative user'),
    );

    // Product.
    $form['general']['subscriber']['table']['firstname']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#maxlength' => 40,
      '#title' => t('First name'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('First name')),
    );

    $form['general']['subscriber']['table']['lastname']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#maxlength' => 40,
      '#title' => t('Last name'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Last name')),
    );

    $form['general']['subscriber']['table']['email']['value'] = array(
      '#type' => 'textfield',
      '#required' => TRUE,
      '#title' => t('Email'),
      '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Email')),
    );
  }

  $t1 = t('Prefix of a number range has been changed');
  $t2 = t('Changing the prefix of a number range, results in an update of the extensions of all of the existing subscribers, hunt groups, auto attendants and conference rooms. In order to update the numbers on the devices of the subscribers, the devices need to be rebooted.');
  $t3 = t('OK');
  $t4 = t('Cancel');

  $markup = <<<EOF
  <a href="#" class="hpbx-customer-prefix-change-warning-trigger" style="display:none;">open</a>
  <div id="hpbx-customer-prefix-change-warning-dialog" class="hpbx-dialog hpbx-customer-prefix-change-warning-dialog hpbx-popup mfp-hide">
      <div class="hpbx-dialog-header">
          <div class="hpbx-dialog-header-title">{$t1}</div>
      </div>
      <div class="hpbx-dialog-contents hpbx-scrollable-container">
          <p>{$t2}</p>
          <div class="clearfix"></div>
      </div>
      <div class="hpbx-dialog-footer">
          <div class="hpbx-dialog-footer-left">
              <input type="button" class="sg-btn sg-btn-small sg-without-icon sg-btn-primary" value="{$t3}">
          </div>
          <div class="hpbx-dialog-footer-right">
              <a class="hpbx-tab-form-link-button hpbx-dialog-close-handle" href="#">{$t4}</a>
          </div>
      </div>
  </div>
EOF;
  $form['prefix_warning_popup'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );

  $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );

  if (user_access('hpbx view reseller')) {
    $url = 'hpbx/affiliate/'. $reseller_id .'/customers';
  }
  else {
    $url = 'hpbx/customers';
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

function hpbx_customer_edit_form_after_build(&$form, &$form_state){
  if(isset($form_state['values']['general']['table']['multi_site']['value']) && !empty($form_state['values']['general']['table']['multi_site']['value'])){
	$form['general']['emergency']['table']['city']['value']['#required'] = NULL;
    $form['general']['emergency']['table']['township']['value']['#required'] = NULL;
    $form['general']['emergency']['table']['z_gm']['value']['#required'] = NULL;
    $form['general']['emergency']['table']['region']['value']['#required'] = NULL;    
  }else{
	$form['general']['emergency']['table']['city']['value']['#required'] = True;
    $form['general']['emergency']['table']['township']['value']['#required'] = True;
    $form['general']['emergency']['table']['z_gm']['value']['#required'] = True;
    $form['general']['emergency']['table']['region']['value']['#required'] = True;    
  }
  
  return $form;
}

function hpbx_customer_parse_bundle_settings($bundle_settings) {
  global $ActiveReseller;

  $settings['bundle_settings'] = array();
  foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
    $settings['bundle_settings'][$SubscriberProfileSet->id]['value'] = $bundle_settings['profile_set_' . $SubscriberProfileSet->id]['setting']['value'];

    switch ($settings['bundle_settings'][$SubscriberProfileSet->id]['value']) {

      case 'limit_total':
        $settings['bundle_settings'][$SubscriberProfileSet->id]['total_amount'] = (int) $bundle_settings['profile_set_' . $SubscriberProfileSet->id]['amount']['value'];
        break;
      case 'limit_per_profile':

        $settings['bundle_settings'][$SubscriberProfileSet->id]['profiles'] = array();
        foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {
          if ($SubscriberProfile->id == $ActiveReseller->_settings->pilot_subscriber_profile_id) continue;

          $settings['bundle_settings'][$SubscriberProfileSet->id]['profiles'][$SubscriberProfile->id]['value'] = $bundle_settings['profile_' . $SubscriberProfile->id]['setting']['value'];

          if ($settings['bundle_settings'][$SubscriberProfileSet->id]['profiles'][$SubscriberProfile->id]['value'] == 'input') {
            $settings['bundle_settings'][$SubscriberProfileSet->id]['profiles'][$SubscriberProfile->id]['amount'] = (int)  $bundle_settings['profile_' . $SubscriberProfile->id]['profile_amount']['value'];
          }

        }
        break;
    }
  }

  return $settings['bundle_settings'];
}

/**
 * @param string $primary_number
 * @param int $customer_id
 *
 * @return int
 */
function hpbx_customer_primary_number_in_use($primary_number, $customer_id) {
  global $ActiveReseller;

  try {

    // Search for a number.
    $query = db_select('hpbx_customers', 'c')
      ->fields('c', array('customer_id'))
      ->condition('c.primary_number', $primary_number)
      ->condition('reseller_id', $ActiveReseller->id);

    if (!is_null($customer_id) && !empty($customer_id)) {
      // Exclude own customer_id
      $query->condition('customer_id', $customer_id, '<>');
    }
    $result = $query->execute();

    return $result->fetchField();
  }
  catch (Exception $e) {
    watchdog('hpbx_customer_range', $e->getMessage());
    return FALSE;
  }
}

/**
 * @param string $company_name
 * @param int $customer_id
 *
 * @return bool
 */
function hpbx_customer_company_name_in_use($company_name, $customer_id) {
  global $ActiveReseller;

  try {

    // Search for a number.
    $query = db_select('hpbx_customers', 'c')
      ->fields('c', array('customer_id'))
      ->condition('c.company', trim($company_name))
      ->condition('reseller_id', $ActiveReseller->id);

    if (!is_null($customer_id) && !empty($customer_id)) {
      $query->condition('customer_id', $customer_id, '<>');
    }

    $result = $query->execute();

    return $result->fetchField();
  }
  catch (Exception $e) {
    watchdog('hpbx_customer_range', $e->getMessage());
    return FALSE;
  }
}

/**
 * @param string $from
 * @param string $to
 * @param integer $customer_id
 *
 * @return bool
 */
 /*
function hpbx_customer_range_in_use($from, $to, $customer_id = NULL) {
  global $ActiveReseller;

  try {

    // Search for a number.
    $query = db_select('hpbx_numbers', 'n')
      ->fields('n', array('customer_id'))
      ->condition('n.number', $ActiveReseller->number_get_e164($from), '>=')
      ->condition('n.number', $ActiveReseller->number_get_e164($to), '<=')
      ->condition('reseller_id', $ActiveReseller->id);

    if (!is_null($customer_id) && !empty($customer_id)) {
      $query->condition('customer_id', $customer_id, '<>');
    }


    $result = $query->execute();

    return $result->fetchField();
  }
  catch (Exception $e) {
    watchdog('hpbx_customer_range', $e->getMessage());
    return FALSE;
  }
}
*/

/**
* @param string $from
* @param string $to
* @param integer $customer_id
*
* @return bool
*/
function hpbx_customer_range_in_use($from, $to, $customer_id = NULL) {
 global $ActiveReseller;
 
 try {
 
   // Search for a number in hpbx_numbers table.
   $query = db_select('hpbx_numbers', 'n')
     ->fields('n', array('customer_id'))
     ->condition('n.number', $ActiveReseller->number_get_e164($from), '>=')
     ->condition('n.number', $ActiveReseller->number_get_e164($to), '<=')
     ->condition('reseller_id', $ActiveReseller->id);
 
   if (!is_null($customer_id) && !empty($customer_id)) {
     $query->condition('customer_id', $customer_id, '<>');
   }
 
 
   $result = $query->execute();
 
   if ($customer_id = $result->fetchField()) {
     return $customer_id;
   }
 
    // Search for a number in hpbx_customers table.
   $query = db_select('hpbx_customers', 'c')
     ->fields('n', array('customer_id'))
     ->condition('c.primary_number', $ActiveReseller->number_get_e164($from), '>=')
     ->condition('c.primary_number', $ActiveReseller->number_get_e164($to), '<=')
     ->condition('reseller_id', $ActiveReseller->id);
 
   if (!is_null($customer_id) && !empty($customer_id)) {
     $query->condition('customer_id', $customer_id, '<>');
   }
 
   $result = $query->execute();
 
   if ($customer_id = $result->fetchField()) {
     return $customer_id;
   }
 
   return FALSE;
 }
 catch (Exception $e) {
   watchdog('hpbx_customer_range', $e->getMessage());
   return FALSE;
 }
}
//Added function for Bug# 9719 - PE DEV: Create customer - Filtering in dropdowns for customer location is not working after receiving an error in the customer creation
function hpbx_form_hpbx_customer_edit_form_alter(&$form, &$form_state, $form_id){
  global $ActiveReseller;
  $emergency_indexes = variable_get('hpbx_emergency_indexes_'. $ActiveReseller->id, array());
  unset($emergency_indexes['']);
	
  $form['general']['#attached']['js'][] = array(
    'data' => array('hpbx_emergency_indexes' => $emergency_indexes),
     'type' => 'setting'
  );
  if (arg(1)== 'affiliate') {
	$form['general']['#attached']['js'][] = array(
     'data' => array('reseller_id' => $ActiveReseller->id),
     'type' => 'setting'
    );
  }; 
  
  if($form_id == 'hpbx_customer_edit_form'){
  	 $form['#after_build'][] = 'hpbx_customer_edit_form_after_build'; 
     array_unshift($form['#validate'],'hpbx_customer_edit_form_validate_multisite');	
  }   
}

function hpbx_customer_edit_form_validate_multisite(&$form, &$form_state){
	
  if(!empty($form_state['values']['general']['table']['multi_site']['value'])){
    $dupe_site = array();
    $multifields = array('id' => 'id','site_name' => 'Site Name', 'region' => 'Region', 'township' => 'Township', 'city' => 'City', 'z_gm' => 'Z gm.'  );
    if(count($form_state['input']['general']['multisite']['table'])==1){
      foreach($form_state['input']['general']['multisite']['table'] as $id => $values){
        foreach($values as $key => $value){
          if(empty($value['data'])){
  	        form_set_error('general][multisite][table]['.$id.']['.$key, t('@key field is required', array('@key' => $multifields[$key])));
          }
        }
      }
    }else{
      foreach($form_state['input']['general']['multisite']['table'] as $id => $values){        
        if(isset($values['site_name']['data']) && !empty($values['site_name']['data'])){
          if (++$dupe_site[$values['site_name']['data']] > 1) {
            form_set_error('general][multisite][table][',t('The Site Name should be unique'));
          }
          $site_name[] = trim($values['site_name']['data']);
        }

        if(!empty($values['site_name']['data']) || !empty($values['ips']['data']) || !empty($values['region']['data'])|| !empty($values['township']['data']) || !empty($values['city']['data']) || !empty($values['z_gm']['data'])){
          foreach($values as $key => $value){
          	if($key != 'ips'){
			  if(empty($value['data'])){
		        form_set_error('general][multisite][table]['.$id.']['.$key, t('@key field is required', array('@key' => $multifields[$key])));
	          }
			}	        
          }
        }	
      }
    }
    
    foreach($form_state['input']['general']['multisite']['table'] as $id => $values){
      if(!empty($values['site_name']['data'])){
      	if(preg_match("/[\"\'\&]/",$values['site_name']['data']))	{
		  form_set_error('general][multisite][table]['.$id.'][site_name', t('Site Names containing \'," or & are not allowed.'));	
		}
	  }
	}
    foreach($form_state['input']['general']['multisite']['table'] as $id => $values){
      if(!empty($values['ips']['data'])){      	
      	$subnets = explode(PHP_EOL, $values['ips']['data']);
        if (count($subnets)) {
          foreach ($subnets as $key => $subnet) {

            $subnet2 = trim($subnet);

            if (empty($subnets[$key])) {
              continue;
            }

            if (strpos($subnet2, '/') !== FALSE) {
              list ($subnet2, $bits) = explode('/', $subnet2);
            }
            else {
              $bits = 32;
            }

            if (!ip2long($subnet2)) {            	 
              form_set_error('general][multisite][table]['.$id.'][ips', t('The passed IP address @IP is invalid', array('@IP' => $subnet)));
            }

            if ($bits!=32) {
            // if ($bits < 1 || $bits > 32) {
            // @todo allow other subnets then /32 when Sipwise has fixed their side.
              form_set_error('general][multisite][table]['.$id.'][ips', t('The passed IP address @IP is invalid', array('@IP' => $subnet)));
            }
          } 
        // Reset array keys.          
        }	  	
	  }	  
	}
  }
	   
  if (count(form_get_errors())) {
        //Add all rows to the storage - $_SESSION['ccs']['hpbx']['add_more_sites']['values'][$SipwiseEntity->table]['rows']
    $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows'] = $form_state['input']['general']['multisite']['table'];       
        

    $add = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add'];
    if (!empty($add['site_name']['data']) || !empty($add['ips']['data']) || !empty($add['region']['data'])|| !empty($add['township']['data']) || !empty($add['city']['data']) || !empty($add['z_gm']['data']) ) {
      $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows'][uniqid()] = $add;
     
     //Modified for Bug #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero	
	  foreach($_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows'] as $id => $data){	 
	    if($id == "add"){
	      $form['general']['multisite']['table']['add']['site_name']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['site_name']['data']; 
	      $form['general']['multisite']['table']['add']['ips']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['ips']['data'];
		  $form['general']['multisite']['table']['add']['region']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['region']['data'];
		  $form['general']['multisite']['table']['add']['township']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['township']['data'];
		  $form['general']['multisite']['table']['add']['city']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['city']['data'];
		  $form['general']['multisite']['table']['add']['z_gm']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more_sites']['values']['multisite']['rows']['add']['z_gm']['data'];
	    }
	  }	
    }      
  }  
}

/**
 * Form validate handler for hpbx_customer_edit_form().
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_customer_edit_form_validate(&$form, &$form_state) {

  // Add table styling.
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_customers.js', array('scope' => 'footer'));

  $numberranges = array();
  if (!empty($form_state['values']['reseller_id'])) {

    // Temporary set the reseller_id.
    sipwise_api_activate_reseller($form_state['values']['reseller_id']);
  }
  global $ActiveReseller;

  // Bundle settings.
  if ($form_state['values']['id'] && !empty($ActiveReseller->_settings->bundle_settings) && isset($form_state['values']['general']['bundle_settings'])) { 

    $Customer = SipwiseEntity::load('customers', $form_state['values']['id']);

    $bundle_settings = hpbx_customer_parse_bundle_settings($form_state['values']['general']['bundle_settings']);

    $bundle_settings = json_decode(json_encode($bundle_settings));

    // Now we have to verify all individual profiles (except pilot) against the
    // bundle_settings limits.
    foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
      foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {

        // Ignore the pilot subscriber.
        if ($SubscriberProfile->id == $ActiveReseller->_settings->pilot_subscriber_profile_id) {
          continue;
        }

        // Verify if the setting is allowed.
        $bundle_result = hpbx_bundle_settings_is_allowed($Customer, $SubscriberProfile->id, HPBX_BUNDLE_SETTINGS_OFFSET_COUNTER_INCREASE_NO, $bundle_settings);

        if (!$bundle_result) {

          // Based on profile.
          form_set_error('general][bundle_settings][profile_' . $SubscriberProfile->id . '][profile_amount][value', t('Not allowed to set this limit for the @profile_name profile, as the customer currently exceeds this limit', array('@profile_name' => drupal_strtolower($SubscriberProfile->name))));
        }
        elseif ($bundle_result < 0) {

          // Based on profile_set_id.
          form_set_error('general][bundle_settings][profile_set_' . $SubscriberProfile->profile_set_id . '][amount][value', t('Not allowed to set this limit for the @profile_set_name profile set, as the customer currently exceeds this limit', array('@profile_set_name' => drupal_strtolower($SubscriberProfileSet->name))));
        }
      }
    }
  }
  // Verify if the customer external id already exists - Fix for issue #9570
  $externalid = sipwise_api_get_all('customers',array('reseller_id' => $form_state['values']['reseller_id'],'external_id' => $form_state['values']['general']['table']['external_id']['value'] ));

  if (!preg_match('/^[0-9]{' . $ActiveReseller->_settings->customer_id_length . '}$/', $form_state['values']['general']['table']['external_id']['value'])) {
        form_set_error('general][table][external_id][value', t('Invalid Customer ID. Please use a @length digit number', array('@length' => $ActiveReseller->_settings->customer_id_length)));
  }
  if($form_state['values']['id'] != '') {
      $Customer = SipwiseEntity::load('customers', $form_state['values']['id']);
        if ((!empty($externalid)) && ($Customer->external_id != $form_state['values']['general']['table']['external_id']['value'])) {
          form_set_error('general][table][external_id][value', t('The Customer id @customer_id already exists', array('@customer_id' => $form_state['values']['general']['table']['external_id']['value'])));
      }
  }elseif(!empty($externalid)){
      form_set_error('general][table][external_id][value', t('The Customer id @customer_id already exists', array('@customer_id' => $form_state['values']['general']['table']['external_id']['value'])));
  }

  if (hpbx_customer_company_name_in_use($form_state['values']['general']['table']['company']['value'], $form_state['values']['id'])) {
    form_set_error('general][table][company][value', t('Company name is already assigned to customer @company', array(
      '@company' => $form_state['values']['general']['table']['company']['value']
    )));
  }


  // Verify if we need to generate the pilot number (<CountryCode><PilotNumberPrefix><CustomerID>)
  if (isset($ActiveReseller->_settings->automatic_pilot_number_generation) && $ActiveReseller->_settings->automatic_pilot_number_generation) {
    $form_state['values']['general']['table']['pilot_number']['value'] =
      $ActiveReseller->_settings->automatic_pilot_number_generation_prefix .
      trim($form_state['values']['general']['table']['external_id']['value']);
  }

  $extension_length = $form_state['values']['general']['table']['extension_length']['value'];

  // Prefix length must be the same for every number block.
  $lengths = array();

  if (!preg_match('/^\+?[0-9]+$/', $form_state['values']['general']['table']['pilot_number']['value']))  {
    form_set_error('general][table][pilot_number][value', t('Invalid pilot number'));
  }

  // Asign pilotnumber and removal national digit from it.
  $pilot_number = $form_state['values']['general']['table']['pilot_number']['value'];
  $ActiveReseller->remove_national_digits($pilot_number);

  $extensions_start = array();
  $new_ranges = array();
  $new_numberranges = array();

  $new_numberranges[] = array(
    'from' => $pilot_number,
    'to' => $pilot_number,
  );

  // Add pilot number to the ranges, so we can verify if it is overlapping with the ranges.
  $new_ranges[] = $form_state['values']['general']['table']['pilot_number']['value'];

  $possible_extensions = array();
  foreach ($form_state['input']['general']['numberranges']['table'] as $id => $values) {

    // Store the range, to verify if we have duplicates.
    $new_ranges[] = $values['area']['data'] . $values['start']['data'];

    // Store the  range to verify if have duplicates.
    $new_numberranges[] = array(
      'from' => $values['area']['data'] . $values['start']['data'],
      'to' => $values['area']['data'] . ($values['start']['data'] + $values['length']['data'] - 1),
    );

    // Ignore empty ranges.
    if (empty($values['area']['data']) && empty($values['start']['data'])) {
      continue;
    }

    // Before validation the area code, extract the national digit.
    $ActiveReseller->remove_national_digits($values['area']['data']);
	//Modified for Bug #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero
    if (empty($values['area']['data']) ) {
      form_set_error("general][numberranges][table][$id][area", t('Empty area code'));
    }else if ((strlen($values['area']['data']) < $ActiveReseller->_settings->area_code_min_digits - strlen($ActiveReseller->_settings->digit_for_national_calls)) ||
      (strlen($values['area']['data']) > $ActiveReseller->_settings->area_code_max_digits - strlen($ActiveReseller->_settings->digit_for_national_calls)) || !preg_match('/^[0-9]+$/', $values['area']['data'])) {
      form_set_error('general][numberranges][table]['.$id.'][area', t('Invalid area code size'));	   
    }
    //Modified for Bug #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero
    if (empty($values['start']['data'])) {
      form_set_error("general][numberranges][table][$id][start", t('Empty start number'));
    }else if ((strlen($values['start']['data']) < $ActiveReseller->_settings->subscriber_number_min_digits) ||
      (strlen($values['start']['data']) > $ActiveReseller->_settings->subscriber_number_max_digits) || !preg_match('/^[0-9]+$/', $values['start']['data'])) {	  
      form_set_error('general][numberranges][table]['.$id.'][start', t('Invalid start number size'));	  
    }

   
    //Added #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero
    if (!empty($values['prefix']['data']) && !preg_match('/^[0-9]+$/', $values['prefix']['data'])) {
      form_set_error('general][numberranges][table]['.$id.'][prefix', t('Invalid prefix'));
    }
    
    if (empty($values['length']['data'])) {
      form_set_error('general][numberranges][table]['.$id.'][length', t('Please select a number range length'));
    }
    $lengths[strlen($values['prefix']['data'])] = strlen($values['prefix']['data']);

    // Validate number range prefix.
    if ($values['prefix']['data'] != '') {

      if (strlen($ActiveReseller->_settings->digit_for_national_calls) && preg_match('/^'. $ActiveReseller->_settings->digit_for_national_calls .'/', $values['prefix']['data'])) {
        form_set_error('numberranges][table', t('Prefix cannot start with @na', array('@na' => $ActiveReseller->_settings->digit_for_national_calls)));
      }

      // Not allowed to start with.
      foreach ($ActiveReseller->_settings->extensions_are_not_allowed_to_start_with as $na) {

        if (preg_match('/^'. $na .'/', $values['prefix']['data'])) {
          form_set_error('numberranges][table', t('Prefix cannot start with @na', array('@na' => $na)));
        }
      }

      // Find all possible extensions based on new ranges + prefixes.
      for ($x = (int)($values['area']['data'] . $values['start']['data']);$x<= (int)($values['area']['data'] . ($values['start']['data'] + $values['length']['data'] - 1));$x++) {
        $extension_suffix_length = $extension_length - strlen($values['prefix']['data']);
        $possible_extension = $values['prefix']['data']. substr($x, $extension_suffix_length * - 1);
        $possible_extensions[] = $possible_extension;
      }
    }

    // Verify if the result of the extension length + number range prefix results in a non allowed.
    if ($form_state['values']['general']['table']['derive_extension_from_did']['value']) {

      $extension_start = hpbx_get_number_range_extension_start($extension_length, $values['prefix']['data'],
        $values['start']['data'], $values['length']['data']);

      // Verify if there is potential overlap between extensons when adding multiple numberranges.
      $overlap_id = array_search($extension_start, $extensions_start);
      if ($overlap_id === FALSE) {
        $extensions_start[$id] = $extension_start;
      }
      else {
        form_set_error('', t('Potential overlap between extensions'));
        form_set_error('numberranges][table]['. $id . '][start');
        form_set_error('numberranges][table]['. $id . '][prefix');
        form_set_error('numberranges][table]['. $overlap_id . '][start');
        form_set_error('numberranges][table]['. $overlap_id . '][prefix');
      }

      // Not allowed to start with.
      foreach ($ActiveReseller->_settings->extensions_are_not_allowed_to_start_with as $na) {

        if (preg_match('/^'. substr($na, 0, strlen($extension_start)) .'/', $extension_start)) {
          form_set_error('numberranges][table', t('The combination of extension length and the choosen number range results in extensions starting with @na, which is not allowed.', array('@na' => $na)));
        }
      }

      // Exclude national call digit.
      if (strlen($ActiveReseller->_settings->digit_for_national_calls) && substr($extension_start, 0, 1) == $ActiveReseller->_settings->digit_for_national_calls) {
        form_set_error('numberranges][table', t('The combination of extension length and the choosen number range results in extensions starting with the digit for national calls which is not allowed.'));
      }

      // Exclude extensions.
      foreach ($ActiveReseller->_settings->exclude_extensions as $na) {

        if ($extension_length == strlen($na) && preg_match('/^'. substr($na, 0, strlen($extension_start)) .'/', $extension_start)) {
          form_set_error('numberranges][table', t('The combination of extension length and the choosen numberrange could result in an extension number equal to @na, which is not allowed.', array('@na' => $na)));
        }
      }
    }

    // Verify the added range is unique.
    $from = $values['area']['data'] . $values['start']['data'];
    $to = $values['area']['data'] . ($values['start']['data'] + $values['length']['data'] - 1);

    // Remove the leading national digit, if present.
    $ActiveReseller->remove_national_digits($from);

    // Remove the leading national digit, if present.
    $ActiveReseller->remove_national_digits($to);

    if ($in_use_customer_id = hpbx_customer_range_in_use($from, $to, $form_state['values']['id'])) {

      $Customer3 = SipwiseEntity::load('customers', $in_use_customer_id);
      $CustomerContact3 = SipwiseEntity::load('customercontacts', $Customer3->contact_id);

      form_set_error('numberranges][table', t('The number range @range has overlap with a number range assigned to customer @customer',
        array(
          '@range' => $values['area']['data'] . $values['start']['data'],
          '@customer' => $CustomerContact3->company
        )
      ));
    }

    if ($range_in_use = hpbx_customer_range_in_use($pilot_number, $pilot_number, $form_state['values']['id'])) {
      $Customer3 = SipwiseEntity::load('customers', $range_in_use['customer_id']);
      $CustomerContact3 = SipwiseEntity::load('customercontacts', $Customer3->contact_id);

      form_set_error('general][table][pilot_number][value', t('The pilot number @pilot_number is already assigned to customer @customer',
        array(
          '@pilot_number' => $form_state['values']['general']['table']['pilot_number']['value'],
          '@customer' => $CustomerContact3->company
        )
      ));
    }
  }

  if (count(array_unique($possible_extensions))<count($possible_extensions)) {
    form_set_error('numberranges][table', t('Possible overlap in extensions. Please review the numberrange prefixes.'));
  }

  if (count(array_diff_key( $new_ranges , array_unique( $new_ranges ) ))) {
    form_set_error('numberranges][table', t('Please enter unique number ranges and/or pilot number.'));
  }

  // No prefix length at all so no numberrange entered.
  if (count($lengths) == 0) {
    form_set_error('numberranges][table', t('At least one number range must be defined. Enter the number range details and hit the "Add" button to add a new number range.'));
  }

  // More then 1 prefix lengths; Must be the same.
  if (count($lengths)> 1) {
    form_set_error('numberranges][table', t('If a prefix is set for a number range, all numberranges must have a prefix'));
  }

  // Prefix length can't increase extensionlength -1
  $prefix_length = current($lengths);
  
  //Modified for Bug #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero
  $lengths_add[strlen($form_state['input']['general']['numberranges']['table']['add']['prefix']['data'])] = strlen($form_state['input']['general']['numberranges']['table']['add']['prefix']['data']);
  $prefix_len = current($lengths_add);
  if ($prefix_length >= $form_state['values']['general']['table']['extension_length']['value'] || $prefix_len >= $form_state['values']['general']['table']['extension_length']['value']) {
    form_set_error('general][table][extension_length][value', t('The prefix must have less digits then the choosen extension length'));
  }

  // Verify if the numberranges of this customer are not overlapping.
  foreach ($new_numberranges as $key => $range) {

    foreach ($new_numberranges as $key2 => $range2) {

      if ($key != $key2) {

        if (($range['from'] >= $range2['from'] &&  $range['from'] <= $range2['to'])) {
          form_set_error('numberranges][table', t('Overlapping number ranges and/or pilot number are not allowed'));
        }
        if (($range['to'] >= $range2['from'] &&  $range['to'] <= $range2['to'])) {
          form_set_error('numberranges][table', t('Overlapping number ranges and/or pilot number are not allowed'));
        }
      }
    }
  }

  // Verify if the pilotnumber is not yet in use as primary number; only in case
  // automatic pilot number generation is turned off.
  if (!isset($ActiveReseller->_settings->automatic_pilot_number_generation) || !$ActiveReseller->_settings->automatic_pilot_number_generation) {
    if ($in_use_customer_id = hpbx_customer_primary_number_in_use(
      $ActiveReseller->number_get_e164($form_state['values']['general']['table']['pilot_number']['value']),
      $form_state['values']['id'])) {

      $Customer3 = SipwiseEntity::load('customers', $in_use_customer_id);
      $CustomerContact3 = SipwiseEntity::load('customercontacts', $Customer3->contact_id);

      form_set_error('general][table][pilot_number][value', t('Pilot number is already assigned to customer @company', array('@company' => $CustomerContact3->company)));
    }
  }

  if (!empty($form_state['values']['general']['subscriber']['table']['email']['value']) &&
    user_load_by_mail(trim($form_state['values']['general']['subscriber']['table']['email']['value']))) {

    form_set_error('general][subscriber][table][email][value', t('A user with the provided email address is already present.'));
  }

  if (isset($_FILES)) {
    foreach ($_FILES as $handle => $sf) {
      if (!empty($sf['name'])) {
        if (empty($sf['tmp_name']) || !file_exists($sf['tmp_name']) ||  $sf['error']) {
          form_set_error('', t('Upload of @handle failed', array('@handle' => $handle)));
        }

        //hpbx_audio_get_valid_file_extensions()
        $ext = pathinfo($sf['name'], PATHINFO_EXTENSION);

        if (!in_array(strtolower($ext), hpbx_audio_get_valid_file_extensions())) {
          form_set_error('table]['. $handle . '][value', t('The file format @ext is not supported', array('@ext' => $ext)));
        }
      }
    }
  }

  if (count(form_get_errors())) {
    // Add all rows to the storage.
    $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows'] = $form_state['input']['general']['numberranges']['table'];

    $add = $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows']['add'];
    if (!empty($add['area']['data']) || !empty($add['start']['data'])) {
      $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows'][uniqid()] = $add;
     //unset($_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows']['add']);
     //Modified for Bug #1100 - PE LAB: Create Customer - Numberrange removed after error message about prefix starting with a zero	
	  foreach($_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows'] as $id => $data){	 
	    if($id == "add"){
	      $form['general']['numberranges']['table']['add']['start']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows']['add']['start']['data']; 
	      $form['general']['numberranges']['table']['add']['area']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows']['add']['area']['data'];
		  $form['general']['numberranges']['table']['add']['prefix']['data']['#value'] = $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows']['add']['prefix']['data'];
	    }
	  }	
    }
    //$form_state['input']['general']['numberranges']['table'] = $_SESSION['ccs']['hpbx']['add_more']['values']['numberranges']['rows'];
  }

  if (isset($ActiveReseller->_settings->enable_restricted_access) &&
    $ActiveReseller->_settings->enable_restricted_access && isset($form_state['values']['general']['table']['allowed_ips']['value']) && !empty($form_state['values']['general']['table']['allowed_ips']['value']) && empty($form_state['values']['general']['table']['multi_site']['value'])) {
	  // Validate allowed IP's.
      $subnets = explode(PHP_EOL, $form_state['values']['general']['table']['allowed_ips']['value']);

    $form_state['values']['general']['table']['allowed_ips']['value'] = array();

    if (count($subnets)) {
      foreach ($subnets as $key => $subnet) {

        $subnet2 = trim($subnet);

        if (empty($subnets[$key])) {
          continue;
        }

        if (strpos($subnet2, '/') !== FALSE) {
          list ($subnet2, $bits) = explode('/', $subnet2);
        }
        else {
          $bits = 32;
        }

        if (!ip2long($subnet2)) {
          form_set_error('table][allowed_ips][value', t('The passed IP address @IP is invalid', array('@IP' => $subnet)));
        }

        if ($bits!=32) {
          // if ($bits < 1 || $bits > 32) {
          // @todo allow other subnets then /32 when Sipwise has fixed their side.
          form_set_error('table][allowed_ips][value', t('The passed IP address @IP is invalid', array('@IP' => $subnet)));
        }

        $form_state['values']['general']['table']['allowed_ips']['value'][$subnet2 . '/' . $bits] = $subnet2 . '/' . $bits;
      }
      // Reset array keys.
      $form_state['values']['general']['table']['allowed_ips']['value'] = array_values($form_state['values']['general']['table']['allowed_ips']['value']);
    }
  }
  
  if(isset($form_state['input']['general']['multisite']['table'])){
  	$site_count = 0;
  	foreach ($form_state['input']['general']['multisite']['table'] as $key => $values){
		if(!empty($values['site_name']['data'])){
			$site_count++;
		}
	}
	if($site_count > $ActiveReseller->_settings->number_of_sites){
		form_set_error('][multisite][value', t('You have exceeded the maximum number of @site sites', array('@site' => $ActiveReseller->_settings->number_of_sites)));
	}
  }
}

/**
 * Form submit handler for hpbx_customer_edit_form().
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_customer_edit_form_submit($form, &$form_state) {

  global $ActiveReseller; 
  $flag = array();  
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.reseller.inc');
  
  if (!empty($form_state['values']['reseller_id'])) {
    // Temporary set the reseller_id.
    sipwise_api_activate_reseller($form_state['values']['reseller_id']);
  }

  try {

    $Domain = SipwiseEntity::load('domains', $form_state['values']['general']['table']['domain_id']['value']);
	
    if ($form_state['values']['id']) {
      $Customer = SipwiseEntity::load('customers', $form_state['values']['id']);
    }
    else {
      $Customer = SipwiseEntity::create('customers');
    }

    $allowed_clis = array();

    $Customer->type = 'pbxaccount';
    $Customer->status = 'active';
    $Customer->external_id = trim($form_state['values']['general']['table']['external_id']['value']);

    if ($BillingProfiles = sipwise_api_get_all('billingprofiles', array('reseller_id' => $ActiveReseller->id))) {
      $BillingProfile = current($BillingProfiles);
      $Customer->billing_profile_id = $BillingProfile->id;
    }
    else {
      die("Can't find billing profile");
    }

    // These settings are not supported by Sipwise API. They are stored in a special way, using some tricks.
    $settings = array();
    $settings['derive_extension_from_did'] = (bool) $form_state['values']['general']['table']['derive_extension_from_did']['value'];
    $settings['extension_length'] = (int) $form_state['values']['general']['table']['extension_length']['value'];
    $settings['profile_set_id'] = (int) $form_state['values']['general']['table']['profile_set_id']['value'];

    // Set Allowed IP's (already parsed in _validate()).
    if (isset($ActiveReseller->_settings->enable_restricted_access) && $ActiveReseller->_settings->enable_restricted_access &&
      count($form_state['values']['general']['table']['allowed_ips']['value']) && empty($form_state['values']['general']['table']['multi_site']['value'])) {

      $settings['allowed_ips'] = array_values($form_state['values']['general']['table']['allowed_ips']['value']);
    }
	
    if(isset($form_state['values']['general']['table']['multi_site']['value']) &&  !empty($form_state['values']['general']['table']['multi_site']['value'])){
		$settings['multi_site'] = (int)$form_state['values']['general']['table']['multi_site']['value'];
	}
	
    $settings['numberranges'] = array();

    // In case an non-suspended customer is set to suspended, the suspended date needs to be set.
    if (empty($Customer->_settings->suspend_date) && !empty($form_state['values']['general']['table']['suspended']['value'])) {
      $settings['suspend_date'] = date('Y-m-d H:i:s');
    }
    elseif (!empty($Customer->_settings->suspend_date) && empty($form_state['values']['general']['table']['suspended']['value'])) {
      //$settings['suspend_date'] =
    }
    elseif (!empty($Customer->_settings->suspend_date)) {
      $settings['suspend_date'] = $Customer->_settings->suspend_date;
    }

    $pilot_force_inbound_calls_to_peer = FALSE;
    $exclude_in_use = TRUE;
    
    if (!$form_state['values']['id']) {
      $exclude_in_use = FALSE;
    }
    
    $alias_numbers = hpbx_get_alias_numbers_from_rows(
      $form_state['input']['general']['numberranges']['table'],
      $settings['numberranges'],
      $allowed_clis, $Customer,
      $exclude_in_use,
      $pilot_force_inbound_calls_to_peer
    );

    // Emerge
    if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing && empty($form_state['values']['general']['table']['multi_site']['value'])) {
      $settings['township'] = $form_state['values']['general']['emergency']['table']['township']['value'];
      $settings['region'] = $form_state['values']['general']['emergency']['table']['region']['value'];
      $settings['city'] = $form_state['values']['general']['emergency']['table']['city']['value'];
      $settings['z_gm'] = $form_state['values']['general']['emergency']['table']['z_gm']['value'];
    }

    // Bundle settings.
    if (!empty($ActiveReseller->_settings->bundle_settings) && isset($form_state['values']['general']['bundle_settings'])) { 
      // Modified for #6769 - Bundling: Create/update Customer screen: Only show the profile set that is selected for the customer	
      foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {      	 
        foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {

        // Ignore the pilot subscriber.
          if ($SubscriberProfile->id == $ActiveReseller->_settings->pilot_subscriber_profile_id) {
               continue;
          }
          if($SubscriberProfileSet->name != 'Generic' && $form_state['values']['general']['table']['profile_set_id']['value'] != $SubscriberProfileSet->id){      	
		    unset($form_state['values']['general']['bundle_settings']['profile_set_'.$SubscriberProfileSet->id]);
		    unset($form_state['values']['general']['bundle_settings']['profile_'.$SubscriberProfile->id]);		   
	      }
		}
	  }	
      
      $settings['bundle_settings'] = hpbx_customer_parse_bundle_settings($form_state['values']['general']['bundle_settings']);
    }
    
    foreach ($form_state['input']['general']['multisite']['table'] as $id => $values) {
      foreach ($values as $key => $value){  
        if(!empty($values['site_name']['data'])){           
          if(($key == 'ips') && !empty($value['data'])){
          	          	
            $allowed_ips = explode(PHP_EOL,$value['data']);
            foreach($allowed_ips as $ips){
              $ip[] = $ips . '/' . '32';              
            }
            $multisite[$values['id']['data']]->ips = $ip;
            unset($ip);
            unset($value);          
          }     
          foreach($value as $k => $v){
            $multisite[$values['id']['data']]->$key = $v;		              
          }
	    }           
      }        
    }

    $oldsite = json_decode(json_encode($Customer->_settings->multisite),true);
    $newsite = json_decode(json_encode($multisite),true);
    foreach($oldsite as $key=>$old){
      if(isset($newsite[$key])){
        if(!empty(array_diff_assoc($newsite[$key],$old))){
          $flag[$key] = $old['site_name'];
        }
      }
    }
    foreach ($multisite as $key => $value){
		unset($multisite[$key]->id);
	}
	if(isset($form_state['values']['general']['table']['multi_site']['value']) &&  !empty($form_state['values']['general']['table']['multi_site']['value'])){
	  $settings['multisite'] = $multisite; 	
	}
       
    $Customer->_settings = (object)$settings;
    $Customer->save();

    // Load the existing (or by sipwise api created when customer was saved) customer contact.
    $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);
    $CustomerContact->reseller_id = $ActiveReseller->id;

    $company_old = $CustomerContact->company;
    $company_new = trim($form_state['values']['general']['table']['company']['value']);

    if ($form_state['values']['id'] && !is_null($CustomerContact->company) && $CustomerContact->company!=$company_new) {
      // On a customer update (thus not create) AND in case the company name was
      // already set before AND the name is different then the value submitted; Then change the OU.

      if (!hpbx_user_rename_customer_organizational_unit($Customer, $company_old, $company_new)) {
        drupal_set_message(t('Failed to change the company name'));
      }
    }

    // Save company name.
    $CustomerContact->company = $company_new;

    // Save e-mail address.
    if (!empty($form_state['values']['general']['subscriber']['table']['email']['value'])) {
      $CustomerContact->email = 'contact@domain.com';
    }
   
    if(isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
      if(isset($form_state['values']['general']['table']['bp_name']['value']) && !empty($form_state['values']['general']['table']['bp_name']['value'])){
	    if($CustomerContact->gpp9 != $form_state['values']['general']['table']['bp_name']['value']){
		  $CustomerContact->gpp9 = $form_state['values']['general']['table']['bp_name']['value'];
		  hpbx_reseller_clear_cache($ActiveReseller->id, TRUE);
        }	
      }
      elseif(isset($form_state['values']['general']['table']['bp_name']['value']) && empty($form_state['values']['general']['table']['bp_name']['value'])){
        $CustomerContact->gpp9 = '';
        hpbx_reseller_clear_cache($ActiveReseller->id, TRUE);
      }
      elseif(isset($form_state['values']['bp_name']) && !empty($form_state['values']['bp_name'])){
          $CustomerContact->gpp9 = $form_state['values']['bp_name'];
          hpbx_reseller_clear_cache($ActiveReseller->id, TRUE);	
      }        	   
    }
    	
    $CustomerContact->save();

    // Load customer preferences.
    if ($CustomerPreferences = SipwiseEntity::load('customerpreferences', $Customer->id)) {

      if (user_access('hpbx edit customer') && isset($allowed_clis)) {
        $CustomerPreferences->allowed_clis = array_values($allowed_clis);
        $CustomerPreferences->allowed_clis_reject_policy = 'override_by_clir';
      }

      if (isset($form_state['values']['general']['table']['concurrent_max']['value'])) {
        $CustomerPreferences->concurrent_max = $form_state['values']['general']['table']['concurrent_max']['value'];
      }

      // For existing customers the suspended state can be set.
      if (!empty($form_state['values']['general']['table']['suspended']['value'])) {

        // Set customer ncos level to suspend (most likely ncos 7).
        $CustomerPreferences->ncos = $ActiveReseller->_settings->suspend_ncos_level;

        if (!hpbx_users_disable_users($Customer)) {
          drupal_set_message(t('Failed to disable user accounts'), 'error');
        }
      }
      else {
        unset($CustomerPreferences->ncos);

        if (!hpbx_users_enable_users($Customer)) {
          drupal_set_message(t('Failed to enable user accounts'), 'error');
        }
      }

      $CustomerPreferences->save();
    }
    else {
      watchdog('sipwise', 'Failed to load customerpreferences for customer id '. $Customer->id . '. '. var_export($CustomerPreferences, TRUE));
    }

    // Verify if the id is set, but was not set before; so in case we are currently adding a new customer.
    if (!$form_state['values']['subscriber_id']) {

      // Create new Pilot Subscriber.
      $PilotSubscriber = SipwiseEntity::create('subscribers');
      $PilotSubscriber->username = uniqid($Customer->id);
      $PilotSubscriber->webusername = uniqid((int)$Customer->id);
      $PilotSubscriber->password = hpbx_subscriber_password($Customer->id);
      $PilotSubscriber->administrative = TRUE;
      $PilotSubscriber->customer_id = $Customer->id;
      $PilotSubscriber->status = 'active';
      $PilotSubscriber->is_pbx_pilot = TRUE;
    }
    else {

      // Load existing pilot subscriber.
      $PilotSubscriber = SipwiseEntity::load('subscribers', $form_state['values']['subscriber_id']);
    }


    // Set the domain id.
    $PilotSubscriber->domain_id = $Domain->id;
    $PilotSubscriber->profile_set_id = $ActiveReseller->_settings->generic_profile_set_id;
    $PilotSubscriber->profile_id = $ActiveReseller->_settings->pilot_subscriber_profile_id;

    if (!empty($form_state['values']['general']['table']['pilot_number']['value'])) {

      if (!isset($ActiveReseller->_settings->automatic_pilot_number_generation) || !$ActiveReseller->_settings->automatic_pilot_number_generation) {

        // Removal national digit, if present.
        $ActiveReseller->remove_national_digits($form_state['values']['general']['table']['pilot_number']['value']);
      }

      // Set the pilot_number as primary number of the pilot subscriber.
      $PilotSubscriber->primary_number = array();
      $PilotSubscriber->primary_number['cc'] = $ActiveReseller->_settings->country_code;
      $PilotSubscriber->primary_number['ac'] = $form_state['values']['general']['table']['pilot_number']['value'][0];
      $PilotSubscriber->primary_number['sn'] = substr($form_state['values']['general']['table']['pilot_number']['value'], 1);
    }

    $PilotSubscriber->alias_numbers = $alias_numbers;

    // Save the pilot subscriber.
    $PilotSubscriber->save();


    $i = str_pad(1, $ActiveReseller->_settings->customer_max_extension_length + 1, STR_PAD_RIGHT);

    // Before changing the prefix for all subscribers, we need to give all
    // subscribers an pbx_extention which can not overlap with any existing
    // of new extension.
    foreach (sipwise_api_get_all('subscribers', array('customer_id' => $Customer->id)) as $Subscriber) {
      if (!$Subscriber->is_pbx_pilot) {
        $i++;
        $Subscriber->pbx_extension = $i;
        $Subscriber->save();
      }
    }

    // Load all sibling subscribers to reset the primary_number, but also
    // change the prefix in case the prefix of the numberrange changed.
    foreach (SipwiseSubscriber::get_all(array('customer_id' => $Customer->id)) as $Subscriber) {

      if (!$Subscriber->is_pilot()) {

        // Set prefix based.
        $prefix = hpbx_get_extension_prefix($Customer, $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN));
        $extension_suffix_length = $Customer->_settings->extension_length - strlen($prefix);
        $Subscriber->pbx_extension = $prefix. substr($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_SN), $extension_suffix_length * - 1);
        $Subscriber->save();

        // Load the subscriberpreferences and update gpp1 and display_name.
        $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id);
        //$SubscriberPreferences->display_name = trim($Subscriber->pbx_extension);
        $SubscriberPreferences->gpp1 = $Subscriber->pbx_extension;

        // To support routing for non-ported numbers to peer, which is necessary
        // to ensure  that calling  non-ported numbers  from HPBX will be routed
        // correctly  (to the peer), we  will set force_inbound_calls_to_peer to
        // TRUE in case the number is not yet ported or to FALSE when the number
        // is ported. To verify if the number is ported, we will use the allowed
        // -clis variable, which is filled based on the inported checkbox at the
        // customer edit form.
        if (in_array($Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_E164), $CustomerPreferences->allowed_clis)) {

          $SubscriberPreferences->force_inbound_calls_to_peer = FALSE;
        }
        else {
          $SubscriberPreferences->force_inbound_calls_to_peer = TRUE;
        }
        if(isset($Customer->_settings->multisite))
        {
          $multisite = json_decode(json_encode($Customer->_settings->multisite),true);
          if(isset($flag)){
            foreach($flag as $key=>$value){
              if($value == $SubscriberPreferences->gpp9){
                $current_site = $multisite[$key];   
              }
            }           
          }
          foreach($multisite as $key=>$site){
            if($SubscriberPreferences->gpp9 == $site['site_name']){
              $current_site = $multisite[$key]; 
            }
          }
          
          if(isset($current_site)){
            //Allowed IPs
            if (isset($ActiveReseller->_settings->enable_restricted_access) &&
              $ActiveReseller->_settings->enable_restricted_access && !$Subscriber->is_fax_number()){
              $SubscriberPreferences->allowed_ips = $current_site['ips'];
            }
            else{
              unset($SubscriberPreferences->allowed_ips);
            }
            //Emergency_mapping_container
            if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing && !$Subscriber->is_fax_number()){
              $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
              $SubscriberPreferences->emergency_mapping_container = $emergency_indexes[$current_site['region']][$current_site['township']][$current_site['city']][$current_site['z_gm']];
            }else{
              unset($SubscriberPreferences->emergency_mapping_container);
            }

            //Site Name
            $SubscriberPreferences->gpp9 = $current_site['site_name'];            
          }
          else{
            foreach($multisite as $site){
              $first_site = $site;break;
            }
            //Allowed IPs
            if (isset($ActiveReseller->_settings->enable_restricted_access) &&
            $ActiveReseller->_settings->enable_restricted_access && !$Subscriber->is_fax_number()){
              $SubscriberPreferences->allowed_ips = $first_site['ips'];
            }
            else{
              unset($SubscriberPreferences->allowed_ips);
            }
            //Emergency_mapping_container
            if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing && !$Subscriber->is_fax_number()){
              $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
              $SubscriberPreferences->emergency_mapping_container = $emergency_indexes[$first_site['region']][$first_site['township']][$first_site['city']][$first_site['z_gm']];
            }else{
              unset($SubscriberPreferences->emergency_mapping_container);
            }

            //Site Name
            $SubscriberPreferences->gpp9 = $first_site['site_name'];                                   
          } 
        }
        else{ 
          if(!empty($SubscriberPreferences->gpp9)){
            unset($SubscriberPreferences->gpp9);  
          }
          // Set allowed ip's in case the settings is set and allowed for
          // this reseller.
          if (isset($ActiveReseller->_settings->enable_restricted_access) &&
            $ActiveReseller->_settings->enable_restricted_access &&
            isset($Customer->_settings->allowed_ips) &&

            // Only set the allowed_ips value for a regular subscriber or a
            // huntgroup
            ($Subscriber->is_regular() || $Subscriber->is_huntgroup())) {

            $SubscriberPreferences->allowed_ips = $Customer->_settings->allowed_ips;
          }
          elseif(isset($SubscriberPreferences->allowed_ips)) {

            // Currently set and not set on customer level, so unset the variable.
            unset($SubscriberPreferences->allowed_ips);
          }

          // Set location id in case polish emergency routing is active for this customer / reseller.
          if (isset($ActiveReseller->_settings->polish_emergency_routing) && $ActiveReseller->_settings->polish_emergency_routing) {
            $emergency_indexes = variable_get('hpbx_emergency_indexes_' . $ActiveReseller->id, array());
            $SubscriberPreferences->emergency_mapping_container = $emergency_indexes[$Customer->_settings->region][$Customer->_settings->township][$Customer->_settings->city][$Customer->_settings->z_gm];

            watchdog('hpbx_customer', 'subscriber emergency_mapping_container: '. $SubscriberPreferences->emergency_mapping_container);
          }
        }
                
        // Save the preferences.
        $SubscriberPreferences->save();
      }
      else {
        // Save subscriber.
        $Subscriber->save();
      }
    }

    // Load pilot subscriber preferences.
    $PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id);
    $PilotSubscriberPreferences->force_inbound_calls_to_peer = $pilot_force_inbound_calls_to_peer;

    $PilotSubscriberPreferences->cli = $ActiveReseller->_settings->country_code . substr(hpbx_parse_primary_number($PilotSubscriber->primary_number), 1);
    $PilotSubscriberPreferences->gpp0 = $PilotSubscriberPreferences->cli;

    // Only when the pilot subscriber is created.
    if (!$form_state['values']['subscriber_id']) {

      // Subscriber successfully created.
      $values = $form_state['values']['general']['subscriber']['table'];
      $role = 'HostedPBX Customer Administrator';

      if ($account = hpbx_users_create_customer_user($Customer, $PilotSubscriber->id,
        $values['email']['value'], $values['firstname']['value'], $values['lastname']['value'],
        $role)) {

        // Now find the uid (==username) of the AD and Drupal user.
        $PilotSubscriber->webusername = $account->uid;
        $PilotSubscriber->save();
      }
      else {
        drupal_set_message(t('Failed to create AD user'), 'error');
        return;
      }
      $PilotSubscriberPreferences->bypass_rtpproxy = 'same_nat';
    }

    $PilotSubscriberPreferences->save();
  }
  catch (Exception $e) {

    // Failed to create customer.
    drupal_set_message(t('Failed to store the customer'), 'error');
    //drupal_set_message(t('Exception:'. $e->getMessage() .' in '. $e->getFile() .' at line '. $e->getLine()), 'error');
    $failed = TRUE;
  }

  if (!empty($ActiveReseller->id)) {
    if (!empty($Customer->id)) {
      // Clear the Drupal cache for this API type.
      $cid = 'customers_'. $ActiveReseller->id . '_'. $Customer->id.'_response';
      cache_clear_all($cid, 'cache_sipwise_api');
      watchdog('sipwise_api_cache', t('Cleared customers with @cid', array('@cid' => $cid)));

      $cid = 'customers_list_'. $ActiveReseller->id.'_';
      cache_clear_all($cid, 'cache_sipwise_api', TRUE);
      watchdog('sipwise_api_cache', t('Cleared customers_list with @cid*', array('@cid' => $cid)));

      // Clear the Drupal cache for this API type.
      $cid = 'customerpreferences_'. $ActiveReseller->id . '_'. $Customer->id .'_response';
      cache_clear_all($cid, 'cache_sipwise_api');
      watchdog('sipwise_api_cache', t('Cleared customerpreferences with @cid', array('@cid' => $cid)));

      $cid = 'customerpreferences_list_'. $ActiveReseller->id.'_';
      cache_clear_all($cid, 'cache_sipwise_api', TRUE);
      watchdog('sipwise_api_cache', t('Cleared customerpreferences list with @cid*', array('@cid' => $cid)));
    }

    if (!empty($Customer->contact_id)) {
      // Clear the Drupal cache for this API type.
      $cid = 'customercontacts_'. $ActiveReseller->id . '_'. $Customer->contact_id .'_response';
      cache_clear_all($cid, 'cache_sipwise_api');
      watchdog('sipwise_api_cache', t('Cleared customercontacts with @cid', array('@cid' => $cid)));

      $cid = 'customercontacts_list_'. $ActiveReseller->id .'_';
      cache_clear_all($cid, 'cache_sipwise_api', TRUE);
      watchdog('sipwise_api_cache', t('Cleared customercontacts list with @cid*', array('@cid' => $cid)));
    }
  }

  if (!isset($failed)) {
    drupal_set_message(t('Customer successfully saved'));
  }

  // In case this was a new customer, remove everyting created.
  elseif (!$form_state['values']['id']) {

    if (!is_null($PilotSubscriber)) {

      if (!empty($PilotSubscriber->webusername)) {
        hpbx_users_remove($PilotSubscriber);
      }

      try {
        // Remove pilot subscriber.
        hpbx_users_remove($PilotSubscriber);
        $PilotSubscriber->delete();
      }
      catch(Exception $e) {}
    }

    // Remove Customer.
    if (!is_null($Customer)) {
      try {
        $Customer->delete();
      }
      catch(Exception $e) {}
    }

    drupal_set_message(t('The customer was not created due to a technical problem. Please contact your system administrator.'), 'error');
  }
   
  if(isset($ActiveReseller->id) && !empty($ActiveReseller->id)){
    hpbx_customer_search_empty($ActiveReseller->id);	
  }elseif(isset($form_state['values']['reseller_id']) && !empty($form_state['values']['reseller_id'])){  
    hpbx_customer_search_empty($form_state['values']['reseller_id']);
  }
   
  // Redirect to the customer overview page.
  if (user_access('hpbx edit own customer settings')) {
    drupal_goto('hpbx/customer');
  }
  else {

    if (!empty($form_state['values']['reseller_id']) && user_access('hpbx view reseller')) {
      drupal_goto('hpbx/affiliate/'. $form_state['values']['reseller_id'] .'/customers');
    }
    else {
      drupal_goto('hpbx/customers');
    }
  }
}

/**
 * @param int $reseller_id
 *
 * @return array
 */
// Modified for #9759 - PE DEV: Customer overview - Improved caching of customer information
function hpbx_customer_get_local($reseller_id, $bp_name = NULL) {
  if (!$reseller_id) return array();

  try {
    // Search for company name and external id.
    $query = db_select('hpbx_customers', 'c')
      ->fields('c');      
    if(!is_null($bp_name)){
		$query->condition(db_and()->condition('reseller_id', $reseller_id)->condition('bp_name', $bp_name));
	}else{
		$query->condition('reseller_id', $reseller_id);
	}	      

    $result = $query->execute();

    return $result->fetchAllAssoc('customer_id');
   
  }  
  catch(Exception $e) {
    watchdog('hpbx_customer_search', $e->getMessage());
    return FALSE;
  }
}

/**
 * @param int $reseller_id
 */
function hpbx_customer_search_empty($reseller_id) {

  try {

    // Remove the customer entry from the local database.
    db_delete('hpbx_customers')
      ->condition('reseller_id', $reseller_id)
      ->execute();

    // Remove all numbers for this customer within the local database.
    db_delete('hpbx_numbers')
      ->condition('reseller_id', $reseller_id)
      ->execute();
  }
  catch (Exception $e) {
    watchdog('SipwiseCustomer', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
  }
}
/**
 * Helper function to verify if the search db table does have the same number
 * of entries as the sipwise customer table.
 *
 * @param integer $reseller_id
 * @return boolean
 */
function hpbx_customer_search_state($reseller_id) {
  global $total_count;
  try {
    $query = array('rows' =>  1, 'page' => 1);

    // Store current reseller_id.
    $orig_reseller_id = sipwise_api_get_active_reseller();

    // Activate temporary reseller.
    sipwise_api_activate_reseller($reseller_id);

    // Execute only; $total_count will be set.
    sipwise_api_get_all('customers', $query);

    // Restore reseller.
    if (!empty($orig_reseller_id) && is_numeric($orig_reseller_id)) {

      // Set default reseller active again.
      sipwise_api_activate_reseller($orig_reseller_id);
    }

    // Retrieve entries from local search index.
    $result = db_select('hpbx_customers', 'c')->fields('c', array('customer_id'))->condition('reseller_id', $reseller_id)->execute();
  }
  catch (Exception $e) {
    watchdog('hpbx_customer_search', $e->getMessage());
    return FALSE;
  }
  return $total_count==$result->rowCount();
}

/**
 * @param array $form
 * @param array $form_state
 * @param int $reseller_id
 *
 * @return array
 */
function hpbx_customer_search_form($form, $form_state, $reseller_id = NULL) {

  global $ActiveReseller;

  /**
   * Customer name -> vp_hpbx_customer_list
   * External ID -> vp_hpbx_customers
   *
   * Number out of a number range that is assigned to a customer
   *  -> vp_hpbx_numbers
   * MAC address of a device that is in use for a customer
   *  -> vp_hpbx_devices
   *
   * Subscriber name -> AD
   * Subscriber email address -> AD
   */
  $form = array();
  $form['query'] = array(
    '#type' => 'textfield',
    '#title' => '',
    '#attributes' => array('placeholder' => t('Search')),
    '#required' => FALSE,
    '#default_value' => isset($_GET['search']) ?
      $_SESSION['ccs']['hpbx']['customer_search']['query'] : '',
  );
  $form['reseller_id'] = array(
    '#type' => 'value',
    '#value' => !is_null($reseller_id) ? $reseller_id : $ActiveReseller->id,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Find'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span>',
  );
  return $form;
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_customer_search_form_validate($form, $form_state) {

  if (!empty($form_state['values']['reseller_id']) && !hpbx_customer_search_state($form_state['values']['reseller_id'])) {
    drupal_set_message(t('The internal customer search database table is not in sync with the Sipwise API. The search results may contain incorrect results.'), 'error');
  }
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_customer_search_form_submit($form, $form_state) {

  $reseller_id = $form_state['values']['reseller_id'];

  $_SESSION['ccs']['hpbx']['customer_search']['query'] = $query = trim($form_state['values']['query']);

  $cn_entries = array();
  unset($_SESSION['ccs']['hpbx']['customer_search']['customers']);

  $ldap_server = hpbx_users_get_ldap_server();

  $Reseller = SipwiseEntity::load('resellers', $form_state['values']['reseller_id']);

  $conditions = array();
  $conditions[] = 'mail=' . ldap_escape(trim($form_state['values']['query']), LDAP_ESCAPE_FILTER);

  // Search for Active Directory users.
  $cn_entries = ccs_ldap_server_search_wrapper($ldap_server, '(&(objectclass=person)(|(' . implode(')(', $conditions) . ')))', hpbx_users_get_reseller_basedn($Reseller));

  foreach ($cn_entries as $user_entry) {

    // Try to load the drupal user account to retrieve the sipwise customer_id.
    if (isset($user_entry[$ldap_server->user_attr][0]) && $account = user_load($user_entry[$ldap_server->user_attr][0])) {

      if (isset($account->data['sipwise']['customer_id'])) {
        $_SESSION['ccs']['hpbx']['customer_search']['customers'][$account->data['sipwise']['customer_id']] = $account->data['sipwise']['customer_id'];
      }
    }
  }

  global $ActiveReseller;

  // Search for a number.
  if ($number = $ActiveReseller->number_get_e164($form_state['values']['query'])) {
    $result = db_select('hpbx_numbers', 'n')
      ->fields('n', array('customer_id'))
      ->condition('n.number', $number)
      ->condition('reseller_id', $reseller_id)
      ->execute()
      ->fetchAssoc();

    if (isset($result['customer_id'])) {
      $_SESSION['ccs']['hpbx']['customer_search']['customers'][$result['customer_id']] = $result['customer_id'];
    }
  }

  $db_or = db_or();
  $db_or->condition('c.company', '%'. $form_state['values']['query'] . '%', 'LIKE');
  $db_or->condition('c.external_id', $form_state['values']['query']);
  if(isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
  	$db_or->condition('c.bp_name', '%'. $form_state['values']['query'] . '%', 'LIKE');
  }

  // Search for company name and external id.
  $query = db_select('hpbx_customers', 'c')
    ->fields('c', array('customer_id'))
    ->condition($db_or)
    ->condition('reseller_id', $reseller_id);

  $result = $query->execute();

  foreach ($result->fetchAllAssoc('customer_id') as $entry) {
    $_SESSION['ccs']['hpbx']['customer_search']['customers'][$entry->customer_id] = $entry->customer_id;
  }

  // Search for mac addresses.
  $query = db_select('hpbx_pbxdevices', 'd')
    ->fields('d', array('customer_id'))
    ->condition('identifier', preg_replace('/[^a-f0-9]/', '', strtolower($form_state['values']['query'])))
    ->condition('reseller_id', $reseller_id);

  $result = $query->execute();

  foreach ($result->fetchAllAssoc('customer_id') as $entry) {
    $_SESSION['ccs']['hpbx']['customer_search']['customers'][$entry->customer_id] = $entry->customer_id;
  }

  if (!isset($_SESSION['ccs']['hpbx']['customer_search']['customers'])) {
    drupal_set_message(t('No customer found that matches the entered search criteria'), 'error');
  }

  if (arg(1) == 'affiliate') {
    drupal_goto('hpbx/affiliate/'.$reseller_id.'/customers',array('query' => array(
        'search' => 'true'
    ),));
  }
  else {
   drupal_goto('hpbx/customers',array('query' => array(
        'search' => 'true'
    ),));
  }
}



/**
 * Remove the customer and all it's child entities.
 *
 * @param integer $customer_id
 */
function hpbx_customer_delete_page($customer_id, $reseller_id = NULL) {
  try {

    global $conf;
    global $user;
    global $ActiveReseller;

    if (!empty($reseller_id)) {
      sipwise_api_activate_reseller($reseller_id);
    }

    // Verify if the customer id is passed.
    if (!is_null($customer_id)) {

      // Load the customer.
      if ($Customer = SipwiseEntity::load('customers', $customer_id)) {

        if ($Customer->delete_allowed() != HPBX_CUSTOMER_SUSPENDED_DELETE_ALLOWED && !variable_get('hpbx_sipwise_delete_allowed', FALSE) ) {
          drupal_access_denied();
          die();
        }
        
        // Get all subcribers based on customer id.
        foreach (sipwise_api_get_all('subscribers', array('customer_id' => $customer_id)) as $subscriber) {
        	
          if (!$subscriber->is_pbx_pilot) {
          	$SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $subscriber->id);
          	// Delete the subcriber in stretto. 
          	$stretto = new StrettoUser();
	        $delete['userName'] = $subscriber->webusername;
            $delete['groupName'] = $ActiveReseller->_settings->stretto_group;	
	        $stretto_subscriber = $stretto->stretto_get('user',$delete);  
          	
          	if(!empty($SubscriberPreferences->softphone_autoprov)){
	  		  if(!empty($stretto_subscriber->CcsUser)){	  		  	
	  	        $stretto->stretto_delete('user', $delete);
	    	  }
	    	  else{
	            throw new ErrorException('Can\'t delete empty StrettoEntity');	  
		      }
	        }                       
          }
        } 


        // 'Delete' the customer; will set the state to 'terminated'.
        $Customer->delete();
        $CustomerContact = SipwiseEntity::load('customercontacts', $Customer->contact_id);
		$CustomerContact->delete();

        // Set message.
        drupal_set_message(t('Customer successfully deleted'));

        if (!empty($Customer->id)) {
          global $ActiveReseller;

          // Clear the Drupal cache for this API type.
          $cid = 'customers_'. $ActiveReseller->id . '_'. $Customer->id.'_response';
          cache_clear_all($cid, 'cache_sipwise_api');
          watchdog('sipwise_api_cache', t('Cleared customers with @cid', array('@cid' => $cid)));

          $cid = 'customers_list_'. $ActiveReseller->id.'_';
          cache_clear_all($cid, 'cache_sipwise_api', TRUE);
          watchdog('sipwise_api_cache', t('Cleared customers_list with @cid*', array('@cid' => $cid)));

          // Clear the Drupal cache for this API type.
          $cid = 'customerpreferences_'. $ActiveReseller->id . '_'. $Customer->id .'_response';
          cache_clear_all($cid, 'cache_sipwise_api');
          watchdog('sipwise_api_cache', t('Cleared customerpreferences with @cid', array('@cid' => $cid)));

          $cid = 'customerpreferences_list_'. $ActiveReseller->id.'_';
          cache_clear_all($cid, 'cache_sipwise_api', TRUE);
          watchdog('sipwise_api_cache', t('Cleared customerpreferences list with @cid*', array('@cid' => $cid)));

          // Clear the Drupal cache for this API type.
          $cid = 'customercontacts_'. $ActiveReseller->id . '_'. $Customer->contact_id .'_response';
          cache_clear_all($cid, 'cache_sipwise_api');
          watchdog('sipwise_api_cache', t('Cleared customercontacts with @cid', array('@cid' => $cid)));

          $cid = 'customercontacts_list_'. $ActiveReseller->id .'_';
          cache_clear_all($cid, 'cache_sipwise_api', TRUE);
          watchdog('sipwise_api_cache', t('Cleared customercontacts list with @cid*', array('@cid' => $cid)));
        }
      }
    }
  }
  catch (Exception $e) {

    drupal_set_message(t('Failed to delete the customer'), 'error');
    watchdog('hpbx', $e->getMessage());
  }

  if (!is_null($reseller_id)) {
    drupal_goto('hpbx/affiliate/'. $reseller_id .'/customers');
  }
  else {
    drupal_goto('hpbx/customers');
  }
}


function hpbx_customer_own_settings_page() {
  global $user;

  if ($Customer = hpbx_get_active_customer()) {
    $form = drupal_get_form('hpbx_customer_edit_form', $Customer->id);

    menu_set_active_item('hpbx/customer');

    return $form;
  }
  else {
    drupal_not_found();
    exit();
  }
}


function hpbx_customer_view_form($form, $form_state, $customer_id) {

  return $form;
}


/**
 *
 * @param unknown $customer_id
 */
function hpbx_customer_page($customer_id, $reseller_id = NULL) {
  // Retrieve the edit form.
  $form = hpbx_customer_edit_form(array(), array(), $customer_id, $reseller_id);

  // Unset the description.
  unset($form['description']);

  // Set all elements as #type => 'item'
  foreach (element_children($form) as $name) {
    hpbx_set_read_only($form[$name]);
  }

  unset($form['numberranges']['table']['#header']['op']);

  foreach (element_children($form['numberranges']['table']) as $key) {
    unset($form['numberranges']['table'][$key]['op']);
  }

  unset($form['actions']);
  unset($form['general']['settings']);
  // Set title.
  drupal_set_title(t('Customer !name', array('!name' => $form['general']['table']['external_id']['value']['#markup'])));

  // Render the form.
  return drupal_render($form);
}
