<?php
if (!function_exists('watchdog')) {
  function watchdog($a, $b) {
    return;
  }
}

/**
 * 
 * @author rgraat
 *
 */
class Sipwise {

  private static function _parse_response($response) {
    $headers = array();

    $header_text = substr($response, 0, strpos($response, "\r\n\r\n"));
    
    $body = substr($response, strlen($header_text) + 4);
    
    foreach (explode("\r\n", $header_text) as $i => $line) {
      
      if ($i === 0) {
        $headers['http_code'] = $line;
      }
      else {
        list ($key, $value) = explode(': ', $line);
        $headers[$key] = $value;
      }
    }
    return array($headers, $body);
  }

  /**
   * 
   * @param string $url
   * @param array $data
   * @param string $customrequest
   *
   * @return mixed|multitype:string multitype:multitype: unknown
   */
  public static function exec($url, $data = NULL, $customrequest = 'GET', $custom_request_headers = NULL) {

    global $user;

    global $conf;
    global $curl_debug_cmd;
    static $ch = array();


    if (!$reseller_id = sipwise_api_get_active_reseller()) {
      if (!empty($user->data['sipwise']['reseller_id'])) {
        $reseller_id = $user->data['sipwise']['reseller_id'];
      }
      else {
        watchdog('hpbx', 'cant find active reseller_id');
        die();
      }
    }

    $request_headers = array(
      'Prefer: return=representation',
      'Content-Type: application/json',
      'Expect: '
    );
    
    if (!is_null($custom_request_headers)) {
      $request_headers = $custom_request_headers;
    }

    if (!isset($ch[$reseller_id])) {

      // Init the curl handler.
      $ch[$reseller_id] = curl_init();
      $options = array(
        CURLOPT_SSLCERT => $conf['sipwise'][$reseller_id]['sipwise_api_cert'],
        CURLOPT_SSLKEY  => $conf['sipwise'][$reseller_id]['sipwise_api_cert'],
        CURLOPT_SSL_VERIFYPEER => FALSE,
        CURLOPT_RETURNTRANSFER => TRUE,
        CURLOPT_SSL_VERIFYHOST => FALSE,
        CURLOPT_IPRESOLVE => CURL_IPRESOLVE_V4,
      );
      curl_setopt_array($ch[$reseller_id] , $options);
    }

    curl_setopt($ch[$reseller_id], CURLOPT_HEADER, TRUE);

    // Construct URL.
    $url = $conf['sipwise'][$reseller_id]['sipwise_api_endpoint'] . $url;
    
    // Set url.
    curl_setopt($ch[$reseller_id], CURLOPT_URL, $url);

    if (!is_null($customrequest) && in_array($customrequest, array('DELETE', 'PATCH', 'POST',  'GET', 'PUT'))) {
      curl_setopt($ch[$reseller_id], CURLOPT_CUSTOMREQUEST, $customrequest);
    }

    $debug_request_headers = array();
    foreach ($request_headers as $rh) {
      $debug_request_headers[] = "-H '". $rh . "'";
    }

    if (!is_null($data)) {

      if (!is_scalar($data)) {
        
        // For empty arrays json encode does convert to empty object. To keep the curly brackets as pre/suf-fix.
        if (!count($data)) {
          $data = new stdClass();
        }

        if (is_array($data) && isset($data['json'])) {

          $data['json'] = json_encode($data['json']);

          curl_setopt($ch[$reseller_id], CURLOPT_POSTFIELDS, $data);
          curl_setopt($ch[$reseller_id], CURLOPT_SAFE_UPLOAD, TRUE);

          // Debugging info.
          $debug_post_data = array();
          $debug_post_data[] = "--form 'json=". $data['json'] . "'";

          foreach ($data as $key => $form_data) {
            if (is_scalar($form_data)) {
              $debug_post_data[] = "--form '". $key ."=". $form_data . "'";
            }
            elseif(is_object($form_data) && !empty($form_data->name)) {
              $debug_post_data[] = "--form '". $key ."=". $form_data->name . "'";
            }
          }
          $curl_debug_cmd = sprintf("curl -i -X ". $customrequest ." ".implode(' ', $debug_request_headers) ." --cert %s --insecure '%s' %s", $conf['sipwise'][$reseller_id]['sipwise_api_cert'], $url, implode(' ', $debug_post_data));
        }
        else {
          $curl_debug_cmd = sprintf("curl -i -X ". $customrequest ." ".implode(' ', $debug_request_headers) ."  --cert %s --insecure '%s' --data-binary '%s'", $conf['sipwise'][$reseller_id]['sipwise_api_cert'], $url, json_encode($data));
          curl_setopt($ch[$reseller_id], CURLOPT_POSTFIELDS, json_encode($data));
          curl_setopt($ch[$reseller_id], CURLOPT_POST, TRUE);
        }


        //fwrite($f, json_encode($data));
        watchdog('sipwise', $curl_debug_cmd);
      }
      else {
        curl_setopt($ch[$reseller_id], CURLOPT_POSTFIELDS, $data);
        curl_setopt($ch[$reseller_id], CURLOPT_POST, TRUE);
      }

      curl_setopt($ch[$reseller_id], CURLOPT_BINARYTRANSFER, TRUE);
      curl_setopt($ch[$reseller_id], CURLOPT_HTTPHEADER, $request_headers);
    }
    else {

      // Verify if request headers where intentionally passed.
      if (!is_null($custom_request_headers)) {

        // Yes; pass them to curl.
        curl_setopt($ch[$reseller_id], CURLOPT_HTTPHEADER, $request_headers);
      }

      $curl_debug_cmd = sprintf("curl -i -X ". $customrequest ." ".implode(' ', $debug_request_headers) ." --cert %s --insecure '%s'", $conf['sipwise'][$reseller_id]['sipwise_api_cert'], $url);
        watchdog('sipwise', $curl_debug_cmd);
    }

  	if($response = curl_exec($ch[$reseller_id])){
    // Check if any error occurred
    if (curl_errno($ch[$reseller_id])) {
      watchdog('sipwise_api', 'cURL Error: ' . curl_error($ch[$reseller_id]));

      throw new Exception();
    }

    list($headers, $body) = self::_parse_response($response);

    $result = array($headers);
    
    // Try to decode the body.
    if (!is_null(json_decode($body))) {
      
      // Decode JSON string to result object.
      $result[] = json_decode($body);
    }
    else {
      
      // No JSON; return the raw body.
      $result[] = $body;
    }

    	return $result;
	}else{
		return FALSE;
	}
  }

  /**
   * 
   * @param unknown $response
   * @throws ErrorException
   * @return boolean
   */
  public static function result($response) {

    // Execute the API request.
    if ($response) {

      // Verify if we did receive an (error) message.
      if (!empty($response->message)) {
        
        global $curl_debug_cmd;
        watchdog('sipwise', 'Exception: '. $response->message . ' ('. $response->code .') ['.$curl_debug_cmd.']');
        throw new ErrorException($response->message, $response->code);
      }
    }
    return TRUE;
  }
}  
