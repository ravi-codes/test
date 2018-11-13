<form method="POST" >
  <?php
  print drupal_render($search_form['form_token']);
  print drupal_render($search_form['form_build_id']);
  print drupal_render($search_form['form_id']);
  drupal_render($search_form['query']);
  drupal_render($search_form['submit']);
  ?>
  <div class="hpbx-help-header">
    <div class="container hpbx-no-menu">
      <div class="row">
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
        <div class="col-xs-5 col-xxs-10 col-sm-4 col-md-3">
          <a class="hpbx-help-header-link" href="<?php print url('hpbx/help/home')?>">
            <span class="hpbx-help-header-link-icon LGI-iconarrow-left"></span><?php print t('Back to all help articles'); ?>
          </a>
        </div>
        <div class="col-xs-5 col-xxs-2 col-sm-4 col-md-3 hpbx-header-search-input-container">
          <div class="hpbx-search-input-container">
            <input type="text" id="edit-query" name="query"  class="sg-element needsclick hpbx-search-input" placeholder="<?php print t('Search') ?>">
            <span class="hpbx-search-input-icon LGI-iconsearch"></span>
          </div>
        </div>
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      </div>
    </div>
    <div class="hpbx-help-close-handle needsclick"><span class="LGI-iconcross"></span></div>
  </div>
</form>
<div class="hpbx-scrollable-container hpbx-help-scrollable-content-container">
  <div class="container hpbx-no-menu">
    <div class="row hpbx-help-content">
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
        <?php if ($node):?>
          <?php global $language, $user;
          $account = $user;
          if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
            $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
          }
          ?>
          <?php if (node_access('update', $node, $account) && $page['content']['system_main']['#form_id'] != 'node_delete_confirm'): ?>
            <a class="sg-btn sg-without-icon sg-btn-primary hpbx-hidden-xxs" href="<?php print base_path(); ?><?php print $language->language;?>/node/<?php print $node->nid; ?>/edit"><?php print t('Edit'); ?></a>
          <?php endif ?>
          <?php if (user_access('edit any hpbx_help_article content', $account) && $page['content']['system_main']['#form_id'] != 'node_delete_confirm'):?>
            <a class="sg-btn sg-without-icon sg-btn-primary hpbx-hidden-xxs" href="<?php print base_path(); ?><?php print $language->language;?>/hpbx/help/node/<?php print $node->nid; ?>/replicate"><?php print t('Replicate'); ?></a>
          <?php endif; ?>
          <?php print render($page['content']) ?>
        <?php endif?>
      </div> <!-- /col-sm-8 -->
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
    </div> <!-- /node-->
  </div>
  <?php if (!isset($node->field_hpbx_type['und'][0]['tid']) || taxonomy_term_load($node->field_hpbx_type['und'][0]['tid'])->name != 'F.A.Q.' && $page['content']['system_main']['#form_id'] != 'node_delete_confirm'): ?>
    <div class="hpbx-help-footer">
      <div class="row hpbx-help-footer">
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
        <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
          <div class="hpbx-help-horizontal-line"></div>
        </div>
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      </div>
    </div>
  <?php endif?>
</div>
