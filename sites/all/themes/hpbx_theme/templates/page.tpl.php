<?php if ($logged_in && $page['header']): ?>
  <?php print render($page['header']); ?>
<?php endif;?>
<div class="hpbx-body-background-color"></div>
<div class="hpbx-body-background-image"></div>
<div class="hpbx-header-background-image"></div>
<?php if ($logged_in):?> 
<div class="hpbx-body-menu-opened-overlay"></div>
<?php endif;?>
<div class="hpbx-header">
    <?php global $hg_current_rev;?> 
    <?php if (!is_null($hg_current_rev)):?>
    <div style="margin-left: 1rem;"><?php print $hg_current_rev;?></div>
    <?php endif;?>
    <div class="hpbx-header-logo"></div>
    <?php if ($logged_in && $page['sidebar_right']): ?>
      <div class="hpbx-menu-handle hpbx-menu-handle-open"></div>
    <?php endif;?>
</div>
<?php
if ($logged_in && $page['sidebar_right']): ?>
<div id="sidebar-right" class="column sidebar right">
  <div class="hpbx-menu-dummy-handle">
    <div class="hpbx-menu-handle hpbx-menu-handle-open"></div>
  </div>
  <div id="sidebar-right-inner" class="inner">
    <?php print render($page['sidebar_right']); ?>
  </div>
</div>
<?php endif; ?> <!-- /sidebar-right -->
<?php $icon = arg(1); $icon_sub = arg(3);?>
<?php 
global $user;
$account = $user;
if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
  $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
}

?>
<?php if ($logged_in && user_access('edit any hpbx_help_article content', $account) && $_GET['q']!='hpbx/password'): ?>
<div class="hpbx-help-edit-url-popup-trigger">
  <span class="LGI-iconedit"></span>
</div>
<?php endif; ?>
<?php if ($logged_in && user_access('hpbx basic', $account) && ($_GET['q']!='hpbx/password')):?>

<?php $help_url = hpbx_help_get_help_url();
if (empty($help_url)) {
  $help_url = url('hpbx/help/home');
}
?>
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
<div class="container <?php if (!$logged_in || ((arg(0) == 'hpbx' || arg(0) == 'ivu') && arg(1)=='password')) : print 'hpbx-no-menu'; endif;?>">
  <div class="row">
    <div class="col-xs-12 hpbx-main-content <?php if (arg(1) == 'fax') print 'hpbx-page-fax'?>">
      <?php if ($title): ?>
        <div class="hpbx-page-title hpbx-<?php print preg_replace('/s$/', '', strtolower($icon)) ?>-page-title">
          <?php if (arg(1) == 'voicemails'):?>
            <div class="hpbx-page-title-voicemail-title">
          <?php endif;?>
          
            <span class="sg-icon <?php 
            if ($icon == 'settings') {
              $icon = 'LGI-iconb-subscriber-large';
            }
            elseif (arg(0) == 'user' && arg(1) == 'reset') {
              $icon = 'LGI-iconreset-password';
            }
            elseif ($icon == 'affiliate' && $icon_sub = 'customer') {
              $icon = 'LGI-iconb-'. preg_replace('/s$/', '', strtolower($icon_sub)) . '-large';
            }
            elseif ($icon == 'affiliates') {
              $icon = 'LGI-iconb-customer-large';
            }
            elseif ($icon == 'pbxdevices' || $icon == 'pbxdevice') {
              $icon = 'LGI-iconb-pbx-device-large';
            }
            elseif ($icon == 'fax2email') {
              $icon = 'LGI-iconb-fax-large';
            }
            else {
              $icon = 'LGI-iconb-'. preg_replace('/s$/', '', strtolower($icon)) . '-large';
            }
            print $icon;
  
            ?>"></span>
            
            <?php 
            $title_attributes = array('class' => array('sg-header-2'));
            
            if (arg(1)=='pbxdevice') {
              $title_attributes['class'][] = 'hpbx-device-title'; 
            }
            ?>
            <h2 <?php print drupal_attributes($title_attributes);?>><?php print $title; ?></h2>
            
            <?php if (arg(1) == 'voicemails'):?>
            </div>
          <?php endif;?>
          
            <?php if ($title_suffix): ?>
              <?php print $title_suffix ?>
            <?php endif; ?>
          
        </div>
        
      <?php endif; ?>      
      <?php print render($page['content']) ?>
    </div>
  </div>
</div>
<?php if ($logged_in && user_access('hpbx basic', $account)) : ?>
  <?php global $hpbx_over_menu ?>
  <?php if (isset($hpbx_over_menu)): ?>
  <?php   print $hpbx_over_menu ?>
  <?php endif ?>
<?php   print theme_hpbx_timeslot_popup($variables); ?>
<?php   print theme_hpbx_help_popups($variables); ?>

<div class="hpbx-alert hpbx-alert-small hpbx-notification-info">
  <div class="hpbx-alert-box">
    <div class="hpbx-notification-icon"></div>
    <div class="hpbx-notification-title"></div>
    <div class="hpbx-notification-message"></div>
    <div class="hpbx-notification-button">
      <button class="sg-btn sg-without-icon sg-white hpbx-alert-close-button"></button>
    </div>
  </div>
</div>


<div class="hpbx-alert hpbx-alert-big hpbx-notification-success">
  <div class="hpbx-alert-box">
    <div class="hpbx-alert-header">
      <div class="hpbx-notification-icon"></div>
      <div class="hpbx-notification-title"></div>
    </div>
    <div class="hpbx-alert-message-section hpbx-first">
      <div class="hpbx-notification-title"></div>
      <div class="hpbx-notification-message"></div>
    </div>
    <div class="hpbx-alert-footer">
        <span class="hpbx-alert-close-button"></span>
    </div>
  </div>
</div>

<div class="hpbx-alert hpbx-alert-confirm">
  <div class="hpbx-alert-box">
    <div class="hpbx-notification-title"><?php print t('Are you sure you want to delete?'); ?></div>
    <div class="hpbx-notification-button">
      <button class="sg-btn sg-without-icon sg-btn-primary hpbx-confirm-positive-button"><?php print t('Yes'); ?></button>
      <button class="sg-btn sg-without-icon sg-white hpbx-confirm-negative-button"><?php print t('No'); ?></button>
    </div>
  </div>
</div>

<?php endif; ?>
  