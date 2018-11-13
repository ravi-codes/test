<div class="hpbx-wizard-background"></div>
<div class="hpbx-wizard-cartoon"></div>
<a class="hpbx-btn-wizard-exit" href="<?php echo url('hpbx/dashboard');?>"><div class="hpbx-btn-wizard-exit-icon"></div><?php print t('Exit'); ?></a>
<div class="hpbx-header">
    <div class="hpbx-header-logo"></div>
</div>

<?php 
global $user;
$account = $user;
if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
  $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
}

?>
<?php if ($logged_in && user_access('edit any hpbx_help_article content', $account)): ?> 
<div class="hpbx-help-edit-url-popup-trigger">
  <span class="LGI-iconedit"></span>
</div>
<?php endif; ?>
<?php if ($logged_in):?>
<?php $help_url = hpbx_help_get_help_url(); 
  if (empty($help_url)) {
    $help_url = url('hpbx/help/home');
  }
?>
<?php if ($logged_in && user_access('edit any hpbx_help_article content', $account)): ?>
<div class="hpbx-help-edit-url-popup-trigger">
    <span class="LGI-iconedit"></span>
</div>
<?php endif; ?>
<div class="hpbx-help-open-handle hpbx-help-open-handle-content-section" id="open-button" data-help-url="<?php echo $help_url; ?>">
    <span class="LGI-iconb-help-large"></span>
</div>
<div class="hpbx-help">
    <iframe class="hpbx-help-iframe" src="<?php echo url('hpbx/help/loader.html'); ?>" width="100%" height="100%" data-default-url="<?php echo $help_url; ?>"></iframe>
</div>
<div class="hpbx-help-loading-overlay">
    <div class="hpbx-help-loading-wait-container">
        <div class="hpbx-loader-animation-container">
            <div class="hpbx-loader-animation">
            </div>
        </div>
    </div>
    <div class="hpbx-help-loading-text hpbx-loader-title"><?php print t('Help is loading...'); ?></div>
</div>
<?php endif; ?>
<div class="container hpbx-no-menu">
  <div class="row">
    <div class="col-xs-12 hpbx-main-content">
      <div class="row">
        <div class="col-xs-12 col-sm-6"></div>
          <div class="col-xs-12 col-sm-6 col-md-5">
            <div class="hpbx-content-container hpbx-hidden">
              <?php print render($page['content']) ?>
            </div>
          </div>
        </div>
    </div>
  </div>
</div>
<div class="hpbx-loader">
    <div class="hpbx-loader-box">
        <div class="hpbx-loader-animation">
        </div>
        <div class="hpbx-loader-progress">
            <div class="hpbx-loader-progress-line"></div>
            <div class="hpbx-loader-progress-dot"></div>
        </div>
        <div class="hpbx-loader-content">
            <div class="hpbx-loader-title"></div>
            <div class="hpbx-loader-subtitle"></div>
        </div>
    </div>
</div>
  <?php if ($logged_in): ?>
  <?php   print theme_hpbx_help_popups($variables); ?>
  <?php endif; ?>
  