<?php
class grandstream extends rps{
  
  function __construct($mac) {
    parent::__construct('grandstream',$mac,$profile_id);
  }

  public function register() {
    $response = $this->deviceProvision();
    return $response;
  }
  
  public function unregister() {
    $response = $this->unDeviceProvision();
    return $response;
  }

  public function is_registered() {
    $data = $this->listDevicesConfig($this->mac);
    $macId=$this->mac[0];
    if(isset($data->ret->macs[0]->$macId->error) && (strtolower($data->ret->macs[0]->$macId->error)=='not found..')){
    	return FALSE;
    }else{
    	return TRUE;
    }
  }  
}