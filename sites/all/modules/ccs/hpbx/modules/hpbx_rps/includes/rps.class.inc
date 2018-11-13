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
    $this->_proxy = $conf['hpbx_rps'][$this->_brand]['proxy'];
    $this->mac = $mac;
    $this->profile_id = $profile_id;
  }
  
  function __call($method, $args) {
    
    $request = xmlrpc_encode_request($this->_func_prefixes[$this->_brand] .'.'. $method, $args);
    $header = array(
      'Content-Type: text/xml',
      'Authorization: Basic '. base64_encode($this->_username . ':'. $this->_password),
      #'Content-length: '. strlen($request),
    );

    if (is_null($this->_ch)) {
      $this->_ch = curl_init();
      
      curl_setopt($this->_ch, CURLOPT_URL, $this->_endpoint);
      curl_setopt($this->_ch, CURLOPT_RETURNTRANSFER, 1);
      curl_setopt($this->_ch, CURLOPT_FOLLOWLOCATION, 1);
    }
    curl_setopt($this->_ch, CURLOPT_PROXY, $this->_proxy);
    curl_setopt($this->_ch, CURLOPT_HTTPHEADER, $header);
    curl_setopt($this->_ch, CURLOPT_POSTFIELDS, $request);
    
    $data = curl_exec($this->_ch);
    if (curl_errno($this->_ch)) {
      throw new Exception(curl_error($this->_ch));
    }

    $response = xmlrpc_decode($data);
    
    watchdog('hpbx_rps', __FUNCTION__ . ' => '. str_replace(PHP_EOL, "", $request) . ' => '. str_replace(PHP_EOL, "", var_export($response, TRUE)));
    
    if ($response && is_array($response) && xmlrpc_is_fault($response)) {
      watchdog('hpbx_rps_yealink', 'xmlrpc fault: '. $response['faultString'] . ' '. $response['faultCode']);
      return FALSE;
    }
    return $response;
  }
}