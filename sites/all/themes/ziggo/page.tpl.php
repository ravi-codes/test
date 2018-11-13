<?php
// $Id$

/**
 * @file page.tpl.php
 * Theme implementation to display a single Drupal page.
 */
?>

<div id="container" class="wrapper">

  <?php if (!empty($page['topbar'])): ?>
  <div id="topbar" class="clearfix">
    <div class="topbar section">
      <?php print render($page['topbar']); ?>
    </div>
  </div><!-- /#header -->
  <?php endif; ?>

  <div id="header" class="clearfix">
    <div class="header-inner section">
      <a href="/" target="" class="logo"></a>
      <?php print render($page['header']); ?>
    </div>
  </div><!-- /#header -->

  <div id="nav-bar">
    <?php print render($page['nav_bar']); ?>
  </div>

  <div id="columns" class="clearfix section">
    <div id="content" class="column">
      <?php print render($page['sidebar_first']); ?>
      <main class="content-inner"<?php print $content_attributes; ?>>
        <?php if (!empty($messages)): print $messages; endif; ?>
        <?php if (!empty($breadcrumb)): print $breadcrumb; endif; ?>
        <?php if ($action_links): ?>
          <ul class="action-links">
            <?php print render($action_links); ?>
          </ul>
        <?php endif; ?>
        <?php print render($page['highlight']); ?>
        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if (!empty($title)): ?><h1 class="title"<?php print $title_attributes; ?>><?php print $title; ?></h1><?php endif; ?>
        <?php print render($title_suffix); ?>
        <?php print render($page['help']); ?>
        <?php print render($page['content_top']); ?>
        <?php print render($tabs); ?>
        <?php print render($page['content']); ?>
        <?php print render($page['content_bottom']); ?>
      </main>
      <?php print render($page['sidebar_second']); ?>
    </div><!-- /#content -->

  </div><!-- /#columns -->

  <div class="push"></div>
</div><!-- /#container -->
<?php print render($page['footer']); ?>

