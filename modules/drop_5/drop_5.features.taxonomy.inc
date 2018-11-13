<?php
/**
 * @file
 * drop_5.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function drop_5_taxonomy_default_vocabularies() {
  return array(
    'hpbx_moh_category' => array(
      'name' => 'HPBX MoH audio category',
      'machine_name' => 'hpbx_moh_category',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
  );
}
