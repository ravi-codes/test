<?php


/**
 * 
 * @author rgraat
 *
 */
class SipwiseEntity {
  private $vars = array();

  public $table;

  private $baseline = array();
  
  function set_gpp_chunks(&$CustomerContact, $settings) {
    
    $chunk_length = 255;
	
    $base64 = base64_encode(gzcompress(json_encode($settings)));     
   //$base64 = base64_encode(json_encode($settings));     
    
    if (strlen($base64) > ($chunk_length*10)) {
      watchdog('gpp_chunks', t('Failed to set gpp chunks. Length: '. strlen($base64) . ' / '. ($chunk_length*9)));
      return FALSE;  
    }
    
    $chunks = explode(PHP_EOL, chunk_split($base64, $chunk_length, PHP_EOL));

    foreach ($chunks as $i => $chunk) {

      if ($chunk!='') {
        switch($i) {
          case 0:
            $CustomerContact->gpp0 = $chunk;
            break;
          case 1:
            $CustomerContact->gpp1 = $chunk;
            break;
          case 2:
            $CustomerContact->gpp2 = $chunk;
            break;
          case 3:
            $CustomerContact->gpp3 = $chunk;
            break;
          case 4:
            $CustomerContact->gpp4 = $chunk;
            break;
          case 5:
            $CustomerContact->gpp5 = $chunk;
            break;
          case 6:
            $CustomerContact->gpp6 = $chunk;
            break;
          case 7:
            $CustomerContact->gpp7 = $chunk;
            break;
          case 8:
            $CustomerContact->gpp8 = $chunk;
            break;
          /*case 9:
            $CustomerContact->gpp9 = $chunk;
            break;*/
        }
      }
    }
    return TRUE;
  }
  
  public static function set_compressed_gpp_chunks(&$CustomerContact, $settings) {
  	
  	self::empty_gpp_chunks($CustomerContact);
    
    $chunk_length = 255;
	
    $base64 = base64_encode(gzcompress(json_encode($settings)));     
    
    if (strlen($base64) > ($chunk_length*10)) {
      watchdog('gpp_chunks', t('Failed to set gpp chunks. Length: '. strlen($base64) . ' / '. ($chunk_length*9)));
      return FALSE;  
    }
    
    $chunks = explode(PHP_EOL, chunk_split($base64, $chunk_length, PHP_EOL));

    foreach ($chunks as $i => $chunk) {

      if ($chunk!='') {
        switch($i) {
          case 0:
            $CustomerContact->gpp0 = $chunk;
            break;
          case 1:
            $CustomerContact->gpp1 = $chunk;
            break;
          case 2:
            $CustomerContact->gpp2 = $chunk;
            break;
          case 3:
            $CustomerContact->gpp3 = $chunk;
            break;
          case 4:
            $CustomerContact->gpp4 = $chunk;
            break;
          case 5:
            $CustomerContact->gpp5 = $chunk;
            break;
          case 6:
            $CustomerContact->gpp6 = $chunk;
            break;
          case 7:
            $CustomerContact->gpp7 = $chunk;
            break;
          case 8:
            $CustomerContact->gpp8 = $chunk;
            break;
          /*case 9:
            $CustomerContact->gpp9 = $chunk;
            break;*/
        }
      }
    }
    return TRUE;
  }

  public static function get_settings_from_gpp_chunks($CustomerContact) {

    $base64 = '';
    
    if (!empty($CustomerContact->gpp0)) {
      $base64 .= $CustomerContact->gpp0;
    }
    if (!empty($CustomerContact->gpp1)) {
      $base64 .= $CustomerContact->gpp1;
    }
    if (!empty($CustomerContact->gpp2)) {
      $base64 .= $CustomerContact->gpp2;
    }
    if (!empty($CustomerContact->gpp3)) {
      $base64 .= $CustomerContact->gpp3;
    }
    if (!empty($CustomerContact->gpp4)) {
      $base64 .= $CustomerContact->gpp4;
    }
    if (!empty($CustomerContact->gpp5)) {
      $base64 .= $CustomerContact->gpp5;
    }
    if (!empty($CustomerContact->gpp6)) {
      $base64 .= $CustomerContact->gpp6;
    }
    if (!empty($CustomerContact->gpp7)) {
      $base64 .= $CustomerContact->gpp7;
    }
    if (!empty($CustomerContact->gpp8)) {
      $base64 .= $CustomerContact->gpp8;
    }
    /*if (!empty($CustomerContact->gpp9)) {
      $base64 .= $CustomerContact->gpp9;
    }*/
    return json_decode(gzuncompress(base64_decode($base64)));
    //return json_decode(base64_decode($base64));
  }
  
  public static function get_settings_from_gpp_chunks_for_migration($CustomerContact) {

    $base64 = '';
    
    if (!empty($CustomerContact->gpp0)) {
      $base64 .= $CustomerContact->gpp0;
    }
    if (!empty($CustomerContact->gpp1)) {
      $base64 .= $CustomerContact->gpp1;
    }
    if (!empty($CustomerContact->gpp2)) {
      $base64 .= $CustomerContact->gpp2;
    }
    if (!empty($CustomerContact->gpp3)) {
      $base64 .= $CustomerContact->gpp3;
    }
    if (!empty($CustomerContact->gpp4)) {
      $base64 .= $CustomerContact->gpp4;
    }
    if (!empty($CustomerContact->gpp5)) {
      $base64 .= $CustomerContact->gpp5;
    }
    if (!empty($CustomerContact->gpp6)) {
      $base64 .= $CustomerContact->gpp6;
    }
    if (!empty($CustomerContact->gpp7)) {
      $base64 .= $CustomerContact->gpp7;
    }
    if (!empty($CustomerContact->gpp8)) {
      $base64 .= $CustomerContact->gpp8;
    }
    /*if (!empty($CustomerContact->gpp9)) {
      $base64 .= $CustomerContact->gpp9;
    }*/
    //return json_decode(gzuncompress(base64_decode($base64)));
    return json_decode(base64_decode($base64));
  }

  private static function empty_gpp_chunks(&$CustomerContact) {
    $CustomerContact->gpp0 = '';
    $CustomerContact->gpp1 = '';
    $CustomerContact->gpp2 = '';
    $CustomerContact->gpp3 = '';
    $CustomerContact->gpp4 = '';
    $CustomerContact->gpp5 = '';
    $CustomerContact->gpp6 = '';
    $CustomerContact->gpp7 = '';
    $CustomerContact->gpp8 = '';
    /*$CustomerContact->gpp9 = '';*/
  }

  /**
   *
   * @param string $type
   * @param mixed $id
   * @param string $reset
   * @param string $references
   *
   * @throws ErrorException
   *
   * @return SipwiseEntity
   */
  public static function load($type, $id, $reset = FALSE, $references = FALSE, $retrieve_settings = TRUE) {
    global $conf;
    if (in_array($type, array('contractbalances'))) {
      return FALSE;
    }

    // Cache customers
    static $results = NULL;
    
    // Verify the customer is not yet retrieved from the API.
    if ($reset || !isset($results[$type][$id])) {

      // Define url
      $url = '/api/'. $type .'/' . $id;
      
      $reseller_id = '0';
      
      // Cache specific API types. For types which are configured using the
      // selfcare (customers, customercontacts, customerpreferences, resellers), the cache must be cleared
      // after saving these types.
      if ( (($type!='resellers' && $reseller_id = sipwise_api_get_active_reseller()) || $type == 'resellers') && $id && in_array($type, $conf['sipwise_api']['sipwise_api_cached_entities'])) {
        
        $cid = $type . '_'. $reseller_id . '_'. $id.'_response';
        
        // Try to read from cache.
        if ($cache = cache_get($cid, 'cache_sipwise_api')) {
          watchdog('sipwise_api_cache', t('Read @type (@id) entity from cache (@cid). request=@request', array('@id' => $id, '@type' => $type, '@cid' => $cid, '@request' => $url)));
          $response = $cache->data;
        }
        
        // Nothing in cache
        else {
          $response = Sipwise::exec($url);

          // Verify if we did receive an (error) message.
          list($headers, $body) = $response;
          if (isset($body->message)) {
            watchdog('sipwise_api_cache', t('@type (@id) entity NOT written to cache (@cid) as we received an error message @message',
              array('@message' => $body->message ,'@id' => $id, '@type' => $type, '@cid' => $cid)));
          }
          else { 
            watchdog('sipwise_api_cache', t('Write @type (@id) entity to cache (@cid). source=@source', array('@id' => $id, '@type' => $type, '@cid' => $cid, '@source' => $url)));
            cache_set($cid, $response, 'cache_sipwise_api');
          }
        }
      }
      else {
        $response = Sipwise::exec($url);
      }

      list($headers, $body) = $response;

      // If content type is not JSON, then output the retrieved file directly and exit.
      if (!preg_match('/^application\/hal\+json/', $headers['Content-Type']) && !preg_match('/^application\/json/', $headers['Content-Type'])) {
        
        foreach ($headers as $header_name => $header_value) { 
          header($header_name . ': '. $header_value);
        }
        return $body;
      }

      // Verify if we did receive an (error) message.
      if (isset($body->message)) {

        watchdog('sipwise_api', 'Failed to load '. $type .' with id '. $id . '. Error message: '. $body->message . ' (' . $body->code . ')');
        //throw new ErrorException($body->message, $body->code);
        return FALSE;
      }

      // Create new SipwiseEntity.
      $SipwiseEntity = SipwiseEntity::create($type, $body);

      if (empty($SipwiseEntity->id)) {
        $SipwiseEntity->id = $id;
      }
      
      if ($retrieve_settings && $type == 'customers' && isset($SipwiseEntity->contact_id)  && 
        $CustomerContact = SipwiseEntity::load('customercontacts', $SipwiseEntity->contact_id)) {

        $SipwiseEntity->_settings = self::get_settings_from_gpp_chunks($CustomerContact);
      }

      elseif ($retrieve_settings && $type == 'resellers' && isset($SipwiseEntity->contract_id)  &&
          $Contract = SipwiseEntity::load('contracts', $SipwiseEntity->contract_id)) {
        
        if ($SystemContact = SipwiseEntity::load('systemcontacts', $Contract->contact_id )) {
          
          $SipwiseEntity->_settings = self::get_settings_from_gpp_chunks($SystemContact);
        }
      }

      // Do we need to set the reference objects.
      if ($references) {

        // Set the reference objects.
        sipwise_api_set_references($SipwiseEntity);
      }

      // Assign response object.
      $results[$type][$id] = $SipwiseEntity;
    }

    // Return requested customer.
    return $results[$type][$id];
  }

  /**
   * 
   * @return mixed
   */
  public function _get_values() {

    $values = array();
    // Remove all objects and the id property.
    foreach ($this->vars as $name => $value) {
      
      // Ignore the referenced objects and the id property.
      if ($name != 'id' && $name[0] != '_') {
        // Set the value.
        $values[$name] = $value;
      }
    }
    return $values;
  }


  /**
   * 
   * @return string
   */
  function __toString() {
    
    $output = "<div class='hpbx-var-dump' style='font-family:courier;'>";
    $output .= '<span style="font-weight: bold;">object('. $this->table .')</span>' . '<br/>';
    $values = $this->_get_values();
    $values['id'] = $this->id;
    
    if ($this->table =='resellers' || $this->table == 'customers') {
      $values['_settings'] = $this->_settings;
    }

    foreach ($values as $key => $value) {

      $output .= "<span style='width:20px;'>&nbsp;&nbsp;</span><span style='color:black;'>". $key . "'</span> => ";

      if (is_string($value)) {
        $output .= "<span style='width:5px;'>&nbsp;</span><span style='color:black;'>" . gettype($value) ."</span>";
        $output .= "&nbsp;<span style='color:red;'>'". $value . "'</span>" . "&nbsp;<i>(length=". strlen($value) . ")</i>";
      }
      elseif (is_integer($value) || is_float($value) || is_double($value)) {
        $output .= "<span style='width:5px;'>&nbsp;</span><span style='color:black;'>" . gettype($value) ."</span>";
        $output .= "&nbsp;<span style='color:green;'>". $value . "</span>";
      }
      else {
        $export = var_export($value, TRUE);
        
        foreach (explode("\n", $export) as $i => $line) {
          if ($i == 0) {
            $output .= '<strong>' . $line . '</strong>';
          }
          else {
            if ($i == 1) {
              $output .= "<span style='color:purple'>";
            }
            $output .= "  ". $line . '<br/>';
          }
        }
        $output .= "</span>";
      }
      $output .= '<br/>';
    }
    $output .= "</div>" . '<br/>'; 
    return $output;
  }

  /**
   * @param string $type
   * @param SipwiseEntity $object
   *
   * @return \SipwiseCustomer|\SipwiseEntity|\SipwisePBXDevice|\SipwiseReseller|\SipwiseSubscriber
   */
  public static function create($type, $object = NULL) {
  	include_once(drupal_get_path('module', 'sipwise_api') . '/includes/SipwiseReseller.class.php');

    if ($type == 'subscribers') {
      return new SipwiseSubscriber($object);
    }
    elseif ($type == 'customers') {
      return new SipwiseCustomer($object);
    }
    elseif ($type == 'resellers') {
      return new SipwiseReseller($object);
    }
    elseif ($type == 'pbxdevices') {
      return new SipwisePBXDevice($object);
    }
    else {
      return new SipwiseEntity($type, $object);
    }
  }

  /**
   * @param string $type
   * @param object $object
   */
  function __construct($type, $object = NULL) {

    $this->table = $type;
    
    if (!is_null($object)) {
      
      // Walk all properties.
      foreach ((array)$object as $name => $value) {

        // Set property.
        $this->$name =  $value;
      }
      
      $this->baseline = $this->vars;
    }
  }

  /**
   * 
   */
  public function save() {

    global $ActiveReseller;
    global $conf;

    // Verify if the entity needs to be saved.
    if (serialize($this->baseline) != serialize($this->vars)) {

      // Verify if the entity needs to be created 
      if (isset($this->id) && !is_null($this->id)) {
        
        // Update request URI.
        list($headers, $body) = Sipwise::exec('/api/'. $this->table . '/'. $this->id, $this->_get_values(), 'PUT');

        if (in_array($this->table, $conf['sipwise_api']['sipwise_api_cached_entities'])) {
          // Clear the Drupal cache for this API type.
          $cid = $this->table . '_'. $ActiveReseller->id . '_'. $this->id .'_response';
          cache_clear_all($cid, 'cache_sipwise_api');
          watchdog('sipwise_api_cache', t('Cleared @table with @cid', array('@table' => $this->table, '@cid' => $cid)));
        }
      
        if (in_array($this->table, array('cftimesets', 'cfdestinationsets', 'cfmappings')) &&
            in_array('callforwards', $conf['sipwise_api']['sipwise_api_cached_entities'])) {
               
              // Also clear callforwards cache.
              $cf_id = (isset($this->subscriber_id) ? $this->subscriber_id : $this->id);
              $cid = 'callforwards' . '_'. $ActiveReseller->id . '_'. $cf_id .'_response';
              cache_clear_all($cid, 'cache_sipwise_api');
              watchdog('sipwise_api_cache', t('Cleared @table with @cid', array('@table' => 'callforwards', '@cid' => $cid)));
        }
            
        // @todo Needs in-depth investigation when to clear the cache. Current situation is not sufficient.
        if (in_array($this->table, $conf['sipwise_api']['sipwise_api_cached_lists'])) {
          $cid = $this->table . '_list_'. $ActiveReseller->id.'_';
          cache_clear_all($cid, 'cache_sipwise_api', TRUE);
          watchdog('sipwise_api_cache', t('Cleared @table_list with @cid*', array('@table' => $this->table, '@cid' => $cid)));
        }
        
        // Special for Customers, but only when customer status != terminated.
        if ($this->table == 'customers' && $this->status!='terminated') {
          
          if (!isset($this->_settings)) {
            $this->_settings = array();
          }
  
          // Load existing customer contact.
          $CustomerContact = SipwiseEntity::load('customercontacts', $this->contact_id);
          
          // Empty gpp fields.
          self::empty_gpp_chunks($CustomerContact);
          
          // Set gpp fields.
          if (self::set_gpp_chunks($CustomerContact, $this->_settings)) {
  
            // Save the customer contact.
            $CustomerContact->save();
          }
          else {
            watchdog('sipwise_api_cache', t('Failed to set gpp chunks'));
          }
        }
        // Special for Resellers, but only when customer status != terminated.
        elseif ($this->table == 'resellers' && $this->status!='terminated') {
        
          if (!isset($this->_settings)) {
            $this->_settings = array();
          }
        
          // Load existing contract.
          $Contract = SipwiseEntity::load('contracts', $this->contract_id);
          
          // Load existing system contact.
          $SystemContact = SipwiseEntity::load('systemcontacts', $Contract->contact_id);
        
          // Empty gpp fields.
          self::empty_gpp_chunks($SystemContact);
        
          // Set gpp fields.
          if (self::set_gpp_chunks($SystemContact, $this->_settings)) {
        
            // Save the customer contact.
            $SystemContact->save();
          }
          else {
            watchdog('sipwise_api_cache', t('Failed to set gpp chunks'));
          }
        }
      }
      else {
  
        if ($this->table == 'customers') {
          
          if (!isset($this->_settings)) {
            $this->_settings = array();
          }
          $CustomerContact = SipwiseEntity::create('customercontacts');
          $CustomerContact->reseller_id = $ActiveReseller->id;
          $CustomerContact->email = uniqid() .'@'.uniqid() . '.nl';
          
          // Empty gpp fields.
          self::empty_gpp_chunks($CustomerContact);
          
          // Set gpp fields.
          if (self::set_gpp_chunks($CustomerContact, $this->_settings)) {
          
            $CustomerContact->save();
          }
          else {
            watchdog('sipwise_api_cache', t('Failed to set gpp chunks'));
          }

          // Save the customer (again).
          $this->contact_id = $CustomerContact->id;
        }
        
        // Create request URI.
        list($headers, $body) = Sipwise::exec('/api/'. $this->table . '/', $this->_get_values(), 'POST');
  
        if (in_array($this->table, $conf['sipwise_api']['sipwise_api_cached_lists'])) {
          $cid = $this->table .'_list_'. $ActiveReseller->id.'_';
          cache_clear_all($cid, 'cache_sipwise_api', TRUE);
          watchdog('sipwise_api_cache', t('Cleared @table_list with @cid*', array('@table' => $this->table, '@cid' => $cid)));
        }
        
        if (isset($headers['http_code']) && $headers['http_code'] == 'HTTP/1.1 201 Created') {
  
          // Set the created ID.
          $this->id = (int)basename($headers['Location']);
        }  
      }
      return Sipwise::result($body);
    }
  }


  /**
   * 
   */
  public function delete() {
    
    if (!empty($this->id)) {

      // Define url
      $url = '/api/'. $this->table .'/' . $this->id;
      
      // Request DELETE.
      list($headers, $body) = Sipwise::exec($url, NULL, 'DELETE');
      
      if ($headers['http_code'] == 'HTTP/1.1 204 No Content') { 
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
    else {
      throw new ErrorException('Can\'t delete empty SipwiseEntity');
    }
  }

  /**
   * 
   * @param unknown $name
   * @throws Exception
   * @return unknown
   */
  function &__get($name) {
    if (isset($this->vars[$name])) {
      return $this->vars[$name];
    }
    
    // Trick to prefent errors.
    $value = NULL;
    return $value;
  }

  /**
   * 
   * @param string $name
   * @param mixed $value
   */
  function __set($name, $value) {

    $this->vars[$name] = $value;
    
    if ($name == 'id') {
      $this->baseline = $this->vars;
    }
  }

  /**
   *
   * @param string $name
   * @param mixed $value
   */
  function __unset($name) {

    unset($this->vars[$name]);
  }

  /**
   * 
   * @param string $name
   *
   * @return boolean
   */
  function __isset($name) {
    if (isset($this->vars[$name])) {
      return TRUE;
    }
    return FALSE;
  }
}