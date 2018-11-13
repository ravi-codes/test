<?php

include './classDatabase.php';

/**
 * CallMeNow
 * 
 * Class for checking authenticity and creating a callfile.
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
 * 300, Invalid Telephonenumber
 * 400, Accessive dialout attempt
 * 
 * 
 * @author mzwerver
 *
 */
class CallMeNow extends Database{

        //Session variables
        private $customerKey;
        private $destinationKey;
        private $numberToDial;
        private $logToScreen = true;	//Set to false if no debug messages should be presented.
        private $logLevel = 3;			//Level of logs to send to logfile /var/log/CallMeNow.log
        
        //database settings
        //public $ServerName
        //public $UserName
       	//public $UserPassword
       	//public $Database
       	
       	//public $ErrorCode
       	//public $ErrorMsg
       	     	       	
        /**
         * Set values
         * @param unknown_type $name
         * @param unknown_type $value
         */
        public function __set($name, $value) {
                $this->$name = $value;
        }

        /**
         * Get Values 
         * @param unknown_type $name
         */
        public function __get($name) {
                return $this->$name;
        }
        
        
        /**
         * Validates the received Customer Key against the database
         * 
         * @param unknown_type $CallMeNowIDHash - 32 char string for ID purposes
         * 
         * @return 	true when validation is successfull;
		 * 			false when validation is NOT successfull;
		 * 			Sets the ErroCode member with the reson code.
		 * 
         */
        public function ValidateCustomerKey($CallMeNowIDHash) {
                // Check Database if hash exists
                // Get key (cmnuck) from HTTP POST and check with Database
                // SELECT CustomerDefinitionID, RedirectURLOnSuccess, RedirectURLOnFail FROM CallMeNowSettings WHERE CallMeNowIDHash = '<key>'
            	$this->SetPrimaryServerSettings($this->ServerName, $this->UserName, $this->UserPassword);
				$this->SetSecondaryServerSettings($this->ServerName, $this->UserName, $this->UserPassword);    
        	
            	if(!isset($CallMeNowIDHash))  
            		$CallMeNowIDHash = $this->customerKey;  
        	
                $retVal = false;
                $resultSet = '';

                $SQLQuery = "SELECT CustomerDefinitionID, RedirectURLOnSuccess, RedirectURLOnFail FROM CallMeNowSettings WHERE CallMeNowIDHash = '".$CallMeNowIDHash."'";
                
                $this->Log(9,$SQLQuery);	//Log to Screen
				
				$this->SetDatabase($this->Database);
                if($this->GoSQL($SQLQuery, &$resultSet)) {
                	
                        if(mssql_num_rows($resultSet) == 1)     {
                                // Found a record, set variabels and proceed.
                                $recordData = mssql_fetch_assoc($resultSet);
                                $this->__set('CustomerDefinitionID',$recordData['CustomerDefinitionID'] );
                                $this->__set('RedirectURLOnSuccess', $recordData['RedirectURLOnSuccess']);
                                $this->__set('RedirectURLOnFail', $recordData['RedirectURLOnFail']);

                                $retVal = true;
                        }
                        else {
                        // No or to many records found
                        	$this->Log(3, "Received an incorrect CustomerKey. Received: $CallMeNowIDHash from".$_SERVER['HTTP_REFERER']);
                        	$this->ErrorCode = 100;
                        	$this->ErrorMsg = "Invalid Customer Key";
                        }
                }
                return $retVal;
        }
        
        
		/**
		 * Validates the desintation against the database
		 * 
		 * @param unknown_type $CallMeNowIDHash - 32 char string for ID purposes
		 * @param unknown_type $DestinationHash - 32 char string for ID purposes
		 * 
		 * @return 	true when validation is successfull;
		 * 			false when validation is NOT successfull;
		 * 			Sets the ErroCode member with the reson code.
		 * 
		 */
	 	public function ValidateDestinationKey($CallMeNowIDHash, $DestinationHash) {
                // Check database if hash exists
                // Get key (cmndid) and destination id (cmndid) from HTTP POST and check with Database
                // SELECT Destination FROM CallMeNowDestinations WHERE CallMeNowIDHash  = <key> and DestinationHash = <key>
            	$this->SetPrimaryServerSettings($this->ServerName, $this->UserName, $this->UserPassword);
				$this->SetSecondaryServerSettings($this->ServerName, $this->UserName, $this->UserPassword);   
				
                if(!isset($CallMeNowIDHash))
                	$CallMeNowIDHash= $this->customerKey;
                if(!isset($DestinationHash))
                	$DestinationHash = $this->destinationKey;

                $retVal = false;
                $resultSet = '';

                $SQLQuery = "SELECT Destination FROM CallMeNowDestinations WHERE CallMeNowIDHash  = '".$CallMeNowIDHash."' AND DestinationHash = '".$DestinationHash."'";
                
                $this->Log(9,$SQLQuery);	//Log to Screen

				$this->SetDatabase($this->Database);
                if($this->GoSQL($SQLQuery, &$resultSet)) {
                	
                        if(mssql_num_rows($resultSet) == 1) {
                        	
                                // Found a record
                                $recordData = mssql_fetch_assoc($resultSet);
                                $this->__set('Destination',$recordData['Destination']);
                                
                                $retVal = true;
                        }
                        else {
                        	
                        	$this->Log(3, "Received an incorrect DestinationKey for :$CallMeNowIDHash from:". $_SERVER['HTTP_REFERER']);
                        	$this->ErrorCode = 200;
                        	$this->ErrorMsg = "Invalid Destination Key";
                        }
                }
                return $retVal;
 		}

        
        /**
         * 
         * @param unknown_type $lastDialTime
         * @param unknown_type $dialInterval
         */
        public function CheckDialHistory($lastDialTime, $dialInterval) {	
        	// Allow only 1 outdial per 15 minutes per session
        	$retVal = false;       	

        	if( ($lastDialTime + (60*$dialInterval)) <= time()) {
        		$retVal = true;    		
	    	}
        	else {
        		$this->Log(3,"Accessive dialout attempt for: ".$_SERVER['HTTP_REFERER']);
        		$this->ErrorCode = 400;
        		$this->ErrorMsg = "Accessive dialout attempt";
        	}
        	
			return $retVal;
        }
        
        /**
         * 
         * @param $telno
         */
        public function ValidateUserDestination($telno) {
        	//Validte the number of the user that pressed Call Me Now
        	//Valid nummers are: National geographic
        	$retVal = false;
        	
        	$this-0>Log(9, "Check DN $telno");

        	$pattern = '#^0[1-7][0-9]{8}$#';
        	if(preg_match($pattern, $telno) && substr($telno,0,3) != '066') {
        		$this->numberToDial = substr($telno,1);
        		$retVal = true;
        	}
        	else {
        		$this->Log(3, "Invalid user destination for: ".$_SERVER['HTTP_REFERER']);
        		$this->ErrorCode = 300;
        		$this->ErrorMsg = "Invalid telephonenumber";
        	}
        		
        	return $retVal;
        }
        

        /**
         * Created the content for the call file and moves this to the correct directory.
         * mount to /var/spool/asterisk/outgoing
         * 
         * Filename: callmenow-C<10 dig>-A<10 dig.>-<datetime>.log
         * Where:
         * 	C<10 dig.> the number of the web user
         * 	A<10 dig.> the number of the agent
         * 
         * @return 
         */
        public function CreateCallFileContent() {

         	$file = $this->CreateCallFile($this->numberToDial, $this->Destination);
         	system("echo \"$file\"> /mnt/callmenow/callmenow-C$this->numberToDial-A$this->Destination-".Date("YmdHis").".call");
         
         	$this->Log(9, $file);
         
         	return true;
       	}

		/**
		 * 
		 * @param unknown_type $destination
		 * @param unknown_type $agent
		 */
        private function CreateCallFile($destination, $agent) {
                $callfile = "Channel: SIP/00$agent@81.175.81.11
MaxRetries:1 
RetryTime:300 
CallerID:\"Anonymous\" <$agent>
Set: ANI=$agent
WaitTime:30 
Context:outgoing_cmn 
Extension:00$destination 
Priority:1
Archive:yes";

                return $callfile;
        }
        
        /**
         * For loggin to /var/log/CallMeNow.log
         * and screen (if set)
         * 
         * Levels:
         * 	1 error			Always send to logfile
         * 	2 critical		Always send to log file
         * 	3 warning		Always send to log file
         * 	4..9 debug
         * 	
         * 
         * @param $level	-	Log level
         * @param $string	-	String to Log
         */
        private function Log($level,$string) {
        	if($level <= $this->logLevel || $level <= 3) { 
        		$datetime = date("Ymd - H:i:s");
        		error_log($datetime." -> Level:$level -> ".$string."\n", 3, "/var/log/CallMeNow.log");
        	}
        	
        	if($this->logToScreen)
        		echo $datetime." - > Level:$level -> "."$string<br>";
        }
         
}

?>
