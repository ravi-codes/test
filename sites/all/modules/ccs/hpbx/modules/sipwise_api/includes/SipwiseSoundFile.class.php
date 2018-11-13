<?php

class SipwiseSoundFile extends SipwiseEntity {


  function __construct($object = NULL) {
      
    $this->table = 'soundfiles';

    if (!is_null($object)) {
    
      // Walk all properties.
      foreach ((array)$object as $name => $value) {
    
        // Set property.
        $this->$name =  $value;
      }
    }
  }

  public function save() {
    
    $query = $this->_get_values();
    unset($query['recording']);
    
    $url = '/api/'. $this->table . '/'.$this->id .'?'. http_build_query($query);

    $request_headers = array(
        'Prefer: return=representation',
        'Content-Type: audio/x-wav',
        'Expect:'
    );
    
    
    // Verify if the entity needs to be created
    if (isset($this->id) && !is_null($this->id)) {
      
      // Update request URI.
      list($headers, $body) = Sipwise::exec($url, $this->recording, 'PUT', $request_headers);
    }
    else {

      // Create request URI.
      list($headers, $body) = Sipwise::exec($url, $this->recording, 'POST', $request_headers);
      
      if (isset($headers['http_code']) && $headers['http_code'] == 'HTTP/1.1 201 Created') {

        // Set the created ID.
        $this->id = (int)basename($headers['Location']);
      }
    }
    
    return Sipwise::result($body);
  }
}



