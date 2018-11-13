<?php

function hpbx_pbxdevice_model_image_callback($profile_id) {
  if ($PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($profile_id)) {
    echo SipwiseEntity::load('pbxdevicemodelimages', $PBXDeviceModel->id);
    die();
  }
  drupal_not_found();
  die();
}

function hpbx_pbxdevice_model_json_callback($profile_id) {
  
  if ($PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($profile_id)) {
    echo json_encode($PBXDeviceModel->linerange);
    die();
  }
}

function hpbx_pbxdevice_get_line_type_key_options($PBXDeviceModel, $linerange_id = NULL, &$linerange_name = NULL) {

  $options = array();
  $options['type'] = array();
  $options['line_key'] = array();

  foreach ($PBXDeviceModel->linerange as $linerange) {

    if ($linerange->id == $linerange_id || $linerange_name == $linerange->name) {

      $linerange_name = $linerange->name;
      if ($linerange->can_private) {
        $options['type']['private'] = t('Private Line');
      }
      if ($linerange->can_shared) {
        $options['type']['shared'] = t('Shared Line');
      }
      if ($linerange->can_blf) {
        $options['type']['blf'] = t('Busy Lamp Field');
      }

      $options['line_key'] = array();
      for ($i=0;$i<$linerange->num_lines;$i++) {
        $options['line_key'][$i] = $i+1;
      }
      break;
    }
  }
  return $options;
}

function hpbx_pbxdevice_get_model_on_profile($profile_id) {
  $PBXDeviceProfile = SipwiseEntity::load('pbxdeviceprofiles', $profile_id);
  $PBXDeviceConfig = SipwiseEntity::load('pbxdeviceconfigs', $PBXDeviceProfile->config_id);
  $PBXDeviceModel = SipwiseEntity::load('pbxdevicemodels', $PBXDeviceConfig->device_id);
  return $PBXDeviceModel;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function hpbx_pbxdevice_edit_form_validate($form, &$form_state) {

  global $ActiveReseller;
  if ($json = json_decode($form_state['values']['lines'])) {
    $form_state['values']['lines'] = $json;
  }
  else {
    die('invalid input');
  }
  
  drupal_add_css(drupal_get_path('module', 'hpbx') . '/includes/css/hpbx_devices.css');
  $Customer = hpbx_get_active_customer();
  
  $lines = array();
  
  // @todo Validate lines (related to customer etc)
  
  if (!count($form_state['values']['lines'])) {
    form_set_error('lines', t('At lease one line must be defined. Enter the line details and hit the "Add" button to add a new line.'));
  }

  // Verify if a huntgroup has a non-blf key assigned.
  foreach ($form_state['values']['lines'] as $line) {

    if (!is_null($line->subscriber_id) && $Subscriber = SipwiseEntity::load('subscribers', $line->subscriber_id)) {

      // Verify ownership.
      if ($Subscriber->customer_id!=$Customer->id) {
        form_set_error('', t('Invalid subscriber'));
      }
      elseif ($line->line_type != 'blf' && $Subscriber->is_pbx_group) {
        form_set_error('', t('A hunt group can only be assigned to a BLF key'));
      }
      elseif ($line->line_type == 'blf' && !$Subscriber->is_pbx_group && !hpbx_attribute_allowed('blf', $Subscriber->profile_id)) {
        form_set_error('', t('Subscriber cannot be assigned to a BLF key, as it is not allowed for the subscriber profile'));
      }
    }
  }

  if (!empty($form_state['values']['table']['identifier']['value'])) {

    // Remove non-alphanumeric characters, trim whitespace and make uppercase.
    $form_state['values']['table']['identifier']['value'] = strtoupper(trim(preg_replace('/\W/', '',$form_state['values']['table']['identifier']['value'])));

    if (strlen($form_state['values']['table']['identifier']['value'])!=12) {
      form_set_error('table][identifier', t('MAC address is invalid, please check the MAC address'));
    }

    if (!ctype_xdigit($form_state['values']['table']['identifier']['value'])) {
      form_set_error('table][identifier', t('MAC address is invalid, please check the MAC address'));
    }
    
    // Verify if the mac address is not already taken.
    foreach (sipwise_api_get_all('pbxdevices', array('identifier' => $form_state['values']['table']['identifier']['value'] )) as $PBXDevice) {
      if (strtoupper($PBXDevice->identifier) == $form_state['values']['table']['identifier']['value']) {

        if (!$form_state['values']['id'] || $form_state['values']['id']!=$PBXDevice->id) {
          
          // In case the device is owned by this customer
          if ($PBXDevice->customer_id == $Customer->id) {
            list($station_name) = explode(':', $PBXDevice->station_name);
            form_set_error('table][identifier', t('MAC address is already assigned to device with device name @station_name', array('@station_name' => $station_name)));
          }
          else {
            form_set_error('table][identifier', t('MAC Address is already assigned to another device'));
          }
        }
      }
    }

    // Unfortunatly we have to check at this place if the Panasonic RPS server will accept our
    // request. So we will add and delete the device on the RPS server, also for Yealink.
    
    // Get linerange info based on passed profile_id -> model -> linerange
    $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($form_state['values']['table']['profile_id']['value']);
    
    if (!empty($PBXDeviceModel->bootstrap_method) && substr($PBXDeviceModel->bootstrap_method,0,strlen('redirect_')) == 'redirect_') {
      require_once(drupal_get_path('module', 'hpbx_rps') .'/includes/rps.class.inc');
    
      list($f, $class) = explode('_', $PBXDeviceModel->bootstrap_method);
    
      $class_path = drupal_get_path('module', 'hpbx_rps') .'/includes/'. strtolower($class) .'.class.inc';
    
      if (file_exists($class_path)) {
        require_once($class_path);

        $RPSDevice = new $class($form_state['values']['table']['identifier']['value'], $form_state['values']['table']['profile_id']['value']);
        if($class == 'grandstream'){
          if ($form_state['values']['id']) {
      	   // Existing pbx device..
      	    $PBXDevice = SipwiseEntity::load('pbxdevices', $form_state['values']['id']);
           }
          if (!$PBXDevice->id) {
            if (!$RPSDevice->register()) {
             // Could not register.
              form_set_error('table][identifier', t('MAC address is invalid, please check the MAC address'));
             }
             else {
               // Device successfully registered, so unregister it again.
               $RPSDevice->unregister();
             }
          } 
        }else{
          // Verify if the device is not yet registered at RPS.
          if (!$RPSDevice->is_registered()) {
            // Register.
           if (!$RPSDevice->register()) {
             // Could not register.
             form_set_error('table][identifier', t('MAC address is invalid, please check the MAC address'));
           }
           else {
            // Device successfully registered, so unregister it again.
             $RPSDevice->unregister();
           }
        }
      }
      }
    }
  }
}

/**
 *
 * @param array $form
 * @param array $form_state
 */
function hpbx_pbxdevice_edit_form_submit($form, &$form_state) {
  try {

    if (!$form_state['values']['id']) {

      // New pbx device..
      $PBXDevice = SipwiseEntity::create('pbxdevices');

      // Set.
      $PBXDevice->customer_id = $form_state['values']['table']['customer_id'];
      $PBXDevice->profile_id = trim($form_state['values']['table']['profile_id']['value']);

      // Create salt.
      $unique_id = uniqid();
    }
    else {

      // Existing pbx device..
      $PBXDevice = SipwiseEntity::load('pbxdevices', $form_state['values']['id']);

      if (strpos($PBXDevice->station_name, ':')===FALSE) {
        // No salt created yet. Create it.
        $unique_id = uniqid();
      }
      else {
        list($false, $unique_id) = explode(':', $PBXDevice->station_name);
      }
    }

    $PBXDevice->identifier = strtoupper(trim($form_state['values']['table']['identifier']['value']));
    $PBXDevice->station_name = trim($form_state['values']['table']['station_name']['value']) . ':'. $unique_id;
    $PBXDevice->lines = array();

    // Get linerange info based on passed profile_id -> model -> linerange
    $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($form_state['values']['table']['profile_id']['value']);
    
    // Walk all line input rows.
    foreach ($form_state['values']['lines'] as $line) {

      if (!is_null($line->subscriber_id)) {

        $PBXDevice->lines[] = array(
          'subscriber_id' => $line->subscriber_id,
          'key_num' => $line->key_num,
          'type' => $line->line_type,
          'linerange' => $line->linerange,
        );
      }
    }

    // Save the pbx device.
    if ($PBXDevice->save()) {

      $_SESSION['ccs']['hpbx']['wizard']['device_id'] = $PBXDevice->id;
      
      
      if (!empty($PBXDeviceModel->bootstrap_method) && substr($PBXDeviceModel->bootstrap_method,0,strlen('redirect_')) == 'redirect_') {
        require_once(drupal_get_path('module', 'hpbx_rps') .'/includes/rps.class.inc');
        
        list($f, $class) = explode('_', $PBXDeviceModel->bootstrap_method); 
        
        $class_path = drupal_get_path('module', 'hpbx_rps') .'/includes/'. strtolower($class) .'.class.inc';
        
        if (file_exists($class_path)) {
          require_once($class_path);
          $RPSDevice = new $class($PBXDevice->identifier, $PBXDevice->profile_id);
          // Verify if the device is not yet registered at RPS.
        if($class == 'grandsteam'){
            if (!$PBXDevice->id) {
              // Register.
            if (!$RPSDevice->register()) {
              // Remove the device from Sipwise again.
              $PBXDevice->delete();
              $PBXDevice->id='';
              drupal_set_message(t('MAC address is invalid, please check the MAC address')); 
            }
           }
        }else{
         if (!$RPSDevice->is_registered()) {
            // Register.
            if (!$RPSDevice->register()) {
              // Remove the device from Sipwise again.
              $PBXDevice->delete();

              // Tell us what went wrong.
              throw new Exception(t('MAC address is invalid, please check the MAC address'));
            }
          }
        }
        }
      }
      
    }
  }
  catch (Exception $e) {

    // Failed to create customer.
    drupal_set_message(t('Failed to update'), 'error');

    //
    drupal_set_message(t($e->getMessage()), 'error');
  }

  if (empty($form_state['values']['no_redirect'])) {
    $output = array('status' => 'ok');
    
    if (!empty($PBXDeviceModel->bootstrap_method) && substr($PBXDeviceModel->bootstrap_method,0,strlen('redirect_')) == 'redirect_') {
      // RPS device; reboot;
      $output['type'] = 'reboot';
    }
    else if($class =='grandstream'){
       $output['type'] = 'reboot';
    }
    else {
      
      if ($sync_url = hpbx_pbxdevice_get_sync_url($PBXDevice->id, url('hpbx/pbxdevice/'. $PBXDevice->id . '/sync', array('absolute' => TRUE)))) {
        $output['sync_url'] = $sync_url;
        $output['id'] = $PBXDevice->id;
      }
      
      if ($form_state['values']['id']) {
        // Existing cisco device; reboot or sync.
        $output['type'] = 'sync_reboot';
      }
      else {
        // non-existing cisco device; sync.
        $output['type'] = 'sync';
      }
    }
    
    echo json_encode($output);
    
    form_clear_error();
    drupal_get_messages(NULL, TRUE);
    die();
  }
}

/**
 * Helper function to retrieve the max version number of the available firmwares.
 *
 * @param string $identifier
 *
 * @return $max_version.
 *   String containing the max version.
 */
function hpbx_device_get_max_firmware_version($profile_id) {

  $max_version = FALSE;

  // Get device module on profile.
  $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($profile_id);

  // Get list of firmwares.
  $PBXDeviceFirmwares = sipwise_api_get_all('pbxdevicefirmwares', array('device_id' => $PBXDeviceModel->id));

  // Extract all version numbers.
  $versions = array();
  foreach ($PBXDeviceFirmwares as $PBXDeviceFirmware) {
    $versions[] = $PBXDeviceFirmware->version;
  }

  // Calculate the max version.
  $max_version = '';
  foreach ($versions as $version) {
    if (version_compare($max_version, $version)) {
      $max_version = $version;
    }
  }
  return $max_version;
} 


/**
 *
 * @param array $form
 * @param array $form_state
 * @param array $customer_id
 *
 * @return Ambigous <multitype:string , multitype:string boolean multitype:multitype:string  multitype:string The  multitype:string Ambigous <The, string, unknown, A, Returns, Optional>   >
 */
function hpbx_pbxdevices_overview_form($form, $form_state, $customer_id = NULL) {

  drupal_set_title(t('Devices'));

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  $Customer = hpbx_get_active_customer($customer_id);

  $form = array();  

  $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">'. t('Create device') . '</h5>';
  $add = l($text, 'hpbx/pbxdevice/add', array('html' => TRUE, 'attributes' => array('class' => array('hpbx-table-caption-link'))));

  $form['table'] = array(
    '#add' => array('data' => $add, 'attributes' => array('class' => 'hpbx-table-caption')),
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-pbx-devices hpbx-table-sortable'),
    '#header' => array(
      'station_name' => array('data'=> t('Device name'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'profile' => array('data'=> t('Brand and model'), 'class' => array('hpbx-sortable', 'hpbx-show')),
      'identifier' => array('data'=> t('MAC address'), 'class' => array('hpbx-sortable', 'hpbx-show-sm')),
      'operations' => array('data'=> '', 'class' => array('hpbx-show')),
    ),
    '#remove_title' => t('Delete device'),
  );

  if ($Customer) {

    // Set customer_id.
    $query = array('customer_id' => $Customer->id);
    foreach (sipwise_api_get_all('pbxdevices', $query, TRUE, TRUE) as $PBXDevice) {

      list($station_name) = explode(':', $PBXDevice->station_name);
      
      $form['table'][$PBXDevice->id]['#url'] = 'pbxdevice/'. $PBXDevice->id . '/edit';
      $form['table'][$PBXDevice->id]['#attributes']['data-delete-url'] = 'pbxdevice/'. $PBXDevice->id . '/delete';
      
      $form['table'][$PBXDevice->id]['station_name']['data'] = array('#markup' => hpbx_truncate_name_element($station_name), '#attributes' => array('title' => check_plain($station_name)));
      $form['table'][$PBXDevice->id]['station_name']['#attributes']['class'] = array('hpbx-show hpbx-nowrap');
      
      $form['table'][$PBXDevice->id]['profile']['data'] = array('#markup' => $PBXDevice->pbxdeviceprofiles->name);
      $form['table'][$PBXDevice->id]['profile']['#attributes']['class'] = array('hpbx-show');
      
      $form['table'][$PBXDevice->id]['identifier']['data'] = array('#markup' => hpbx_get_mac_address_from_identifier($PBXDevice->identifier));
      $form['table'][$PBXDevice->id]['identifier']['#attributes']['class'] = array('hpbx-show-sm');

      $op = array();
      // Check what the advised bootstrap method is.
      $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id);
      
      if (!empty($PBXDeviceModel->bootstrap_method) && $PBXDeviceModel->bootstrap_method == 'http') {
        $op[] = l(t('Sync'), 'hpbx/pbxdevice/'. $PBXDevice->id . '/sync', array('attributes' => array('class' => array('sg-btn sg-btn-small', 'sg-without-icon', 'sg-btn-primary'))));
      }
      $form['table'][$PBXDevice->id]['operations']['data'] = array('#markup' => implode(' ', $op));
      $form['table'][$PBXDevice->id]['operations']['#attributes']['class'] = array('hpbx-show');
    }
  }

  return $form;
}

function hpbx_pbxdevice_delete_page($pbxdevice_id) {
  global $ActiveReseller;
  try {

    if ($PBXDevice = SipwiseEntity::load('pbxdevices', $pbxdevice_id)) {

      // Load active customer.
      $Customer = hpbx_get_active_customer();

      // Verify if the device to be deleted is owned by the active customer.
      if ($PBXDevice->customer_id != $Customer->id) {
        drupal_not_found();
        exit();
      }
      else {

        
        
        $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id);
        
        $PBXDevice->delete();
        
        if (!empty($PBXDeviceModel->bootstrap_method) && substr($PBXDeviceModel->bootstrap_method,0,strlen('redirect_')) == 'redirect_') {
          require_once(drupal_get_path('module', 'hpbx_rps') .'/includes/rps.class.inc');
        
          list($f, $class) = explode('_', $PBXDeviceModel->bootstrap_method);

          $class_path = drupal_get_path('module', 'hpbx_rps') .'/includes/'. strtolower($class) .'.class.inc';
          
          if (file_exists($class_path)) {
            require_once($class_path);
            $RPSDevice = new $class($PBXDevice->identifier, $PBXDevice->profile_id);
            if($class == 'grandstream'){
               if (!$RPSDevice->unregister()) {
                // Tell us what went wrong.
                drupal_set_message(t('Failed to un-provision the device'));
              }
            }else{
               // Verify if the device is registered at Yealink RPS.
              if ($RPSDevice->is_registered()) {
                $RPSDevice = new $class($PBXDevice->identifier, $PBXDevice->profile_id);
              // Unregister.
                if (!$RPSDevice->unregister()) {
                  drupal_set_message(t('Failed to un-provision the device'));
                // Tell us what went wrong.
               //('Failed to unregister device');
              }
            }
          }
          }
         // Set message.
          drupal_set_message(t('Device successfully deleted'));
        }
      }
    }
  }
  catch (Exception $e) {
    drupal_set_message(t($e->getMessage()), 'error');
  }

  foreach (array('pbxdevices') as $type) {
    // Clear the Drupal cache for this API type.
    $cid = $type . '_'. $ActiveReseller->id . '_'. $PBXDevice->id.'_response';
    cache_clear_all($cid, 'cache_sipwise_api');
    watchdog('sipwise_api_cache', t('Cleared @type with @cid', array('@type' => $type, '@cid' => $cid)));

    $cid = $type . '_list_'. $ActiveReseller->id.'_';
    cache_clear_all($cid, 'cache_sipwise_api', TRUE);
    watchdog('sipwise_api_cache', t('Cleared @type list with @cid*', array('@type' => $type, '@cid' => $cid)));
  }
  drupal_goto('hpbx/pbxdevices');
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 */
function hpbx_pbxdevice_line_callback($form, $form_state) {
  return $form['lines'];
}

function hpbx_pbxdevice_profile_callback($form, $form_state) {
  return $form['lines'];
}

function hpbx_pbxdevice_get_sync_url($pbxdevice_id, $ref = NULL) {

  
  global $conf;
  global $ActiveReseller;
  
  if ($PBXDevice = SipwiseEntity::load('pbxdevices', (int)$pbxdevice_id)) {
    // Load active customer.
    $Customer = hpbx_get_active_customer();
    
    if ($PBXDevice->customer_id != $Customer->id) {
      drupal_not_found();
      exit();
    }
    
    // Get (secrect) unique id from station name, use it to create public hash.
    list($false, $unique_id) = explode(':', $PBXDevice->station_name);
    

    $hash = md5(drupal_get_hash_salt() . $unique_id . $PBXDevice->identifier);
    
    $prov_url = $conf['sipwise'][$ActiveReseller->id]['sipwise_prov_endpoint'] . '/sync/'. $hash . '/$MA/?';
    
    if (is_null($ref)) {
      global $base_url, $language;
      $ref = $base_url . '/'. $language->language . '/'. $_GET['q'];
    }
    if($PBXDevice->profile_id != '3'){
      //for LGI environments only
     $prov_url = str_replace( 'http://', 'https://', $prov_url );
     // return 'http://www.ccswebtool.com/hpbx/link.php?ref='. $ref . '&url='. $prov_url . '&ip=';
      return $conf['sipwise'][$ActiveReseller->id]['sipwise_prov_endpoint'] .'/link.php?ref='. $ref . '&url='. $prov_url . '&ip=';
    }else{
      return 'http://www.ccswebtool.com/hpbx/link.php?ref='. $ref . '&url='. $prov_url . '&ip=';
    }
  }
  return FALSE;
}

function hpbx_pbxdevice_sync_form($form, &$form_state, $pbxdevice_id) {


  try {

      drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

      $form = array(
        '#prefix' => '<div class="hpbx-main-content-inner hpbx-sync-device">',
        '#suffix' => '</div>',
        '#attributes' => array('class' => array('hpbx-tab-form')),
      );

      $form['table'] = array(
        '#type' => 'container',
        '#collapsible' => FALSE,
        '#tree' => FALSE,
      );

      $items = array();
      $items['items'] = array();
      
      if ($PBXDevice = SipwiseEntity::load('pbxdevices', $pbxdevice_id)) {
      
        $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id);
      
        if ($PBXDeviceModel->vendor == 'Cisco' && ($PBXDeviceModel->model == 'ATA112' || $PBXDeviceModel->model == 'ATA122')) {
          $items['items'][] = t('1. Pick up the phone connected to the ATA and call * * * *');
          $items['items'][] = t('2. Enter 1 1 0 #');
          $items['items'][] = t('3. You will now hear a message giving you the IP address');
        }
        elseif ($PBXDeviceModel->vendor == 'Cisco' && $PBXDeviceModel->model == 'SPA232D') {
          $items['items'][] = t('1. Enter the phone menu');
          $items['items'][] = t('2. Scroll to settings and press select');
          $items['items'][] = t('3. Scroll to phone info and press select');
          $items['items'][] = t('4. Scroll to view IP Address. This is the IP address of your device');
        }
        else {
          $items['items'][] = t('1. Press the Setup button.');
          $items['items'][] = t('2. Scroll to Network and press select.');
          $items['items'][] = t('3. Scroll to view Current IP. This is the IP address of your device.');
        }
      }
            
      $form['table']['ip'] = array(
        '#type' => 'textfield',
        '#title' => t('IP Address'),
        '#size' => 20,
        '#maxlength' => 15,
        '#attributes' => array(
          'class' => array('sg-element'),
          'placeholder' => t('IP Address'),
          'data-invalid-messages' => t('This field is mandatory___The given value must be a valid IP address'),
          'data-validation-regex' => '^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$',
        ),
        '#description' => theme('item_list', $items),

      );

      // Submit button.
      $form['table']['actions'] = array(
        '#type' => 'actions',
        '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
        '#prefix' => '<div class="row">',
        '#suffix' => '</div>'
      );
      $form['table']['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Sync device'),
        '#prefix' => '<span class="col-xs-12">',
        '#suffix' => '</span>',
        '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
      );


      if ($sync_url = hpbx_pbxdevice_get_sync_url((int)$pbxdevice_id)) {
$js = <<<EOF
(function ($) {
  $('document').ready(function(){
    $('#hpbx-pbxdevice-sync-form input#edit-submit').click(function(){

      window.location = "{$sync_url}" + $('input#edit-ip').val();
      return false;    
    });
  });
})(jQuery);
EOF;
      
drupal_add_js($js, array('type' => 'inline'));
      }

      
      return $form;
  }
  catch (Exception $e) {
    drupal_set_message(t($e->getMessage()), 'error');
  }
}

function hpbx_pbxdevice_sync_failed_page($device_id) {
  drupal_set_message(t('The request could not be sent to the device. Possible causes: <br/><br/>The device is already associated.<br/>The device is not connected to the LAN<br/>The IP adress is incorrect'), 'error');
  drupal_goto('hpbx/pbxdevice/' . $device_id . '/sync');
}

function hpbx_pbxdevice_sync_success_page($device_id) {

  drupal_set_message(t('The request is successfully sent to device. The device will now try to retrieve the profile and one or more firmware versions.'));
  drupal_goto('hpbx/pbxdevices');
}

function hpbx_device_get_subscriber_list() {
  $Customer = hpbx_get_active_customer();
  
  // Get all subscribers to fill the options.
  $subscriber_options = array();
  $sort_names = array();
  foreach ($Customer->get_subscribers($is_pbx_pilot = FALSE, $is_pbx_group = TRUE) as $Subscriber) {

    // Huntgroup can have BLF others are configurable using reseller settings.
    $can_blf = !$Subscriber->is_pbx_group ? hpbx_attribute_allowed('blf', $Subscriber->profile_id) : TRUE;
    $can_shared = $can_private = !$Subscriber->is_pbx_group;

    if ($can_blf || $can_shared || $can_private) {
      if ($ad_entry = $Subscriber->get_active_directory_entry()) {
        $subscriber_options[$Subscriber->id]['name'] = $ad_entry['sn'][0]. ', '. $ad_entry['givenname'][0];
      }
      else {
        $subscriber_options[$Subscriber->id]['name'] = $Subscriber->external_id;
      }

      $sort_names[] = $subscriber_options[$Subscriber->id]['name'];
      $subscriber_options[$Subscriber->id]['alias_number'] = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
      $subscriber_options[$Subscriber->id]['extension'] = $Subscriber->pbx_extension;
      $subscriber_options[$Subscriber->id]['in_use'] = FALSE;

      $subscriber_options[$Subscriber->id]['can_blf'] = $can_blf;

      // HuntGroups are not allowed to be assigned to a private or shared line.
      $subscriber_options[$Subscriber->id]['can_private'] = $can_private;
      $subscriber_options[$Subscriber->id]['can_shared'] = $can_shared;
    }
  }

  // already assigned to a phone.
  $subscriber_in_use = array();
  foreach (sipwise_api_get_all('pbxdevices', array('customer_id' => $Customer->id)) as $PBXDevice_in_use) {
    foreach ($PBXDevice_in_use->lines as $line_in_use) {
      if ($line_in_use->type == 'private') {
        $subscriber_options[$line_in_use->subscriber_id]['in_use'] = TRUE;
      }
    }
  }
  
  // Sort based on $sort_names.
  $a = array($subscriber_options, $sort_names);
  $keys = array_keys($subscriber_options);
  
  array_multisort($a[0], SORT_ASC, SORT_NUMERIC,
  $a[1], SORT_ASC, SORT_NATURAL|SORT_FLAG_CASE,
  $keys);

  $subscriber_options = array_combine($keys, $a[0]);
  return $subscriber_options;
}

/**
 * @param array$form
 * @param array$form_state
 * @param integer|null $pbxdevice_id
 * @return array
 */
function hpbx_pbxdevice_edit_form($form, &$form_state, $pbxdevice_id = NULL) {

  global $user;

  $Customer = hpbx_get_active_customer();

  // Add table styling.
  drupal_add_css(drupal_get_path('module', 'hpbx') . '/includes/css/hpbx_devices.css');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_devices.js', array('scope' => 'footer'));

  drupal_add_js(array('hpbx_subscriber_list' => hpbx_device_get_subscriber_list()), 'setting');
  
  // Request Customer values.
  $PBXDevice = NULL;

  $station_name = '';

  // Verify if the customer_id is passed.
  if (!is_null($pbxdevice_id)) {
    if ($PBXDevice = SipwiseEntity::load('pbxdevices', $pbxdevice_id)) {


      // Verify if the current customer is the owner of this device.
      if ($Customer->id != $PBXDevice->customer_id) {
        drupal_not_found();
        exit();
      }

      $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id);
      
      $linerange_name_id = array();
      foreach ($PBXDeviceModel->linerange as $linerange) {
        if (!isset($linerange_name_id[$linerange->name])) {
          $linerange_name_id[$linerange->name] = $linerange->id;
        }
      }

      foreach ($PBXDevice->lines as $key => &$line) {
        $line->linerange_id = $linerange_name_id[$line->linerange];

        // Verify if subscriber exists, otherwise remove the line.
        if (!SipwiseEntity::load('subscribers', $line->subscriber_id)) unset($PBXDevice->lines[$key]);
      }
      
      if (!empty($PBXDevice->station_name)) {

        // Extract the station name from the station_name:salt (which is needed for security of the sync process.
        list($station_name, $salt) = explode(':', $PBXDevice->station_name);
      }
    }
    else {
      drupal_not_found();
      exit();
    }
    drupal_set_title('');
  }
  else {
    drupal_set_title(t('Add device'));
  }

  $form = array(
    '#prefix' => '<div class="hpbx-main-content-inner hpbx-add-device" style="display:none">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  $form['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );
  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($PBXDevice->id) ? $PBXDevice->id : FALSE,
  );

  // Set the customer id.
  $form['table']['customer_id'] = array(
    '#type' => 'value',
    '#value' => !empty($PBXDevice->customer_id) ? $PBXDevice->customer_id : $Customer->id,
  );

  // Station name.
  $form['table']['station_name']['value'] = array(
    '#type' => 'textfield',
    '#default_value' =>  $station_name,
    '#size' => 30,
    '#required' => TRUE,
    '#title' => t('Name'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Name')),
  );

  // Device profiles.
  $devices = array();

  if ($pbxdevicemodels = sipwise_api_get_all('pbxdevicemodels')) {
    foreach ($pbxdevicemodels as $PBXDeviceModel) {

      if ($PBXDeviceConfigs = sipwise_api_get_all('pbxdeviceconfigs', array('device_id' => $PBXDeviceModel->id))) {
        foreach ($PBXDeviceConfigs as $PBXDeviceConfig) {
          foreach (sipwise_api_get_all('pbxdeviceprofiles', array('config_id' => $PBXDeviceConfig->id)) as $PBXDeviceProfile) {

            $dev = array(
              'value' => $PBXDeviceProfile->id,
              'label' => $PBXDeviceProfile->name,
            );
            $devices[$PBXDeviceModel->vendor][] = $dev;
            $options2[$PBXDeviceProfile->id] = $PBXDeviceProfile->name;
          }
        }
      }
    }
  }

  $vendors = array_keys($devices);
  natsort($vendors);

  $options = array();
  foreach ($vendors as $vendor) {
    natsort($devices[$vendor]);
    $options[$vendor] = array(
      'label' => $vendor,
      'types' => $devices[$vendor]
    );
  }
  
  // Can only be adjusted when creating the device.
  $form['table']['vendor']['value'] = array(
    '#type' => 'select',
    '#options' => drupal_map_assoc($vendors),
    '#header' =>  array(t('Type')),
    //'#default_value' => !empty($PBXDevice->profile_id) ? hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id)->vendor : NULL,
    '#empty_option' => t('Choose a brand'),
    '#required' => TRUE,
    '#title' => t('Brand'),
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-placeholder' => t('Choose an option'),
      'data-custom-change-event' => 'edit-table-profile-vendor',
      'data-device-type-target' => '#edit-table-profile-id-value',
      'data-device-list' => json_encode($options), 
      'data-selected-value' => !empty($PBXDevice->profile_id) ? hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id)->vendor : '',
    ),
  );

  // Can only be adjusted when creating the device.
  $form['table']['profile_id']['value'] = array(
    '#type' => 'select',
    '#options' => $options2,
    '#header' =>  array(t('Model')),
    //'#default_value' => !empty($PBXDevice->profile_id) ? $PBXDevice->profile_id : NULL,
    '#empty_option' => t('Choose a model'),
    '#required' => TRUE,
    '#title' => t('Model'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'),
      'data-placeholder' => t('Choose an option'),
      'data-custom-change-event' => 'edit-table-profile-id-value',
      'data-selected-value' => !empty($PBXDevice->profile_id) ? $PBXDevice->profile_id : '',
    ),
  );

  // MAC Adres
  $form['table']['identifier']['value'] = array(
    '#type' => 'textfield',
    '#maxlength' => 17,
    '#size' => 30,
    '#required' => TRUE,
    '#title' => t('MAC address'),
    '#default_value' => !empty($PBXDevice->identifier) ? $PBXDevice->identifier : '',
    '#attributes' => array(
      'class' => array('sg-element'),
      'placeholder' => t('MAC address'),
      'data-invalid-messages' => t('This field is mandatory___The given value must be of the MAC address type'),
      'data-validation-regex' => '^(?!\s*$).+___^([0-9a-fA-F][0-9a-fA-F][:\.-]??){5}([0-9a-fA-F][0-9a-fA-F])$',
    ),
  );

  if (!is_null($PBXDevice)) {
    drupal_add_js(array('hpbx_lines' => $PBXDevice->lines), 'setting');
  }
  else {
    drupal_add_js(array('hpbx_lines' => array()), 'setting');
  }

  $form['subscribers'] = array(
    '#type' => 'container',
    '#tree' => TRUE
  );

  $form['subscribers']['device_model_image'] = array(
    '#type' => 'markup',
    '#markup' => '<div id="hpbx-device-model-image-wrapper" class="hpbx-device-model-image-wrapper clearfix" style="display:none;"></div>'
  );
  
  $form['lines'] = array(
    '#type' => 'hidden'
  );
  
  // Submit button.
  $form['table']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );
  
  $form['table']['actions']['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next step'),
      '#prefix' => '<div class="col-xs-12"><span class="submit-button">',
      '#suffix' => '</span></div>',
      '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
  $form['table']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<div class="col-xs-12"><span class="submit-button">',
    '#suffix' => '</span></div>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
    '#ajax' => array(
        'callback' => 'hpbx_pbxdevice_edit_form_submit',
        'wrapper' => 'demo-demo-form',
        'method' => 'replace',
        'effect' => 'fade',
    ),
  );
  return $form;
}
