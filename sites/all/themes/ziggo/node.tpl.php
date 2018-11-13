<?php
// $Id$

/**
 * @file node.tpl.php
 * Theme implementation to display a single node.
 */
?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <?php if ($display_submitted || $user_picture): ?>
    <span class="submitted">
      <?php print $user_picture; ?>
      <?php print $submitted; ?>
    </span>
  <?php endif; ?>

  <div class="content"<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      $links = hide($content['links']);
      $links = render($links);
      print render($content);
    ?>
  </div>

  <?php if (!empty($links)): ?>
    <div class="links">
      <?php print render($content['links']); ?>
    </div>
  <?php endif; ?>

  <?php print render($content['comments']); ?>

</div>
