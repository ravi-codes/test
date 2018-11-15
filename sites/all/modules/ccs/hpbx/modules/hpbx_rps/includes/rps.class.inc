<?php 
class rps {
  
  protected $mac;
  protected $profile_id;
  private $_ch = NULL;
  private $_username;
  private $_password;
  private $_endpoint;
  private $_brand;
  private $_func_prefixes = array('yealink' => 'redirect',  'panasonic' => 'ipredirect', 'panasonic_dev' => 'ipredirect', 'panasonic_prod' => 'ipredirect');
  
  function __construct($brand, $mac, $profile_id) {

    $this->_brand = $brand;
    
    global $conf;
    $this->_username = $conf['hpbx_rps'][$this->_brand]['auth']['username'];
    $this->_password = $conf['hpbx_rps'][$this->_brand]['auth']['password'];
    $this->_endpoint = $conf['hpbx_rps'][$this->_brand]['endpoint'];
    $this->_profile = $conf['hpbx_rps'][$this->_brand]['profile'];
    $this->_cid = $conf['hpbx_rps'][$this->_brand]['auth']['cid'];
    $this->_proxy = $conf['hpbx_rps'][$this->_brand]['proxy'];
    $this->_key = $conf['hpbx_rps'][$this->_brand]['auth']['key'];
    $this->mac = $mac;
    $this->profile_id = $profile_id;
  }
  
  function __call($method, $args) {
    
  	if(strtolower($this->_brand)=='grandstream'){
  		$request['cid']= $this->_cid;
	  	$request['method']= $method;
	  	if($method=='deviceProvision'){
	  		$request['params']['profile']= $this->_profile;
	  	}
	  	$request['params']['macs']= array($this->mac);
	  	$request = json_encode($request);
	    $time= $this->get_time();
	    $sig = hash_hmac('sha1',$request.$time,$this->_key);
	    $this->_endpoint = $this->_endpoint . "?time=" . $time . "&sig=" . $sig;
	    $header = array(
	      	'Content-Type: application/json',
	      	'Content-length: '. strlen($request),
    	);	
  	}else{
    $request = xmlrpc_encode_request($this->_func_prefixes[$this->_brand] .'.'. $method, $args);
    $header = array(
      'Content-Type: text/xml',
      'Authorization: Basic '. base64_encode($this->_username . ':'. $this->_password),
      #'Content-length: '. strlen($request),
    );
  	}

    if (is_null($this->_ch)) {
      $this->_ch = curl_init();
      curl_setopt($this->_ch, CURLOPT_PROXY, $this->_proxy);
      curl_setopt($this->_ch, CURLOPT_URL, $this->_endpoint);
      curl_setopt($this->_ch, CURLOPT_RETURNTRANSFER, 1);
      curl_setopt($this->_ch, CURLOPT_FOLLOWLOCATION, 1);
    }
    
    curl_setopt($this->_ch, CURLOPT_PROXY, $this->_proxy);
    curl_setopt($this->_ch, CURLOPT_HTTPHEADER, $header);
    curl_setopt($this->_ch, CURLOPT_POSTFIELDS, $request);
    curl_setopt($this->_ch, CURLOPT_SSL_VERIFYPEER,0);
    
    $data = curl_exec($this->_ch);
    if (curl_errno($this->_ch)) {
      throw new Exception(curl_error($this->_ch));
    }
    if((strtolower($this->_brand)=='grandstream')){
    	$response= $data;
    	$result=json_decode($data);
    	if($result->code!='0'){
    		return FALSE;
    	}else{
    		return $result;
    	}
    }else{
    $response = xmlrpc_decode($data);
    watchdog('hpbx_rps', __FUNCTION__ . ' => '. str_replace(PHP_EOL, "", $request) . ' => '. str_replace(PHP_EOL, "", var_export($response, TRUE)));
    
    if ($response && is_array($response) && xmlrpc_is_fault($response)) {
      watchdog('hpbx_rps_yealink', 'xmlrpc fault: '. $response['faultString'] . ' '. $response['faultCode']);
      return FALSE;
	    }else{
	 	  return $response;	   	
	    }
    }    
    }
  
  /** 
   * Supportive function to retreive server timestamp for grandstream devices.
   */
  function get_time()
  {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $this->_endpoint);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_PROXY, $this->_proxy);
		curl_setopt($ch,CURLOPT_SSL_VERIFYPEER,0);
        $response = curl_exec($ch);
        $val = json_decode($response, true);        
        return $val[time];
  }
}