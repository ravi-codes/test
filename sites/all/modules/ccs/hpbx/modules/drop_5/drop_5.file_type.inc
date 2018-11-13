<?php
/**
 * @file
 * drop_5.file_type.inc
 */

/**
 * Implements hook_file_default_types().
 */
function drop_5_file_default_types() {
  $export = array();

  $file_type = new stdClass();
  $file_type->disabled = FALSE; /* Edit this to true to make a default file_type disabled initially */
  $file_type->api_version = 1;
  $file_type->type = 'hpbx_music_on_hold_audio_file';
  $file_type->label = 'hpbx music-on-hold audio file';
  $file_type->description = '';
  $file_type->mimetypes = array(
    0 => 'audio/x-wav',
    1 => 'audio/ogg',
    2 => 'audio/mpeg',
  );
  $export['hpbx_music_on_hold_audio_file'] = $file_type;

  return $export;
}
