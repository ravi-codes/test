<?php

class SipwiseFax extends SipwiseEntity {


  function __construct($object = NULL) {

    $this->table = 'faxes';

    if (!is_null($object)) {

      // Walk all properties.
      foreach ((array)$object as $name => $value) {

        // Set property.
        $this->$name =  $value;
      }
    }
  }

  public function save() {
    throw new Exception();
  }

  public function send() {

    $request_headers = array(
      'Content-Type: multipart/form-data',
      'Expect: '
    );

    $values = $this->_get_values();

    if (isset($values['faxfile'])) {
      unset($values['faxfile']);

      $file = curl_file_create($this->faxfile, 'application/pdf', 'faxfile');
      $data = array('json' => $values, 'faxfile' => $file);

      list($headers) = Sipwise::exec('/api/faxes/', $data, 'POST', $request_headers);

      return isset($headers['http_code']) && $headers['http_code'] == 'HTTP/1.1 201 Created';
    }
    return FALSE;
  }
}



