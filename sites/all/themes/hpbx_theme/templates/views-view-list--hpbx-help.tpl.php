<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */

global $hpbx_help_list_counter, $hpbx_help_list_terms;

$hpbx_help_list_counter++;

// Calculate total per category (only once needed).
if ($hpbx_help_list_counter == 1) {
  $hpbx_help_list_terms = array();
  foreach ($view->result as $row) {
    $hpbx_help_list_terms[$row->taxonomy_term_data_node_tid] = $row->taxonomy_term_data_node_name;
  }
}

?>

<?php if ($hpbx_help_list_counter == 1): ?>
  <div class="col-xs-5 col-xxs-12 col-sm-4 col-md-3">
<?php elseif ($hpbx_help_list_counter == (ceil(count($hpbx_help_list_terms) / 2) + 1)): ?>
  </div><div class="col-xs-5 col-xxs-12 col-sm-4 col-md-3">
<?php endif?>

<div class="hpbx-help-category-container">
  <div class="hpbx-help-category-image <?php print $options['class'];?>"></div>
  <div class="hpbx-help-category-content">
  <?php if (!empty($title)) : ?>
    <div class="hpbx-help-category-title"><?php print t($title); ?></div>
  <?php endif; ?>
  <?php foreach ($rows as $id => $row): ?>
    <?php print $row; ?>
  <?php endforeach; ?>
  </div>
</div>

<?php if ($hpbx_help_list_counter == count($view->result)): ?>
  </div>
<?php endif ?>
