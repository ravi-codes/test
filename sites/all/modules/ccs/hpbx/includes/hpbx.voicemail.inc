<?php

/**
 * Callback for hpbx/voicemails/count.
 */
function hpbx_voicemails_inbox_count_callback() {
  
  $totals = hpbx_voicemails_get_totals(TRUE, FALSE);
  
  $total = '';
  
  if (isset($totals['personal_inbox'])) {
    $total += $totals['personal_inbox'];
  }
  if (isset($totals['huntgroup_inbox'])) {
    $total += $totals['huntgroup_inbox'];
  }
      
  echo $total;
}

function hpbx_voicemail_delete_page($voicemail_id) {

  try {

    // Delete voicemail.
    if ($Voicemail = SipwiseEntity::load('voicemails', $voicemail_id)) {

      // Load active customer.
      $Subscriber = hpbx_get_active_subscriber();
      
      // Verify if the voicemail to be listened to is owned by the active
      // subscriber or if the subscriber is member of the hungroup.
      if ($Voicemail->subscriber_id != $Subscriber->id && !in_array($Voicemail->subscriber_id, $Subscriber->pbx_group_ids))  {
        drupal_not_found();
        exit();
      }
      $Voicemail->delete();
      
      // Set message.
      drupal_set_message(t('Voicemail successfully deleted'));
    }
  }
  catch (Exception $e) {
    drupal_set_message(t($e->getMessage()), 'error');
  }
  drupal_goto('hpbx/voicemails');
}


function hpbx_voicemail_play($voicemail_id) {

  $type = FALSE;
  if (preg_match('/^play\.(ogg|mp3|wav)$/', arg(3), $matches)) {
    $type = $matches[1];
  }
  
  try {

    if ($type && $Voicemail = SipwiseEntity::load('voicemails', $voicemail_id)) {

      $Subscriber = hpbx_get_active_subscriber();
      
      // Verify if the voicemail to be listened to is owned by the active
      // subscriber or if the subscriber is member of the hungroup.
      if ($Voicemail->subscriber_id != $Subscriber->id && !in_array($Voicemail->subscriber_id, $Subscriber->pbx_group_ids))  {
        drupal_not_found();
        exit();
      }

      // Load the recording.
      $body = SipwiseEntity::load('voicemailrecordings', $voicemail_id);

      if (!empty($body)) {
        switch ($type) {
          case 'wav':
            header('Content-Type: audio/x-wav');
            break;
          case 'mp3':
            header('Content-Type: audio/mpeg');
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

function hpbx_voicemails_overview_form($form, $form_state) {

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js', array('scope' => 'footer'));
  
  global $user;

  $Customer = hpbx_get_active_customer();
  
  $Subscriber = hpbx_get_active_subscriber();
  
  if ($Subscriber->id == $Customer->get_pilot_subscriber()->id) {
    drupal_set_message(t('Create an administrative subscriber first'), 'error');
    drupal_goto('hpbx/subscribers');
  }

  // Set title.
  drupal_set_title(t('Voicemail'));

  global $title_suffix;
  
  $title_suffix = '';
  
    
  // Init form.
  $form = array(
    '#prefix' => '<div class="hpbx-content-container hpbx-content-container-voicemail-page">',  
    '#suffix' => '</div>'
  );

  /*
  $select_form = '';
  if (user_access('hpbx edit subscriber')) {
    $f = drupal_get_form('hpbx_voicemails_select_form');
    $select_form = drupal_render($f);
  }
  */
  
  $header_chk = <<<EOF
  <div class="sg-custom-check-and-radio sg-custom-check">
      <label title="">
          <input type="checkbox" name="hpbx-chk-voicemail-all" id="hpbx-chk-voicemail-all" value="1">
          <span class="sg-caption"></span>
          <div class="sg-alternative-display">
              <div class="sg-icon-check"></div>
          </div>
      </label>
  </div>
EOF;

    // Select form.
    //if (user_access('hpbx edit subscriber')) {

  $subscriber_options = hpbx_get_subscriber_options(
    hpbx_get_active_customer(),
    hpbx_get_active_subscriber(),
    $number_key = FALSE,
    $is_pbx_pilot = FALSE,
    $is_pbx_group = TRUE,
    $autoattendant = FALSE,
    $conference = FALSE,
    $is_subscriber = FALSE,
    $must_be_pbx_group_member = TRUE
  );
      
  $options = array();
  if (isset($subscriber_options[t('Groups')])) {
    $options = $subscriber_options[t('Groups')];
  }
  $form['select'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('hpbx-table-selector-caption')),
  );

  if (count($options)) {
    $form['select']['subscriber_id'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#required' => FALSE,
      '#empty_option' => t('Choose an option'),
      '#empty_value' => $Subscriber->id,
      '#attributes' => array('class' => array('chzn-select', 'sg-element'), 'placeholder' => t('Choose an option')),
      '#theme_wrappers' => array(),
      '#prefix' => '<div class="row"><div class="col-xs-12 col-sm-3"><div class="hpbx-table-selector-title sg-text-intro">'. t('Voicemail group'). ':</div></div>
      <div class="col-xs-12 col-sm-9 col-md-4"><div class="hpbx-table-selector-inputs">',
      '#suffix' => '</div></div></div>',
    );
  }
  else {
    $form['select']['subscriber_id']['#markup'] = '<br/></br>';
  }
      
  $form['select']['folder'] = array(
    '#type' => 'select',
    '#options' => array('INBOX' => t('Unheard'), 'Old' => t('Heard')),
    '#required' => FALSE,
    '#empty_option' => t('All messages'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'), 'placeholder' => t('Choose an option')),
    '#theme_wrappers' => array(),
    '#prefix' => '<div class="row"><div class="col-xs-12 col-sm-3"><div class="hpbx-table-selector-title sg-text-intro">'. t('Show'). ':</div></div>
<div class="col-xs-12 col-sm-9 col-md-4"><div class="hpbx-table-selector-inputs">',
    '#suffix' => '</div></div></div>',
  );
      
      
  $form['select']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Change'),
    '#attributes' => array('class' => array('sg-btn', 'sg-btn-secondary')),
    '#prefix' => '<div class="row"><div class="col-xs-12 col-md-10"><div class="hpbx-table-selector-inputs hpbx-table-selector-submit-container">',
    '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span></div></div></div>',
  );

  if (!empty($_SESSION['ccs']['hpbx']['voicemails']['subscriber_id'])) {
    $form['select']['subscriber_id']['#default_value'] = $_SESSION['ccs']['hpbx']['voicemails']['subscriber_id'];
  }
  else {
    $form['select']['subscriber_id']['#default_value'] = $Subscriber->id;
  }
  
  if (!empty($_SESSION['ccs']['hpbx']['voicemails']['folder'])) {
    $form['select']['folder']['#default_value'] = $_SESSION['ccs']['hpbx']['voicemails']['folder'];
  }
  
  
  $totals = hpbx_voicemails_get_totals();
  
  $title_suffix = '<div class="hpbx-page-title-voicemail-counters">
    <div class="hpbx-voicemail-counters">';
  
  if (isset($totals['personal_inbox'])) {
    $variables = array(
      'type' => 'voicemail-counter-personal',
      'svg_id' => 'hpbx-voicemail-counter-personal-svg',
      'sub' => $totals['personal_inbox'],
      'total' => $totals['personal_total'],
      'sub_label' => t('Personal'),
      'class' => array('hpbx-progress-circle-small', 'hpbx-voicemail-counter-personal'),
      'inner' => $totals['personal_inbox'],
    );
    $title_suffix .= theme('hpbx_circle', $variables);
  }
  if (count($options) && isset($totals['huntgroup_inbox'])) {
    $variables = array(
      'type' => 'voicemail-counter-huntgroup',
      'svg_id' => 'hpbx-voicemail-counter-huntgroup-svg',
      'sub' => $totals['huntgroup_inbox'],
      'total' => $totals['huntgroup_total'],
      'sub_label' => t('Huntgroup'),
      'class' => array('hpbx-progress-circle-small', 'hpbx-voicemail-counter-huntgroup'),
      'inner' => $totals['huntgroup_inbox'],
    );
    $title_suffix .= theme('hpbx_circle', $variables);
  }
  
  $title_suffix .= '</div></div>';
  
  $header = array(
    'audio' => array('data' => $header_chk, 'width' => '30px'),
    'number' => array('data'=> t('Number')),
    'time' => array('data'=> t('Time'), 'width' => '120px'),
  );
  
  $form['table_container'] = array(
    '#prefix' => '<div class="hpbx-tab-audio-container">',
    '#suffix' => '</div>',
  );
  
  $form['table_container']['table'] = array(
    '#type' => 'table',
    '#tree' => TRUE,
    '#theme' => 'hpbx_table',
    '#header' => $header,
    '#class' => array(
      'hpbx-table hpbx-table-fullwidth hpbx-table-inside-tabs hpbx-table-with-select-all hpbx-table-voicemail'
    ),
    //'#remove_title' => t('Delete voicemail'),
  );

  $order = tablesort_get_order($header);

  $query = array();
    
  if (!empty($form['select']['subscriber_id']['#default_value'])) {
    $query['subscriber_id'] = $form['select']['subscriber_id']['#default_value'];
  } 
  else {
    $query['subscriber_id'] = $Subscriber->id;
  }
    
  $query['page'] = pager_find_page() + 1;
  $query['rows'] = 50;
  $query['order_by'] = $order['sql'];
  $query['order_by_direction'] = tablesort_get_sort($header);

  if (!empty($_SESSION['ccs']['hpbx']['voicemails']['folder'])) {
    $query['folder'] = $_SESSION['ccs']['hpbx']['voicemails']['folder'];
  }

  $i = 0;
  // Reset options.
  // Query subscribers.
  foreach (sipwise_api_get_all('voicemails', $query) as $Voicemail) {

    // Convert to local reseller timezone.
    hpbx_convert_to_local_datetime($Voicemail->time);

    $i++;
    // For every new date add a new table row.
    $cur_date = date('d-m-Y', strtotime(substr($Voicemail->time, 0, 23)));
    
    if (is_null($prev_date) || (!is_null($prev_date) && $prev_date!=$cur_date)) {

      $u = uniqid();
      
      $form['table_container']['table'][$u] = array('#attributes' => array('class'=> array('hpbx-table-head-row', 'sg-text-small')));
      
      if ($i==1) {
        $form['table_container']['table'][$u]['empty']['data'] = array('#markup' => '<div class="sg-custom-check-and-radio sg-custom-check">
                                                    <label title="">
                                                        <input type="checkbox" name="hpbx-chk-voicemail-all" id="hpbx-chk-voicemail-all" value="1">
                                                        <span class="sg-caption"></span>
                                                        <div class="sg-alternative-display">
                                                            <div class="sg-icon-check"></div>
                                                        </div>
                                                    </label>
                                                </div>');
      }
      else {
        $form['table_container']['table'][$u]['empty']['data'] = array('#markup' => '');
      }
      
      $form['table_container']['table'][$u]['empty']['#attributes']['class'] = array('hpbx-show');
      //$form['table_container']['table'][$u]['time']['data'] = array('#markup' => t(drupal_ucfirst(date('l d-m-Y', strtotime(substr($Voicemail->time, 0, 23))))));
      $form['table_container']['table'][$u]['time']['data'] = array('#markup' => t(drupal_ucfirst(date('l', strtotime(substr($Voicemail->time, 0, 23)))))." ".drupal_ucfirst(date('d-m-Y', strtotime(substr($Voicemail->time, 0, 23)))));
      $form['table_container']['table'][$u]['time']['#attributes']['colspan'] = 2;
      $form['table_container']['table'][$u]['time']['#attributes']['class'] = array('hpbx-show');
    }
    
    $prev_date = $cur_date;
    
    $form['table_container']['table'][$Voicemail->id] = array();
    $form['table_container']['table'][$Voicemail->id]['#attributes'] = array(
      'class' => array('hpbx-table-audio-container', 'hpbx-audio-container'),
      'data-audio-url' => 'voicemail/'. $Voicemail->id . '/play',
      'data-voicemail-id' => $Voicemail->id,
      'data-delete-url' => 'voicemail/'. $Voicemail->id . '/delete',
    );

    if ($Voicemail->folder == 'Old') {
      $form['table_container']['table'][$Voicemail->id]['#attributes']['class'][] = 'hpbx-read';
    }
    $form['table_container']['table'][$Voicemail->id]['audio']['#attributes'] = array(
        'class' => array('hpbx-audio-control-icon', 'hpbx-show')
    );
    $form['table_container']['table'][$Voicemail->id]['audio']['data'] = array(
      '#type' => 'checkbox',
      '#theme_wrappers' => array(),
      '#attributes' => array('id' => 'hpbx-chk-voicemail-'. $Voicemail->id),
    );

    // Caller.
    $form['table_container']['table'][$Voicemail->id]['number']['data'] = array('#markup' => ($Voicemail->caller == '"Anonymous" <anonymous>' ? t('Anonymous') : $Voicemail->caller));
    $form['table_container']['table'][$Voicemail->id]['number']['data']['#markup'] .= hpbx_audio_player('hpbx/voicemail/'. $Voicemail->id . '/play.wav');
    $form['table_container']['table'][$Voicemail->id]['number']['#attributes']['class'] = array('hpbx-show', 'hpbx-table-voicemail-number');
    
    // Start time
    $form['table_container']['table'][$Voicemail->id]['time']['data'] = array('#markup' => '<span class="hpbx-table-voicemail-time-caption">'. date('H:i', strtotime(substr($Voicemail->time, 0, 23))) . '</span><div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" type="button" title="'. t('Delete voicemail') . '"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>');
    $form['table_container']['table'][$Voicemail->id]['time']['#attributes']['class'] = array('hpbx-show', 'hpbx-table-voicemail-time');
  }

  $form['table_container']['actions'] = array(
    '#prefix' => '<div class="hpbx-audio-background-active"></div><div class="hpbx-audio-progress-bar"></div><div class="hpbx-table-button-row">'
  );
  
  if (isset($form['table_container']['table']) && count($form['table_container']['table'])) {

    // Move to Old.
    $form['table_container']['actions']['move-to-old'] = array(
      '#type' => 'submit',
      '#value' => t('Mark selected as heard'),
      '#prefix' => '<div class="hpbx-table-button">',
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span></div>',
      '#attributes' => array('class' => array('sg-btn sg-btn-secondary')),
    );

    $form['table_container']['actions']['move-to-inbox'] = array(
      '#type' => 'submit',
      '#value' => t('Mark selected as unheard'),
      '#prefix' => '<div class="hpbx-table-button">',
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></span></div>',
      '#attributes' => array('class' => array('sg-btn sg-btn-secondary')),
    );
    
    // Permanently delete.
    $form['table_container']['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete selected'),
      '#prefix' => '<div class="hpbx-table-button">',
      '#suffix' => '<span class="sg-btn-icon LGI-iconb-arrow-right-bold"></div>',
      '#attributes' => array('class' => array('sg-btn sg-btn-secondary')),
    );
  }

  //$form['table_container']['actions']['#suffix'] = ;
  global $total_count;

  $pager = '';
  $pagers = array();
  if ($total_count > $query['rows']) {
    $total_pages = ceil($total_count / $query['rows']);

    // Set pager options.
    pager_default_initialize($total_count, $query['rows']);

    // Render pager.
    $pagers = theme('pager', array('element' => 0, 'quantity' => 5, 'parameters' => array(), 'tags' => ''));
  }
  
  if (!empty($pagers)) {
    $form['#suffix'] = '<div class="hpbx-pagination-container">';
    $form['#suffix'] .= $pagers;
    $form['#suffix'] .= '</div>';
  }
  
  
  return $form;
}


function hpbx_voicemails_get_totals($get_subscriber = TRUE, $get_huntgroup = TRUE) {
  global $total_count;
  
  $personal_inbox = $personal_total = $huntgroup_inbox = $huntgroup_total = $huntgroup_total_inbox = 0;
  
  $ret_hg = FALSE;
  
  if ($get_huntgroup) {
    // Huntgroup.
    $subscriber_options = hpbx_get_subscriber_options(
      hpbx_get_active_customer(),
      hpbx_get_active_subscriber(),
      $number_key = FALSE,
      $is_pbx_pilot = FALSE,
      $is_pbx_group = TRUE,
      $autoattendant = FALSE,
      $conference = FALSE,
      $is_subscriber = FALSE,
      $must_be_pbx_group_member = TRUE
    );
  
    $subscriber_options = $subscriber_options[t('Groups')];
    $pbx_group_ids = array_keys($subscriber_options);
  
    
    foreach ($pbx_group_ids as $pbx_group_id) {
      $ret_hg = TRUE;
      
      $query = array();
      $query['subscriber_id'] = $pbx_group_id;
      $query['rows'] = 1;
      $query['folder'] = 'INBOX';
      sipwise_api_get_all('voicemails', $query);
      $huntgroup_inbox = $total_count;
      
      $query['folder'] = 'Old';
      sipwise_api_get_all('voicemails', $query);
      $huntgroup_old = $total_count;
      $huntgroup_total += ($huntgroup_inbox + $huntgroup_old);
      $huntgroup_total_inbox +=  $huntgroup_inbox;
    }
  }
  
  if ($get_subscriber) {
    // Subscriber.
    $query = array();
    $query['subscriber_id'] = hpbx_get_active_subscriber()->id;
    $query['rows'] = 1;
    $query['folder'] = 'INBOX';
    sipwise_api_get_all('voicemails', $query);
    $personal_inbox = $total_count;
  
    $query['folder'] = 'Old';
    sipwise_api_get_all('voicemails', $query);
    $personal_old = $total_count;
    
    $personal_total = $personal_inbox + $personal_old;
  }
  
  if ($ret_hg) {
    return (array('personal_inbox' => $personal_inbox, 'personal_total' => $personal_total, 'huntgroup_inbox' => $huntgroup_total_inbox, 'huntgroup_total' => $huntgroup_total));
  }
  return (array('personal_inbox' => $personal_inbox, 'personal_total' => $personal_total));
}

function hpbx_voicemails_overview_form_submit($form, $form_state) {

  switch ($form_state['values']['op']) {
    
    // Set filters.
    case t('Change'):
      unset(
          $_SESSION['ccs']['hpbx']['voicemails']['subscriber_id'],
          $_SESSION['ccs']['hpbx']['voicemails']['folder']
      );
      
      if (!empty($form_state['values']['subscriber_id'])) {
        $_SESSION['ccs']['hpbx']['voicemails']['subscriber_id'] = $form_state['values']['subscriber_id'];
      }
      if (!empty($form_state['values']['folder'])) {
        $_SESSION['ccs']['hpbx']['voicemails']['folder'] = $form_state['values']['folder'];
      }
      
      break;
    // Move to Old.
    case t('Mark selected as heard'):

      foreach ($form_state['values']['table'] as $voicemail_id => $data) {
        // Load.
        
        if ($data['audio']['data']) {
          $Voicemail = SipwiseEntity::load('voicemails', $voicemail_id);

          // Set folder.
          $Voicemail->folder = 'Old';
          
          // Save.
          $Voicemail->save();
        }
      }
      
      break;
      
    // Move to INBOX.
    case t('Mark selected as unheard'):
      
      foreach ($form_state['values']['table'] as $voicemail_id => $data) {
        if ($data['audio']['data']) {
          // Load.
          $Voicemail = SipwiseEntity::load('voicemails', $voicemail_id);
  
          // Set folder.
          $Voicemail->folder = 'INBOX';
  
          // Save.
          $Voicemail->save();
        }
      }
      
      break;
    // Delete selected.
    case t('Delete selected'):

      foreach (array('Old', 'INBOX') as $type) {
        foreach ($form_state['values']['table'] as $voicemail_id => $data) {
          if ($data['audio']['data']) {
            // Load.
            $Voicemail = SipwiseEntity::load('voicemails', $voicemail_id);
        
            // Save.
            $Voicemail->delete();
          }
        }  
      }
      break;
  }
}