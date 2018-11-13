<?php

class SipwiseCustomer extends SipwiseEntity {

  function __construct($object) {
    parent::__construct('customers', $object);

    // Load the _settings so we can update the search table(s).
    if (isset($this->contact_id) && $CustomerContact = SipwiseEntity::load('customercontacts', $this->contact_id)) {
      $this->_settings = self::get_settings_from_gpp_chunks($CustomerContact);
    }

    // Update the search table.
    if ($this->id) {
      $this->local_search_table_set();
    }
  }

  /**
   * @param $date
   *
   * @return int
   */
 // Modified for #9759 - PE DEV: Customer overview - Improved caching of customer information  
  static function deleted_allowed_on_date($date) {

    global $ActiveReseller;
    $suspend_period = $ActiveReseller->_settings->suspend_period;
    define('HPBX_CUSTOMER_SUSPENDED_OFFSET_DAYS', '-'.$suspend_period.' days');

    if (!isset($date)) {
      return HPBX_CUSTOMER_DELETE_NOT_ALLOWED;
    }
    elseif (isset($date) && strtotime($date) < strtotime(HPBX_CUSTOMER_SUSPENDED_OFFSET_DAYS)) {
      return HPBX_CUSTOMER_SUSPENDED_DELETE_ALLOWED;
    }
    elseif (isset($date) && strtotime($date) > strtotime(HPBX_CUSTOMER_SUSPENDED_OFFSET_DAYS)) {
      return HPBX_CUSTOMER_SUSPENDED_DELETE_NOT_ALLOWED;
    }
    return HPBX_CUSTOMER_DELETE_NOT_ALLOWED;
  }

  /**
   * @return int
   */
  function delete_allowed() {
    return self::deleted_allowed_on_date($this->_settings->suspend_date);
  }

  /**
   *
   */
  private function remove_active_directory_ou() {
    hpbx_users_remove_ou($this);
  }

  /**
   *
   */
  private function remove_users() {

    // Retrieve all customer subscribers, this to remove all the related active
    // directory accounts.
    if ($Subscribers = sipwise_api_get_all('subscribers', array('customer_id' => $this->id))) {

      // Remove all subscribers and the assigned active directory account.
      foreach ($Subscribers as $Subscriber) {
        hpbx_users_remove($Subscriber);
      }
    }
  }

  /**
   *
   * @param string $number
   *
   * @return boolean
   */
  public function get_area_code($number) {

    global $ActiveReseller;

    $ActiveReseller->remove_national_digits($number);

    // Find the range the number belongs to.
    foreach ($this->_settings->numberranges as $range) {

      // Add prefix 1, to prevent leading zero's to be stripped of while casting to integer.
      $end = $start = (int) '1' . $range->area . $range->start;
      $end += (int) $range->length;

      // start 1012065000
      // end 1012065999
      for ($i = $start; $i < $end; $i++) {

        // Extract areacode.
        if (isset($range->area)) {

          // 012065000
          if (substr($i, 1) == $number) {
            return $range->area;
          }
        }
      }
    }
    return FALSE;
  }


  /**
   * Helper function to 'delete' the customer entity.
   *
   * @see SipwiseEntity::delete()
   */
  public function delete() {

    if (!empty($this->id)) {

      $this->remove_users();

      // Set status as terminated, which will delete the entity.
      $this->status = 'terminated';

      // Save the entity.
      $result = $this->save();

      // Remove all the users for this customer.
      $this->remove_active_directory_ou();

      return $result;
    }
    else {
      throw new ErrorException('Can\'t delete empty SipwiseEntity');
    }
  }

  /**
   * (non-PHPdoc)
   * @see SipwiseEntity::save()
   */
  public function save() {
    $result = parent::save();
    if ($this->status == 'terminated') {
      $this->local_search_table_remove();
    }
    else {
      $this->local_search_table_set();
    }
    return $result;
  }

  /**
   *
   */
  private function local_search_table_set() {

    global $ActiveReseller;
    $CustomerContact = SipwiseEntity::load('customercontacts', $this->contact_id);
    $CustomerPreferences = SipwiseEntity::load('customerpreferences', $this->id);

    try {
      $query = db_merge('hpbx_customers')
        ->key(array(
          'reseller_id' => $ActiveReseller->id,
          'customer_id' => $this->id
        ))
        ->fields(array(
          'reseller_id' => $ActiveReseller->id,
          'company' => $CustomerContact->company,
          'external_id' => $this->external_id,
          'bp_name' => !empty($CustomerContact->gpp9) ? $CustomerContact->gpp9 : NULL,
          'multisite' => !empty($this->_settings->multi_site) ? $this->_settings->multi_site : NULL,
          'status' => $this->status,
          'suspend_date' => isset($this->_settings->suspend_date) ? $this->_settings->suspend_date : NULL,
          'ncos' => isset($CustomerPreferences->ncos) ? $CustomerPreferences->ncos : NULL,
        ));

      $query->execute();

      if (isset($this->_settings->numberranges)) {

        $values = array();
        foreach ($this->_settings->numberranges as $range) {
          if (!empty($range->area) && !empty($range->start) && !empty($range->length)) {

            $ActiveReseller->remove_national_digits($range->area);

            // Add prefix 1, to prevent leading zero's to be stripped of while casting to integer.
            $end = $start = (int) '1' . $range->start;
            $end += (int) $range->length;

            for ($i = $start; $i < $end; $i++) {
              $values[] = array(
                'customer_id' => $this->id,
                'reseller_id' => $ActiveReseller->id,
                'number' => $ActiveReseller->_settings->country_code . trim($range->area) . substr($i, 1)
              );
            }
          }
        }

        if (count($values)) {
          db_delete('hpbx_numbers')
            ->condition('customer_id', $this->id)
            ->execute();
          $query = db_insert('hpbx_numbers')->fields(array(
            'customer_id',
            'number',
            'reseller_id'
          ));
          foreach ($values as $entry) {
            $query->values($entry);
          }
          $query->execute();
        }
      }
    }
    catch (Exception $e) {
      watchdog('SipwiseCustomer', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
    }
  }

  /**
   *
   */
  private function local_search_table_remove() {

    try {
      global $ActiveReseller;

      // Remove the customer entry from the local database.
      db_delete('hpbx_customers')
        ->condition('customer_id', $this->id)
        ->condition('reseller_id', $ActiveReseller->id)
        ->execute();

      // Remove all numbers for this customer within the local database.
      db_delete('hpbx_numbers')
        ->condition('customer_id', $this->id)
        ->condition('reseller_id', $ActiveReseller->id)
        ->execute();

    }
    catch (Exception $e) {
      watchdog('SipwiseCustomer', $e->getMessage() . 'at ' . $e->getFile() . '(' . $e->getLine() . ')');
    }
  }

  /**
   * Helper function to get the pilot subscriber for this customer.
   *
   * @return SipwiseSubscriber $PilotSubscriber;
   */
  public function get_pilot_subscriber() {

    $PilotSubscriber = NULL;

    if (!is_null($this->id)) {
      $query = array('customer_id' => $this->id, 'is_pbx_pilot' => 1);

      if ($Subscribers = sipwise_api_get_all('subscribers', $query, TRUE, FALSE, FALSE)) {
        foreach ($Subscribers as $Subscriber) {
          $PilotSubscriber = $Subscriber;
          break;
        }
      }
    }
    return $PilotSubscriber;
  }


  public function get_fax_subscribers() {
    return $this->get_subscribers(FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
  }

  /**
   *
   * @param boolean $is_pbx_pilot
   * @param boolean $is_pbx_group
   * @param boolean $autoattendant
   * @param boolean $conference
   * @param boolean $fax
   * @param boolean $regular
   *
   * @return SipwiseSubscriber[]
   */
  public function get_subscribers($is_pbx_pilot = FALSE, $is_pbx_group = FALSE,
      $autoattendant = FALSE, $conference = FALSE, $fax = FALSE, $regular = TRUE) {

    $Subscribers = SipwiseSubscriber::get_all(array('customer_id' => $this->id), FALSE);

    foreach ($Subscribers as $key => $Subscriber) {
      if ($Subscriber->is_auto_attendant() && !$autoattendant) unset($Subscribers[$key]);
      elseif ($Subscriber->is_conference_room() && !$conference) unset($Subscribers[$key]);
      elseif ($Subscriber->is_pilot() && !$is_pbx_pilot) unset($Subscribers[$key]);
      elseif ($Subscriber->is_huntgroup() && !$is_pbx_group) unset($Subscribers[$key]);
      elseif ($Subscriber->is_fax_number() && !$fax) unset($Subscribers[$key]);
      elseif ($Subscriber->is_regular() && !$regular) unset($Subscribers[$key]);
    }
    return $Subscribers;
  }

  /**
   * @return SipwiseSubscriber[]
   */
  public function get_huntgroups() {
    return $this->get_subscribers(FALSE, TRUE, FALSE, FALSE, FALSE, FALSE);
  }

  /**
   * @return SipwiseSubscriber[]
   */
  public function get_conference_rooms() {
    return $this->get_subscribers(FALSE, FALSE, FALSE, TRUE, FALSE, FALSE);
  }

  /**
   * @return SipwiseSubscriber[]
   */
  public function get_auto_attendants() {
    return $this->get_subscribers(FALSE, FALSE, TRUE, FALSE, FALSE, FALSE);
  }

  /**
   * Helper function to get all free numbers for this customer.
   *
   * @param SipwiseSubscriber $Subscriber
   * @return array $options
   */
  public function get_free_numbers($Subscriber = NULL) {
  
    global $ActiveReseller;
  
    $options = array();
  
    $PilotSubscriber = $this->get_pilot_subscriber();

    foreach ($PilotSubscriber->alias_numbers as $an) {
      $options[$an->ac . $an->sn] = $ActiveReseller->_settings->digit_for_national_calls . $an->ac . $an->sn;
    }

    // Add own number.
    if (!is_null($Subscriber)) {
  
      if (isset($Subscriber->alias_numbers[0]->cc) && isset($Subscriber->alias_numbers[0]->ac) && isset($Subscriber->alias_numbers[0]->sn)) {
        $an = $Subscriber->alias_numbers[0]->ac . $Subscriber->alias_numbers[0]->sn;
        $options[$an] = $ActiveReseller->_settings->digit_for_national_calls .$an;
      }
    }
    asort($options);
    return $options;
  }
}



