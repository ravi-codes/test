<?php

define('HPBX_NUMBER_FORMAT_E164', 1);
define('HPBX_NUMBER_FORMAT_INTERNATIONAL', 2);
define('HPBX_NUMBER_FORMAT_GSN', 4);
define('HPBX_NUMBER_FORMAT_SN', 8);

class SipwiseSubscriber extends SipwiseEntity {

  /**
   * SipwiseSubscriber constructor.
   *
   * @param object $object
   */
  function __construct($object) {
    parent::__construct('subscribers', $object);

    if ($this->is_pilot() && isset($this->primary_number)) {
      $this->local_primary_number_set();
    }
  }

  /**
   * Verify if this is an administrative user.
   * @return bool
   */
  public function is_admin() {
    return $this->administrative;
  }

  /**
   *
   */
  private function local_primary_number_set() {
    global $ActiveReseller;
    if ($this->is_pilot() && isset($this->primary_number)) {

      try {
        db_update('hpbx_customers')
        ->condition('customer_id', $this->customer_id)
        ->condition('reseller_id', $ActiveReseller->id)
        ->fields(array(
          'primary_number' => hpbx_parse_primary_number($this->primary_number, '', FALSE)
        ))
        ->execute();
      }
      catch(Exception $e) {
        watchdog('SipwiseSubscriber', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
      }
    }
  }

  /**
   * (non-PHPdoc)
   * @see SipwiseEntity::save()
   */
  public function save() {
    $result = parent::save();

    // Update the primary number in case this is a pilot subscriber.
    if ($this->is_pilot() && isset($this->primary_number)) {
      $this->local_primary_number_set();
    }
    return $result;
  }

  /**
   * @return bool
   */
  public function is_pilot() {
    return $this->is_pbx_pilot;
  }

  /**
   * @return bool
   */
  public function is_regular() {
    return !$this->is_pilot() && !$this->is_huntgroup() && !$this->is_conference_room() && !$this->is_fax_number() && !$this->is_auto_attendant();
  }

  /**
   * @return bool
   */
  public function is_huntgroup() {
    global $ActiveReseller;
    return $this->is_pbx_group && $this->profile_id != $ActiveReseller->_settings->fax_subscriber_profile_id;
  }

  /**
   * @return bool
   */
  public function is_conference_room() {
    global $ActiveReseller;
    return $this->profile_id == $ActiveReseller->_settings->conference_room_subscriber_profile_id;
  }

  /**
   * @return bool
   */
  public function is_fax_number() {
    global $ActiveReseller;
    return $this->is_pbx_group && $this->profile_id == $ActiveReseller->_settings->fax_subscriber_profile_id;
  }

  /**
   * @return bool
   */
  public function is_auto_attendant() {
    global $ActiveReseller;
    return $this->profile_id == $ActiveReseller->_settings->auto_attendant_subscriber_profile_id;
  }

  /**
   * @param string $format
   *
   * @return string|boolean
   */
  public function get_alias_number($format) {
    switch ($format) {
      case HPBX_NUMBER_FORMAT_E164:
        return $this->alias_number_get_e164();
      case HPBX_NUMBER_FORMAT_INTERNATIONAL:
        return $this->alias_number_get_international();
      case HPBX_NUMBER_FORMAT_GSN:
        return $this->alias_number_get_national();
      case HPBX_NUMBER_FORMAT_SN:
        return $this->alias_number_get_no_national_digit();
    }
    return FALSE;
  }

  /**
   * @param SipwiseSubscriber $Subscriber
   *
   * @return bool
   */
  public function is_member(SipwiseSubscriber $Subscriber) {
    return in_array($Subscriber->id, $this->pbx_groupmember_ids);
  }

  /**
   * 
   * @return array
   */
  public function get_active_directory_entry() {
    return !empty($this->webusername) ? hpbx_users_get_user($this->webusername) : FALSE;
  }

  /**
   * @param array $query
   * @param bool $reset
   * @param bool $references
   * @param bool $set_total
   *
   * @return SipwiseSubscriber[]
   */
  public static function get_all($query = array(), $reset = TRUE, $references = FALSE, $set_total = TRUE) {
    return sipwise_api_get_all('subscribers', $query, $reset, $references, $set_total);
  }

  /**
   * Retrieve the first alias_number in national format. Replacement
   * for hpbx_get_subscriber_alias_number().
   *
   * @return string|boolean
   */
  private function alias_number_get_national() {
    global $ActiveReseller;
    if (isset($this->alias_numbers) && count($this->alias_numbers) === 1) {
     if ($alias_number = $this->alias_numbers[0]->ac . $this->alias_numbers[0]->sn) { 
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
      if(strlen($alias_number) >= $min_length && strlen($alias_number) <= $max_length){
       return $ActiveReseller->_settings->digit_for_national_calls  . $this->alias_numbers[0]->ac . $this->alias_numbers[0]->sn;
      }else{
        return FALSE;
      }
     }
      // Subscriber does have only one alias_number.
      //return $ActiveReseller->_settings->digit_for_national_calls  . $this->alias_numbers[0]->ac . $this->alias_numbers[0]->sn;
    }
    return FALSE;
  }

  /**
   * Retrieve the first alias_number in national format.
   *
   * @return string|boolean
   */
  private function alias_number_get_no_national_digit() {
    global $ActiveReseller;
    if ($an = $this->get_alias_number(HPBX_NUMBER_FORMAT_GSN)) {
      $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
      $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
      $an_length = strlen($an);
      if($an_length >= $min_length && $an_length <= $max_length){
       return substr($an, strlen($ActiveReseller->_settings->digit_for_national_calls));
      }else{
        return FALSE;
      }
      //return substr($an, strlen($ActiveReseller->_settings->digit_for_national_calls));
    }
    return FALSE;
  }

  /**
   * @return string
   */
  private function alias_number_get_international() {
    if ($number = $this->alias_number_get_e164()) return '00'. $number;
    return FALSE;
  }

  /**
   * Retrieve the first alias_number in national format.
   *
   * @return string|boolean
   */
  private function alias_number_get_e164() {
    global $ActiveReseller;
    
    if ($this->alias_number_get_no_national_digit()) {
      return $ActiveReseller->_settings->country_code . $this->alias_number_get_no_national_digit();
    }
    return FALSE;
  }
  
  /**
   * Helper function to set or add the alias number.
   *
   * @param SipwiseEntity $Customer
   * @param string $number
   * @param bool $overwrite
   */
  public function alias_number_set($Customer, $number, $overwrite = TRUE) {
  
    global $ActiveReseller;
     $min_length = $ActiveReseller->_settings->area_code_min_digits + $ActiveReseller->_settings->subscriber_number_min_digits;
     $max_length = $ActiveReseller->_settings->area_code_max_digits + $ActiveReseller->_settings->subscriber_number_max_digits;
     if(strlen($number) >= $min_length && strlen($number) <= $max_length){
       $number_areacode = $number;
     }else{
       $number_areacode = substr($number ,0, -($Customer->_settings->extension_length));
     }
    // Verify if number is not empty.
    if (!empty($number)) {
  
      // If first digit is digit for national calls, remove it.
      $ActiveReseller->remove_national_digits($number);
  
      // Verify if the area code is found, should not happen.
      
      if (!$area = $Customer->get_area_code($number_areacode)) {
  
        watchdog('hpbx', 'Could not find area code for number '. $number);
        die();
      }
  
      // Create alias number.
      $an = new stdClass();
      $an->cc = $ActiveReseller->_settings->country_code;
      $an->ac = (int)$area;
      $an->sn = substr($number, strlen($area));
  
      // Do we need to overwrite the current alias?
      if ($overwrite) {
  
        // Set alias number.
        $this->alias_numbers = array($an);
      }
      else {
  
        // Add to array of alias numbers.
        $this->alias_numbers[] = $an;
        $this->alias_numbers = array_values($this->alias_numbers);
      }
    }
    else {
      // Unset the alias numbers.
      unset($this->alias_numbers);
    }
  }

  /**
   * Helper function to unset the alias_number, normally only used for the
   * pilot subscriber.
   *
   * @param SipwiseEntity $Customer
   * @param string $number
   */
  function alias_number_unset($Customer, $number) {
  
    global $ActiveReseller;
  
    // Verify if number is not empty.
    if (!empty($number)) {
  
      // If first digit is digit for national calls, remove it.
      $ActiveReseller->remove_national_digits($number);
  
      // Verify if the area code is found, should not happen.
      if (!$area = $Customer->get_area_code($number)) {
        watchdog('hpbx', 'Could not find area code for number '. $number);
        die();
      }
  
      // Create alias number.
      $an = new stdClass();
      $an->cc = $ActiveReseller->_settings->country_code;
      $an->ac = $area;
      $an->sn = substr($number, strlen($area));
  
      foreach ($this->alias_numbers as $p_key => $p_an) {
  
        if ($p_an->cc == $an->cc && $p_an->ac == $an->ac && $p_an->sn == $an->sn) {
          break;
        }
      }
  
      if (isset($p_key)) {
        unset($this->alias_numbers[$p_key]);
      }
  
      // Correct the array.
      $this->alias_numbers = array_values($this->alias_numbers);
    }
  }
}



