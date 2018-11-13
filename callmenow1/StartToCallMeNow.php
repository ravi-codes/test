<?php

/**
 * CallMeNow
 * 
 * Using a HTTP Post method a customer can send 2, 32 char identifiers.
 * First identifier is a unique customer key.
 * Second key is a destination identifier.
 * 
 * Both keys are used to authenticate the request.
 * Using the destination hash the telephone number of agent is determained.
 * 
 * Also in the post a field holiding the number to dial for the webuser (customer of our customer)
 * Error Codes:
 * 
 * 100, Invalid Customer Key
 * 200, Invalid Destination Key
 * 300, Invalid DialedNumber
 * 400, Dial timeout exceeded
 * 
 * @author mzwerver
 * @copyright 2011
 */

//Include class CallMeNow
include './class.CallMeNow.php';

//Check all fields for validation
session_start();

//POST values -> 
	// cmnuck
	// cmndid
	// MyNumber
if(isset($_POST['cmnuck']) && isset($_POST['cmndid']) && isset($_POST['MyNumber'])) {
	
	// Create object to CMN class
	$cmn = new CallMeNow();
	
	// set database settings
	$cmn->ServerName = 'algra.a.local';
	$cmn->UserName = 'webuser';
	$cmn->UserPassword = 'webuser';
	$cmn->Database = 'web';
	
	//Set default status for redirect options
	$dialout = false;
	
	//Validate MyNumber &  Customer Key & DestintationID
	if($cmn->ValidateUserDestination($_POST['MyNumber']) && 
		$cmn->ValidateCustomerKey($_POST['cmnuck']) && 
		$cmn->ValidateDestinationKey($_POST['cmnuck'], $_POST['cmndid'] )) {
			
		//Check number of calls by this user/ip/session
		if(isset($_SESSION['cmndialtime']))	{
			if($cmn->CheckDialHistory($_SESSION['cmndialtime'],15)) {
				//Valid create call file
				$_SESSION['cmndialtime'] = time();
				$cmn->CreateCallFileContent();
				$dialout = true;
			}
		}
		else
		{
			//Valid create call file
			$_SESSION['cmndialtime'] = time();
			$cmn->CreateCallFileContent();
			$dialout = true;
		} 
	}
}

//Determine the redirect location
//When in db then db location other wise the referer page
if($dialout) {
	$resultData = array ('result'=>'true');
	$redirectLocation = $cmn->RedirectURLOnSuccess;
}
else {
	$resultData = array('result'=>'false',
				'errorcode'=>$cmn->ErrorCode,
				'msg'=>$cmn->ErrorMsg);
	$redirectLocation = $cmn->RedirectURLOnFail;
}
	
if(strlen($redirectLocation) < 1)
	$redirectLocation = $_SERVER['HTTP_REFERER']; //back to customer

// Location?result=ok
// Location?result=error&code=100&msg="Invalid Telephone number"
$Http_query = http_build_query($resultData);

//Check if there is a ? in the query of referer or in database
if(strpos($redirectLocation,"?"))
	header("Location: $redirectLocation&$Http_query");	//Add query paramters to existing parameters
else
	header("Location: $redirectLocation?$Http_query");	//Create new query paramters
?>