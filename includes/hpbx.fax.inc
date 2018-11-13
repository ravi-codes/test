<?php

/**
 * @param SipwiseEntity $Call
 *
 * @return string
 */
function hpbx_fax_faxlog_get_called($Call) {
  return hpbx_callrecords_normalize($Call->callee);
}

/**
 * @param SipwiseEntity $Call
 *
 * @return string
 */
function hpbx_fax_faxlog_get_caller($Call) {
  return hpbx_callrecords_normalize($Call->caller);
}

function hpbx_fax_page() {

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js',
    array('scope' => 'footer'));

  $Customer = hpbx_get_active_customer();
  $Subscriber = hpbx_get_active_subscriber();

  // Init form.
  $form = array();

  // Only access in case the currenly logged in user has membership.
  $sendfax_access = FALSE;
  foreach ($Customer->get_fax_subscribers() as $Subscriber2) {
    if ($Subscriber2->is_member($Subscriber)) $sendfax_access = TRUE;
  }

  $fieldsets = array();

  if (user_access('hpbx view fax2email')) {
    $fieldsets['fax-2-email'] = array('title' => t('Fax numbers'));
  }

  if (user_access('hpbx send fax') || $sendfax_access) {
    $fieldsets['sendfax'] = array('title' => t('Send fax'), 'access' => user_access('hpbx send fax') || $sendfax_access);
  }
  if (user_access('hpbx view fax log') || $sendfax_access) {
    $fieldsets['faxlog'] = array('title' => t('Fax log'), 'access' => user_access('hpbx view fax log') || $sendfax_access);
  }

  // Create tabs.
  $items = array();
  foreach ($fieldsets as $key => $element) {
    $data = '<div class="hpbx-tab-item-title">
                <a href="#tab-' . strtolower(str_replace(' ', '-', $element['title'])) . '">' . $element['title'] . '</a>
            </div>';
    $data .= '<div class="hpbx-tab-item-background"></div>';
    $item = array('data' => $data);
    $items[] = $item;
  }

  // This is to specify the vertical tab holder.
  // One page can have two vertical tabs, but it's always one main tab with sub tabs.
  $form['tabs'] = array(
    '#type' => 'markup',
    '#markup' => substr(str_replace('<div class="item-list">', '', theme('item_list', array('items' => $items))), 0, -6),
    '#prefix' => '<div class="hpbx-tabs hpbx-page-fax">',
  );

  foreach ($fieldsets as $key => $fieldset) {
    $form[$key] = array(
      '#type' => 'container',
      '#tabform' => TRUE,
      '#title' => !empty($fieldset['title']) ? $fieldset['title'] : '',
      '#collapsible' => FALSE,
      '#tree' => TRUE,
      '#access' => isset($fieldset['access']) ? $fieldset['access'] : TRUE,
      '#attributes' => array(
        'id' => 'tab-' . strtolower(str_replace(' ', '-', $fieldset['title'])),
      ),
    );
  }

  if (user_access('hpbx view fax2email')) {
    // Last item place suffix.
    //$form[$key]['#suffix'] = '</div>';

    // No tab-form wrapper div.
    $form['fax-2-email']['#tabform'] = FALSE;

    // Create the add link.
    $text = '<span class="sg-icon sg-icon-plus-round"></span> <h5 class="sg-header-5">' . t('Create new fax number') . '</h5>';
    $add_link = l($text, 'hpbx/fax2email/add', array(
      'html' => TRUE,
      'attributes' => array('class' => array('hpbx-tabs-create-bar'))
    ));
    $form['fax-2-email']['link'] = array(
      '#type' => 'markup',
      '#markup' => $add_link
    );

    // Generate table with fax2email numbers.
    $form['fax-2-email']['table'] = hpbx_subscribers_get_subscribers_table(NULL, TRUE);
  }
  if (user_access('hpbx send fax') || $sendfax_access) {
    $elements = drupal_get_form('hpbx_fax_sendfax_form');
    $form['sendfax']['table']['#markup'] = drupal_render($elements);
  }

  if (user_access('hpbx view fax log') || $sendfax_access) {
    $form['faxlog']['#tabform'] = FALSE;
    $form['faxlog']['table']['#markup'] = hpbx_fax_faxlog_get_table();
  }
  return drupal_render($form);
}

/**
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function hpbx_fax_sendfax_form($form, $form_state) {
  $Customer = hpbx_get_active_customer();
  $Subscriber = hpbx_get_active_subscriber();
  $subscriber_options = array();

  // Only access in case the currenly logged in user has membership.
  foreach ($Customer->get_fax_subscribers() as $Subscriber2) {
    if ($Subscriber2->is_member($Subscriber)) {
      $subscriber_options[$Subscriber2->id] = $Subscriber2->external_id . ' - '. $Subscriber2->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
    }
  }

  $form['sendfax']['subscriber_id'] = array(
    '#type' => 'select',
    '#options' => $subscriber_options,
    '#required' => TRUE,
    '#title' => t('Fax number'). '*',
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  // Destination number.
  $form['sendfax']['destination'] = array(
    '#type' => 'textfield',
    '#default_value' =>  '',
    '#title' => t('Destination number') .'*',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => ''),
  );

  // Page header
  $form['sendfax']['pageheader'] = array(
    '#type' => 'textfield',
    '#default_value' =>  '',
    '#title' => t('Page header'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => ''),
  );

  // Page content.
  $form['sendfax']['data'] = array(
    '#type' => 'textarea',
    '#title' => t('Page content'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => ''),
  );
  $form['sendfax']['file'] = array(
    '#type' => 'file',
    '#default_value' => '',
    '#theme_wrappers' => array('hpbx_file'),
    '#attributes' => array('class' => array('hpbx-click-with-alert-input')),
    '#wrapper_attributes' => array(
      'class' => array('sg-btn', 'sg-btn-file-input', 'sg-btn-secondary', 'hpbx-click-with-alert needsclick'),
      'data-alert-title' => t('Max file size'),
      'data-alert-message' => t('The maximum file size is 10MB.</br></br>Supported file type: PDF'),
      'data-alert-button-text' => t('OK'),
    )
  );

  // Submit button.
  $form['sendfax']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
  );
  $form['sendfax']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
    '#prefix' => '<span class="col-xs-12">',
    '#suffix' => '</span>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary', 'sg-disabled')),
  );  
  return $form['sendfax'];
}

/**
 * @param array $form
 * @param array $form_state
 * Modified for the bug #9571 - PE DEV: Send Fax tab - page is redirected to fax numbers page when user forgets the file upload
 * /**
 * Implements hook_form_FORM_ID_alter() to redirect the page to tab-send-fax.
 */ 
function hpbx_form_hpbx_fax_sendfax_form_alter(&$form, &$form_state, $form_id){  
  if($form_id == 'hpbx_fax_sendfax_form'){
    $form['#action'] = '#tab-send-fax';           
  } 
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_fax_faxlog_subscriber_select_form_submit($form, $form_state) {

  $Customer = hpbx_get_active_customer();
  unset($_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'],
    $_SESSION['ccs']['hpbx']['faxlog']['time_from'],
    $_SESSION['ccs']['hpbx']['faxlog']['time_to'],
    $_SESSION['ccs']['hpbx']['faxlog']['start'],
    $_SESSION['ccs']['hpbx']['faxlog']['end']
  );

  if (!empty($form_state['values']['subscriber_id'])) {
    $_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'] = $form_state['values']['subscriber_id'];
  }

  if (!empty($form_state['values']['start']) && strtotime(str_replace(', ', '', $form_state['values']['start']))) {
    $_SESSION['ccs']['hpbx']['faxlog']['time_from'] = date('Y-m-d 00:00:00', strtotime(str_replace(', ', '', $form_state['values']['start'])));
    $_SESSION['ccs']['hpbx']['faxlog']['start'] = $form_state['values']['start'];
  }

  if (!empty($form_state['values']['end']) && strtotime(str_replace(', ', '', $form_state['values']['end']))) {
    $_SESSION['ccs']['hpbx']['faxlog']['time_to'] = date('Y-m-d 23:59:59', strtotime(str_replace(', ', '', $form_state['values']['end'])));
    $_SESSION['ccs']['hpbx']['faxlog']['end'] = $form_state['values']['end'];
  }

  drupal_goto('hpbx/fax', array('fragment' => 'tab-fax-log'));
}

function hpbx_fax_faxlog_subscriber_callback($form, $form_state) {
  return $form['table'];
}

/**
 * @return array
 */
function hpbx_fax_faxlog_subscriber_select_form() {

  // Set language for picker.date.js.
  global $language;
  $filename = drupal_get_path('module', 'hpbx') . '/includes/js/pickadate/';
  $filename .= $language->language . '_' . strtoupper($language->language) . '.js';

  if (file_exists($filename)) {
    drupal_add_js($filename, array('scope' => 'footer'));
  }

  // Get active customer.
  $Customer = hpbx_get_active_customer();

  // Get active subscriber.
  $Subscriber = hpbx_get_active_subscriber();

  $subscriber_options = array();
  // Only access in case the currenly logged in user has membership.
  foreach ($Customer->get_fax_subscribers() as $Subscriber2) {
    if ($Subscriber2->is_member($Subscriber)) {
      $subscriber_options[$Subscriber2->id] = $Subscriber2->external_id . ' - '. $Subscriber2->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
    }
  }

  $form = array(
    '#prefix' => '<div class="hpbx-table-call-log"><div class="hpbx-table-selector-caption">',
    '#suffix' => '</div></div>',
  );

  $form['subscriber_id'] = array(
    '#prefix' => '<div class="row">
                    <div class="col-xs-12 col-sm-3">
                      <div class="hpbx-table-selector-title sg-text-intro">'. t('Fax number'). '</div>
                    </div>
                    <div class="col-xs-12 col-sm-9 col-md-7">
                      <div class="hpbx-table-selector-inputs">',
    '#type' => 'select',
    '#options' => $subscriber_options,
    '#required' => FALSE,
    '#attributes' => array(
      'class' => array('chzn-select', 'sg-element'),
      'placeholder' => t('Choose an option')
    ),
    '#theme_wrappers' => array(),
    '#suffix' => '</div>
                </div>
              </div>',
  );
  $form['start'] = array(
    '#prefix' => '<div class="row"><div class="col-xs-12 col-sm-3"><div class="hpbx-table-selector-title sg-text-intro">'. t('Date'). '</div></div>
      <div class="col-xs-12 col-sm-4 col-md-3"><div class="hpbx-table-selector-inputs">',
    '#type' => 'textfield',
    '#required' => FALSE,
    '#title' => t('Date'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Start'), 'id' => 'hpbx-call-log-start-date'),
    '#theme_wrappers' => array(),
    '#suffix' => '</div></div>',
  );
  $form['end'] = array(
    '#prefix' => '<div class="col-xs-12 col-sm-1"><div class="hpbx-table-selector-subcaption">'. t('through') .'</div></div>
      <div class="col-xs-12 col-sm-4 col-md-3"><div class="hpbx-table-selector-inputs">',
    '#type' => 'textfield',
    '#title' => t('through'),
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('End'), 'id' => 'hpbx-call-log-end-date'),
    '#theme_wrappers' => array(),
    '#suffix' => '</div></div></div>',
  );
  $form['submit'] = array(
    '#prefix' => '<div class="row"><div class="col-xs-12 col-md-10"><div class="hpbx-table-selector-inputs hpbx-table-selector-submit-container">',
    '#type' => 'submit',
    '#value' => t('Change'),
    '#attributes' => array('class' => array('sg-btn', 'sg-btn-secondary')),
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span></div></div></div>',
  );

  if (!empty($_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'])) {
    $form['subscriber_id']['#default_value'] = $_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'];
  }
  else {
    $form['subscriber_id']['#default_value'] = key($subscriber_options);
  }

  if (!empty($_SESSION['ccs']['hpbx']['faxlog']['start'])) {
    $form['start']['#default_value'] = $_SESSION['ccs']['hpbx']['faxlog']['start'];
  }
  if (!empty($_SESSION['ccs']['hpbx']['faxlog']['end'])) {
    $form['end']['#default_value'] = $_SESSION['ccs']['hpbx']['faxlog']['end'];
  }
  return $form;
}

function hpbx_fax_faxlog_get_table() {

  module_load_include('inc', 'hpbx', 'includes/hpbx.callrecords');

  global $ActiveReseller;

  $labels = array(
    'cfu' => t('Forwarded unconditionally'),
    'cfb' => t('Forwarded due to busy'),
    'cfna' => t('Forwarded due to unavailable'),
    'cft' => t('Forwared due to timeout'),
    'call' => t('Normal call'),
    'inbound' => t('Inbound'),
    'outbound' => t('Outbound'),
  );

  $Customer = hpbx_get_active_customer();

  // Set title.
  drupal_set_title(t('Fax'));

  // Init form.
  $form = array();



  $header = array(
    'direction' => array('data'=> '&nbsp;', 'class' => array('hpbx-show')),
    'start_time' => array('data'=> t('Date & Time'), 'field' => 'time', 'sort' => 'desc', 'class' => array('hpbx-show-xs')),
    'information' => array('data'=> t('Information'), 'field' => 'time', 'sort' => 'desc', 'class' => array('hpbx-show', 'hpbx-hide-xs')),
    'numbers' => array('data'=> t('Numbers'), 'field' => 'numbers', 'class' => array('hpbx-show-xs', 'hpbx-hide-sm')),
    'caller' => array('data'=> t('Sender'), 'field' => 'caller', 'class' => array('hpbx-show-sm')),
    'called' => array('data'=> t('Receiver'), 'field' => 'callee', 'class' => array('hpbx-show-sm')),
    'status' => array('data'=> t('Status'), 'field' => 'status', 'class' => array('hpbx-show-sm')),
    'pages' => array('data'=> t('Pages'), 'field' => 'pages', 'class' => array('hpbx-show-sm')),
    'operations' => array('class' => array('hpbx-show')),
  );

  $form['table'] = array(
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-call-log hpbx-table-sortable'),
    '#header' => $header,
  );

  $select_form = drupal_get_form('hpbx_fax_faxlog_subscriber_select_form');
  $form['table']['#prefix'] = drupal_render($select_form);

  $order = tablesort_get_order($header);
  $query = array();


  // Extra safety.
  if (empty($Customer->id)) {
    drupal_not_found();
    die();
  }


  if (isset($_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'])) {
    $query['subscriber_id'] = $_SESSION['ccs']['hpbx']['faxlog'][$Customer->id]['subscriber_id'];
  }
  elseif (!empty($form['subscriber_id']['#default_value'])) {
    $query['subscriber_id'] = $form['subscriber_id']['#default_value'];
  }
  elseif (!empty($select_form['subscriber_id']['#default_value'])) {
    $query['subscriber_id'] = $select_form['subscriber_id']['#default_value'];
  }

  if (!empty($_SESSION['ccs']['hpbx']['faxlog']['time_from'])) {
    $query['time_from'] = $_SESSION['ccs']['hpbx']['faxlog']['time_from'];

    // Convert to sipwise timezone.
    hpbx_convert_to_sipwise_datetime($query['time_from']);
  }
  if (!empty($_SESSION['ccs']['hpbx']['faxlog']['time_to'])) {
    $query['time_to'] = $_SESSION['ccs']['hpbx']['faxlog']['time_to'];

    // Convert to sipwise timezone.
    hpbx_convert_to_sipwise_datetime($query['time_to']);
  }

  $query['page'] = pager_find_page() + 1;
  $query['rows'] = 10;
  $query['order_by'] = $order['sql'];
  $query['order_by_direction'] = tablesort_get_sort($header);

  $prev_date = NULL;

  $pager = '';

  if (!empty($query['subscriber_id'])) {
    foreach (sipwise_api_get_all('faxes', $query) as $i => $Call) {
	
	//Bug #9633 - PE DEV: Fax log - Fax logs are not corrected for the timezone
      // Convert to local reseller timezone.
      hpbx_convert_to_local_datetime($Call->time);
	  
      // For every new date add a new table row.
      $cur_date = date('d-m-Y', strtotime(substr($Call->time, 0, 23)));

      if (is_null($prev_date) || (!is_null($prev_date) && $prev_date != $cur_date)) {

        $form['table'][$cur_date] = array(
          '#attributes' => array(
            'class' => array(
              'hpbx-table-head-row',
              'sg-text-small'
            )
          )
        );
        $form['table'][$cur_date]['empty']['data'] = array('#markup' => '');
        $form['table'][$cur_date]['empty']['#attributes']['class'] = array('hpbx-show');
        $form['table'][$cur_date]['start_time']['data'] = array('#markup' => t(drupal_ucfirst(date('l', strtotime($cur_date)))) . ' ' . date('d-m-Y', strtotime($cur_date)));
        $form['table'][$cur_date]['start_time']['#attributes']['colspan'] = 8;
        $form['table'][$cur_date]['start_time']['#attributes']['class'] = array('hpbx-show');
      }

      $prev_date = $cur_date;

      $direction_markup = '<div id="' . $Call->id . '" title="' . $labels[$Call->direction . 'bound'] . '" class="hpbx-table-icon hpbx-call-icon hpbx-' . $Call->direction . 'bound"></div>';

      // 1.  Direction
      $form['table'][$Call->id . '-' . $i]['direction'] = array(
        'data' => array('#markup' => $direction_markup),
        '#attributes' => array(
          'class' => array(
            'hpbx-show',
            'hpbx-table-icon-container'
          )
        )
      );

      // 2. Start time.
      $form['table'][$Call->id . '-' . $i]['start_time'] = array(
        'data' => array('#markup' => date('H:i:s', strtotime(substr($Call->time, 0, 23)))),
        '#attributes' => array('class' => array('hpbx-show-xs'))
      );
      if ($i == 0) {
        $form['table'][$Call->id . '-' . $i]['start_time']['#attributes']['class'][] = 'hpbx-invisible-when-expanded';
      }

      // 3. Information for responsive design.
      $form['table'][$Call->id . '-' . $i]['information'] = array(
        'data' => array('#markup' => date('H:i:s', strtotime(substr($Call->time, 0, 23))) . '<br>' . hpbx_fax_faxlog_get_caller($Call) . '<span class="hpbx-hide-when-expanded"><br>' . hpbx_fax_faxlog_get_called($Call) . '</span>'),
        '#attributes' => array('class' => array('hpbx-show', 'hpbx-hide-xs'))
      );

      // 4. Information for responsive design.
      $form['table'][$Call->id . '-' . $i]['basic'] = array(
        'data' => array('#markup' => hpbx_fax_faxlog_get_caller($Call) . '<span class="hpbx-hide-when-expanded"><br>' . hpbx_fax_faxlog_get_called($Call) . '</span>'),
        '#attributes' => array('class' => array('hpbx-show-xs', 'hpbx-hide-sm'))
      );

      // 5. Caller
      $form['table'][$Call->id . '-' . $i]['caller'] = array(
        'data' => array('#markup' => hpbx_fax_faxlog_get_caller($Call)),
        '#attributes' => array('class' => array('hpbx-show-sm'))
      );

      if ($i == 0) {
        $form['table'][$Call->id . '-' . $i]['caller']['data']['#markup'] .= '<div class="hpbx-expandable-content-counter">' . ($i + 1) . '</div>';
      }
      else {
        $form['table'][$Call->id . '-' . $i]['caller']['data']['#markup'] .= '<div class="hpbx-horizontal-line"></div>';
        $form['table'][$Call->id . '-' . $i]['caller']['#attributes']['class'][] = ' hpbx-invisible-when-expanded';
      }

      // 6. Called.
      $form['table'][$Call->id . '-' . $i]['called'] = array(
        'data' => array('#markup' => hpbx_fax_faxlog_get_called($Call)),
        '#attributes' => array(
          'class' => array(
            'hpbx-show-sm',
            ($i == 0 ? 'hpbx-invisible-when-expanded' : '')
          )
        )
      );

      if ($i) {
        $form['table'][$Call->id . '-' . $i]['called']['data']['#markup'] .= '<div class="hpbx-expandable-content-counter">' . ($i + 1) . '</div>';
      }


      // State.
      $form['table'][$Call->id . '-' . $i]['status'] = array(
        'data' => array('#markup' => t(drupal_ucfirst(strtolower($Call->status)))),
        '#attributes' => array(
          'class' => array(
            'hpbx-show-sm',
            ($i == 0 ? 'hpbx-invisible-when-expanded' : '')
          )
        )
      );

      // Duration.
      $form['table'][$Call->id . '-' . $i]['pages'] = array(
        'data' => array('#markup' => $Call->pages),
        '#attributes' => array(
          'class' => array(
            'hpbx-show-sm',
            ($i == 0 ? 'hpbx-invisible-when-expanded' : '')
          )
        )
      );

      $form['table'][$Call->id . '-' . $i]['operations'] = array(
        'data' => array('#markup' => ($i == 0 ? '<div class="hpbx-table-icon hpbx-expand-icon"></div>' : '')),
        '#attributes' => array('class' => array('hpbx-show hpbx-table-icon-container'))
      );
    }

    global $total_count;


    if ($total_count > $query['rows']) {
      $total_pages = ceil($total_count / $query['rows']);

      // Set pager options.
      pager_default_initialize($total_count, $query['rows']);

      // Render pager.
      $pager = theme('pager', array('element' => 0, 'quantity' => 5, 'parameters' => array(), 'tags' => ''));

      $form['table']['extra'] = array('#attributes' => array('class' => array('hpbx-table-extra-row-when-has-pagination')));
      $form['table']['extra']['direction']['#markup'] = 'test';
      $form['table']['extra']['direction']['#attributes'] = array('colspan' => 6);
    }
  }



  return drupal_render($form) . '</div>'. $pager . '</div>';
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_fax_sendfax_form_validate($form, &$form_state) {

  global $ActiveReseller;


  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js',
    array('scope' => 'footer'));


  // Validate sendfax origin subscriber.
  if (!$Subscriber = SipwiseEntity::load('subscribers', $form_state['values']['subscriber_id'])) {
    form_set_error('sendfax][subscriber_id', t('Problem retrieving the originating subscriber'));
  }

  // Validate the destination number.
  if(!preg_match('/^\+?[0-9]+$/', $form_state['values']['destination'])) {
    form_set_error('sendfax][destination', t('Please enter a valid destination number'));
  }
  else {
    $form_state['values']['destination'] = trim($form_state['values']['destination']);
  }

  // Validate sendfax uploaded file.
  if (!empty($_FILES['files']['name']['file'])) {

    // Verify if any error occured while uploading the csv file.
    if ($_FILES['files']['error']['file'] !== UPLOAD_ERR_OK) {
      watchdog('hpbx_fax', 'Unknown error while uploading the fax file');
      form_set_error('file', t('Unknown error while uploading the fax file'));
    }

    // Verify if the uploaded file does exist.
    if (!file_exists($_FILES['files']['tmp_name']['file'])) {
      watchdog('hpbx_fax', 'Uploaded PDF file does not exist (anymore');
      form_set_error('file', t('Unknown error while uploading the fax file'));
    }

    if (!($handle = fopen($_FILES['files']['tmp_name']['file'], 'r'))) {
      watchdog('hpbx_fax', 'Could not open uploaded PDF file');
      form_set_error('file', t('Unknown error while uploading the fax file'));
    }

    // Verify if the mime type is correct.
    if (mime_content_type ($_FILES['files']['tmp_name']['file'])!='application/pdf') {
      watchdog('hpbx_fax', 'Please upload a valid PDF file');
      form_set_error('file', t('Please upload a PDF file'));
    }
  }
  else {
    form_set_error('file', t('Please upload a PDF file'));
  }
}

/**
 * @param array $form
 * @param array $form_state
 */
function hpbx_fax_sendfax_form_submit($form, $form_state) {

  $SipwiseFax = new SipwiseFax();
  $SipwiseFax->subscriber_id = $form_state['values']['subscriber_id'];
  $SipwiseFax->destination = $form_state['values']['destination'];
  $SipwiseFax->pageheader = $form_state['values']['pageheader'];
  $SipwiseFax->data = $form_state['values']['data'];
  $SipwiseFax->faxfile = $_FILES['files']['tmp_name']['file'];

  if ($SipwiseFax->send()) {
    drupal_set_message(t('Your fax is being processed. This may take a few minutes.'));
  }
  else {
    drupal_set_message(t('Failed to sent fax'), 'error');
  }
  drupal_goto('hpbx/fax', array('fragment' => 'tab-send-fax'));
}
