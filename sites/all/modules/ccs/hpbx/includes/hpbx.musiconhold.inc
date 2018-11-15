<?php

/**
 *
 * @param array $form
 * @param array $form_state
 * @param string $customer_id
 *
 * @return multitype:string Ambigous <The, string, unknown, A, Returns, Optional>
 */
function hpbx_musiconhold_edit_form($form, $form_state) {

  global $user, $ActiveReseller;

  // Load the customer.
  $Customer = hpbx_get_active_customer();

  $PilotSubscriber = $Customer->get_pilot_subscriber();

  drupal_set_title(t('Music on hold'));

  $form = array(
    '#prefix' => '<div class="hpbx-main-content-inner hpbx-music-on-hold">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  $form['general']['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#title' => t('Settings'),
  );
  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Customer->id) ? $Customer->id : FALSE,
  );
  // Set the existing id.
  $form['subscriber_id'] = array(
    '#type' => 'value',
    '#value' => !empty($PilotSubscriber->id) ? $PilotSubscriber->id : FALSE,
  );

  $SoundFile = NULL;
  $sf  = NULL;
  // Get the music on hold sound file from the pilot subscriber preferences.
  if (isset($PilotSubscriber->id) && $PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id)) {

    if ($SoundSet =  hpbx_get_soundset_on_name('Music On Hold SoundSet '. $Customer->id, $Customer->id)) {
      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
        if ($SoundFile->handle == 'music_on_hold') {
          $sf = $SoundFile;
          break;
        }
      }
    }
  }

  $handle = 'music_on_hold';
  $title = 'Music on Hold';
  
  $markup = '
<div class="hpbx-tab-form-label-field-row hpbx-upload-audio-container">
    <div class="row">';
  
  if (!empty($sf->filename)) {
    $markup .= '
<<<<<<< HEAD
                <div class="col-xs-12 hpbx-tab-audio-container hpbx-audio-container hpbx-audio-container-singular" data-audio-url="/hpbx/audiorecording/'. $SoundFile->id .'/play" data-audio-id="'.$SoundFile->id.'">
=======
                <div class="col-xs-12 hpbx-tab-audio-container hpbx-audio-container hpbx-audio-container-singular" data-audio-url="/hpbx/audiorecording/'. $sf->id .'/play" data-audio-id="'.$SoundFile->id.'">
>>>>>>> branch1
                  <div class="hpbx-audio-background-active"></div>
                  <div class="hpbx-audio-progress-bar"></div>
                  <div class="hpbx-audio-content">
                    <div class="hpbx-audio-fallback">';
                      
                      $markup .= hpbx_audio_player('hpbx/audiorecording/'. $sf->id .'/play.wav');
                          
         $markup .= '  </div>    
                    <div class="hpbx-audio-control-icon"></div>
                    <div class="hpbx-audio-title">'. $sf->filename .'</div>
                    <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" title="'. t('Delete upload'). '"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
                 </div>
                  <input type="checkbox" class="hpbx-audio-is-replaced" name="hpbx-audio-is-replaced-music-on-hold" value="1" />';
  }
  
  $markup .= '</div>
            </div>
           </div>';
  
  
  // Show audio player for handle.
  $form['general']['table'][$handle] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );


  $form['general']['table'][$handle. '_upload']['value'] = array(
    '#title' => t('Upload Music on Hold audio'),
    '#type' => 'file',
    '#label_row_attributes' => array('class' => array('hpbx-tab-form-label-field-row', 'hpbx-audio-selector-container'), 'data-connected-id' => 'music-on-hold'),
    '#attributes' => array(
      'class' => array('hpbx-click-with-alert-input'),
    ),
    '#name' => $handle,
    '#theme_wrappers' => array('hpbx_audio_upload', 'form_element'),
    '#field_size_class' => array('col-xs-12', 'col-md-8'),
    '#wrapper_attributes' => array(
      'class' => array(
        'sg-btn',
        'sg-btn-file-input',
        'sg-btn-secondary',
        'hpbx-click-with-alert',
        'needsclick',
      ),
      'data-alert-title' => t('File requirements'),
      'data-alert-message' => t('If you upload an audio file, make sure you pay for or own the content rights for this audio file.</br></br>The maximum allowed duration of the audio file is 60 minutes.</br></br>Supported file types: wav, mp3, flac, ogg, raw'),
      'data-alert-button-text' => t('OK'),
    ),
    '#field_suffix' => array('#type' => 'markup', '#markup' => '
  </div></div><div class="hidden-xs col-sm-1 col-md-1">
    <div class="hpbx-tab-form-field">
      <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" title="'. t('Clear upload') . '">
        <span class="sg-btn-icon LGI-iconb-cross-bold"></span>
        </div>
    </div>
  </div><div><div>')
  );

  $t1 = t('Or choose a song from our default !icon !ml_link', array(
    '!icon' => '<span class="LGI-iconmedia-library hpbx-audio-selector-library-icon"></span>',
    '!ml_link' => l(t('music library'), '',
      array(
        'attributes' => array(
          'class' => 
            array('hpbx-audio-selector-library-link', 'hpbx-dialog-open-trigger'),
          'data-dialog-id' => '#hpbx-music-on-hold-dialog',
          'data-dialog-close' => 'hpbxMusicLibraryCloseHandler'
        )
      ))
  ));



  $form['general']['table']['media'] = array(
      '#type' => 'markup',
      '#markup' => <<<EOF
<div class="row">
    <div class="hpbx-tab-form-label-field-row hpbx-audio-selector-container" data-connected-id="music-on-hold">
        <div class="col-xs-12 col-md-11">
            <div class="hpbx-tab-form-label">{$t1}
            </div>
            <div class="hpbx-tab-form-label hpbx-selected"></div>
        </div>
        <div class="col-xs-2 col-sm-1 col-md-1">
            <div class="hpbx-tab-form-field">
                <div class="hpbx-delete js-delete sg-btn sg-btn-small sg-btn-secondary" type="button" title="Clear upload"><span class="sg-btn-icon LGI-iconb-cross-bold"></span></div>
            </div>
        </div>
    </div>
</div>
EOF
  );
  
  
  // Submit button.
  $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>',
      
  );
  $form['general']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<div class="col-xs-12"><span class="submit-button">',
    '#suffix' => '</span></div>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
  $form['general']['#suffix'] = hpbx_musiconhold_library_dialog();
  return $form;
}

function hpbx_musiconhold_library_dialog() {
  
  $result = db_select('file_managed', 'f')
  ->fields('f', array('fid', 'filename'))
  ->condition('type', 'hpbx_music_on_hold_audio_file')
  ->condition('filemime', 'audio/x-wav')
  ->condition('status', 1)
  ->execute();

  $voc = taxonomy_vocabulary_machine_name_load('hpbx_moh_category');
  $tree = taxonomy_get_tree($voc->vid);
  $categories = array();
  foreach ($tree as $term) {
    $categories[$term->tid] = $term->name;
  }
  
  $files = array();
  foreach ($result as $row) {
  
    if ($file = file_load($row->fid)) {
      if (!empty($file->field_hpbx_moh_audio_category['und'][0]['tid'])) {
        $files[$categories[$file->field_hpbx_moh_audio_category['und'][0]['tid']]][$file->fid] = $file;
      }
    }
  }
  
  $rows = array();
  foreach ($files as $category => $audio_files) {
    
    $row = array();
    $row[] = array(
      'data' => '',
      'class' => array('hpbx-show')
    );
    $row[] = array(
      'data' => t($category),
      'class' => array('hpbx-show'),
      'colspan' => 2
    );
    $rows[] = array(
      'data' => $row,
      'class' => array('hpbx-table-head-row', 'sg-text-small')
    );

    foreach ($audio_files as $audio_file) {
      
      if ($duration = hpbx_audio_get_wav_duration(drupal_realpath($audio_file->uri))) {
        $duration = gmdate("i:s", $duration); 
      }
      $row = array();
      
      $attributes = array(
        'type' => 'radio',
        'name' => 'hpbx-rdo-music-on-hold',
        'id' => 'hpbx-rdo-music-on-hold-'. $audio_file->fid,
        'value' => $audio_file->fid
      );
      $row[] = array(
        'data' => ' <div class="sg-custom-check-and-radio custom-radio">
                      <label title="">
                        <input '. drupal_attributes($attributes) .'/>
                        <span class="sg-caption"></span>
                        <div class="sg-alternative-display">
                            <div class="sg-outer"><div class="sg-inner"></div></div>
                        </div>
                      </label>
                    </div>',
        'class' => array('hpbx-audio-control-icon', 'hpbx-show')
      );
      $uri = preg_replace('/\.(wav|ogg|mp3)$/', '', $audio_file->uri);
      $row[] = array(
        'data' => '<div class="hpbx-table-music-title">'. $audio_file->filename . '</div>'. 
        hpbx_audio_player($audio_file->uri), 
        'class' => array('hpbx-show', 'hpbx-table-music-title-container')
      );
      $row[] = array(
        'data' => '<span class="hpbx-table-music-time-caption">'. ($duration ? $duration : '0:00') . '</span>',
        'class' => array('hpbx-show', 'hpbx-show hpbx-table-music-time')
      );
      
      $rows[] = array(
        'data' => $row,
        'class' => array('hpbx-table-audio-container', 'hpbx-audio-container'),
        'data-audio-url' => file_create_url($uri),
        'data-audio-id' => $audio_file->fid,
      );
    }
  }
  
  $table = theme('table', array(
    'rows' => $rows,
    'attributes' => array(
      'class'=> array('hpbx-table', 'hpbx-table-fullwidth', 'hpbx-table-inside-tabs')
    ),
    'colgroups' => array(
      array(array('class' => array('hpbx-col-checkbox'), 'width' => '30px')),
      array(array('class' => array('hpbx-col-number'))),
      array(array('class' => array('hpbx-col-time'), 'width' => '86px')),
    )
  ));
  
  $t1 = t('Choose your music on hold media');
  $t2 = t('Select');
  $t3 = t('Cancel');

$markup = <<<EOF

<div id="hpbx-music-on-hold-dialog" class="hpbx-dialog hpbx-music-on-hold-dialog hpbx-popup mfp-hide" data-result-selector='[name="hpbx-rdo-music-on-hold"]:checked' data-ok-event="hpbx-dialog-ok-music-on-hold">
            <div class="hpbx-dialog-header">
                <div class="hpbx-dialog-header-icon"><span class="LGI-iconb-music-on-hold-large"></span></div>
                <div class="hpbx-dialog-header-title">{$t1}</div>
            </div>
            <div class="hpbx-dialog-contents hpbx-scrollable-container">
                <div class="hpbx-tab-audio-container">
                    {$table}
                    <div class="hpbx-audio-background-active"></div>
                    <div class="hpbx-audio-progress-bar"></div>
                </div>
                <div class="clearfix"></div>
            </div>
            <div class="hpbx-dialog-footer">
                <div class="hpbx-dialog-footer-left">
                    <input type="button" class="sg-btn sg-btn-small sg-without-icon sg-btn-primary hpbx-dialog-ok" value="{$t2}">
                </div>
                <div class="hpbx-dialog-footer-right">
                    <a class="hpbx-tab-form-link-button hpbx-dialog-close-handle" href="#">{$t3}</a>
                </div>
            </div>
        </div>
EOF;

return $markup;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @param string $customer_id
 */
function hpbx_musiconhold_edit_form_validate($form, &$form_state) {

  ini_set('memory_limit','2048M');

  // Add table styling.

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx.js');

  if (isset($_FILES)) {
    foreach ($_FILES as $handle => $sf) {
      if (!empty($sf['name'])) {
        if (empty($sf['tmp_name']) || !file_exists($sf['tmp_name']) ||  $sf['error']) {
          form_set_error('', t('Upload of Music on Hold failed'));
        }

        //hpbx_audio_get_valid_file_extensions()
        $ext = pathinfo($sf['name'], PATHINFO_EXTENSION);

        if (!in_array(strtolower($ext), hpbx_audio_get_valid_file_extensions())) {
          form_set_error('table]['. $handle . '][value', t('The extension \'@ext\' is not supported', array('@ext' => $ext)));
          return;
        }
        
        // Validate wav duration.
        if ($wav = hpbx_audio_convert_to_wav($sf['tmp_name'], $sf['name'])) {

          if ((strlen($wav)/1024/1024) > 60) {
            form_set_error('', t('The audio file you tried to upload is too big'));
          }
        }
        else {
          form_set_error('', t('Upload of Music on Hold failed'));
        }
      }
    }
  }

  // Verify if the user has access to the predefined music on hold file.
  if (!empty($_POST['hpbx-rdo-music-on-hold'])) {

    if (!$file = file_load((int)$_POST['hpbx-rdo-music-on-hold'])) {
      form_set_error('', t('File not found'));
    }
    if ($file->type != 'hpbx_music_on_hold_audio_file' || !user_access('hpbx edit subscriber')) {
      form_set_error('', t('Access denied'));
    }
  }
}


/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @param string $customer_id
 */
function hpbx_musiconhold_edit_form_submit($form, $form_state) {

  global $ActiveReseller;
  global $user;

  try {

    // Load existing customer.
    $Customer = SipwiseEntity::load('customers', $form_state['values']['id']);
    
      // Load existing pilot subscriber.
    $PilotSubscriber = SipwiseEntity::load('subscribers', $form_state['values']['subscriber_id']);

    // Load pilot subscriber preferences.
    $PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id);

    // Get existing or create new SoundSet.
    $SoundSet = hpbx_soundset_get_subscriber_soundset($PilotSubscriber, 'Music On Hold SoundSet '. $Customer->id);

    $PilotSubscriberPreferences->contract_sound_set = 'Music On Hold SoundSet '. $Customer->id;
    $PilotSubscriberPreferences->save();
    
    
    // Check if the original music_on_hold file was deleted, we need to delete
    // it (and maybe upload a new file afterwords.
    if (isset($form_state['input']['hpbx-audio-is-replaced-music-on-hold']) &&
      $SoundSet =  hpbx_get_soundset_on_name($PilotSubscriberPreferences->contract_sound_set, $Customer->id)) {
  
      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
        if ($SoundFile->handle == 'music_on_hold') {
          $SoundFile->delete();
        }
      }
    }
    
    if (isset($_FILES['music_on_hold']) || !empty($_POST['hpbx-rdo-music-on-hold'])) {

      $soundfiles = array();
      $soundfiles['music_on_hold'] = NULL;

      // Update the soundfiles for the soundset.
      foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {

        if ($SoundFile->handle == 'music_on_hold') {
          $soundfiles['music_on_hold'] = $SoundFile;
        }
      }

      $handle = 'music_on_hold';
      
      // Is it already set.
      if (is_null($soundfiles[$handle])) {

        // Create new soundfile.
        $soundfiles[$handle] = new SipwiseSoundFile();
        $soundfiles[$handle]->handle = $handle;
        $soundfiles[$handle]->set_id = $SoundSet->id;
      }
      
      if (!empty($_POST['hpbx-rdo-music-on-hold']) && $file = file_load((int)$_POST['hpbx-rdo-music-on-hold'])) {

        if ($wav = file_get_contents($file->uri)) {
          // Update existing.
          $soundfiles[$handle]->filename = $file->filename;
          $soundfiles[$handle]->recording = $wav;
          $soundfiles[$handle]->loopplay = 'true'; // This is correct as the API demands a string for this value.
          $soundfiles[$handle]->save();
        }
        
      }
      elseif (isset($_FILES[$handle])) {
          
        $sf = $_FILES[$handle];
        
        if (!empty($sf['name'])) {

          // Update existing.
          $soundfiles[$handle]->filename = $sf['name'];

          //global $wav;
          //if (!empty($wav)) {
          if ($wav = hpbx_audio_convert_to_wav($sf['tmp_name'], $sf['name'])) {

            // Convert to wav (8000, 1 channel) and assign to SoundFile.
            $soundfiles[$handle]->recording = $wav;
            $soundfiles[$handle]->loopplay = 'true'; // This is correct as the API demands a string for this value.
            $soundfiles[$handle]->save();
          }

          //dpm('Did save SoundFile ('.$soundfiles[$handle]->id.') with handle '. $handle);
        }
      }
    }
    
    // Retrieve fresh list of soundfiles.
    $soundfiles = hpbx_get_soundfiles_from_soundset($Customer, $PilotSubscriberPreferences->contract_sound_set, 'music_on_hold');

    // Set music for all subscribers with the advanced profile.
    foreach ($Customer->get_subscribers(FALSE, TRUE) as $Subscriber) {

      if ($SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id)) {

        if ($Subscriber->is_pbx_group) {
          $soundset_name = 'Huntgroup Subscriber '. $Subscriber->id;
        }
        else {
          $soundset_name = 'Subscriber '. $Subscriber->id;
        }

        // Get existing or create new SoundSet.
        $SoundSet = hpbx_soundset_get_subscriber_soundset($Subscriber, $soundset_name);


        $SubscriberPreferences->contract_sound_set = $SoundSet->name;

        if (isset($soundfiles['music_on_hold']) && hpbx_attribute_allowed('music_on_hold', $Subscriber->profile_id, $Subscriber->profile_set_id) === TRUE) {
          $SubscriberPreferences->music_on_hold = TRUE;
          
          // Copy music on hold soundfile to subscriber soundset with handle music_on_hold.
          hpbx_soundfile_copy($soundfiles['music_on_hold'], $SoundSet);
        }
        else {

          // Delete sound file.
          foreach (sipwise_api_get_all('soundfiles', array('set_id' => $SoundSet->id)) as $SoundFile) {
            if ($SoundFile->handle == 'music_on_hold') {
              $SoundFile->delete();
            }
          }
          
          // Disable music on hold.
          $SubscriberPreferences->music_on_hold = FALSE;
        }
        
        // If queue is active for this subscriber, set the queue_waiting_music.
        if (hpbx_attribute_allowed('cloud_pbx_callqueue', $Subscriber->profile_id) && isset($soundfiles['music_on_hold'])) {

          // Copy music on hold soundfile to subscriber soundset with handle music_on_hold.
          hpbx_soundfile_copy($soundfiles['music_on_hold'], $SoundSet, 'queue_waiting_music');
        }
        
        $SubscriberPreferences->save();
      }
    }
  }
  catch (Exception $e) {

    // Failed to create customer.
    drupal_set_message(t('Failed to store music on hold'), 'error');
    drupal_set_message(t('Exception:'. $e->getMessage() .' in '. $e->getFile() .' at line '. $e->getLine()), 'error');
    $failed = TRUE;
  }

  if (!isset($failed)) {
    drupal_set_message(t('Music on hold successfully saved'));
  }
}
