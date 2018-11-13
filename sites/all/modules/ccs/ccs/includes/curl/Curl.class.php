<?php
class Curl {

	public $response, $username, $password;

	public $ch;
  public $url;
  public $postfields;
  public $default_http_headers;
	function __construct($gzip = FALSE) {

	  $f = fopen('/tmp/curlstderr', 'w');

		$this->ch = curl_init();
		$this->default_http_headers = array();
		$this->default_http_headers[] = "Accept: application/octet-stream";
		$this->default_http_headers[] = "Connection: keep-alive";

		curl_setopt($this->ch, CURLOPT_VERBOSE, 0);
		curl_setopt($this->ch, CURLOPT_HEADER, 0);
		curl_setopt($this->ch, CURLOPT_FOLLOWLOCATION, 1);
		curl_setopt($this->ch, CURLOPT_RETURNTRANSFER, 1);
		//curl_setopt($this->ch, CURLOPT_COOKIEFILE, '/var/www/vp/titan_trace/cookie.txt');
		curl_setopt($this->ch, CURLOPT_SSL_VERIFYPEER, 0);
		curl_setopt($this->ch, CURLOPT_SSL_VERIFYHOST, 0);
		//curl_setopt($this->ch, CURLOPT_COOKIEJAR,  '/tmp/titan_trace_cookie.txt');
		curl_setopt($this->ch, CURLOPT_HTTPHEADER, $this->default_http_headers);
		curl_setopt($this->ch, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 ); 
		curl_setopt($this->ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_setopt($this->ch, CURLOPT_USERPWD, 'a\ccs_trace:MFQf6w4BFrgCdKn');
		curl_setopt($this->ch, CURLOPT_STDERR, $f);
	}

	function __destruct() {
		//echo "destruct" . PHP_EOL;
		curl_close($this->ch);
	}

	function do_request($url, $data = NULL, $json = FALSE, $headers = array()) {
		$this->url = $url;

		curl_setopt($this->ch, CURLOPT_URL, $this->url);

		$this->postfields = '';
		if (!is_null($data)) {

		  if (is_array($data)) {
			  $this->postfields = http_build_query($data);
		  }
		  else {
		    $this->postfields = $data;
		  }

			if (is_null($headers)) {
			  $headers = array();
			}

			$http_header = array_merge($this->default_http_headers, $headers, array(
				"Content-Type: application/x-www-form-urlencoded",
			));
			curl_setopt($this->ch, CURLOPT_POSTFIELDS, $this->postfields);
			curl_setopt($this->ch, CURLOPT_HTTPHEADER, $http_header);
		}
		else {
			curl_setopt($this->ch, CURLOPT_HTTPHEADER, $this->default_http_headers);
			curl_setopt($this->ch, CURLOPT_HTTPGET, 1);
		}

		$this->response = curl_exec($this->ch);

		if (curl_errno($this->ch)) {
      echo "cURL error: ". curl_error($this->ch) . PHP_EOL;
			curl_close($this->ch);
			return false;
		}
		return TRUE;
	}
}

