<?php

/**
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function hpbx_callrecords_subscriber_select_form($form, $form_state) {

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

  // Set subscriber opties.
  if (user_access('hpbx edit subscriber')) {
    $subscriber_options = hpbx_get_subscriber_options($Customer);
  }
  else {
    // huntgroups where current user is member of.
    $subscriber_options = hpbx_get_subscriber_options($Customer,  $Subscriber, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, TRUE);

    // Add own number.
    $subscriber_options  = array($Subscriber->id => t('Own number')) + $subscriber_options;
  }
  $form = array(
    '#prefix' => '<div class="hpbx-table-call-log"><div class="hpbx-table-selector-caption">',
    '#suffix' => '</div></div>',
  );

  $form['subscriber_id'] = array(
    '#type' => 'select',
    '#options' => $subscriber_options,
    '#required' => FALSE,
    '#empty_option' => user_access('hpbx edit subscriber') ? t('All') : NULL,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'), 'placeholder' => t('Choose an option')),
    '#theme_wrappers' => array(),
    '#prefix' => '<div class="row"><div class="col-xs-12 col-sm-3"><div class="hpbx-table-selector-title sg-text-intro">'. t('Subscriber or group'). '</div></div>
      <div class="col-xs-12 col-sm-9 col-md-7"><div class="hpbx-table-selector-inputs">',
    '#suffix' => '</div></div></div>',
  );
  $form['start'] = array(
    '#type' => 'textfield',
    '#required' => FALSE,
    '#title' => t('Date'),
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('Start'), 'id' => 'hpbx-call-log-start-date'),
    '#theme_wrappers' => array(),
    '#prefix' => '<div class="row"><div class="col-xs-12 col-sm-3"><div class="hpbx-table-selector-title sg-text-intro">'. t('Date'). '</div></div>
      <div class="col-xs-12 col-sm-4 col-md-3"><div class="hpbx-table-selector-inputs">',
    '#suffix' => '</div></div>',
  );
  $form['end'] = array(
    '#type' => 'textfield',
    '#title' => t('through'),
    '#required' => FALSE,
    '#attributes' => array('class' => array('sg-element'), 'placeholder' => t('End'), 'id' => 'hpbx-call-log-end-date'),
    '#theme_wrappers' => array(),
    '#prefix' => '<div class="col-xs-12 col-sm-1"><div class="hpbx-table-selector-subcaption">'. t('through') .'</div></div>
      <div class="col-xs-12 col-sm-4 col-md-3"><div class="hpbx-table-selector-inputs">',
    '#suffix' => '</div></div></div>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Change'),
    '#attributes' => array('class' => array('sg-btn', 'sg-btn-secondary')),
    '#prefix' => '<div class="row"><div class="col-xs-12 col-md-10"><div class="hpbx-table-selector-inputs hpbx-table-selector-submit-container">',
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span></div></div></div>',
  );


  if (!empty($_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'])) {
    $form['subscriber_id']['#default_value'] = $_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'];
  }
  elseif (!is_null($_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id']) && $_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'] !== '') {
    $form['subscriber_id']['#default_value'] = $Subscriber->id;
  }

  if (!empty($_SESSION['ccs']['hpbx']['callrecords']['start'])) {
    $form['start']['#default_value'] = $_SESSION['ccs']['hpbx']['callrecords']['start'];
  }
  if (!empty($_SESSION['ccs']['hpbx']['callrecords']['end'])) {
    $form['end']['#default_value'] = $_SESSION['ccs']['hpbx']['callrecords']['end'];
  }
  return $form;
}

/**
 * Form submit handler for hpbx_callrecords_subscriber_select_form()
 *
 * @param array $form
 * @param array $form_state
 *
 * @return void
 */
function hpbx_callrecords_subscriber_select_form_submit($form, $form_state) {
  $Customer = hpbx_get_active_customer();
  unset($_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'],
    $_SESSION['ccs']['hpbx']['callrecords']['start_ge'],
    $_SESSION['ccs']['hpbx']['callrecords']['start_le'],
    $_SESSION['ccs']['hpbx']['callrecords']['start'],
    $_SESSION['ccs']['hpbx']['callrecords']['end']
  );

  if (!empty($form_state['values']['subscriber_id'])) {
    $_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'] = $form_state['values']['subscriber_id'];
  }

  if (!empty($form_state['values']['start']) && strtotime(str_replace(', ', '', $form_state['values']['start']))) {
    $_SESSION['ccs']['hpbx']['callrecords']['start_ge'] = date('Y-m-d 00:00:00', strtotime(str_replace(', ', '', $form_state['values']['start'])));
    $_SESSION['ccs']['hpbx']['callrecords']['start'] = $form_state['values']['start'];
  }

  if (!empty($form_state['values']['end']) && strtotime(str_replace(', ', '', $form_state['values']['end']))) {
    $_SESSION['ccs']['hpbx']['callrecords']['start_le'] = date('Y-m-d 23:59:59', strtotime(str_replace(', ', '', $form_state['values']['end'])));
    $_SESSION['ccs']['hpbx']['callrecords']['end'] = $form_state['values']['end'];
  }

  // Added to fix the Bug #9583 PE DEV: Call records - Incorrect redirect when changing the subscriber/date interval of the call records
  drupal_goto('hpbx/call-log',array('query' => array(
      'search' => 'true'
  ),));
}

/**
 * @param array $form
 * @param array $form_state
 *
 * @return array
 */
function hpbx_callrecords_subscriber_callback($form, $form_state) {
  return $form['table'];
}

/**
 * @param array &$call_entries
 * @param array $query
 *
 * @return array
 */
function hpbx_callrecords_get_call_entries(&$call_entries, $query) {
  $prev_call_id = $uniqid = NULL;

  foreach (sipwise_api_get_all('calllists', $query) as $Call) {

    // Convert to local reseller timezone.
    hpbx_convert_to_local_datetime($Call->start_time);

    // Strip end of call_id for internal calls.
    if (substr($Call->call_id, -6) == '_pbx-1') {
      $Call->call_id = substr($Call->call_id, 0, -6);
    }

    if (is_null($uniqid) || $prev_call_id!=$Call->call_id) {
      // new call id.
	  //To fix the Bug - #9551 PE DEV: Call records - Missing records in case of multiple call forwards
      $uniqid = uniqid('calllog_',true);
      //$key = 1;
      $key = 0;
      $prev_call_id = $Call->call_id;
    }

    $Call->uniqid = $uniqid;
    //To fix the Bug - #9551 PE DEV: Call records - Missing records in case of multiple call forwards
    /* 
    $Call->weight = 0;
    if ($Call->type == 'call') {
      $Call->weight = 0;
      $call_entries[$Call->call_id][0] = $Call;
    }
    else {
      $call_entries[$Call->call_id][$key] = $Call;
      $key++;
    }*/

    $call_entries[$Call->call_id][$key] = $Call;
    $key++;
  }
}

/**
 * Callback function for hpbx/call-log
 *
 * @return string
 */
function hpbx_callrecords_overview_page() {

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
  $Subscriber = hpbx_get_active_subscriber();

  // Set title.
  drupal_set_title(t('Call records'));

  // Init form.
  $form = array();

  $f = drupal_get_form('hpbx_callrecords_subscriber_select_form');

  $subscriber_select_form = drupal_render($f);

  $header = array(
    'direction' => array('data'=> '&nbsp;', 'class' => array('hpbx-show')),
    'start_time' => array('data'=> t('Date & Time'), 'field' => 'start_time', 'sort' => 'desc', 'class' => array('hpbx-show-xs')),
    'information' => array('data'=> t('Information'), 'field' => 'start_time', 'sort' => 'desc', 'class' => array('hpbx-show', 'hpbx-hide-xs')),
    'numbers' => array('data'=> t('Numbers'), 'field' => 'numbers', 'class' => array('hpbx-show-xs', 'hpbx-hide-sm')),
    'caller' => array('data'=> t('Caller'), 'field' => 'caller', 'class' => array('hpbx-show-sm')),
    'called' => array('data'=> t('Called'), 'field' => 'called', 'class' => array('hpbx-show-sm')),
    'type' => array('data'=> t('Call type'), 'field' => 'type', 'class' => array('hpbx-show-md')),
    'status' => array('data'=> t('Status'), 'field' => 'status', 'class' => array('hpbx-show-sm')),
    'duration' => array('data'=> t('Duration'), 'field' => 'duration', 'class' => array('hpbx-show-sm')),
    'operations' => array('class' => array('hpbx-show')),
  );

  $form['table'] = array(
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#class' => array('hpbx-table hpbx-table-fullwidth hpbx-table-call-log hpbx-table-sortable'),
    '#header' => $header,
  );


  //if (user_access('hpbx edit subscriber')) {
  $form['table']['#prefix'] = $subscriber_select_form;
  //}
  //else {
  //$form['table']['#prefix'] = '<div class="hpbx-table-call-log"><div class="hpbx-table-selector-caption">&nbsp;</div></div>';
  //}
  $order = tablesort_get_order($header);
  $query = array();


  // Extra safety.
  if (empty($Customer->id)) {
    drupal_not_found();
    die();
  }

  $query['customer_id'] = $Customer->id;
  $query['intra_alias_field'] = 'gpp1';

  if (isset($_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'])) {
    $query['subscriber_id'] = $_SESSION['ccs']['hpbx']['callrecords'][$Customer->id]['subscriber_id'];
  }
  elseif (!empty($form['subscriber_id']['#default_value'])) {
    $query['subscriber_id'] = $form['subscriber_id']['#default_value'];
  }
  elseif (!user_access('hpbx edit subscriber')) {
    // Filter on own subscriber id.
    $query['subscriber_id'] = $Subscriber->id;
  }

  if (!empty($_SESSION['ccs']['hpbx']['callrecords']['start_ge'])) {
    $query['start_ge'] = $_SESSION['ccs']['hpbx']['callrecords']['start_ge'];

    // Convert to sipwise timezone.
    hpbx_convert_to_sipwise_datetime($query['start_ge']);
  }
  if (!empty($_SESSION['ccs']['hpbx']['callrecords']['start_le'])) {
    $query['start_le'] = $_SESSION['ccs']['hpbx']['callrecords']['start_le'];

    // Convert to sipwise timezone.
    hpbx_convert_to_sipwise_datetime($query['start_le']);
  }

  $call_entries = array();

  $page = pager_find_page() + 1;
  $query['rows'] = 30;
  $query['order_by'] = $order['sql'];
  $query['order_by_direction'] = tablesort_get_sort($header);

  $query['page'] = $page;
  hpbx_callrecords_get_call_entries($call_entries, $query);
  $entries_to_display = count($call_entries);

  // Get the previous page rows, so we can show all legs per call.
  $query['page'] = $page - 1;
  if ($query['page'] > 0) {
    hpbx_callrecords_get_call_entries($call_entries, $query);
  }

  // Get the next page rows, so we can show all legs per call.
  $query['page'] = $page + 1;
  hpbx_callrecords_get_call_entries($call_entries, $query);

  // Remove all antries we don't need.
  array_splice($call_entries, $entries_to_display);

  // Only show

  // Walk calls.

  foreach ($call_entries as $call_id => $Calls) {

    $parent_id = FALSE;

    if (count($Calls) > 1) {
      //To fix the Bug - #9551 PE DEV: Call records - Missing records in case of multiple call forwards
      /*for ($x=1;$x<count($Calls);$x++) {

        if ($Calls[$x]->source_user == $Calls[0]->destination_user) {
          $Calls[$x]->weight = 1;
        }
        else {
          $Calls[$x]->weight = 2;
        }
      }

      // Sort calls on id.
      usort($Calls, function($a, $b) {
        return strcmp($a->weight, $b->weight);
      });*/
      
      // Sort calls on id.
      rsort($Calls);
      // Make a duplicate of the first call leg to show it in the expandaded version.
      array_unshift($Calls, $Calls[0]);
    }

    foreach ($Calls as $i => $Call) {

      if ($Call->status == 'cancel') {
        $Call->status = 'cancelled';
      }

      // For every new date add a new table row.
      $cur_date = date('d-m-Y', strtotime(substr($Call->start_time, 0, 23)));

      if (is_null($prev_date) || (!is_null($prev_date) && $prev_date!=$cur_date)) {

        $form['table'][$cur_date] = array('#attributes' => array('class'=> array('hpbx-table-head-row', 'sg-text-small')));
        $form['table'][$cur_date]['empty']['data'] = array('#markup' => '');
        $form['table'][$cur_date]['empty']['#attributes']['class'] = array('hpbx-show');
        $form['table'][$cur_date]['start_time']['data'] = array('#markup' => t(drupal_ucfirst(date('l', strtotime($cur_date)))) . ' '. date('d-m-Y', strtotime($cur_date)));
        $form['table'][$cur_date]['start_time']['#attributes']['colspan'] = 8;
        $form['table'][$cur_date]['start_time']['#attributes']['class'] = array('hpbx-show');
      }

      $prev_date = $cur_date;

      if (count($Calls) > 1 && $i == 0) {
        $form['table'][$Call->id . '-'. $i]['#attributes'] = array(
          'class' => array('hpbx-table-row-expandable'),
          'data-expandable-id' => $Call->uniqid
        );
        $parent_id = $Call->uniqid;
      }
      elseif ($parent_id) {
        $form['table'][$Call->id . '-'. $i]['#attributes'] = array(
          'class' => array('hpbx-table-row-expandable-content'),
          'data-expandable-parent-id' => $parent_id
        );
      }

      if (count($Calls) > 1 && $i == count($Calls)) {
        $form['table'][$Call->id . '-'. $i]['#attributes']['class'][] = 'hpbx-table-row-expandable-content-last';
      }

      // Columns.
      $direction_markup = '';
      if ($i == 0) {
        $direction_markup = '<div id="'. $Call->call_id . '" title="'.$labels[$Call->direction .'bound'].'" class="hpbx-table-icon hpbx-call-icon hpbx-'. $Call->direction .'bound"></div>';
      }

      // 1.  Direction
      $form['table'][$Call->id . '-'. $i]['direction'] = array(
        'data' =>  array('#markup' => $direction_markup),
        '#attributes' => array('class' => array('hpbx-show', 'hpbx-table-icon-container'))
      );

      // 2. Start time.
      $form['table'][$Call->id . '-'. $i]['start_time'] = array(
        'data' =>  array('#markup' => date('H:i:s', strtotime(substr($Call->start_time, 0, 23)))),
        '#attributes' => array('class' => array('hpbx-show-xs'))
      );
      if ($i == 0) {
        $form['table'][$Call->id . '-'. $i]['start_time']['#attributes']['class'][] = 'hpbx-invisible-when-expanded';
      }

      // 3. Information for responsive design.
      $form['table'][$Call->id . '-'. $i]['information'] = array(
        'data' => array('#markup' => date('H:i:s', strtotime(substr($Call->start_time, 0, 23))) . '<br>'. hpbx_callrecords_get_caller($Call) . '<span class="hpbx-hide-when-expanded"><br>'.hpbx_callrecords_get_called($Call).'</span>'),
        '#attributes' => array('class' => array('hpbx-show', 'hpbx-hide-xs'))
      );

      // 4. Information for responsive design.
      $form['table'][$Call->id . '-'. $i]['basic'] = array(
        'data' => array('#markup' => hpbx_callrecords_get_caller($Call) . '<span class="hpbx-hide-when-expanded"><br>'.hpbx_callrecords_get_called($Call).'</span>'),
        '#attributes' => array('class' => array('hpbx-show-xs', 'hpbx-hide-sm'))
      );

      // 5. Caller
      $form['table'][$Call->id . '-'. $i]['caller'] = array(
        'data' => array('#markup' => hpbx_callrecords_get_caller($Call)),
        '#attributes' => array('class' => array('hpbx-show-sm'))
      );

      if ($i == 0) {
        $form['table'][$Call->id . '-'. $i]['caller']['data']['#markup'] .= '<div class="hpbx-expandable-content-counter">'. ($i+1) .'</div>';
      }
      else {
        $form['table'][$Call->id . '-'. $i]['caller']['data']['#markup'] .= '<div class="hpbx-horizontal-line"></div>';
        $form['table'][$Call->id . '-'. $i]['caller']['#attributes']['class'][] = ' hpbx-invisible-when-expanded';
      }

      // 6. Called.
      $form['table'][$Call->id . '-'. $i]['called'] = array(
        'data' => array('#markup' => hpbx_callrecords_get_called($Call)),
        '#attributes' => array('class' => array('hpbx-show-sm', ($i==0 ? 'hpbx-invisible-when-expanded': '')))
      );

      if ($i) {
        $form['table'][$Call->id . '-'. $i]['called']['data']['#markup'] .= '<div class="hpbx-expandable-content-counter">' . ($i+1) . '</div>';
      }

      // Type of Call.
      $form['table'][$Call->id . '-'. $i]['type'] = array(
        'data' => array('#markup' => '<span title="'.$labels[$Call->type].'"class="LGI-icon'. $Call->type . '"></span>'),
        '#attributes' => array('class' => array('hpbx-show-md', ($i==0 ? 'hpbx-invisible-when-expanded': '')))
      );

      // State.
      $form['table'][$Call->id . '-'. $i]['status'] = array(
        //Bug #9594 - PE DEV: correct typo noanswer	  
        //'data' => array('#markup' => t(drupal_ucfirst($Call->status))),
        'data' => array('#markup' => t(drupal_ucfirst($Call->status=='noanswer'?"no answer" : $Call->status))),
        '#attributes' => array('class' => array('hpbx-show-sm', ($i==0 ? 'hpbx-invisible-when-expanded': '')))
      );

      // Duration.
      $call_duration = explode('.',$Call->duration); // issue fix #9552 - Removed milliseconds in call records duration
      $form['table'][$Call->id . '-'. $i]['duration'] = array(
        'data' => array('#markup' => $call_duration[0]),
        '#attributes' => array('class' => array('hpbx-show-sm', ($i==0 ? 'hpbx-invisible-when-expanded': '')))
      );

      $form['table'][$Call->id . '-'. $i]['operations'] = array(
        'data' => array('#markup' => ($i==0 ? '<div class="hpbx-table-icon hpbx-expand-icon"></div>' : '')),
        '#attributes' => array('class' => array('hpbx-show hpbx-table-icon-container'))
      );
    }
  }

  global $total_count;

  $pager = '';
  if ($total_count > $query['rows']) {
    //$total_pages = ceil($total_count / $query['rows']);

    // Set pager options.
    pager_default_initialize($total_count, $query['rows']);

    // Render pager.
    $pager = theme('pager', array('element' => 0, 'quantity' => 5, 'parameters' => array(), 'tags' => ''));
  }
  return drupal_render($form) . '<div class="'. 'hpbx-pagination-container' .'">'. $pager . '</div>';
}

/**
 * Helper function to normalize a number received from  Sipwise.
 *
 * @param $cli
 *
 * @return string
 */
function hpbx_callrecords_normalize($cli) {
  if (empty($cli)) {
    return t('Anonymous');
  }
  elseif (preg_match('/^conference/', $cli)) {
    return t('Conference room');
  }
  elseif (preg_match('/^office-hours/', $cli)) {
    return t('Office hours');
  }
  elseif (preg_match('/^auto-attendant/', $cli)) {
    return t('Auto attendant');
  }
  elseif (preg_match('/^vmb/', $cli)) {
    return t('Voicemail');
  }
  elseif (preg_match('/^voicebox\@.+/', $cli)) {
    return t('Voicemail retrieval');
  }
  elseif (preg_match('/^vmu[0-9]+@voicebox\.local$/', $cli)) {
    return t('Voicemail deposit');
  }
  elseif (preg_match('/^voiceboxpass\@.+/', $cli)) {
    return t('Voicemail via ext.');
  }
  elseif (preg_match('/^emergency_([0-9A-Z]+)\@/', $cli, $matches)) {
    return substr($matches[1], -3);
  }
  // Remove domains.
  $cli = preg_replace('/@.+/', '', $cli);

  $cli = str_replace('terminated', t('terminated'), $cli);

  // Decode %xy to correct character.
  $cli = urldecode($cli);

  return drupal_ucfirst($cli);
}

/**
 * @param $Call
 *
 * @return string
 */
function hpbx_callrecords_get_called($Call) {
  if ($Call->direction == 'out') {
    $prefix = !empty($_GET['debug_calls']) ? '['.$Call->id.':other_cli] ' : '';
    return $prefix . hpbx_callrecords_normalize($Call->other_cli);
  }
  elseif ($Call->direction == 'in') {
    $prefix = !empty($_GET['debug_calls']) ? '['.$Call->id.':own_cli] ' : '';
    return $prefix . hpbx_callrecords_normalize($Call->own_cli);
  }
  return FALSE;
}

/**
 * @param $Call
 *
 * @return string
 */
function hpbx_callrecords_get_caller($Call) {
  if ($Call->direction == 'out') {
    $prefix = !empty($_GET['debug_calls']) ? '['.$Call->id.':own_cli] ' : '';
    return $prefix . hpbx_callrecords_normalize($Call->own_cli);
  }
  elseif ($Call->direction == 'in') {
    $prefix = !empty($_GET['debug_calls']) ? '['.$Call->id.':other_cli] ' : '';
    return $prefix . hpbx_callrecords_normalize($Call->other_cli);
  }
  return FALSE;
}
