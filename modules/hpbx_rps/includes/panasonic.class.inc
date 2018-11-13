<?php
class Panasonic extends rps {

  function __construct($mac, $profile_id) {
    parent::__construct('panasonic', $mac, $profile_id);
  }

  public function register() {
    global $conf; 

    // Get max version of available firmwares, available for the device model.
    $max_version = hpbx_device_get_max_firmware_version($this->profile_id);

    if (!empty($max_version)) {
      return $this->registerPhone2($this->mac, $conf['hpbx_rps']['panasonic']['servername'], $max_version, '');
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