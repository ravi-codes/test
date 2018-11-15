<?php

function hpbx_reseller_report_page($reseller_id) {
  require_once (drupal_get_path('module', 'ccs') . '/includes/PHPExcel/Classes/PHPExcel/IOFactory.php');
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.subscriber.bulk.inc');
  
  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }
  global $ActiveReseller;

  $Profiles = array();
  
  $objPHPExcel = new PHPExcel();
  $objPHPExcel->setActiveSheetIndex(0);
    
  $headers = array();
  $headers['company'] = t('Company name');
  $headers['external_id'] = t('PTSID');
  $headers['pilot_number'] = t('Pilot Number');
  $headers['concurrent_max'] = t('Concurrent Max');
  $headers['status'] = t('Status');
  $headers['devices'] = t('Devices');
    
  foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
    foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {
      if($SubscriberProfileSet->name != 'Generic'){
      	$headers[$SubscriberProfileSet->description . ' ' . $SubscriberProfile->name] = $SubscriberProfileSet->description . ' ' . $SubscriberProfile->name;
	  	$Profiles[$SubscriberProfile->id] = $SubscriberProfileSet->description . ' ' . $SubscriberProfile->name;
      }
	  else{
	  	$headers[$SubscriberProfile->name] = $SubscriberProfile->name . ' ' . 'Profile';
      	$Profiles[$SubscriberProfile->id] = $SubscriberProfile->name . ' ' . 'Profile';	 	
	  }
	}
  }
  
  $i = 0;
  foreach ($headers as $key => $label) {
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue(chr(65+($i++)). '1', $label);
  }    
    
  $rows = array();
   
  $query = array();
  $query['reseller_id'] = $reseller_id;

  // Get all CustomerContacts, to speed up displaying the list.
  $customercontacts = sipwise_api_get_all('customercontacts', $query, TRUE);

  // Get all CustomerPreferences, to speed up displaying the list.
  $customerpreferences = sipwise_api_get_all('customerpreferences', $query, TRUE);

  // Get all customers.
  $customers = sipwise_api_get_all('customers', $query, TRUE, FALSE, FALSE);
  
  
  foreach ($customers as $Customer) {
  	$cust = array();
    $queries = array('customer_id' => $Customer->id);
    $CustomerContact = $customercontacts[$Customer->contact_id];
    $Subscribers = sipwise_api_get_all('subscribers', $queries, TRUE);
    foreach($Subscribers as $subscriber){
      if($subscriber->is_pbx_pilot == FALSE){
      	$temp = array();
	  	$temp['company_name'] = $CustomerContact->company;
        $temp['external_id'] = $Customer->external_id;
        $temp['number'] = $subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
        $temp['subscriber_type'] = $Profiles[$subscriber->profile_id];
	  }else{
	  	continue;
	  }
	  $cust[] = $temp;
	}
	$numbers[] = $cust;    
  }
          
  // Get all customers.
  foreach ($customers as $Customer) {
    // Set customer_id as query.
    $query = array('customer_id' => $Customer->id);
    $CustomerContact = $customercontacts[$Customer->contact_id];
    $CustomerPreferences = $customerpreferences[$Customer->id];
  
    $row = array();
    
    $row['company'] = $CustomerContact->company;
    $row['external_id'] = $Customer->external_id;
    $row['pilot_number'] = (string)$ActiveReseller->_settings->country_code . substr(hpbx_parse_primary_number($Customer->get_pilot_subscriber()->primary_number), strlen($ActiveReseller->_settings->digit_for_national_calls));
    $row['concurrent_max'] = $CustomerPreferences->concurrent_max;
    $row['status'] = (!empty($ActiveReseller->_settings->suspend_ncos_level) && $CustomerPreferences->ncos == $ActiveReseller->_settings->suspend_ncos_level) ? t('Suspended') : t(drupal_ucfirst($Customer->status));
    $row['devices'] = count(sipwise_api_get_all('pbxdevices', $query, TRUE));
        
    $Subscribers = sipwise_api_get_all('subscribers', $query, TRUE);
    $keys = array_values($Profiles);
    $stats = array_fill_keys($keys, 0);
    
    foreach($Subscribers as $subscriber){
      $stats[$Profiles[$subscriber->profile_id]]++;
    }
	foreach($stats as $key => $val){
	  $row[$key] = $val;	
	}  
    $rows[] = $row;
  }
  
  $id = 2;
  foreach($rows as $value){
  	$i = 0;
  	foreach($value as $key => $val){
  	  if(is_numeric($val)){
	    $objPHPExcel->setActiveSheetIndex(0)->getCell(chr(65+($i++)). $id)->setValueExplicit($val, PHPExcel_Cell_DataType::TYPE_STRING);
	  }else{
	    $objPHPExcel->setActiveSheetIndex(0)->setCellValue(chr(65+($i++)). $id, $val);		
	  }
	}
  	$id++;
  }
  $objPHPExcel->getActiveSheet()->setTitle(t('Customers'));
  $objPHPExcel->setActiveSheetIndex(0);
  hpbx_subscriber_bulk_excel_set_header_style($objPHPExcel, 'A1:'.chr(65+(--$i)) . '1');
  foreach (range('A', chr(65+$i)) as $columnID) {
    $objPHPExcel->getActiveSheet()->getColumnDimension($columnID)->setAutoSize(true);
  }
    
  
  $header = array();
  $header['company'] = t('Company name');
  $header['external_id'] = t('PTSID');
  $header['concurrent_max'] = t('Number');
  $header['subscriber_type'] = t('Subscriber Type');
  
  $objPHPExcel->createSheet();
  $objPHPExcel->setActiveSheetIndex(1);
  
  
  $j = 0;
  foreach ($header as $key => $label) {
    $objPHPExcel->setActiveSheetIndex(1)->setCellValue(chr(65+($j++)). '1', $label);
  }
  $id = 2;
  foreach($numbers as $customer){
  	foreach($customer as $subscriber){
  	  $i = 0;	
	  foreach($subscriber as $key => $val){
	  	if(is_numeric($val)){
		  $objPHPExcel->setActiveSheetIndex(1)->getCell(chr(65+($i++)). $id)->setValueExplicit($val, PHPExcel_Cell_DataType::TYPE_STRING);	
		}else{
		  $objPHPExcel->setActiveSheetIndex(1)->setCellValue(chr(65+($i++)). $id, $val);	
		}
	  }
	  $id++;	
	}
  }
  
  $objPHPExcel->setActiveSheetIndex(1);
  $objPHPExcel->getActiveSheet()->setTitle(t('Numbers'));
  hpbx_subscriber_bulk_excel_set_header_style($objPHPExcel, 'A1:'.chr(65+(--$i)) . '1');
  
  foreach (range('A', chr(65+$i)) as $columnID) {
    $objPHPExcel->getActiveSheet()->getColumnDimension($columnID)->setAutoSize(true);
  }
    
  header('Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet');
  header('Content-Disposition: attachment;filename="report-' . date('d-m-Y') . '.xlsx"');
  header('Cache-Control: max-age=0');
  $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel2007');
  $objWriter->save('php://output');
  exit;
}


/**
 * 
 * @param unknown $reseller_id
 */
function hpbx_reseller_clear_cache($reseller_id, $cron = FALSE) {

  global $conf;
  
  $cid = 'resellers_0_';
  cache_clear_all($cid, 'cache_sipwise_api', TRUE);
  watchdog('sipwise_api_cache', t('Cleared @type with @cid*', array('@cid' => $cid)));
  
  
  foreach ($conf['sipwise_api']['sipwise_api_cached_lists'] as $type) {
    $cid = $type .'_list_' . $reseller_id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared @type with @cid*', array('@cid' => $cid)));
    
    if($cron == FALSE){
		drupal_set_message(t('Cleared the cache for API type @api_type for @reseller_id (list)', array('@api_type' => $type, '@reseller_id' => $reseller_id)));
	}
    
  }
  
  foreach ($conf['sipwise_api']['sipwise_api_cached_entities'] as $type) {
    $cid = $type .'_'. $reseller_id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared @type with @cid*', array('@cid' => $cid)));
    if($cron == FALSE){
		drupal_set_message(t('Cleared the cache for API type @api_type for @reseller_id (entity)', array('@api_type' => $type, '@reseller_id' => $reseller_id )));
	}
    
  }

  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.customer.inc');

  // Remove all entries for this reseller.
  hpbx_customer_search_empty($reseller_id);  
  
  if($cron == FALSE){
  	 drupal_goto('hpbx/affiliates');
  }
 
}

/**
 *
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function hpbx_resellers_overview_form($form, $form_state) {
  global $conf, $ActiveReseller;
  $text = '<span class="sg-icon"></span> <h5 class="sg-header-5"></h5>';

  $form = array();
  $form['general'] = array(
    '#add' => array('data' => $text, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-resellers hpbx-table-sortable'),
    '#header' => array(
      'name' => array('data'=> t('Name'), 'class' => array('hpbx-show')),
      'operations' => array('data'=> '', 'class' => array( 'hpbx-show'))
    ),
  );
  
  foreach (hpbx_get_resellers() as $Reseller) {
    if (!isset($conf['sipwise'][$Reseller->id])) continue;
    $form['general'][$Reseller->id]['name']['data'] = array('#markup' => $Reseller->name);
    $form['general'][$Reseller->id]['name']['#attributes']['class'] = array('hpbx-show');
    sipwise_api_activate_reseller($Reseller->id);
    $op = array();
    $op[] =  l(t('Settings'), 'hpbx/affiliate/'. $Reseller->id . '/edit', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
    $op[] = l(t('Users'), 'hpbx/affiliate/'. $Reseller->id . '/users', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
    $op[] = l(t('Customers'), 'hpbx/affiliate/'. $Reseller->id . '/customers', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
    if(isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
		$op[] = l(t('Business Partners'), 'hpbx/affiliate/'. $Reseller->id . '/bps', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
	}else{
		$op[] = '<span title="'. t('You are not authorised to create Business Partners') .'" class="sg-btn-small sg-btn sg-without-icon sg-disabled">'. t('Business Partners') . '</span>';
	}
    
    $op[] = l(t('Clear cache'), 'hpbx/affiliate/'. $Reseller->id . '/cc', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));

    $form['general'][$Reseller->id]['operations']['data'] = array('#markup' => implode(' ', $op));
    $form['general'][$Reseller->id]['operations']['#attributes']['class'] = array('hpbx-show');
  }
  return $form;
}

/**
 * Callback hpbx/affiliate/%/emergency_download
 *
 * @param integer $reseller_id
 */
function hpbx_reseller_emergency_mappings_download($reseller_id) {

  $orig_reseller_id = sipwise_api_get_active_reseller();

  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }
  else {
    // Use the reseller which is already active.
    $reseller_id = $orig_reseller_id;
  }


  $headers = array(
    'Accept: text/csv',
  );

  list($headers, $body) = Sipwise::exec('/api/emergencymappings/?reseller_id='. $reseller_id, NULL, 'GET', $headers);
  if (isset($body->message) || strpos($headers['http_code'], '504 Gateway Time-out')!==FALSE) {
    if (!empty($body->message)) {
      watchdog('sipwise_api', 'Failed to load emergencymappings. Error message: '. $body->message . ' (' . $body->code . ')');
    }
    else {
      watchdog('sipwise_api', 'Failed to load emergencymappings');
    }
    drupal_not_found();
    die();
  }

  if (!empty($orig_reseller_id) && is_numeric($orig_reseller_id)) {
    sipwise_api_activate_reseller($orig_reseller_id);
  }

  // Set headers.
  header('Content-type: text/csv');
  header('Content-Length: ' . strlen($body));
  header('Content-Disposition: attachment; filename="report-' . date('Ymdhis') . '.csv"');

  echo $body;
  die();
}

/**
 * Callback hpbx/affiliate/%/emergency_download
 * 
 * @param integer $reseller_id
 */
function hpbx_reseller_emergency_indexes_download($reseller_id) {

  $emergency_indexes = variable_get('hpbx_emergency_indexes_'. $reseller_id, array());

  $dv = array();

  foreach ($emergency_indexes as $region => $townships) {
    foreach ($townships as $township => $cities) {
      foreach ($cities as $city => $z_gms) {
        foreach ($z_gms as $z_gm => $location_id) {
          $row = array($location_id, $city, $z_gm, $township, $region);
          $dv[] = implode(';', $row);
        }
      }
    }
  }

  // Set headers.
  header('Content-type: text/csv');
  header('Content-Length: ' . strlen(implode(PHP_EOL, $dv)));
  header('Content-Disposition: attachment; filename="report-' . date('Ymdhis') . '.csv"');
  echo implode(PHP_EOL, $dv);
  die();
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function hpbx_reseller_edit_form($form, $form_state, $reseller_id = NULL) {
	
  global $conf, $ActiveReseller;
 
  $orig_reseller_id = sipwise_api_get_active_reseller();

  if (!is_null($reseller_id)) {
    sipwise_api_activate_reseller($reseller_id);
  }
  else {
    // Use the reseller which is already active.
    $reseller_id = $orig_reseller_id;
  }
  
  // Add table styling.
  

  drupal_add_css(drupal_get_path('module', 'hpbx') . '/includes/css/hpbx.css');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));
  
  
  // Request Customer values.
  $Reseller = NULL;
  if (!is_null($reseller_id)) {
    if ($Reseller = SipwiseEntity::load('resellers', $reseller_id)) {
      drupal_set_title(t('!name', array('!name' => $Reseller->name)));
    }
    else {
      drupal_not_found();
      exit();
    }
  }
  else {
    drupal_not_found();
    die();
  }

  $actions = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );
  $actions['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<span class="submit-button">',
    '#suffix' => '</span>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
  $form = array(
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  
  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $Reseller->id,
  );
  
   // Create fieldsets.
  $fieldsets = array(
    'general' =>  array('title' => t('General'), 'tab' => 'General'),
    'dashboard' => array('title' => t('Dashboard'), 'tab' => 'Dashboard'),
    'mail' => array('title' => t('Email'), 'tab' => 'Email'),
    'terms' => array('title' => t('Terms and Conditions'), 'tab' =>'Terms and Conditions'),
    'emergency' => array('title' => t('Emergency Settings'), 'tab' => 'Emergency Settings'),
  );

  // Create tabs.
  $items = array();
  foreach ($fieldsets as $key => $element) {
    if (user_access('hpbx edit '. $key .' reseller settings')) {
    
    	$data = '<div class="hpbx-tab-item-title">
                  <a href="#tab-'. strtolower(str_replace(' ', '-', $element['tab'])) . '">'. $element['title'] .'</a>
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
      '#access' => user_access('hpbx edit '. $key .' reseller settings'),
      '#attributes' => array(
        'id' => 'tab-'. strtolower(str_replace(' ', '-', $fieldset['tab'])),
      ),
    );
  }
  
  $form['mail']['#theme_wrappers'] = array('hpbx_foldable_list', 'container');
  $form['mail']['#attributes']['class'] = array('hpbx-tab-form hpbx-tab-form-foldable-list');
  $form['mail']['#tabform'] = FALSE;
  
  $form['mail']['general'] = array(
    '#type' => 'container',
    '#title' => t('Logo & Footer'),
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#tree' => TRUE,
  );
  
  if (!empty($Reseller->_settings->logo_path)) {
    $form['mail']['general']['logo_preview']['value'] = array(
      '#type' => 'markup',
      '#title' => '',
      '#markup' => theme('image', array('path' => $Reseller->_settings->logo_path)),
      '#title' => t('Logo preview'),
    );
  }
  
  // Customer_administrator_welcome subject.
  $form['mail']['general']['logo']['value'] = array(
    '#type' => 'file',
    '#title' => t('Logo'),
    '#default_value' => '',
    '#description' => t('Upload an email logo. Allowed extensions: jpg, jpeg, png'),
  );
  
  
  $form['mail']['general']['mail_footer']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Footer'),
    '#default_value' => isset($Reseller->_settings->mail_footer) ? $Reseller->_settings->mail_footer : '',
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $mail_key = 'customer_administrator_welcome_'. $Reseller->id;

  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Administrator welcome email'),
  );
  
  // Customer_administrator_welcome subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_subject', 'New login credentials for [hpbx:company] in [hpbx:profile_set_name]'),
    '#required' => TRUE,
     '#attributes' => array('class' => array('sg-element')),
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
  
  // Customer_administrator_welcome body.
  $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_body', $body),
    '#required' => TRUE,
    '#rows' => 17,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $mail_key = 'customer_subscriber_welcome_'. $Reseller->id;
  
  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Subscriber welcome email'),
  );
  // Customer_subscriber_welcome subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_subject', 'New login credentials for [hpbx:firstname] [hpbx:lastname] in [hpbx:lgi_product_name]'),
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
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
    '#default_value' => variable_get('user_mail_'. $mail_key .'_body', $body),
    '#required' => TRUE,
    '#rows' => 17,
    '#attributes' => array('class' => array('sg-element')),
  );

  $mail_key = 'affiliate_user_welcome_'. $Reseller->id;

  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Affiliate user welcome email'),
  );
  // Customer_subscriber_welcome subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_subject', 'Login information for [hpbx:firstname] [hpbx:lastname] at [hpbx:lgi_brand_name]'),
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
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

  // Customer_subscriber_welcome body.
  $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_body', $body),
    '#required' => TRUE,
    '#rows' => 17,
    '#attributes' => array('class' => array('sg-element')),
  );

  ///////////
  if(isset($ActiveReseller->_settings->enable_business_partners) && !empty($ActiveReseller->_settings->enable_business_partners)){
   $mail_key = 'bp_administrator_welcome_'. $Reseller->id;
  
   $form['mail']['user_mail_'. $mail_key] = array(
     '#type' => 'container',
     '#tree' => TRUE,
     '#theme_wrappers' => array('hpbx_foldable_list_item'),
     '#title' => t('Business Partner welcome email'),
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

  // reset_password body.
   $form['mail']['user_mail_'. $mail_key]['body']['value'] = array(
     '#type' => 'textarea',
     '#title' => t('Body'),
     '#default_value' => variable_get('user_mail_'. $mail_key .'_body', $body),
     '#required' => TRUE,
     '#rows' => 17,
   );  	
 }
    
  ///////////

    //Bug #9722 - Make password recovery email text configurable in affiliate settings
  $mail_key = 'reset_password_'. $Reseller->id;

  $form['mail']['user_mail_'. $mail_key] = array(
    '#type' => 'container',
    '#tree' => TRUE,
    '#theme_wrappers' => array('hpbx_foldable_list_item'),
    '#title' => t('Reset Password'),
  );
  // reset_password subject.
  $form['mail']['user_mail_'. $mail_key]['subject']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => variable_get('user_mail_'. $mail_key .'_subject', 'Replacement login information for [hpbx:firstname] [hpbx:lastname] at [hpbx:lgi_brand_name]'),
    '#required' => TRUE,
  );

  $body = <<<EOF
[hpbx:firstname] [hpbx:lastname],

A request to reset the password for your account has been made at [hpbx:lgi_brand_name].
  
You may now log in by clicking this link or copying and pasting it to your browser:
<small>(If the link does not work, you can also copy and paste it into your browser)</small>
  
[hpbx:one-time-login-url]
  
This link can only be used once to log in and will lead you to a page where you can set your password. It expires after one day and nothing will happen if it's not used.
  
-- [hpbx:lgi_brand_name]
EOF;

  // reset_password body.
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

  $form['terms']['links']['value'] = array(
      '#type' => 'item',
      '#markup' => !empty($Reseller->_settings->terms_nid) ? l(t('View'), 'node/'. $Reseller->_settings->terms_nid) . ' ' . l(t('Edit'), 'node/'. $Reseller->_settings->terms_nid . '/edit') : '' ,
  );
  
  $form['terms']['actions'] = $actions;
  
  $form['dashboard']['help_telnumber']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Phonenumber'),
    '#default_value' => !empty($Reseller->_settings->help_telnumber) ? $Reseller->_settings->help_telnumber : '',
    '#required' => TRUE,
    '#maxlength' => 15,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['dashboard']['help_website']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help website url'),
    '#default_value' => !empty($Reseller->_settings->help_website) ? $Reseller->_settings->help_website : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['dashboard']['help_mail']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help mail address'),
    '#default_value' => !empty($Reseller->_settings->help_mail) ? $Reseller->_settings->help_mail : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['dashboard']['help_name']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Help name'),
    '#default_value' => !empty($Reseller->_settings->help_name) ? $Reseller->_settings->help_name : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['dashboard']['actions'] = $actions;

  $form['general']['name']['value'] = array(
    '#type' => 'item',
    '#title' => t('Name'),
    '#markup' => !empty($Reseller->name) ? $Reseller->name : '',
  );
  
  $form['general']['timezone']['value'] = array(
    '#type' => 'select',
    '#title' => t('Time Zone'),
    '#options' => system_time_zones(),
    '#default_value' => isset($Reseller->_settings->timezone) ? $Reseller->_settings->timezone: NULL,
    '#required' => TRUE,
    '#empty_value' => '',
    '#attributes' => array('class' => array('chzn-select', 'sg-element')),
  );
  
  $form['general']['timezone_correction']['value'] = array(
    '#type' => 'select',
    '#title' => t('Time Zone correction required'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->timezone_correction) ? $Reseller->_settings->timezone_correction : NULL,
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  if($ActiveReseller->_settings->help_mail == 'No mail support'){
   $form['general']['domain_id']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Domain ID'),
    '#default_value' => !empty($Reseller->_settings->domain_id) ? $Reseller->_settings->domain_id : '',
    '#required' => TRUE,
    '#maxlength' => 3,
    '#attributes' => array('class' => array('sg-element')),
  );
  }else{
    $domains = sipwise_api_get_all('domains',array('reseller_id' => $Reseller->id));
    foreach ($domains as $domain){
      $options[$domain->id] = $domain->domain; 
    }
    $form['general']['domain_id']['value'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Domain ID'),    
    '#required' => TRUE,
    '#options' => $options,
	'#prefix' => '<span style="white-space: nowrap;">',
	'#suffix' => '</span>',
	'#default_value' => isset($Reseller->_settings->domain_id) ? $Reseller->_settings->domain_id : '',
    '#field_size_class' => array('col-xs-12', 'col-sm-8', 'col-md-1'),    
  );
  }
  $form['general']['digit_for_national_calls']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('digit_for_national_calls'),
    '#default_value' => isset($Reseller->_settings->digit_for_national_calls) ? $Reseller->_settings->digit_for_national_calls : '',
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['country_code']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('country_code'),
    '#default_value' => isset($Reseller->_settings->country_code) ? $Reseller->_settings->country_code : '',
    '#required' => TRUE,
      '#attributes' => array('class' => array('sg-element')),
  );

  $form['general']['area_code_min_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('area_code_min_digits'),
    '#default_value' => isset($Reseller->_settings->area_code_min_digits) ? $Reseller->_settings->area_code_min_digits : '',
    '#required' => TRUE,
    '#description' => t('Number of digits includes the digit for national calls'),
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['area_code_max_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('area_code_max_digits'),
    '#default_value' => isset($Reseller->_settings->area_code_max_digits) ? $Reseller->_settings->area_code_max_digits : '',
    '#required' => TRUE,
    '#description' => t('Number of digits includes the digit for national calls'),
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['subscriber_number_min_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('subscriber_number_min_digits'),
    '#default_value' => isset($Reseller->_settings->subscriber_number_min_digits) ? $Reseller->_settings->subscriber_number_min_digits : '',
    '#required' => TRUE,
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['subscriber_number_max_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('subscriber_number_max_digits'),
    '#default_value' => isset($Reseller->_settings->subscriber_number_max_digits) ? $Reseller->_settings->subscriber_number_max_digits : '',
    '#required' => TRUE,
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['pilot_subscriber_number_min_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('pilot_subscriber_number_min_digits'),
    '#default_value' => isset($Reseller->_settings->pilot_subscriber_number_min_digits) ? $Reseller->_settings->pilot_subscriber_number_min_digits : '',
    '#required' => TRUE,
    '#description' => t('Number of digits includes the digit for national calls'),
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['pilot_subscriber_number_max_digits']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('pilot_subscriber_number_max_digits'),
    '#default_value' => isset($Reseller->_settings->pilot_subscriber_number_max_digits) ? $Reseller->_settings->pilot_subscriber_number_max_digits : '',
    '#required' => TRUE,
    '#description' => t('Number of digits includes the digit for national calls'),
      '#attributes' => array('class' => array('sg-element')),
  );

  $form['general']['automatic_pilot_number_generation']['value'] = array(
    '#type' => 'select',
    '#title' => t('automatic_pilot_number_generation'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->automatic_pilot_number_generation) ? $Reseller->_settings->automatic_pilot_number_generation : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['automatic_pilot_number_generation_prefix']['value'] = array(
      '#type' => 'textfield',
      '#title' => t('automatic_pilot_number_generation_prefix'),
      '#default_value' => isset($Reseller->_settings->automatic_pilot_number_generation_prefix) ? $Reseller->_settings->automatic_pilot_number_generation_prefix : '',
      '#required' => FALSE,
      '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['customer_concurrent_max']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('customer_concurrent_max'),
    '#default_value' => isset($Reseller->_settings->customer_concurrent_max) ? implode(PHP_EOL, $Reseller->_settings->customer_concurrent_max) : '',
    '#required' => TRUE,
    '#description' => t('Number of concurrent max calls.'),
    '#attributes' => array('class' => array('sg-element')),
      
  );
  $form['general']['customer_max_extension_length']['value'] = array(
    '#type' => 'select',
    '#title' => t('customer_max_extension_length'),
    '#default_value' => isset($Reseller->_settings->customer_max_extension_length) ? $Reseller->_settings->customer_max_extension_length : '',
    '#required' => TRUE,
    '#description' => t('Max extension length.'),
    '#options' => drupal_map_assoc(range(2,5)),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['extensions_are_not_allowed_to_start_with']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('extensions_are_not_allowed_to_start_with'),
    '#default_value' => isset($Reseller->_settings->extensions_are_not_allowed_to_start_with) ? implode(PHP_EOL, $Reseller->_settings->extensions_are_not_allowed_to_start_with) : '',
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['exclude_extensions']['value'] = array(
    '#type' => 'textarea',
    '#title' => t('exclude_extensions'),
    '#default_value' => isset($Reseller->_settings->exclude_extensions) ? implode(PHP_EOL, $Reseller->_settings->exclude_extensions) : '',
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['general']['allow_subscribers_without_did_default']['value'] = array(
    '#type' => 'select',
    '#title' => t('Allow Subscribers without DID default'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->allow_subscribers_without_did_default) ? $Reseller->_settings->allow_subscribers_without_did_default : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['polish_emergency_routing']['value'] = array(
    '#type' => 'select',
    '#title' => t('polish_emergency_routing'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->polish_emergency_routing) ? $Reseller->_settings->polish_emergency_routing : 0,
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['bundle_settings']['value'] = array(
    '#type' => 'select',
    '#title' => t('bundle_settings'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->bundle_settings) ? $Reseller->_settings->bundle_settings : 0,
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['emergency_prefix']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('emergency_prefix'),
    '#default_value' => isset($Reseller->_settings->emergency_prefix) ? $Reseller->_settings->emergency_prefix : '',
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element')),
  );
  $form['general']['execute_logic_for_area_code_dependant_emergency_suffix']['value'] = array(
    '#type' => 'select',
    '#title' => t('execute_logic_for_area_code_dependant_emergency_suffix'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->execute_logic_for_area_code_dependant_emergency_suffix) ? $Reseller->_settings->execute_logic_for_area_code_dependant_emergency_suffix : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $options = array();
  foreach (sipwise_api_get_all('ncoslevels') as $NCOSLevel) {
      $options[$NCOSLevel->level] = $NCOSLevel->level . ' - '. t($NCOSLevel->description);
  }
  ksort($options);
  
  $form['general']['suspend_ncos_level']['value'] = array(
    '#type' => 'select',
    '#title' => t('suspend_ncos_level'),
    '#options' => $options,
    '#default_value' => isset($Reseller->_settings->suspend_ncos_level) ? $Reseller->_settings->suspend_ncos_level : '',
    '#required' => FALSE,
    '#empty_value' => '',
      '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  $form['general']['default_subscriber_ncos_level']['value'] = array(
    '#type' => 'select',
    '#title' => t('default_subscriber_ncos_level'),
    '#options' => $options,
    '#default_value' => isset($Reseller->_settings->default_subscriber_ncos_level) ? $Reseller->_settings->default_subscriber_ncos_level : '',
    '#required' => FALSE,
    '#empty_value' => '',
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  $form['general']['mail_from_address']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('mail_from_address'),
    '#default_value' => isset($Reseller->_settings->mail_from_address) ? $Reseller->_settings->mail_from_address : '',
    '#required' => TRUE,
      '#attributes' => array('class' => array('sg-element')),
  );

  $subscriber_profile_options = $subscriber_profilesets_options = array();
  foreach (sipwise_api_get_all('subscriberprofilesets') as $profileset) {
    $subscriber_profilesets_options[$profileset->id] = $profileset->id . ': '. $profileset->name;
    foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $profileset->id)) as $profile) {
      $subscriber_profile_options[$profile->id] = $profile->id . ': '. $profile->name;
    }
  }
  
  $form['general']['generic_profile_set_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('generic_profile_set_id'),
    '#default_value' => isset($Reseller->_settings->generic_profile_set_id) ? $Reseller->_settings->generic_profile_set_id : '',
    '#required' => TRUE,
    '#options' => $subscriber_profilesets_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  
  $form['general']['pilot_subscriber_profile_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('pilot_subscriber_profile_id'),
    '#default_value' => isset($Reseller->_settings->pilot_subscriber_profile_id) ? $Reseller->_settings->pilot_subscriber_profile_id : '',
    '#required' => TRUE,
    '#options' => $subscriber_profile_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['hunt_group_subscriber_profile_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('hunt_group_subscriber_profile_id'),
    '#default_value' => isset($Reseller->_settings->hunt_group_subscriber_profile_id) ? $Reseller->_settings->hunt_group_subscriber_profile_id : '',
    '#required' => TRUE,
    '#options' => $subscriber_profile_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  
  $form['general']['auto_attendant_subscriber_profile_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('auto_attendant_subscriber_profile_id'),
    '#default_value' => isset($Reseller->_settings->auto_attendant_subscriber_profile_id) ? $Reseller->_settings->auto_attendant_subscriber_profile_id : '',
    '#required' => TRUE,
    '#options' => $subscriber_profile_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['fax_subscriber_profile_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('fax_subscriber_profile_id'),
    '#default_value' => isset($Reseller->_settings->fax_subscriber_profile_id) ? $Reseller->_settings->fax_subscriber_profile_id : '',
    '#required' => FALSE,
    '#empty_value' => '',
    '#options' => $subscriber_profile_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['conference_room_subscriber_profile_id']['value'] = array(
    '#type' => 'select',
    '#title' => t('conference_room_subscriber_profile_id'),
    '#default_value' => isset($Reseller->_settings->conference_room_subscriber_profile_id) ? $Reseller->_settings->conference_room_subscriber_profile_id : '',
    '#required' => TRUE,
    '#options' => $subscriber_profile_options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  $options = array(
    'blf' => t('can be configured as a BLF line type'),
    'huntgroup_member' => t('can be a HuntGroup member'),
    'cli' => t('can set the CLI'),
    'sendfax' => t('can send faxes'),
    'mobile_softclient' => t('can use mobile softclient'),
    'desktop_softclient' => t('can use desktop softclient'),
  );

  $profile_allowed_features = $Reseller->_settings->profile_allowed_features;
  
  foreach (sipwise_api_get_all('subscriberprofilesets') as $SubscriberProfileSet) {
    if (!empty($Reseller->_settings->generic_profile_set_id) && $SubscriberProfileSet->id != $Reseller->_settings->generic_profile_set_id) {
      foreach (sipwise_api_get_all('subscriberprofiles', array('profile_set_id' => $SubscriberProfileSet->id)) as $SubscriberProfile) {

        if ($form['general']['#access']) {
          $form['general']['profile_allowed_features']['value'][$SubscriberProfile->id] = array(
            '#type' => 'checkboxes',
            '#options' => $options,
            '#title' => $SubscriberProfileSet->name . ' : ' . $SubscriberProfile->name . ' subscriber',
          );
        }

        foreach (array_keys($options) as $a) {

          // Verify if the user has access to this tab.
          if ($form['general']['#access']) {

            // Render the checkboxes.
            $form['general']['profile_allowed_features']['value'][$SubscriberProfile->id][$a] = array(
              '#field_size_class' => array('col-xs-12', 'col-sm-8', 'col-md-1'),
              '#label_attributes' => array(
                'class' => array(
                  'col-xs-12',
                  'col-sm-3',
                  'col-md-10'
                )
              ),
              '#default_value' => isset($profile_allowed_features->{(string) $SubscriberProfile->id}->$a) && $profile_allowed_features->{(string) $SubscriberProfile->id}->$a
            );
          }
          else {

            // In case the general tab is disabled, render the checkboxes as
            // static value. In this way they will be available within the
            // _validate and _submit handlers.
            $form['general']['profile_allowed_features']['value'][$SubscriberProfile->id][$a] = array(
              '#type' => 'value',
              '#value' => (isset($profile_allowed_features->{(string) $SubscriberProfile->id}->$a) && $profile_allowed_features->{(string) $SubscriberProfile->id}->$a) ? $a : FALSE,
            );
          }
        }
      }
    }
  }

  $form['general']['customer_id_length']['value'] = array(
    '#type' => 'select',
    '#title' => t('customer_id_length'),
    '#default_value' => isset($Reseller->_settings->customer_id_length) ? $Reseller->_settings->customer_id_length : '',
    '#required' => TRUE,
    '#maxlength' => 3,
    '#options' => drupal_map_assoc(range(6, 20)),
    '#empty_value' => '',
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );


  $form['general']['terms_nid']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Terms & Conditions Drupal node ID'),
    '#size' => 5,
    '#maxlength' => 5,
    '#default_value' => isset($Reseller->_settings->terms_nid) ? $Reseller->_settings->terms_nid : '',
    '#attributes' => array('class' => array('sg-element')),
  );

  $form['general']['enable_restricted_access']['value'] = array(
    '#type' => 'select',
    '#title' => t('enable_restricted_access'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->enable_restricted_access) ? $Reseller->_settings->enable_restricted_access : NULL,
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  
  $form['general']['enable_business_partners']['value'] = array(
    '#type' => 'select',
    '#title' => t('Enable Business Partners'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->enable_business_partners) ? $Reseller->_settings->enable_business_partners : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  
  $form['general']['enable_multisite']['value'] = array(
    '#type' => 'select',
    '#title' => t('Enable Multisite'),
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#default_value' => isset($Reseller->_settings->enable_multisite) ? $Reseller->_settings->enable_multisite : '',
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  $form['general']['number_of_sites']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of Sites'),
    '#size' => 5,
    '#maxlength' => 2,
    '#default_value' => isset($Reseller->_settings->number_of_sites) ? $Reseller->_settings->number_of_sites : '',
    '#attributes' => array('class' => array('sg-element')),
  );
   $form['general']['suspend_period']['value'] = array(
    '#type' => 'select',
    '#title' => t('Suspend period'),
    '#default_value' => isset($Reseller->_settings->suspend_period) ? $Reseller->_settings->suspend_period : '',
    '#required' => TRUE,
    '#maxlength' => 1,
    '#options' => drupal_map_assoc(range(0, 7)),
    '#empty_value' => '',
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );
  //Stretto Provisioning
  //Loading Stretto Provisioning
    $stretto_group = array();
    $stretto = new StrettoUser();
    $stretto_connect = $stretto->stretto_connect('user');
  // Checking the stretto conf value.
  if (isset($conf['stretto']) && !empty($conf['stretto']) && $stretto_connect != 0) {
    
    $stretto_group = $stretto->stretto_get_all($table = 'usergroup', $attribute = 'groupName', $vaule = 'top');
    //array_shift($stretto_group);
    natsort($stretto_group);
  }

  $form['general']['stretto_group']['value'] = array(
    '#type' => 'select',
    '#title' => t('Stretto group'),
    '#options' => (isset($stretto_group) && !empty($stretto_group)) ? $stretto_group : array(),
    '#default_value' => isset($Reseller->_settings->stretto_group) ? $Reseller->_settings->stretto_group : NULL,
    '#empty_option' => t('None'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'),
    ),
  );
  
  $form['general']['stretto_desktop_profile']['value'] = array(
    '#type' => 'select',
    '#title' => t('Stretto Desktop profile'),
    '#options' => $stretto_profiles,     
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-placeholder' => t('None'),      
      'data-selected-value' => !empty($Reseller->_settings->stretto_desktop_profile) ? $Reseller->_settings->stretto_desktop_profile : '',
    ),
  );
  
  $form['general']['stretto_mobile_profile']['value'] = array(
    '#type' => 'select',
    '#title' => t('Stretto Mobile profile'),
    '#options' => $stretto_profiles,     
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-placeholder' => t('None'),      
      'data-selected-value' => !empty($Reseller->_settings->stretto_mobile_profile) ? $Reseller->_settings->stretto_mobile_profile : '',
    ),
  );
  
  $form['general']['stretto_mobiledesktop_profile']['value'] = array(
    '#type' => 'select',
    '#title' => t('Stretto Mobile & Desktop profile'),
    '#options' => $stretto_profiles,        
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'), 
      'data-placeholder' => t('None'),     
      'data-selected-value' => !empty($Reseller->_settings->stretto_mobiledesktop_profile) ? $Reseller->_settings->stretto_mobiledesktop_profile : '',
    ),
  );
   
  $form['general']['actions'] = $actions;

  foreach ($form['general'] as $key => &$el) {
    if (!empty($el['value']['#title'])) {
      $el['value']['#title'] = t(ucfirst(str_replace("_", " ", preg_replace('/_(\w)/e', 'strtoupper("$0")', $el['value']['#title']))));
    }
  }
    
  $emergency_indexes = variable_get('hpbx_emergency_indexes_'. $Reseller->id, array());
  if (isset($Reseller->_settings->polish_emergency_routing) && $Reseller->_settings->polish_emergency_routing) {

    $dv = array();
    
    foreach ($emergency_indexes as $region => $townships) {
      foreach ($townships as $township => $cities) {
        foreach ($cities as $city => $z_gms) {
          foreach ($z_gms as $z_gm => $location_id) {
            $row = array($location_id, $city, $z_gm, $township, $region);
            $dv[] = implode(';', $row);
          }
        }
      }
    }
    $form['emergency']['hpbx_emergency_indexes']['value'] = array(
      '#type' => 'textarea',
      '#title' => t('Emergency Routing'),
      '#default_value' => implode(PHP_EOL, $dv),
      '#required' => FALSE,
      '#rows' => 50,
      '#attributes' => array('class' => array('sg-element')),
    );
  }
  else {
    $form['emergency']['hpbx_emergency_index_default']['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Default emergency suffix'),
      '#default_value' => variable_get('hpbx_emergency_indexes_'. $Reseller->id . '_default', ''),
      '#required' => FALSE,
      '#attributes' => array('class' => array('sg-element')),
    );

    $dv = '';

    foreach ($emergency_indexes as $area => $index) {
      $dv .= $area . ','. $index . PHP_EOL;
    }
    $form['emergency']['hpbx_emergency_indexes']['value'] = array(
      '#type' => 'textarea',
      '#title' => t('Emergency Suffixes'),
      '#default_value' => $dv,
      '#required' => FALSE,
      '#rows' => 50,
      '#attributes' => array('class' => array('sg-element')),
    );
  }
  
  if (isset($Reseller->_settings->polish_emergency_routing) && $Reseller->_settings->polish_emergency_routing) {

    // For Polish emergency routing, disable the textarea. Only allow uploads of csv.
    $form['emergency']['hpbx_emergency_indexes']['value']['#attributes']['disabled'] = 'disabled';
    $form['emergency']['hpbx_emergency_indexes_upload']['value'] = array(
      '#type' => 'file',
      '#title' => t('Location CSV'),
      '#default_value' => '',
      '#description' => t('Upload the CSV containing the Location information'),
    );
    $form['emergency']['hpbx_emergency_indexes_download']['value'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/affiliate/'. $Reseller->id .'/emergency_download',
      '#title' => 'Download CSV',
      '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
    );

    $form['emergency']['hpbx_emergency_mappings_upload']['value'] = array(
      '#prefix' => '<br><br><br>',
      '#type' => 'file',
      '#title' => t('Emergency Number Mapping CSV'),
      '#default_value' => '',
      '#attributes' => array('name' => 'mappings'),
      '#description' => t('Upload the CSV containing the Emergency Numbers and Prefix information'),
    );
    $form['emergency']['hpbx_emergency_indexes_purge']['value'] = array(
      '#type' => 'checkbox',
      '#title' => 'Purge existing',
      '#title_display' => 'before',
    );
    $form['emergency']['hpbx_emergency_mappings_download']['value'] = array(
      '#type' => 'link',
      '#href' => 'hpbx/affiliate/'. $Reseller->id .'/mappings_download',
      '#title' => 'Download CSV',
      '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
    );
  }
  
  $form['general']['playstore_url']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Android mobile client URL'),
    '#default_value' => isset($Reseller->_settings->playstore_url) ? $Reseller->_settings->playstore_url : '',
    '#attributes' => array('class' => array('sg-element')),
  );
  
  $form['general']['apple_playstore_url']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('iOS mobile client URL'),
    '#default_value' => isset($Reseller->_settings->apple_playstore_url) ? $Reseller->_settings->apple_playstore_url : '',
    '#attributes' => array('class' => array('sg-element')),
  );

  
  $form['emergency']['actions'] = $actions;
     
  if (!empty($orig_reseller_id) && is_numeric($orig_reseller_id)) {
    sipwise_api_activate_reseller($orig_reseller_id);
  }

  return $form;
}

function hpbx_reseller_edit_form_validate($form, &$form_state) {

  $profile_ids = array();
  foreach (explode(PHP_EOL, trim($form_state['values']['general']['huntgroup_allowed_profile_ids']['value'])) as $profile_id) {
    $profile_id = trim($profile_id);
  
    if ($profile_id!='' && !is_numeric($profile_id)) {
      form_set_error('general][huntgroup_allowed_profile_ids][value', t('Only digits allowed: @profile_id', array('@profile_id' => $profile_id)));
    }
    elseif ($profile_id!='') {
      $profile_ids[] = $profile_id;
    }
  }
  $form_state['values']['general']['huntgroup_allowed_profile_ids']['value'] = $profile_ids;

  $extensions = array();
  foreach (explode(PHP_EOL, trim($form_state['values']['general']['exclude_extensions']['value'])) as $extension) {
    $extension = trim($extension);
    
    if ($extension!='' && !is_numeric($extension)) {
      form_set_error('general][exclude_extensions][value', t('Only digits allowed'));
    }
    elseif ($extension!='') {
      $extensions[] = $extension;
    }
  }
  $form_state['values']['general']['exclude_extensions']['value'] = $extensions;
  
  $extensions = array();
  foreach (explode(PHP_EOL, trim($form_state['values']['general']['extensions_are_not_allowed_to_start_with']['value'])) as $extension) {
    $extension = trim($extension);
    if ($extension!='' && !is_numeric($extension)) {
      form_set_error('general][extensions_are_not_allowed_to_start_with][value', t('Only digits allowed'));
    }
    elseif ($extension!='') {
      $extensions[] = $extension;
    }
  }
  $form_state['values']['general']['extensions_are_not_allowed_to_start_with']['value'] = $extensions;

  
  $max_totals = array();
  foreach (explode(PHP_EOL, trim($form_state['values']['general']['customer_concurrent_max']['value'])) as $max_total) {
    $max_total = trim($max_total);
    
    
    if ($max_total!='' && !is_numeric($max_total)) {
      form_set_error('general][customer_concurrent_max][value', t('Only digits allowed') . $max_total);
    }
    elseif ($max_total!='') {
      $max_totals[] = $max_total;
    }
  }
  $form_state['values']['general']['customer_concurrent_max']['value'] = $max_totals;

  // Correct the state of checkboxes for profile based features.
  foreach ($form_state['values']['general']['profile_allowed_features']['value'] as $profile_id => &$profile_features) {
    foreach ($profile_features as $profile_feature => &$value) {
      $value = ($profile_feature === $value);
    }
  }

  foreach ($form_state['values']['general'] as $key => $element) {
    if (!in_array($key, array('domain_id','huntgroup_allowed_profile_ids', 'profile_allowed_features', 'timezone', 'customer_concurrent_max', 'suspend_ncos_level', 'default_subscriber_ncos_level', 'mail_from_address', 'exclude_extensions', 'extensions_are_not_allowed_to_start_with', 'stretto_group', 'stretto_desktop_profile', 'stretto_mobile_profile', 'stretto_mobiledesktop_profile', 'playstore_url', 'apple_playstore_url')) && $element['value']!='' && !is_numeric($element['value'])) {
      form_set_error('general]['. $key .'][value', t('Only digits allowed for field @field', array('@field' => $key)));
    }
  }

  if ($form_state['values']['general']['customer_max_extension_length']['value'] >= $form_state['values']['general']['subscriber_number_min_digits']['value']) {
    form_set_error('general][customer_max_extension_length][value', t('Customer max extension length must be less than subscriber number min digits.'));
  }

  sipwise_api_activate_reseller($form_state['values']['id']);
  
  // Load reseller.
  $Reseller = SipwiseEntity::load('resellers', $form_state['values']['id']);
  
  // Emergency routing
  if (isset($Reseller->_settings->polish_emergency_routing) && $Reseller->_settings->polish_emergency_routing) {

    if (!empty($_FILES['files']['name']['emergency'])) {

      // Verify if any error occured while uploading the csv file.
      if ($_FILES['files']['error']['emergency'] !== UPLOAD_ERR_OK) {
        form_set_error('emergency][hpbx_emergency_indexes_upload', t('Unknown error while uploading the polish emergency indexes CSV file'));
      }

      // Verify if the uploaded file does exist.
      if (!file_exists($_FILES['files']['tmp_name']['emergency'])) {
        form_set_error('emergency][hpbx_emergency_indexes_upload', t('The uploaded indexes file does not exist (anymore)'));
      }

      if (!($handle = fopen($_FILES['files']['tmp_name']['emergency'], 'r'))) {
        form_set_error('emergency][hpbx_emergency_indexes_upload', t('Could not open uploaded indexes file'));
      }
    }

    if (!empty($_FILES['mappings']['name'])) {

      // Verify if any error occured while uploading the csv file.
      if ($_FILES['mappings']['error'] !== UPLOAD_ERR_OK) {
        form_set_error('emergency][hpbx_emergency_mappings_upload', t('Unknown error while uploading the mappings file'));
      }

      // Verify if the uploaded file does exist.
      if (!file_exists($_FILES['mappings']['tmp_name'])) {
        form_set_error('emergency][hpbx_emergency_mappings_upload', t('The uploaded mappings file does not exist (anymore)'));
      }

      if (!($handle = fopen($_FILES['mappings']['tmp_name'], 'r'))) {
        form_set_error('emergency][hpbx_emergency_mappings_upload', t('Could not open uploaded mappings file'));
      }
    }
  }
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function hpbx_reseller_edit_form_submit($form, $form_state) {

  $orig_reseller_id = sipwise_api_get_active_reseller();
  
  sipwise_api_activate_reseller($form_state['values']['id']);
  
  // Load reseller.
  $Reseller = SipwiseEntity::load('resellers', $form_state['values']['id']);

  // Initialize settings.
  $settings = array();
  
  $settings['logo_path'] = $Reseller->_settings->logo_path;
  
  // Push all elements to the settings array.
  foreach ($form_state['values']['general'] as $key => $field) {
    $settings[$key] = $field['value'];
  }

  
  // Push all elements to the settings array.
  foreach ($form_state['values']['dashboard'] as $key => $field) {
    $settings[$key] = $field['value'];
  }
  
  // Push all elements to the settings array.
  foreach ($form_state['values']['terms'] as $key => $field) {
    $settings[$key] = $field['value'];
  }
  
  // Store mail footer.
  $settings['mail_footer'] = $form_state['values']['mail']['general']['mail_footer']['value'];

  // Save logo to filesystem (sites/default/files) and set to reseller settings.
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
  
  // Set the settings.
  $Reseller->_settings = $settings;
  
  // Save the Reseller.
  $Reseller->save();

  // Emergency routing
  if (isset($Reseller->_settings['polish_emergency_routing']) && $Reseller->_settings['polish_emergency_routing']) {

    if (!empty($_FILES['mappings']['tmp_name']) && $contents = file_get_contents($_FILES['mappings']['tmp_name'])) {
      $query = array();
      $query['reseller_id'] = $form_state['values']['id'];
      $query['purge_existing'] = $form_state['values']['emergency']['hpbx_emergency_indexes_purge'] ? 'true' : 'false';
      Sipwise::exec('/api/emergencymappings/?' . drupal_http_build_query($query), $contents, 'POST', array('Content-Type: text/csv', 'Expect:'));
    }

    if (!empty($_FILES['files']['tmp_name']['emergency']) && $contents = file_get_contents($_FILES['files']['tmp_name']['emergency'])) {

      $data = preg_split('/\n|\r\n|\r/', $contents);

      $emergency_indexes = array();
      foreach ($data as $line) {
        $values = array_map('trim', preg_split('/\,|\;/', $line));
        $emergency_indexes[$values[4]][$values[3]][$values[1]][$values[2]] = $values[0];
      }


      // Delete all entries for this reseller.
      db_delete('hpbx_pl_emergency_indexes')->condition('reseller_id', $Reseller->id)->execute();

      // Insert in db.
      $query = db_insert('hpbx_pl_emergency_indexes');
      $query->fields(array('reseller_id', 'region', 'township', 'city', 'zgm', 'location_id'));
      foreach ($emergency_indexes as $region => $townships) {
        foreach ($townships as $township => $cities) {
          foreach ($cities as $city => $z_gms) {
            foreach ($z_gms as $z_gm => $location_id) {

              if (!empty($location_id)) {
                $entry = array();
                $entry['reseller_id'] = $Reseller->id;
                $entry['region'] = $region;
                $entry['township'] = $township;
                $entry['city'] = $city;
                $entry['zgm'] = $z_gm;
                $entry['location_id'] = $location_id;
                $query->values($entry);
              }
            }
          }
        }
      }

      try {
        $query->execute();
      }
      catch (Exception $e) {
        var_dump($e->getMessage());
        die();
      }
      variable_set('hpbx_emergency_indexes_' . $Reseller->id, $emergency_indexes);
    }
  }
  else {
    // Non Polish.
    $emergency_indexes = array();
    foreach (explode(PHP_EOL, $form_state['values']['emergency']['hpbx_emergency_indexes']['value']) as $entry) {
      list($area, $index) = explode(",", $entry);

      if (!empty($area) && !empty($index)) {
        $emergency_indexes[trim($area)] = trim($index);
      }
    }
    variable_set('hpbx_emergency_indexes_'. $Reseller->id, $emergency_indexes);
  }

  variable_set('hpbx_emergency_indexes_'. $Reseller->id . '_default', $form_state['values']['emergency']['hpbx_emergency_index_default']['value']);
  
  // Clear the Drupal cache for this API type.
  $cid = 'resellers_0_'. $Reseller->id .'_response';
  cache_clear_all($cid, 'cache_sipwise_api');
  watchdog('sipwise_api_cache', t('Cleared resellers with @cid', array('@cid' => $cid)));

  $cid = 'resellers_list_';
  cache_clear_all($cid, 'cache_sipwise_api', TRUE);
  watchdog('sipwise_api_cache', t('Cleared resellers_list with @cid*', array('@cid' => $cid)));

  $cid = 'contracts_'. $Reseller->id .'_'. $Reseller->contract_id . '_response';
  cache_clear_all($cid, 'cache_sipwise_api');
  watchdog('sipwise_api_cache', t('Cleared contracts with @cid', array('@cid' => $cid)));

  if ($Contract = SipwiseEntity::load('contracts', $Reseller->contract_id)) {
    $cid = 'systemcontacts_'. $Reseller->id .'_'. $Contract->contact_id . '_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared systemcontacts with @cid', array('@cid' => $cid)));
  }

  foreach (array('affiliate_user_welcome', 'customer_administrator_welcome', 'customer_subscriber_welcome', 'reset_password','bp_administrator_welcome') as $mail_key) {
    $mail_key .= '_'. $form_state['values']['id'];
    variable_set('user_mail_'. $mail_key .'_subject', $form_state['values']['mail']['user_mail_'. $mail_key]['subject']['value']);
    variable_set('user_mail_'. $mail_key .'_body', $form_state['values']['mail']['user_mail_'. $mail_key]['body']['value']);
  }

  
  sipwise_api_activate_reseller($orig_reseller_id);
  
  
  
  drupal_set_message(t('Affiliate settings successfully saved'));

  if (user_access('hpbx view reseller')) {
    drupal_goto('hpbx/affiliates');
  }
  elseif(user_access('hpbx view customer')) {
    drupal_goto('hpbx/customers');
  }
}

