<?php

class SipwisePBXDevice extends SipwiseEntity {
  
  function __construct($object) {
    parent::__construct('pbxdevices', $object);
    
    // Update the search table.
    $this->local_search_table_set();
  }

  /**
   * Helper function to 'delete' the customer entity.
   *
   * @see SipwiseEntity::delete()
   */
  public function delete() {
  
    $result = parent::delete();
    
    $this->local_search_table_remove();
    
    return $result;
  }
  
  /**
   * (non-PHPdoc)
   * @see SipwiseEntity::save()
   */
  public function save() {
    $result = parent::save();


    // Update the search table.
    if ($this->id) {
      $this->local_search_table_set();
    }
    return $result;
  }
  
  /**
   *
   */
  private function local_search_table_set() {
    try {
      global $ActiveReseller;
      
      $query = db_merge('hpbx_pbxdevices')
      ->key(array('pbxdevice_id' => $this->id, 'reseller_id' => $ActiveReseller->id))
      ->fields(array(
        'reseller_id' => $ActiveReseller->id,
        'customer_id' => $this->customer_id,
        'identifier' => strtolower($this->identifier),
      ));
  
      $query->execute();
    }
    catch(Exception $e) {
      watchdog('SipwisePBXDevice', $e->getMessage() . ' => '. $e->getLine());
    }
  }
  
  /**
   *
   */
  private function local_search_table_remove() {
  
    try {
      global $ActiveReseller;

      // Remove the customer entry from the local database.
      db_delete('hpbx_pbxdevices')
      ->condition('pbxdevice_id', $this->id)
      ->condition('reseller_id', $ActiveReseller->id)
      ->execute();
      
    }
    catch(Exception $e) {
      watchdog('SipwisePBXDevice', $e->getMessage());
    }
  }
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
}



