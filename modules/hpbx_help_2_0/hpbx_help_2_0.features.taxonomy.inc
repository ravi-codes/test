<?php
/**
 * @file
 * hpbx_help_2_0.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function hpbx_help_2_0_taxonomy_default_vocabularies() {
  return array(
    'hpbx_help_type' => array(
      'name' => 'HPBX Help type',
      'machine_name' => 'hpbx_help_type',
      'description' => 'Defines help article type',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
  );
}
