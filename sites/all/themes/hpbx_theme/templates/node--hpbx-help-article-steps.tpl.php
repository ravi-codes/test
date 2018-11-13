<?php if (!$page): ?>
  <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
<?php endif; ?>

<div class="sg-text-intro">

<?php print render($content['hpbx_body']);?>

<?php if (!empty($content['field_step'])):?>
  <div class="hpbx-help-steps-container">
    <?php print render($content['field_step']);?>                    
  </div>
<?php endif ?>

<?php if (!empty($content['links']['terms'])): ?>
  <div class="terms"><?php print render($content['links']['terms']); ?></div>
<?php endif;?>

<?php if (!empty($content['links'])): ?>
  <div class="links"><?php print render($content['links']); ?></div>
<?php endif; ?>
        