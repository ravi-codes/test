<?php
global $user;
$account = $user;

if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
  $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
}
?>

<div class="container hpbx-no-menu">
  <div class="row hpbx-help-content">
    <div class="col-xs-12">
      <?php if(user_access('edit any hpbx_help_article content', $account)): ?>
      <div class="row">
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
        <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
          <div class="hpbx-help-button-container">
            <div class="row">
              <div class="col-xs-6 col-xxs-12">
                <div class="hpbx-help-button-container-button">
                  <a href="<?php print url('node/add/hpbx-help-article');?>" class="sg-btn sg-without-icon sg-btn-secondary sg-element"><?php print t('Add help article');?></a>
                </div>
              </div>
              <div class="col-xs-6 col-xxs-12">
                <div class="hpbx-help-button-container-button hpbx-last">
                  <a href="<?php print url('hpbx/help/sitemap');?>" class="sg-btn sg-without-icon sg-btn-secondary sg-element"><?php print t('Go to sitemap');?></a>
                </div>
              </div>
            </div>
          </div>
        </div>
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      </div>
      <?php endif;?>
    </div>
    <div class="row">
        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
        <div class="<?php print $classes; ?>">
          <?php if ($rows): ?>
            <div class="view-content">
              <?php print $rows; ?>
            </div>
          <?php endif; ?>

        </div>

        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      </div>
    </div>
  </div>
</div>



