<?php
/**
 *
 * @param string $path.
 *
 * @return string
 */
function hpbx_audio_player($path) {

  return '<embed src="'. url(drupal_get_path('module', 'hpbx') .
      "/includes/wavplayer.swf", array('absolute' => TRUE, 'language' => '')).
      '?gui=full&h=20&w=200&sound='. url($path, array('absolute' => TRUE, 'language' => '')) .'&" bgcolor="#ffffff" width="100"
    height="20" name="haxe" quality="high" align="middle" scale="noscale"
    allowscriptaccess="always" type="application/x-shockwave-flash"
    pluginspage="http://www.macromedia.com/go/getflashplayer"/>';
}

/**
 * @param $path
 *
 * @return bool|float
 */
function hpbx_audio_get_wav_duration($path) {

  if (preg_match('/\.wav$/', $path)) {
    
    if ($fp = fopen($path, 'r')) {

      $size_in_bytes = filesize($path);
      
      fseek($fp, 20);
      $rawheader = fread($fp, 16);

      $header = unpack('vtype/vchannels/Vsamplerate/Vbytespersec/valignment/vbits', $rawheader);
      $sec = ceil($size_in_bytes / $header['bytespersec']);
      return $sec;
    }
  }
  return FALSE;
}

/**
 *
 * @param int $soundfile_id
 */
function hpbx_audio_play($soundfile_id) {
  $type = FALSE;
  if (preg_match('/^play\.(ogg|mp3|wav)$/', arg(3), $matches)) {
    $type = $matches[1];
  }
  
  try {

    if ($type && $SoundFile = SipwiseEntity::load('soundfiles', $soundfile_id)) {

      // Load set the sound file is assigned to.
      if ($SoundSet = SipwiseEntity::load('soundsets', $SoundFile->set_id)) {

        
        if (!user_access('hpbx edit customer')) {
          $Customer = hpbx_get_active_customer();

          // Verify if the sound set is owned by the active customer.
          if ($SoundSet->customer_id != $Customer->id) {
            drupal_not_found();
            exit();
          }
        }

        $body = SipwiseEntity::load('soundfilerecordings', $SoundFile->id);
        
        if (!empty($body)) {
          switch ($type) {
          case 'wav':
            header('Content-Type: audio/x-wav');
            break;
          case 'mp3':
            $body = hpbx_audio_convert_wav_to_wav($body);
            $body = hpbx_audio_convert_wav_to_mp3($body);
            break;
          case 'ogg':
            header('Content-Type: audio/ogg');
            $body = hpbx_audio_convert_wav_to_ogg($body);
            break;  
          }
          header('Content-Disposition: attachment; filename="play.'. $type .'"');
          header('Content-Length: '. strlen($body));
          
          if (!empty($body)) {
            echo $body;
          }
          die();
        }
      }
    }
    else {
      drupal_not_found();
      die();
    }
  }
  catch (Exception $e) {
    die();
  }
  die();
}

/**
 * Helper function convert a wav to mp3.
 *
 * @param string $wav
 *
 * @return string
 */
function hpbx_audio_convert_wav_to_mp3($wav) {

  // Write wav to tmp file, as sox needs to stat the file on disk before it can be converted.
  $dest = file_directory_temp(). '/'. uniqid() .'.wav';
  file_put_contents($dest, $wav);

  // Convert tmp wav file to mp3.
  return shell_exec('/usr/bin/lame --silent --vbr-new -V 3 '. escapeshellarg($dest) .' -');
}

/**
 * Helper function to convert wav to ogg.
 *
 * @param string  $wav
 *
 * @return string
 */
function hpbx_audio_convert_wav_to_ogg($wav) {

  // Write wav to tmp file, as sox needs to stat the file on disk before it can be converted.
  $dest = file_directory_temp(). '/'. uniqid() .'.wav';
  file_put_contents($dest, $wav);

  // Convert tmp wav file to ogg.
  return shell_exec('/usr/local/bin/sox -t wav '.  escapeshellarg($dest).' -t ogg -C 10 -c 2 -');
}

/**
 * Helper function to convert wav to correct wav format.
 *
 * @param string $wav
 *
 * @return string
 */
function hpbx_audio_convert_wav_to_wav($wav) {

  // Write wav to tmp file, as sox needs to stat the file on disk before it can be converted.
  $dest = file_directory_temp(). '/'. uniqid() .'.wav';
  file_put_contents($dest, $wav);

  return shell_exec('/usr/local/bin/sox -t wav '.  escapeshellarg($dest).' -t wav -e signed-integer -');
}

/**
 * Retrieve system supported audio file extensions.
 *
 * @return array
 */
function hpbx_audio_get_valid_file_extensions() {

  static $extensions = NULL;

  if (is_null($extensions)) {
    $extensions = explode(" ", shell_exec("/usr/local/bin/sox -h |grep \"AUDIO FILE\""));
  }
  return $extensions;
}



/**
 * Helper function to convert the passed file to wav.
 *
 * @param string $path
 * @param string $filename
 *
 * @return string
 */
function hpbx_audio_convert_to_wav($path, $filename) {

  $ext = pathinfo($filename, PATHINFO_EXTENSION);

  if (!in_array(strtolower($ext), hpbx_audio_get_valid_file_extensions())) {
    drupal_set_message(t('The file extension @ext is not supported', array('@ext' => $ext)), 'error');
    return FALSE;
  }

  if (file_exists($path)) {
    $dest = file_directory_temp(). '/'. uniqid() .'-'. $filename;

    if (ccs_copy($path, $dest)) {

      $cmd = '/usr/local/bin/sox '. escapeshellarg($dest) .' -t wav -r 8000 -c 1 - ';

      $descriptorspec = array(
          0 => array("pipe", "r"),  // stdin is a pipe that the child will read from
          1 => array("pipe", "w"),  // stdout is a pipe that the child will write to
          2 => array("pipe", "w"),
      );
      
      $process = proc_open($cmd, $descriptorspec, $pipes);
      
      if (is_resource($process)) {
        // $pipes now looks like this:
        // 0 => writeable handle connected to child stdin
        // 1 => readable handle connected to child stdout
        // Any error output will be appended to /tmp/error-output.txt
      
        $wav = stream_get_contents($pipes[1]);
        fclose($pipes[1]);
      
        $errors = stream_get_contents($pipes[2]);
        fclose($pipes[2]);

        // It is important that you close any pipes before calling
        // proc_close in order to avoid a deadlock
        proc_close($process);
        
        if (preg_match('/FAIL/', $errors)) {
          drupal_set_message(t('Failed to upload the audio file. Possibly this is a corrupt audio file or invalid extension in combination with used audio codec.'), 'error ');
          return FALSE;
        }
        return $wav;
      }
    }
    else {
      watchdog(__FUNCTION__, "Copy failed from ". $path . " to ". $dest);
    }
  }
  else {
    watchdog(__FUNCTION__, "File ". $path . "doesn't exists");
  }
  return FALSE;
}
