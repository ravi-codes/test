<?php 
class rps {
  
  protected $mac;
  protected $profile_id;
  private $_ch = NULL;
  private $_username;
  private $_password;
  private $_endpoint;
  private $_brand;
  private $_func_prefixes = array('yealink' => 'redirect',  'panasonic' => 'ipredirect');
  
  function __construct($brand, $mac) {

    $this->_brand = $brand;
    
    global $conf;
    $this->_cid = $conf['hpbx_rps'][$this->_brand]['auth']['cid'];
    $this->_key = $conf['hpbx_rps'][$this->_brand]['auth']['key'];
    $this->_profile = $conf['hpbx_rps'][$this->_brand]['profile']['test'];
    $this->_endpoint = $conf['hpbx_rps'][$this->_brand]['endpoint'];
    $this->_mac =$mac;
  }
  
  function __call($method, $args) {
  	$request['cid']= $this->_cid;
  	$request['method']= $method;
  	if($method=='deviceProvision'){
  		$request['params']['profile']= $this->_profile;
  	}
  	$request['params']['macs']= $this->_mac;
  	$request = json_encode($request);
    //$request = '{"cid":"2018","params":{"macs":["000B829BD405"],"profile":"DemoDev"},"method":"deviceProvision"}';
    $time= $this->get_time();
    $sig = hash_hmac('sha1',$request.$time,$this->_key);
    $url = $this->_endpoint . "?time=" . $time . "&sig=" . $sig;

    if (is_null($this->_ch)) {
      $this->_ch = curl_init();
      
      curl_setopt($this->_ch, CURLOPT_URL, $url);
      curl_setopt($this->_ch, CURLOPT_CUSTOMREQUEST, 'POST');
      curl_setopt($this->_ch, CURLOPT_POSTFIELDS, $request);
      curl_setopt($this->_ch, CURLOPT_RETURNTRANSFER, true);
	  curl_setopt($this->_ch, CURLOPT_SSL_VERIFYPEER,0);
      curl_setopt($this->_ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json'));
    }    
    $response = curl_exec($this->_ch);
    if(curl_error($this->_ch))
	{
		echo 'error:' . curl_error($this->_ch);
	}
    watchdog('hpbx_rps', __FUNCTION__ . ' => '. str_replace(PHP_EOL, "", $request) . ' => '. str_replace(PHP_EOL, "", var_export($response, TRUE)));
    
    if ($response && is_array($response) && xmlrpc_is_fault($response)) {
      watchdog('hpbx_rps_yealink', 'xmlrpc fault: '. $response['faultString'] . ' '. $response['faultCode']);
      return FALSE;
    }
    return $response;
  }
  
  function get_time()
  {
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $this->_endpoint);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch,CURLOPT_SSL_VERIFYPEER,0);
        $response = curl_exec($ch);
        $val = json_decode($response, true);        
        return $val[time];
  }
}