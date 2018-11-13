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
              <?php if (substr($_GET['q'], 0, 16) == 'hpbx/help/search'):?>
                  <a class="hpbx-help-header-link" href="<?php print url('hpbx/help/home')?>">
                    <span class="hpbx-help-header-link-icon LGI-iconarrow-left"></span><?php print t('Back to all help articles'); ?>
                  </a>
                <?php else:?>
                  <div class="hpbx-help-header-title">
                    <span class="hpbx-help-header-title-icon LGI-iconb-help-large"></span> <?php print t('Help'); ?>
                  </div>
                <?php endif;?>
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
  <?php print render($page['content']) ?>
</div>
