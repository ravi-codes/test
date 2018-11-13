<?php

class SipwiseReseller extends SipwiseEntity {
  function __construct($object) {
    parent::__construct('resellers', $object);
  }

  /**
   *
   * @param string $number
   */
  public function remove_national_digits(&$number) {
    $number = preg_replace('/^('. $this->_settings->digit_for_national_calls .')/', '', $number);
  }

  /**
   * Retrieve the passed number in e.164 format.
   *
   * @param $number
   * @return bool|string
   */
  public function number_get_e164($number) {

    if (preg_match('/^'. $this->_settings->country_code.'/', $number)) {
      
      // Already E.164.
      return $number;
    }
    elseif (strlen($this->_settings->digit_for_national_calls) && preg_match('/^'. $this->_settings->digit_for_national_calls .'/', $number)) {
      // National format with leading national call digit

      // Return E.164.
      return  $this->_settings->country_code . substr($number, strlen($this->_settings->digit_for_national_calls));
    }
    else {
      // No E.164, no national with leading digit, so it must be a subscriber number (SN).
      return $this->_settings->country_code . $number;
    }
    return FALSE;
  }
  
  
  /**
   * Helper function to 'delete' the reseller entity.
   * 
   * @see SipwiseEntity::delete()
   */
  public function delete() {
    throw new ErrorException('Deleting a reseller is currently not allowed');
  }
}



