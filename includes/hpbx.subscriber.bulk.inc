<?php 

/**
 * Helper function to create the table header array.
 *
 * @return array
 */
function hpbx_subscriber_bulk_header() {

  $Customer = hpbx_get_active_customer(); 
  $header = array(
    'valid' => t('Valid?'),
    'no_selfcare' => t('Disable self care rights'),
    'external_id' => t('Name'),
    'firstname' => t('First name'),
    'lastname' => t('Last name'),
    'email' => t('Email'),
    'administrative' => t('Administrative'),
    'alias_number' => t('Number'),
    'pbx_extension' => t('Extension'),
    'cli' => t('Caller line identification'),
    'clir' => t('Caller Line Identification Restriction'),
    'profile_id' => t('Subscriber profile'),
    'block_in_clir' => t('Block anonymous inbound calls'),
    'ncos' => t('Call barrings'),
    'block_out_override_pin' => t('PIN for overriding call barrings'),
    'language' => t('Language of Voicemail announcements'),
    'station_name' => t('Device name'),
    'device_brand' => t('Brand'),
    'device_model' => t('Model'),
    'device_identifier' => t('MAC address'),
    'delete' => t('Delete'),
  );

  // Remove values for properties not allowed within selected profile.
  foreach (array('cli', 'block_in_clir', 'block_out_override_pin', 'language') as $element) {
    if (!hpbx_attribute_allowed($element)) {
  
      if (isset($header[$element])) {
        unset($header[$element]);
      }
    }
  }
   
  if(($Customer->_settings->derive_extension_from_did) && (!$Customer->_settings->allow_subscribers_without_did)) {
      unset($header['pbx_extension']);
  }
  return $header;
}

/**
 * Helper function to create dummy data, used within the template file.
 *
 * @return array.
 */
function hpbx_subscriber_bulk_dummy_data() {

  $Customer = hpbx_get_active_customer();
  $alias_numbers = array_values($Customer->get_free_numbers());
  
  // Set extenion in case of derive from did.
  $pbx_extensions = array();
  if (!$Customer->_settings->derive_extension_from_did) {
    for($i=0;$i<3;$i++) {
      if (!empty($alias_numbers[$i])) {
        $prefix = hpbx_get_extension_prefix($Customer, $alias_numbers[$i]);
        $extension_suffix_length = $Customer->_settings->extension_length - strlen($prefix);
        $pbx_extensions[$i] = $prefix. substr($alias_numbers[$i], $extension_suffix_length * - 1);
      }
    }
  }

  return array(
    array(
      'no_selfcare' => t('No'),
      'external_id' => '',
      'firstname' => 'John',
      'lastname' => 'Brown',
      'email' => 'john.brown@example.com',
      'administrative' => t('No'),
      'alias_number' => !empty($alias_numbers[0]) ? $alias_numbers[0] : '',
      'pbx_extension' => !empty($pbx_extensions[0]) ? $pbx_extensions[0] : '',
      'cli' => 'random',
      'clir' =>'random',
      'profile_id' => 'random', // Pick a random value from the list.
      'block_in_clir' => t('No'),
      'clir' => t('Yes'),
      'ncos' => 'random', // Pick a random value from the list.
      'block_out_override_pin' => '1234',
      'language' => 'English',
      'station_name' => 'Device 1',
      'device_model' => 'random',
      'device_identifier' => '01:23:45:67:89:01',
    ),
    array(
      'no_selfcare' => t('No'),
      'external_id' => '',
      'firstname' => 'Mark',
      'lastname' => 'White',
      'email' => 'mark.white@example.com',
      'administrative' => t('Yes'),
      'alias_number' => !empty($alias_numbers[1]) ? $alias_numbers[1] : '',
      'pbx_extension' => !empty($pbx_extensions[1]) ? $pbx_extensions[1] : '',
      'cli' => 'random',
      'clir' =>'random',
      'profile_id' => 'random', // Pick a random value from the list.
      'block_in_clir' => t('Yes'),
      'clir' => t('No'),
      'ncos' => 'random', // Pick a random value from the list.
      'block_out_override_pin' => '1234',
      'language' => 'English',
      'station_name' => 'Device 2',
      'device_model' => 'random',
      'device_identifier' => '12:34:56:78:90:12',
    ),
    array(
      'no_selfcare' => t('Yes'),
      'external_id' => 'Elevator',
      'firstname' => '',
      'lastname' => '',
      'email' => '',
      'administrative' => '',
      'alias_number' => !empty($alias_numbers[2]) ? $alias_numbers[2] : '',
      'pbx_extension' => !empty($pbx_extensions[2]) ? $pbx_extensions[2] : '',
      'cli' => 'random',
      'clir' =>'random',
      'profile_id' => 'random', // Pick a random value from the list.
      'block_in_clir' => t('No'),
      'clir' => t('Yes'),
      'ncos' => 'random', // Pick a random value from the list.
      'block_out_override_pin' => '1234',
      'language' => 'English',
      'station_name' => 'Device 3',
      'device_model' => 'random',
      'device_identifier' => '34:56:78:90:12:34',
    )
  );
}

/**
 * Helper function to set the header styling.
 *
 * @param unknown $objPHPExcel
 * @param unknown $cells
 */
function hpbx_subscriber_bulk_excel_set_header_style(&$objPHPExcel, $cells){
  $objPHPExcel->getActiveSheet()->getStyle($cells)->getFill()->applyFromArray(array(
    'type' => PHPExcel_Style_Fill::FILL_SOLID,
    'startcolor' => array(
      'rgb' => '585858'
    ))
  );
  
  $objPHPExcel->getActiveSheet()->getStyle($cells)->applyFromArray(array(
    'font' => array(
      'name'      => 'Arial',
      'bold'      => TRUE,
      'italic'    => FALSE,
      'strike'    => FALSE,
      'color'     => array(
        'rgb' => 'ffffff'
      )
    )
  ));
}

/**
 * Callback for ajax submit.
 */
function hpbx_subscriber_bulk_submit() {

  $Customer = hpbx_get_active_customer();
  
  $subscriber_form = array();
  $subscriber_form_state = array('values' => array('customer_id' => $Customer->id));
  
  $pbxdevice_form = array();
  $pbxdevice_form_state = array('values' => array('customer_id' => $Customer->id));
  
  hpbx_subscriber_bulk_submit_subscriber_validate($subscriber_form, $subscriber_form_state);
  
  // If all device related values are empty; do not validate the device.
  if (!empty($_POST['station_name']) || !empty($_POST['station_name']) || !empty($_POST['station_name'])) {
    hpbx_subscriber_bulk_submit_pbxdevice_validate($pbxdevice_form, $pbxdevice_form_state);
  }

  hpbx_subscriber_bulk_submit_subscriber_submit($subscriber_form, $subscriber_form_state);
  
  // If all device related values are empty; do not create the device.
  if (!empty($_POST['station_name']) || !empty($_POST['station_name']) || !empty($_POST['station_name'])) {
    hpbx_subscriber_bulk_submit_pbxdevice_submit($pbxdevice_form, $pbxdevice_form_state);
  }
  
  echo json_encode(array('isSuccessful' => TRUE, 'errors' => array()));
  drupal_exit();
}

/**
 * 
 */
function hpbx_subscriber_bulk_submit_pbxdevice_validate(&$form, &$form_state) {

  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.subscriber.inc');
  
  $Customer = hpbx_get_active_customer();
  $form = array();
  $form_state['values'] = array();
  
  
  $values = array(
    'station_name' => array('value' => check_plain($_POST['station_name'])),
    'profile_id' => array('value' => (int)$_POST['device_model']),
    'identifier' => array('value' => check_plain($_POST['device_identifier'])),
    'device_model' => array('value' => (int)$_POST['device_model']),
  );
  
  
  $form_state['values']['table'] = $values;
  $form_state['values']['table']['customer_id'] = $Customer->id;
  
  // Set the no_redirect option, to prevent a redirect after successfully
  // submitting the form.
  $form_state['values']['no_redirect'] = TRUE;
  
  
  // Determine correct usable linerange.
  $linerange = hpbx_subscriber_get_private_linerange($form_state['values']['table']['device_model']['value']);
  
  $errors = array();

  if (empty($linerange->name)) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('Device key can\'t be determined. ');
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
  if (empty($form_state['values']['table']['station_name']['value'])) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('!name field is required.', array('!name' => t('Device name')));
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
  if (empty($form_state['values']['table']['profile_id']['value'])) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('!name field is required.', array('!name' =>  t('Subscriber profile')));
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
  if (empty($form_state['values']['table']['identifier']['value'])) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('!name field is required.', array('!name' => t('MAC Address')));
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }

  // Add dummy line.
  $line = new stdClass();
  $line->subscriber_id = NULL;
  
  $form_state['values']['lines'] = json_encode(array($line));

  // Validate the form.
  hpbx_pbxdevice_edit_form_validate($form, $form_state);
  
  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, array('table][', 'pbxdevice]['));
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $json_form_errors));
    drupal_exit();
  }
}

function hpbx_subscriber_bulk_submit_pbxdevice_submit(&$form, &$form_state) {  
  
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.subscriber.inc');
  
  // Determine correct usable linerange.
  $linerange = hpbx_subscriber_get_private_linerange($form_state['values']['table']['device_model']['value']);
  
  // Add real line.
  $line = new stdClass();
  $line->subscriber_id = $_SESSION['ccs']['hpbx']['wizard']['subscriber_id'];
  $line->line_type = 'private';
  $line->key_num = 0;
  $line->linerange = $linerange->name;
  
  $form_state['values']['lines'] = array($line);

  // Submit the form.
  hpbx_pbxdevice_edit_form_submit($form, $form_state);
  
  if (empty($_SESSION['ccs']['hpbx']['wizard']['device_id'])) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('Device not created');
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
}

function hpbx_subscriber_bulk_submit_subscriber_validate(&$form, &$form_state) {  

  $Customer = hpbx_get_active_customer();
  $header = hpbx_subscriber_bulk_header();
  
  // Get template row to verify if all input arguments are passed.
  $row_template = hpbx_subscriber_bulk_get_template_row();
  unset($row_template['valid'], $row_template['delete']);
  
  $errors = array();
  foreach ($row_template as $key => $element) {
  
    // Verify if value is posted.
    if (!isset($_POST[$key])) {
      $error = new stdClass();
      $error->element = '';
      $error->message = t($header[$key] . ' not set');
      $errors[] = $error;
      break;
    }
  
    // Verify if the pasted select value is allowed (is it in the option list).
    elseif ($key!='device_brand' && $key!='device_model' && isset($element['data']['#type']) && $element['data']['#type'] == 'select' &&
        (!isset($element['data']['#options']) || !count($element['data']['#options']) || 
        array_search($_POST[$key], $element['data']['#type']['#options'])===FALSE)) {
      $error = new stdClass();
      $error->element = '';
      $error->message = t('Not allowed value "'. $_POST[$key] .'" set for '. $header[$key]);
      $errors[] = $error;
      break;
    }

    // Sanitize input.
    $form_state['values']['general']['settings'][$key]['value'] = $_POST[$key];
  }
  
  if (count($errors)) {
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
  
  // Set the no_redirect option, to prevent a redirect after successfully
  // submitting the form.
  $form_state['values']['no_redirect'] = TRUE;
  
  // Set extenion in case of derive from did.
  if ($Customer->_settings->derive_extension_from_did && !($Customer->_settings->allow_subscribers_without_did) && !empty($form_state['values']['general']['settings']['alias_number']['value'])) {
    $prefix = hpbx_get_extension_prefix($Customer, $form_state['values']['general']['settings']['alias_number']['value']);
    $extension_suffix_length = $Customer->_settings->extension_length - strlen($prefix);
    $form_state['values']['general']['settings']['pbx_extension']['value'] = $prefix. substr($form_state['values']['general']['settings']['alias_number']['value'], $extension_suffix_length * - 1);
  
  }
  else if($Customer->_settings->derive_extension_from_did && $Customer->_settings->allow_subscribers_without_did && !empty($form_state['values']['general']['settings']['alias_number']['value']) && is_numeric($form_state['values']['general']['settings']['alias_number']['value']) && empty($form_state['values']['general']['settings']['pbx_extension']['value'])){
     $prefix = hpbx_get_extension_prefix($Customer, $form_state['values']['general']['settings']['alias_number']['value']);
    $extension_suffix_length = $Customer->_settings->extension_length - strlen($prefix);
    $form_state['values']['general']['settings']['pbx_extension']['value'] = $prefix. substr($form_state['values']['general']['settings']['alias_number']['value'], $extension_suffix_length * - 1); 
  }
  if($Customer->_settings->allow_subscribers_without_did && !empty($form_state['values']['general']['settings']['alias_number']['value']) && ($form_state['values']['general']['settings']['alias_number']['value']== "None")){
    $form_state['values']['general']['settings']['alias_number']['value'] = 0 ;
  }
  // Validate the form.
  hpbx_subscriber_edit_form_validate($form, $form_state);
  
  // Retrieve errors (in case there are any).
  $form_errors = form_get_errors();
  
  if ($form_errors) {
    $json_form_errors = hpbx_get_json_form_errors($form_errors, 'general][settings][');
    form_clear_error();
    drupal_get_messages('error');
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $json_form_errors));
    drupal_exit();
  }
}

function hpbx_subscriber_bulk_submit_subscriber_submit(&$form, &$form_state) {
  // Submit the form.
  hpbx_subscriber_edit_form_submit($form, $form_state);
  
  if (empty($_SESSION['ccs']['hpbx']['wizard']['subscriber_id'])) {
    $error = new stdClass();
    $error->element = '';
    $error->message = t('Subscriber not created');
    $errors[] = $error;
    echo json_encode(array('isSuccessful' => FALSE, 'errors' => $errors));
    drupal_exit();
  }
}

/**
 * Callback for hpbx/subscriber/bulk/template.
 */
function hpbx_subscriber_bulk_template_download() {
  $Customer = hpbx_get_active_customer();

  require_once (drupal_get_path('module', 'ccs') . '/includes/PHPExcel/Classes/PHPExcel/IOFactory.php');

  $objPHPExcel = new PHPExcel();
  // Set header.
  $header = hpbx_subscriber_bulk_header();
  unset($header['valid'], $header['delete'], $header['device_brand']);

  $header['device_model'] = t('Device');

  $row_template = hpbx_subscriber_bulk_get_template_row();

  $row_template['device_model']['data']['#attributes']['data-device-list'] = $row_template['device_brand']['data']['#attributes']['data-device-list'];
  unset($row_template['device_brand']);

  // For the dropdowns, we have to create lists, as the max limit for 'simple'
  // lists are 256 chars and can't contain comma's. This limited is exceeded
  // due to the length of callbarring names.

  $objPHPExcel->createSheet(1);

  $lists = array();
  $datalist = array();
  $cli = $row_template['cli']['data']['#options']; 
       foreach ($row_template['cli']['data']['#options'] as $type => $options) { 
        foreach ($options as $value => $option) {
         foreach ($option as $value1 => $option1){
          $datalist['default']= 'Own number';
          $datalist[$value1] = $option1;
         }
        }
       }
     
  // Set initial column (A).
  $list = 0;
  foreach ($row_template as $key => $element) {
    if ($element['data']['#type'] == 'select') {

      if (isset($element['data']['#attributes']['data-device-list'])) {
        if ($json = json_decode($element['data']['#attributes']['data-device-list'])) {

          $element['data']['#options'] = array();
          foreach ($json as $models) {

            foreach ($models->types as $model) {
              $element['data']['#options'][] = $model->label;
            }
          }
        }
      }
     else if($element['data']['#attributes']['data-field-name'] == "cli"){
        $element['data']['#options'] = array();
         foreach ($datalist as $data) {
              $element['data']['#options'][] = $data; 
            }
       }
     
       
   
      natsort($element['data']['#options']);
      $row = 1;
      foreach ($element['data']['#options'] as $value) {

        if (!empty($value)) {
          if (is_string($value) && is_numeric($value)) {

            // Make it a string (prepend with ').
            $objPHPExcel->setActiveSheetIndex(1)
              ->setCellValueExplicit(chr(65 + ($list)) . (string) $row, $value);
          }
          else {

            // Set value.
            $objPHPExcel->setActiveSheetIndex(1)
              ->setCellValue(chr(65 + ($list)) . (string) $row, $value);
          }

          // Increase row.
          $row++;
        }
      }

      // Make a named range of the filled cells.
      $objPHPExcel->addNamedRange(
        new PHPExcel_NamedRange(
          $key, // e.g. ncos.
          $objPHPExcel->setActiveSheetIndex(1),
          chr(65 + ($list)) . '1:' . chr(65 + ($list)) . (string) $row // e.g. A1:A10.
        )
      );

      // Increase list number (column)
      $list++;
    }
  }

  // Add list of free numbers.
  $row = 1;
   // Fetch available numbers
    $alias_numbers = json_decode($row_template['alias_number']['data']['#attributes']['data-values']);
    $alias_numbers = json_decode(json_encode($alias_numbers), true);  
   foreach($alias_numbers as $alias_number){
     $number[$alias_number['value']]=$alias_number['label'];
   }
  foreach ($number as $value) {

    // Prepend numbers with quote, to make it a string and not an integer.
    $objPHPExcel->setActiveSheetIndex(1)->setCellValueExplicit(chr(65+($list)). (string)$row++, $value);
  }

  // Make a named range of the filled alias_number cells.
  $objPHPExcel->addNamedRange(
    new PHPExcel_NamedRange(
      'alias_number',
      $objPHPExcel->setActiveSheetIndex(1),
      chr(65+($list)) . '1:'. chr(65+($list)) . (string)$row // e.g. A1:A10.
    )
  );

  // Make the sheet with lists 'very hidden'.
  $objPHPExcel->setActiveSheetIndex(1)->setSheetState(PHPExcel_Worksheet::SHEETSTATE_VERYHIDDEN);
  
  $i = 0;
  foreach ($header as $key => $label) {
    $objPHPExcel->setActiveSheetIndex(0)->setCellValue(chr(65+($i++)). '1', $label);
  }
  
  // Set dummy data.
  $row = 2;
  foreach (hpbx_subscriber_bulk_dummy_data() as $data) {
    $i = 0;
    foreach ($data as $key => $value) {

      if (isset($row_template[$key])) {
        
        if ($row_template[$key]['data']['#type'] == 'select' || $key == 'alias_number') {
          
          $pDataValidation = new PHPExcel_Cell_DataValidation();
          $pDataValidation->setType( PHPExcel_Cell_DataValidation::TYPE_LIST );
          $pDataValidation->setErrorStyle( PHPExcel_Cell_DataValidation::STYLE_INFORMATION );
          $pDataValidation->setAllowBlank(TRUE);
          $pDataValidation->setShowInputMessage(TRUE);
          $pDataValidation->setShowErrorMessage(TRUE);
          $pDataValidation->setShowDropDown(TRUE);
          $pDataValidation->setErrorTitle(t('Input error'));
          $pDataValidation->setError(t('Value is not in list'));
          $pDataValidation->setPromptTitle(t('Pick a value from the list'));
          $pDataValidation->setPrompt(t('Please pick a value from the drop-down list.'));
          
          // Set the correct list as dropdown validation list.
          $pDataValidation->setFormula1('='. $key);
          
          // Assign validation object.
          $objPHPExcel->setActiveSheetIndex(0)->getCell(chr(65+($i)). (string)$row)->setDataValidation($pDataValidation);
        }

        if ($row_template[$key]['data']['#type'] == 'select' && $value == 'random') {
          $value = t($row_template[$key]['data']['#options'][array_rand($row_template[$key]['data']['#options'])]);
        }
        if (is_string($value) && is_numeric($value)) {

          // Make it a string (prepend with ').
          $objPHPExcel->setActiveSheetIndex(0)->setCellValueExplicit(chr(65+($i)). (string)$row, $value);
        }
        else {

          // Set value.
          $objPHPExcel->setActiveSheetIndex(0)->setCellValue(chr(65+($i)). (string)$row, $value);
        }
        $i++;
      }
    }
    $row++;
  }
  
  
  // Set title.
  $objPHPExcel->getActiveSheet()->setTitle(t('Bulk import'));
  
  // Set active sheet.
  $objPHPExcel->setActiveSheetIndex(0);

  // Set header style (bold, gray background etc).
  hpbx_subscriber_bulk_excel_set_header_style($objPHPExcel, 'A1:'.chr(65+(--$i)) . '1');
  
  // Set Column auto-width.
  foreach (range('A', chr(65+$i)) as $columnID) {
    $objPHPExcel->getActiveSheet()->getColumnDimension($columnID)->setAutoSize(true);
  }
  
  // Redirect output to a client’s web browser (Excel2007)
  header('Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet');
  header('Content-Disposition: attachment;filename="template.xlsx"');
  header('Cache-Control: max-age=0');
  $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel2007');
  $objWriter->save('php://output');
  exit;
}

/**
 * Helper function to retrieve the entries from uploaded xlsx file.
 * @param string $filename
 * @param integer $headers_at
 * @return array|bool
 */
function hpbx_subscriber_bulk_get_entries($filename, $headers_at = 0) {
  try {
    require_once (drupal_get_path('module', 'ccs') . '/includes/PHPExcel/Classes/PHPExcel/IOFactory.php');

    $Excel = PHPExcel_IOFactory::load($filename);
  
    // Get first sheet.
    $Worksheet = $Excel->getSheet(0);
  
    $raw_rows = array();
    $empty = 0;
    foreach ($Worksheet->getRowIterator() as $i => $row) {


      foreach ($row->getCellIterator() as $r => $cell) {
        $raw_rows[$i][$r] = trim($cell->getCalculatedValue());
      }

      if (!isset($raw_rows[$i])) {
        $empty++;
        if ($empty==10) break;
      }
      else {
        $empty = 0;
      }

    }

    array_splice($raw_rows, 0, $headers_at);

    // Get headers from sheet.
    $header = $raw_rows[$headers_at];

    $h = hpbx_subscriber_bulk_header();
    unset($h['device_brand']);
    $h['device_model'] = t('Device');

    // Get header array with keys.
    $header_keys = array_flip($h);

    // Remove sheet header from sheet.
    unset($raw_rows[$headers_at]);
  
    $rows = array();
  
    // Create a new list with the field names as key.
    foreach ($raw_rows as $key1 => $raw_row) {
  
      $row = array();
  
      foreach ($raw_row as $key2 => $value) {
        if ($header[$key2]!='' && isset($header_keys[$header[$key2]])) {

          $row[$header_keys[$header[$key2]]] = $value;
        }
      }      
        
      $rows[] = $row;
    }

    return $rows;
  }
  catch (Exception $e) {
    drupal_set_message(t('Failed to retrieve entries from Excel file'), 'error');
    watchdog('hpbx_subscriber_bulk', $e->getMessage());
    return FALSE;
  }
}

function hpbx_subscriber_bulk_get_template_row() {

  $Customer = hpbx_get_active_customer();

  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.subscriber.inc');
  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');

  $dummy_form = $dummy_form_state = array();
  $subscriber_form = hpbx_subscriber_edit_form($dummy_form, $dummy_form_state);
  $pbxdevice_form = hpbx_pbxdevice_edit_form($dummy_form, $dummy_form_state);

  $row_template = array();
  $row_template['valid'] = array(
      'class' => 'hpbx-bulk-upload-status-container',
      'data' => array(
          '#type' => 'markup',
          '#markup'=> '
        <div class="hpbx-bulk-upload-status-icon"></div>
        <div class="hpbx-bulk-upload-status-icon hpbx-bulk-upload-second-checkmark"></div>
        <div class="hpbx-bulk-upload-status-upload-failure-message"></div>'
      )
  );

  $row_template['no_selfcare']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-create-account'),
          'data-field-name' => 'no_selfcare'
      ),
      '#options' => array(
          0 => t('No'),
          1 => t('Yes')
      ),
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );

  $row_template['external_id'] = array(
      'data-invalid-messages' => t('This field is mandatory___The maximum length of this field is 255'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-name'),
              'data-field-name' => 'external_id',
              'data-regex' => '^(?!\s*$).+___^.{0,255}$',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );

  $row_template['firstname']= array(
      'data-invalid-messages' => t('This field is mandatory___The maximum length of this field is 255'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
            'class' => array('hpbx-bulk-upload-field-first-name'),
            'data-field-name' => 'firstname',
            'data-regex' => '^(?!\s*$).+___^.{0,255}$',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );

  $row_template['lastname'] = array(
      'data-invalid-messages' => t('This field is mandatory___The maximum length of this field is 255'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-last-name'),
              'data-field-name' => 'lastname',
              'data-regex' => '^(?!\s*$).+___^.{0,255}$',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );

  $row_template['email'] = array(
      'data-invalid-messages' => t('This field is mandatory___The given value must be of the email type___The maximum length of this field is 255'),
      'data-not-unique-message'=> t('One of the values is not unique'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-email'),
              'data-field-name' => 'email',
              'data-regex' => "^(?!\s*$).+___[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?___^.{0,255}$",
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );


  $row_template['administrative']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-administrative'),
          'data-field-name' => 'administrative'
      ),
      '#options' => array(
          0 => t('No'),
          1 => t('Yes')
      ),
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );

  $data_values = array();
  $alias_numbers = $Customer->get_free_numbers();
  if(isset($Customer->_settings->allow_subscribers_without_did) && $Customer->_settings->allow_subscribers_without_did){ 
    $data_values['0']=array('value' => 'None', 'label' => 'None');
    $alias_number_regex = "^\+?[0-9]\d{1,14}$|^None$";
	$alias_number_invalid = t('The given value must be of the phone number type / None');
    
  }else{
    $alias_number_regex = "^\+?[0-9]\d{1,14}$";
	$alias_number_invalid = t('The given value must be of the phone number type');	
  }
  foreach ($alias_numbers as $key => $number) {
    $data_values[$key] = array('value' => $number, 'label' => $number);
  }
  
  $row_template['alias_number'] = array(
      'data-invalid-messages' => "$alias_number_invalid",
      'data-not-unique-message' => t('One of the values is not unique'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-autocomplete hpbx-bulk-upload-field-number'),
              'data-values' => json_encode(array_values($data_values)),
              'data-regex' => "$alias_number_regex",
              'data-field-name' => 'alias_number',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );
  if (!($Customer->_settings->derive_extension_from_did && !$Customer->_settings->allow_subscribers_without_did)) {
    $row_template['pbx_extension'] = array(
      'data-invalid-messages' => t('Extension requires length of @length digits', array('@length' => $Customer->_settings->extension_length)),
      'data' =>
      array(
        '#type' => 'textfield',
        '#attributes' => array(
          'class' => array('hpbx-autocomplete hpbx-bulk-upload-field-extension'),
          'data-field-name' => 'pbx_extension',
          'data-regex' => '^\d{'.$Customer->_settings->extension_length.'}$',
          'maxlength' => $Customer->_settings->extension_length,
          'size' => '',
        ),
        '#row_wrapper' => FALSE,
        '#field_size_class' => array(),
        '#tab_form_field_classes' => array(),
      )
    );
 }



    // Get list of subscribers.
    $subscriber_options = hpbx_get_subscriber_options_subscriber_create($Customer, $Subscriber, TRUE, FALSE, TRUE,  TRUE, FALSE, TRUE, FALSE);
    
    // Prepend the list with some default settings.
   
    $subscriber_options = array_merge(array('default' => t('Own number'), $subscriber_options));
    	
   

  $row_template['cli']['data'] = array(
    '#type' => 'select',
    '#attributes' => array(
      'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-caller-line-identification'),
      'data-field-name' => 'cli'
    ),
    '#options' => $subscriber_options,
    '#row_wrapper' => FALSE,
    '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );
//restrict cli
// if($Customer->_settings->allow_subscribers_without_did){
	  // CLI Restriction.
	$row_template['clir']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-block-anonymous-inbound-calls'),
          'data-field-name' => 'clir'
      ),
      '#options' => array(
          0 => t('No'),
          1 => t('Yes')
      ),
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );
 // }


  $row_template['profile_id']['data'] = array(
    '#type' => 'select',
    '#attributes' => array(
      'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-subscriber-profile'),
      'data-field-name' => 'profile_id'
    ),
    '#options' => $subscriber_form['general']['settings']['profile_id']['value']['#options'],
    '#row_wrapper' => FALSE,
    '#field_size_class' => array(),
    '#tab_form_field_classes' => array(),
  );

  $row_template['block_in_clir']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-block-anonymous-inbound-calls'),
          'data-field-name' => 'block_in_clir'
      ),
      '#options' => array(
          0 => t('No'),
          1 => t('Yes')
      ),
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );

  $ncos_options = array('' => '');
  $ncos_options += $subscriber_form['general']['settings']['ncos']['value']['#options'];
  
  $row_template['ncos']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-call-barrings'),
          'data-field-name' => 'ncos'
      ),
      '#options' => $ncos_options,
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );

  $row_template['block_out_override_pin'] = array(
      'data-invalid-messages' => t('The maximum length of this field is 4___The given value is not completely a digit string'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-pin'),
              'data-regex' => '^.{0,4}$___^[0-9]*$',
              'data-field-name' => 'block_out_override_pin',
          ),
          '#maxlength' => 4,
          '#size' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );

  $row_template['language']['data'] = array(
    '#type' => 'select',
    '#attributes' => array(
      'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-language'),
      'data-field-name' => 'language'
    ),
    '#row_wrapper' => FALSE,
    '#field_size_class' => array(),
    '#tab_form_field_classes' => array(),
  );


  $row_template['language']['data']['#options']= array();
  if ($preferencedefs = sipwise_get_preferencedefs('subscriber')) {
    if (isset($preferencedefs->language->enum_values)) {
      foreach ($preferencedefs->language->enum_values as $enum) {
        if (!empty($enum->value)) {
          if ($enum->default_val) {
            $row_template['language']['data']['#default_value'] = $enum->value;
          }
          $row_template['language']['data']['#options'][$enum->value] = t(drupal_ucfirst($enum->label));
        }
      }
    }
  }

  // Sort options list.
  asort($row_template['language']['data']['#options']);



  $row_template['station_name'] = array(
      'data-invalid-messages' => t('The maximum length of this field is 255'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-device-name'),
              'data-regex' => "^.{0,255}$",
              'data-field-name' => 'station_name',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
      )
  );

  // Device profiles.
  $devices = array();
  $devices[''][] = array('value' => '','label' => '',); //Bug #9568 Device models are not shown in the dropdown before changing the device brand
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
  $row_template['device_brand']['data'] = array(
    '#type' => 'select',
    '#attributes' => array(
      'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-device-brand'),
      'data-field-name' => 'device_brand',
      'data-device-list' => json_encode($options),
      'data-device-type-target' => 'device_model',
    ),
    '#options' => array(),
    '#row_wrapper' => FALSE,
    '#field_size_class' => array(),
    '#tab_form_field_classes' => array(),
  );

  $row_template['device_model']['data'] = array(
      '#type' => 'select',
      '#attributes' => array(
          'class' => array('chzn-select', 'chzn-done', 'sg-element', 'no-search', 'hpbx-bulk-upload-field-device-type'),
          'data-field-name' => 'device_model'
      ),
      '#options' => array(),
    '#limit_validation_errors' => array(),
      '#row_wrapper' => FALSE,
      '#field_size_class' => array(),
      '#tab_form_field_classes' => array(),
  );
  // Modified for Bug #9531- PE DEV: Bulk import - MAC address in other format as xx:xx:xx:xx:xx:xx is indicated as incorrect in front end validation
  $row_template['device_identifier'] = array(
      'data-invalid-messages' => 'This field is mandatory___The given value must be of the MAC address type',
      'data-not-unique-message' => t('One of the values is not unique'),
      'data' => array(
          '#type' => 'textfield',
          '#attributes' => array(
              'class' => array('hpbx-bulk-upload-field-device-mac-address'),
              'data-regex' => '^(?!\s*$).+___^([0-9a-fA-F][0-9a-fA-F][:\.-]??){5}([0-9a-fA-F][0-9a-fA-F])$',
              'data-field-name' => 'device_identifier',
          ),
          '#size' => '',
          '#maxlength' => '',
          '#row_wrapper' => FALSE,
          '#field_size_class' => array(),
          '#tab_form_field_classes' => array(),
    )
  );

  $row_template['delete'] = array(
    'class' => array('hpbx-bulk-upload-remove-container'),
    'data' => array(
      '#markup' => '<span class="LGI-iconbin hpbx-bulk-upload-remove-icon"></span>',
    ),
  );

  // Remove values for properties not allowed within selected profile.
  foreach (array('clir', 'block_in_clir', 'block_out_override_pin', 'language') as $element) {
    if (!hpbx_attribute_allowed($element)) {
  
      if (isset($row_template[$element])) {
        unset($row_template[$element]);
      }
    }
  }
  return $row_template;
}


/**
 * Callback for hpbx/subscriber/bulk.
 *
 * @return string
 */
function hpbx_subscriber_bulk_page() {

  $Customer = hpbx_get_active_customer();

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_subscriber_bulk.js', array('scope' => 'footer'));
  
  $row_template = hpbx_subscriber_bulk_get_template_row();
  
  drupal_set_title(t('Bulk import'));
  $data_entries = array();
  if (isset($_FILES['hpbx-bulk-file']) && file_exists($_FILES['hpbx-bulk-file']['tmp_name'])) {
    $data_entries = hpbx_subscriber_bulk_get_entries($_FILES['hpbx-bulk-file']['tmp_name']);

    // Parse device_model to device_brand and device_model.
    $vendors = array();
    if ($pbxdevicemodels = sipwise_api_get_all('pbxdevicemodels')) {
      foreach ($pbxdevicemodels as $PBXDeviceModel) {
        $vendors[$PBXDeviceModel->vendor] = TRUE;
      }
      $vendors = array_keys($vendors);
    }

    foreach ($data_entries as $k => &$data_entry) {
      if (isset($data_entry['device_model'])) {
        foreach ($vendors as $vendor) {
          if (preg_match('/^'. $vendor . '/', $data_entry['device_model'])) {
            $data_entry['device_brand'] =  $vendor;
            break;
          }
        }
      }
    }
  }
  $rows = array();

  foreach ($data_entries as $data) {

    $row = $row_template;
    
    if (!empty($data['no_selfcare']) && (array_search($data['no_selfcare'], $row_template['no_selfcare']['data']['#options'])!==FALSE)) {
      $row['no_selfcare']['data']['#value'] = array_search($data['no_selfcare'], $row_template['no_selfcare']['data']['#options']);
    }
    
    $row['external_id']['data']['#attributes']['value'] = !empty($data['external_id']) ? $data['external_id'] : '';
    $row['firstname']['data']['#attributes']['value'] = !empty($data['firstname']) ? $data['firstname'] : '';
    $row['lastname']['data']['#attributes']['value'] = !empty($data['lastname']) ? $data['lastname'] : t('Example last name');
    $row['email']['data']['#attributes']['value'] = !empty($data['email']) ? check_plain($data['email']) : t('example@test.com');
    
    if (!empty($data['administrative']) && (array_search($data['administrative'], $row_template['administrative']['data']['#options'])!==FALSE)) {
      $row['administrative']['data']['#value'] = array_search($data['administrative'], $row_template['administrative']['data']['#options']);
    }
    
    $row['alias_number']['data']['#attributes']['value'] = !empty($data['alias_number']) ? check_plain($data['alias_number']) : '';
     foreach ($row_template['cli']['data']['#options'] as $type => $options) { 
        foreach ($options as $value => $option) {
         foreach ($option as $value1 => $option1){
          $datalist['default']= 'Own number';
          $datalist[$value1] = $option1;
         }
        }
       }
    if (!empty($data['cli']) && (array_search($data['cli'], $datalist)!==FALSE)) { 
      $row['cli']['data']['#value'] = array_search($data['cli'], $datalist);
    }
   
    if (!($Customer->_settings->derive_extension_from_did && !$Customer->_settings->allow_subscribers_without_did)) {
      $row['pbx_extension']['data']['#attributes']['value'] = !empty($data['pbx_extension']) ? (int)$data['pbx_extension'] : '';
    }
 //if($Customer->_settings->allow_subscribers_without_did){
   if (!empty($data['clir']) && (array_search($data['clir'], $row_template['clir']['data']['#options'])!==FALSE)) {
     $row['clir']['data']['#value'] = array_search($data['clir'], $row_template['clir']['data']['#options']);
    }
    //}
    if (!empty($data['profile_id']) && (array_search($data['profile_id'], $row_template['profile_id']['data']['#options'])!==FALSE)) {
      $row['profile_id']['data']['#value'] = array_search($data['profile_id'], $row_template['profile_id']['data']['#options']);
    }

    if (isset($row_template['block_in_clir']) && !empty($data['block_in_clir']) && (array_search($data['block_in_clir'], $row_template['block_in_clir']['data']['#options'])!==FALSE)) {
      $row['block_in_clir']['data']['#value'] = array_search($data['block_in_clir'], $row_template['block_in_clir']['data']['#options']);
    }
    
    if (!empty($data['ncos']) && (array_search($data['ncos'], $row_template['ncos']['data']['#options'])!==FALSE)) {
      $row['ncos']['data']['#value'] = array_search($data['ncos'], $row_template['ncos']['data']['#options']);
    }
    
    if (isset($row_template['block_out_override_pin'])) {
      $row['block_out_override_pin']['data']['#attributes']['value'] = !empty($data['block_out_override_pin']) ? (int)$data['block_out_override_pin'] : '';
    }

    if (!empty($data['language']) && (array_search($data['language'], $row_template['language']['data']['#options'])!==FALSE)) {
      $row['language']['data']['#value'] = array_search($data['language'], $row_template['language']['data']['#options']);
    }
    
    $row['station_name']['data']['#attributes']['value'] = !empty($data['station_name']) ? $data['station_name'] : '';

    if (!empty($data['device_brand'])) {
      $row['device_brand']['data']['#attributes']['data-selected-value'] = $data['device_brand'];
    }

    if (!empty($data['device_model'])) {
      $row['device_model']['data']['#attributes']['data-selected-value'] = $data['device_model'];
    }

    $row['device_identifier']['data']['#attributes']['value'] = isset($data['device_identifier']) ? check_plain($data['device_identifier']) : '';

    $rows[] = array(
      'data' => $row,
      'class' => array('hpbx-bulk-upload-data-row')
    );
  }

  $header = hpbx_subscriber_bulk_header();
  
  foreach ($header as $header_key => &$header_value) {
    $type_class = FALSE;
    
    if ($header_key == 'delete') {
      $type_class = 'hpbx-bulk-column-delete';
    }
    elseif (isset($row_template[$header_key]['data']['#type']) && 
        $row_template[$header_key]['data']['#type'] == 'select') {
      $type_class = 'hpbx-bulk-column-select';
    }
    elseif ($header_key == 'valid' || (isset($row_template[$header_key]['data']['#type']) && $row_template[$header_key]['data']['#type'] == 'checkbox')) {
      $type_class = 'hpbx-bulk-column-checkmark';
    }
    
    if ($type_class) {
      $header_value = array('data' => $header_value, 'class' => array($type_class));
    }
  }
  
  $variables = array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array(
      'class' => array('hpbx-bulk-upload-table hpbx-bulk-upload-table-fill'),
      'cellspacing' => 0,
      'cellpadding' => 0
    ),
  );
  
  
  
  // Append uploaded entries.
  $output = theme('table', $variables);

  // Append table with 1 row to be used by JS, in case user needs to add new/extra entry.
  $output .= '<div class="hpbx-bulk-upload-add">'. PHP_EOL . '<span class="LGI-iconplus hpbx-bulk-upload-add-icon"></span>';

  // Change table vars.
  
  
  foreach ($row_template as &$element) {
    if (isset($element['data']['#type']) && $element['data']['#type'] == 'select') {
      $element['data']['#attributes']['class'][] = 'chzn-done';
    }
  }

  $variables = array(
      'header' => array(),
      'rows' => array(array('class'=> array('hpbx-bulk-upload-data-row'), 'data' => $row_template)),
      'attributes' => array('class' => array('hpbx-bulk-upload-add-table'))
  );

  $output .= theme('table', $variables); 
  $output .= '</div>';

  return $output;
}
