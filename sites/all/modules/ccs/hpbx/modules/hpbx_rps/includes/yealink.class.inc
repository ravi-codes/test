<?php
class Yealink extends rps{
  
  function __construct($mac) {
    parent::__construct('yealink', $mac, $profile_id);
  }

  public function register() {
    global $conf;
    $response = $this->registerDevice($this->mac, $conf['hpbx_rps']['yealink']['servername']);
    return ($response[0] && $response[1] == 'OK');
  }

  public function unregister() {
    $response = $this->deRegisterDevice($this->mac);
    return ($response[0] && $response[1] == 'OK');
  }

  public function is_registered() {
    
    $response = $this->checkDevice($this->mac);
    if ($response[0]) {
      switch ($response[1]) {
        case 'Unregistered': // Not assigned to a specific server.
        case 'Registered': // MAC has been assigned to a specific server.
        case 'Registered Elsewhere': //MAC has been registered by another RPS // @todo what happens when other customer has still registered device??
          return TRUE;
        case 'Unknown': // MAC has not been added to RPS.
          return FALSE;
      }
    }
    return FALSE;
  }
}