<div id="block-<?php print $block->module .'-'. $block->delta ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
    
      
      <?php print render($title_prefix); ?>
    <?php if ($block->subject): ?>
    	<div class="block-box-header block-box-header-title">
      <h2 class="block-title"<?php print $title_attributes; ?>><?php print $block->subject ?></h2>
      <div class="right"> </div>
      </div>
    <?php endif;?>
     <?php print render($title_suffix); ?>
    <div class="block-inner">
		<div class="content" <?php print $content_attributes; ?>>
		  <?php print $content; ?>
		</div>
    </div>
    <div class="block-bottom">
      <div class="right">
        <!---->
      </div>
    </div>
  </div>