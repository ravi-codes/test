<?php
class Panasonic extends rps {
  protected $panasonic_rds;
  function __construct($mac, $profile_id) {
  	$this->panasonic_rds = variable_get('panasonic_rds', FALSE);
    parent::__construct($this->panasonic_rds, $mac, $profile_id);
  }

  public function register() {
    global $conf; 

    // Get max version of available firmwares, available for the device model.
    $max_version = hpbx_device_get_max_firmware_version($this->profile_id);

    if (!empty($max_version)) {
      return $this->registerPhone2($this->mac, $conf['hpbx_rps'][$this->panasonic_rds]['servername'], $max_version, '');
    }
    return FALSE;
  }

  public function unregister() {
    return $this->unregisterPhone($this->mac);
  }

  public function is_registered() {

    if (!$this->getPhone($this->mac)) {
      return FALSE;
    }
    return TRUE;
  }
}