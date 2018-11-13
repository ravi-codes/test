<?php

class Stretto{
	
  /**
   * @param string $url
   * @param array $data
   *
   * @return array $result
   */	
	
  public static function exec($url, $data=NULL){
    global $conf;
    $username = $conf['stretto']['username'];
    $password = $conf['stretto']['password'];

    foreach($data as $key => $datum){
      $str[] =$key . "=" . $datum.PHP_EOL;	
    }	
    
    $string = implode('', $str);		    
    $dir_uri = file_stream_wrapper_get_instance_by_uri('private://');
    $tmp_file = $dir_uri->realpath().'/data.txt';
        
    if(!file_exists($tmp_file)){
 	  fopen($tmp_file, 'w+');
 	}
 	
    file_put_contents($tmp_file, $string);
    
    $curl = curl_init();
    
    $header = array(
      'Content-Type: text/plain'         
    );
    
    $url = $conf['stretto']['api_endpoint'].$url; 
    curl_setopt($curl, CURLOPT_URL,$url);
    curl_setopt($curl, CURLOPT_SSLVERSION,CURL_SSLVERSION_SSLv3);
    curl_setopt($curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
    curl_setopt($curl, CURLOPT_USERPWD,$username.":".$password);
    curl_setopt($curl, CURLOPT_HTTPHEADER,$header);
    
    if(!empty($data)){
      curl_setopt($curl, CURLOPT_POST, TRUE);
      curl_setopt($curl, CURLOPT_POSTFIELDS,file_get_contents($tmp_file));
    }
    
    curl_setopt($curl, CURLOPT_BINARYTRANSFER, TRUE);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($curl, CURLOPT_FOLLOWLOCATION, true);
    $response = curl_exec($curl);
    $httpcode = curl_getinfo($curl, CURLINFO_HTTP_CODE);
        
    if($response != null){
      $xml = simplexml_load_string($response, 'SimpleXMLElement', LIBXML_NOCDATA);
      $result = json_decode(json_encode($xml));
      return $result;
    }
    else{
      return $httpcode;
    }
  }
}
