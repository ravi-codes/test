<?php

function _hpbx_customer_definition() {
	$definition = array(
    'customer' => array(
      'actions' => array(
        'login' => array(
          'help' => 'Login as an user for a new session',
          'callback' => '_customer_resource_login',
          'args' => array(
			array(
              'name' => 'username',
              'type' => 'string',
              'description' => 'A valid username',
              'source' => array('data' => 'username'),
              'optional' => FALSE,
			),
			array(
              'name' => 'password',
              'type' => 'string',
              'description' => 'A valid password',
              'source' => array('data' => 'password'),
              'optional' => FALSE,
			),
		),
          'access callback' => 'services_access_menu',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),
		),
        'create' => array(
          'help' => 'Create a hpbx customer',
          'callback' => '_hpbx_customer_create',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Create HBPX customer',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
		'update' => array(
          'help' => 'Update a hpbx customer',
          'callback' => '_hpbx_customer_update',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Update a HBPX customer',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
        'delete' => array(
          'help' => 'Terminate a hpbx customer',
          'callback' => '_hpbx_customer_terminate',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Terminate a HBPX customer',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
		'suspend' => array(
          'help' => 'Suspend a hpbx customer',
          'callback' => '_hpbx_customer_suspend',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Suspend a HBPX customer',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
		'unsuspend' => array(
          'help' => 'Unsuspend a hpbx customer',
          'callback' => '_hpbx_customer_unsuspend',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Unsuspend a HBPX customer',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
        'numberadd' => array(
          'help' => 'add number range to hpbx customer',
          'callback' => '_hpbx_number_range_add',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Add Number Range',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
        'numberupdate' => array(
          'help' => 'add number range to hpbx customer',
          'callback' => '_hpbx_number_range_update',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'Update Number Range',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		),
		'numberrangedelete' => array(
          'help' => 'number range delete',
          'callback' => '_hpbx_customer_numberrangedelete',
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/hpbx_customer_service'),          
          'args' => array(
			array(
              'name' => 'customerDetails',
              'type' => 'array',
              'description' => 'number range delete',
              'source' => 'data',
              'optional' => FALSE,
			),
			)
		)
	)
	)
	);

	return $definition;
}

/**
 * Validate roles and permission of logged in user.
 *
 */
function _hpbx_validate_logged_user($uid,$permission,$permission2) {
	$account = user_load($uid);
	if (user_access($permission,$account)) {
		return TRUE;
	}else if(user_access($permission2,$account)){
		return TRUE;
	}else{
		return FALSE;
	}
}

/**
 * Create a new customer.
 *
 */
function _hpbx_customer_create($customerDetails) {
	global $user;

	//Fetch customer related information.
	$resellerId=$customerDetails['resellerId'];
	$customerId = $customerDetails['customer']['customerId'];
	$companyName = $customerDetails['customer']['companyName'];
	$pilotNumber = $customerDetails['customer']['pilotNumber'];
	$deriveExtensionFromDID = $customerDetails['customer']['deriveExtensionFromDID'];
	$allowSubscribersWithoutDID = $customerDetails['customer']['allowSubscribersWithoutDID'];
	$extensionLength = $customerDetails['customer']['extensionLength'];

	//Fetch customer administration information.
	$firstName = $customerDetails['customerAdministrator']['firstName'];
	$lastName = $customerDetails['customerAdministrator']['lastName'];
	$email = $customerDetails['customerAdministrator']['email'];

	//Fetch customer location information.
	$region = $customerDetails['customerLocation']['region'];
	$township = $customerDetails['customerLocation']['township'];
	$city = $customerDetails['customerLocation']['city'];
	$zgm = $customerDetails['customerLocation']['zgm'];

	$valErrors=array();
	$genericErrors= array();

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($customerId) || $customerId==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}

	if (!_hpbx_validate_logged_user($user->uid,'hpbx add reseller customer','hpbx add customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	//Validate the input parameter field size against the sipwise length allowed.
	if(isset($companyName) && strlen($companyName) > 255){
		$error=array('reasonCode'=>'1012','reasonDescription'=>'companyName Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($pilotNumber) && strlen($pilotNumber) > 30){
		$error=array('reasonCode'=>'1013','reasonDescription'=>'pilotNumber Fieldsize is large.');
		$genericErrors[]=$error;
	}
	if(isset($deriveExtensionFromDID) && strlen($deriveExtensionFromDID) > 3){
		$error=array('reasonCode'=>'1014','reasonDescription'=>'deriveExtensionFromDID Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($allowSubscribersWithoutDID) && strlen($allowSubscribersWithoutDID) > 3){
		$error=array('reasonCode'=>'1015','reasonDescription'=>'allowSubscribersWithoutDID Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($extensionLength) && strlen($extensionLength) > 1){
		$error=array('reasonCode'=>'1016','reasonDescription'=>'extensionLength Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($firstName) && strlen($firstName) > 40){
		$error=array('reasonCode'=>'1017','reasonDescription'=>'firstName Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($lastName) && strlen($lastName) > 40){
		$error=array('reasonCode'=>'1018','reasonDescription'=>'lastName Fieldsize is large.');
		$genericErrors[]=$error;
	}
	if(isset($email) && strlen($email) > 128){
		$error=array('reasonCode'=>'1019','reasonDescription'=>'email Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($region) && strlen($region) > 255){
		$error=array('reasonCode'=>'1020','reasonDescription'=>'region Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($township) && strlen($township) > 255){
		$error=array('reasonCode'=>'1021','reasonDescription'=>'township Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($city) && strlen($city) > 255){
		$error=array('reasonCode'=>'1022','reasonDescription'=>'city Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($zgm) && strlen($zgm) > 32){
		$error=array('reasonCode'=>'1023','reasonDescription'=>'zgm Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if (isset($customerDetails['bundleSettings'])) {
		foreach($customerDetails['bundleSettings'] as $bundleSettings)
		{
			foreach($bundleSettings as $profileSets)
			{
				$profileSetLimitAmount = $profileSets['profileSetLimitAmount'];
				$profileLimits = $profileSets['profileLimits'];

				if(isset($profileSetLimitAmount) && strlen($profileSetLimitAmount) > 128){
					$error=array('reasonCode'=>'1024','reasonDescription'=>'profileSetLimitAmount Fieldsize is large.');
					$genericErrors[]=$error;
				}
					
				if(isset($profileLimits) && !empty($profileLimits)){
					foreach($profileLimits as $profileDetails){
						if(strlen($profileDetails['profileLimitAmount']) > 128){
							$error=array('reasonCode'=>'1025','reasonDescription'=>'profileLimitAmount Fieldsize is large.');
							$genericErrors[]=$error;
						}
					}
				}
			}
		}
	}

	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_create_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Update a customer.
 *
 */
function _hpbx_customer_update($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$customerId = $customerDetails['customerId'];

	//Fetch customer related information.
	$companyName = $customerDetails['customer']['companyName'];
	$customerIdNew = $customerDetails['customer']['customerIdNew'];
	$pilotNumber = $customerDetails['customer']['pilotNumber'];
	$maximumTotalConcurrentCalls = $customerDetails['customer']['maximumTotalConcurrentCalls'];
	$deriveExtensionFromDID = $customerDetails['customer']['deriveExtensionFromDID'];
	$allowSubscribersWithoutDID = $customerDetails['customer']['allowSubscribersWithoutDID'];
	$extensionLength = $customerDetails['customer']['extensionLength'];

	//Fetch customer location related information.
	$region = $customerDetails['customerLocation']['region'];
	$township = $customerDetails['customerLocation']['township'];
	$city = $customerDetails['customerLocation']['city'];
	$zgm = $customerDetails['customerLocation']['zgm'];

	$valErrors=array();
	$genericErrors= array();

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($customerId) || $customerId==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}

	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	//Validate the input parameter field size against the sipwise length allowed.
	if(isset($companyName) && strlen($companyName) > 255){
		$error=array('reasonCode'=>'1012','reasonDescription'=>'companyName Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($pilotNumber) && strlen($pilotNumber) > 30){
		$error=array('reasonCode'=>'1013','reasonDescription'=>'pilotNumber Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($deriveExtensionFromDID) && strlen($deriveExtensionFromDID) > 3){
		$error=array('reasonCode'=>'1014','reasonDescription'=>'deriveExtensionFromDID Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($allowSubscribersWithoutDID) && strlen($allowSubscribersWithoutDID) > 3){
		$error=array('reasonCode'=>'1015','reasonDescription'=>'allowSubscribersWithoutDID Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($extensionLength) && strlen($extensionLength) > 1){
		$error=array('reasonCode'=>'1016','reasonDescription'=>'extensionLength Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($region) && strlen($region) > 255){
		$error=array('reasonCode'=>'1020','reasonDescription'=>'region Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($township) && strlen($township) > 255){
		$error=array('reasonCode'=>'1021','reasonDescription'=>'township Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($city) && strlen($city) > 255){
		$error=array('reasonCode'=>'1022','reasonDescription'=>'city Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if(isset($zgm) && strlen($zgm) > 32){
		$error=array('reasonCode'=>'1023','reasonDescription'=>'zgm Fieldsize is large.');
		$genericErrors[]=$error;
	}

	if (isset($customerDetails['bundleSettings'])) {
		foreach($customerDetails['bundleSettings'] as $bundleSettings)
		{
			foreach($bundleSettings as $profileSets)
			{
				$profileSetLimitAmount = $profileSets['profileSetLimitAmount'];
				$profileLimits = $profileSets['profileLimits'];

				if(isset($profileSetLimitAmount) && strlen($profileSetLimitAmount) > 128){
					$error=array('reasonCode'=>'1024','reasonDescription'=>'profileSetLimitAmount Fieldsize is large.');
					$genericErrors[]=$error;
				}
				if(isset($profileLimits) && !empty($profileLimits)){
					foreach($profileLimits as $profileDetails){
						if(strlen($profileDetails['profileLimitAmount']) > 128){
							$error=array('reasonCode'=>'1025','reasonDescription'=>'profileLimitAmount Fieldsize is large.');
							$genericErrors[]=$error;
						}
					}
				}
			}
		}
	}
	//print_r($genericErrors); die;
	if(empty($genericErrors) && empty($valErrors))
	{
		return hpbx_customer_update_service($customerDetails);
	}
	else
	{
		$errors=array("result"=>"Failure","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Terminate a customer.
 *
 */
function _hpbx_customer_terminate($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$external_id=$customerDetails['customerId'];
	$valErrors=array();
	$genericErrors= array();

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($external_id) || $external_id==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}

	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_terminate_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Suspend a customer.
 *
 */
function _hpbx_customer_suspend($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$external_id=$customerDetails['customerId'];
	$valErrors=array();
	$genericErrors= array();

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($external_id) || $external_id==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}

	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_suspend_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Unsuspend a customer.
 *
 */
function _hpbx_customer_unsuspend($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$external_id=$customerDetails['customerId'];
	$valErrors=array();
	$genericErrors= array();

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($external_id) || $external_id==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}

	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_unsuspend_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Add number range against a customer.
 *
 */
function _hpbx_number_range_add($customerDetails){
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$customerId=$customerDetails['customerId'];

	$valErrors=array();
	$genericErrors= array();
	if (!_hpbx_validate_logged_user($user->uid,'hpbx add reseller customer','hpbx add customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($customerId) || $customerId==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}
	foreach($customerDetails['numberRange'] as $custDetails){
		$ported=$custDetails['portedOrNewNumberFlag'];
		$areaCode=$custDetails['areaCode'];
		$startNumber=$custDetails['startNumber'];
		$length=$custDetails['length'];
		$prefix = $custDetails['prefix'];

		if(!isset($ported) || $ported==''){
			$error=array('reasonCode'=>'1026','reasonDescription'=>'Please provide the ported or new number flag');
			$genericErrors[]=$error;
		}
		if(!isset($areaCode) || $areaCode==''){
			$error=array('reasonCode'=>'1027','reasonDescription'=>'Please provide the areaCode');
			$genericErrors[]=$error;
		}
		if(!isset($startNumber) || $startNumber ==''){
			$error=array('reasonCode'=>'1028','reasonDescription'=>'Please provide the startNumber');
			$genericErrors[]=$error;
		}
		if(!isset($length) || $length==''){
			$error=array('reasonCode'=>'1029','reasonDescription'=>'Please provide the length');
			$genericErrors[]=$error;
		}
	}
	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_number_range_add_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}


/**
 * Update number range against a customer.
 *
 */
function _hpbx_number_range_update($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$customerId=$customerDetails['customerId'];

	$valErrors=array();
	$genericErrors= array();
	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}

	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}

	if(!isset($customerId) || $customerId==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}
	foreach($customerDetails['numberRange'] as $custDetails){
		$ported=$custDetails['portedOrNewNumberFlag'];
		$areaCode=$custDetails['areaCode'];
		$startNumber=$custDetails['startNumber'];
		$length=$custDetails['length'];
		$prefix = $custDetails['prefix'];

		if(!isset($areaCode) || $areaCode==''){
			$error=array('reasonCode'=>'1027','reasonDescription'=>'Please provide the areaCode');
			$genericErrors[]=$error;
		}
		if(!isset($startNumber) || $startNumber ==''){
			$error=array('reasonCode'=>'1028','reasonDescription'=>'Please provide the startNumber');
			$genericErrors[]=$error;
		}
		if((!isset($ported) || $ported =='')&&(!isset($prefix) || $prefix =='')){
			$error=array('reasonCode'=>'0048','reasonDescription'=>'One of the fields portedOrNewNumberFlag or prefix must have a value');
			$valErrors[]=$error;
		}
	}
	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_number_range_update_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Number range delete.
 *
 */
function _hpbx_customer_numberrangedelete($customerDetails) {
	global $user;
	$resellerId=$customerDetails['resellerId'];
	$external_id=$customerDetails['customerId'];

	$valErrors=array();
	$genericErrors= array();
	if (!_hpbx_validate_logged_user($user->uid,'hpbx edit reseller customer','hpbx edit customer')) {
		$error=array('reasonCode'=>'0054','reasonDescription'=>'Logged in user is not authorized for this Reseller.');
		$valErrors[]=$error;
	}
	if(!isset($resellerId) || $resellerId==''){
		$error=array('reasonCode'=>'1001','reasonDescription'=>'Please provide the resellerId');
		$genericErrors[]=$error;
	}
	if(!isset($external_id) || $external_id==''){
		$error=array('reasonCode'=>'1002','reasonDescription'=>'Please provide the customerId');
		$genericErrors[]=$error;
	}
	foreach($customerDetails['numberRange'] as $custDetails)
	{
		$areaCode=$custDetails['areaCode'];
		$startNumber=$custDetails['startNumber'];
			
		if(!isset($areaCode) || $areaCode==''){
			$error=array('reasonCode'=>'1037','reasonDescription'=>'Please provide the areaCode');
			$genericErrors[]=$error;
		}
		if(!isset($startNumber) || $startNumber ==''){
			$error=array('reasonCode'=>'1038','reasonDescription'=>'Please provide the startNumber');
			$genericErrors[]=$error;
		}
	}
	if(empty($genericErrors) && empty($valErrors)){
		return hpbx_customer_numberrange_delete_service($customerDetails);
	}else{
		$errors=array("result"=>"NOK","validationErrors"=>$valErrors,"genericErrors"=>$genericErrors);
		return $errors;
	}
}

/**
 * Login a user using the specified credentials.
 *
 * Note this will transfer a plaintext password.
 *
 * @param $username
 *   Username to be logged in.
 * @param $password
 *   Password, must be plain text and not hashed.
 *
 * @return
 *   A valid session object.
 */
function _customer_resource_login($username, $password) {
	global $user;

	if ($user->uid) {
		// user is already logged in
		$errorDescription = 'Already logged in as ' .$user->name;
		$error = array("result"=>'NOK','reasonCode'=>'406','reasonDescription'=>$errorDescription);
		return $error;
	}

	// Check if account is active.
	if (user_is_blocked($username)) {
		$errorDescription = 'The username %name has not been activated or is blocked.' .$username;
		$error = array("result"=>'NOK','reasonCode'=>'403','reasonDescription'=>$errorDescription);
		return $error;
	}

	// Emulate drupal native flood control: check for flood condition.
	$flood_state = array();
	if (variable_get('services_flood_control_enabled', TRUE)) {
		$flood_state = _customer_resource_flood_control_precheck($username);
	}

	// Only authenticate if a flood condition was not detected.
	if (empty($flood_state['flood_control_triggered'])) {
		$uid = user_authenticate($username, $password);
	}
	else {
		$uid = FALSE;
	}

	if($uid==''){
		try{
			if(filter_var($username, FILTER_VALIDATE_EMAIL)){
				$username = getUserName($username);
			}
			if($username){
				$userDetails=array("values"=>array("name"=>$username,"pass"=>$password));
				$returnUser=TRUE;
				$ldapLogin =_ldap_authentication_user_login_authenticate_validate($userDetails,$returnUser);
				$uid = $ldapLogin->uid;
			}else{
				$error = array("result"=>'NOK','reasonCode'=>'401','reasonDescription'=>'Invalid login attempt, please re-check username or password.');
				return $error;
			}
		}catch(Exception $e){
			$error = array("result"=>'NOK','reasonCode'=>'401','reasonDescription'=>'Invalid login attempt, please re-check username or password.');
			return $error;
		}
	}

	// Emulate drupal native flood control: register flood event, and throw error
	// if a flood condition was previously detected
	if (variable_get('services_flood_control_enabled', TRUE)) {
		$flood_state['uid'] = $uid;
		_customer_resource_flood_control_postcheck($flood_state);
	}

	if ($uid) {
		$user = user_load($uid);
		if ($user->uid) {
			user_login_finalize();

			$return = new stdClass();
			$return->sessid = session_id();
			$return->session_name = session_name();
			$return->token = drupal_get_token('services');

			$account = clone $user;

			services_remove_user_data($account);

			$return->user = $account;

			return $return;
		}
	}
	$error = array("result"=>'NOK','reasonCode'=>'401','reasonDescription'=>'Invalid login attempt, please re-check username or password.');
	return $error;
}

/**
 * Emulate native Drupal flood control, phase 1.
 *
 * This function checks for a flood condition, and determines the identifier
 * for user based flood checks. This is done prior to user authentication.
 *
 * @param string $username
 *   The name of the user who is attempting to log in.
 * @return array
 *   An array containing zero or more of the following keys:
 *   - flood_control_triggered: either 'user' or 'ip' if a flood condition
 *     was detected.
 *   - flood_control_user_identifier: the identifier to use to register
 *     user-based flood events.
 *
 * @see _customer_resource_flood_control_postcheck().
 * @see user_login_authenticate_validate().
 */
function _customer_resource_flood_control_precheck($username) {
	$flood_state = array();
	// Do not allow any login from the current user's IP if the limit has been
	// reached. Default is 50 failed attempts allowed in one hour. This is
	// independent of the per-user limit to catch attempts from one IP to log
	// in to many different user accounts.  We have a reasonably high limit
	// since there may be only one apparent IP for all users at an institution.
	if (!flood_is_allowed('failed_login_attempt_ip', variable_get('user_failed_login_ip_limit', 50), variable_get('user_failed_login_ip_window', 3600))) {
		$flood_state['flood_control_triggered'] = 'ip';
	}
	else {
		$account = db_query("SELECT * FROM {users} WHERE name = :name AND status = 1", array(':name' => $username))->fetchObject();
		if ($account) {
			if (variable_get('user_failed_login_identifier_uid_only', FALSE)) {
				// Register flood events based on the uid only, so they apply for any
				// IP address. This is the most secure option.
				$identifier = $account->uid;
			}
			else {
				// The default identifier is a combination of uid and IP address. This
				// is less secure but more resistant to denial-of-service attacks that
				// could lock out all users with public user names.
				$identifier = $account->uid . '-' . ip_address();
			}
			$flood_state['flood_control_user_identifier'] = $identifier;

			// Don't allow login if the limit for this user has been reached.
			// Default is to allow 5 failed attempts every 6 hours.
			if (!flood_is_allowed('failed_login_attempt_user', variable_get('user_failed_login_user_limit', 5), variable_get('user_failed_login_user_window', 21600), $identifier)) {
				$flood_state['flood_control_triggered'] = 'user';
			}
		}
	}
	return $flood_state;
}

/**
 + * Emulate native Drupal flood control, phase 2.
 + *
 + * This function records a failed login attempt, and triggers an error if a
 + * flood condition was previously detected.
 + *
 + * @param array $flood_state
 + *   An array of flood information as returned by
 + *   _customer_resource_flood_control_precheck().
 + *
 + * @throws ServicesException
 + *   If a flood condition was previously detected.
 + *
 + * @see _customer_resource_flood_control_precheck().
 + * @see user_login_final_validate().
 + */
function _customer_resource_flood_control_postcheck($flood_state) {
	if (empty($flood_state['uid'])) {
		// Always register an IP-based failed login event.
		flood_register_event('failed_login_attempt_ip', variable_get('user_failed_login_ip_window', 3600));
		// Register a per-user failed login event.
		if (isset($flood_state['flood_control_user_identifier'])) {
			flood_register_event('failed_login_attempt_user', variable_get('user_failed_login_user_window', 21600), $flood_state['flood_control_user_identifier']);
		}
		if (isset($flood_state['flood_control_triggered'])) {
			if ($flood_state['flood_control_triggered'] == 'user') {
				services_error(t('Account is temporarily blocked.'), 406);
			}
			else {
				// We did not find a uid, so the limit is IP-based.
				services_error(t('This IP address is temporarily blocked.'), 406);
			}
		}
	}
	elseif (isset($flood_state['flood_control_user_identifier'])) {
		// Clear past failures for this user so as not to block a user who might
		// log in and out more than once in an hour.
		flood_clear_event('failed_login_attempt_user', $flood_state['flood_control_user_identifier']);
	}
}

function getUserName($username){
	$name = db_query("SELECT count(*) as count ,name FROM {users} WHERE mail = :mail AND status = 1", array(':mail' => $username))->fetchObject();
	if($name->count=1){
		return $name->name;
	}else{
		return False;
	}	
}

