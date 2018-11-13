<div class="hpbx-full-screen-header">
    <div class="hpbx-full-screen-header-title">
        <div class="hpbx-full-screen-header-title-icon"><span class="LGI-iconb-upload-large"></span></div>
        <div class="hpbx-full-screen-header-title-text"><?php print t('Bulk import');?></div>
    </div>
    <form method="POST" enctype="multipart/form-data">
      <div class="hpbx-full-screen-header-links">
          
          <a class="hpbx-full-screen-header-link" href="<?php print url('hpbx/subscriber/bulk/template');?>">
              <div class="hpbx-full-screen-header-link-icon"><span class="LGI-icondownload"></span></div>
              <div class="hpbx-full-screen-header-link-text"><?php print t('Download template');?></div>
          </a>
          <a class="hpbx-full-screen-header-link hpbx-file-upload-link" href="#">
              <div class="hpbx-full-screen-header-link-icon"><span class="LGI-iconupload"></span></div>
              <div class="hpbx-full-screen-header-link-text"><?php print t('Upload xlsx');?></div>
          </a>
      </div>
      <input type="file" style="display:none;" name="hpbx-bulk-file">
    </form>
    
</div>
<div class="hpbx-full-screen-content">
  <div class="hpbx-scrollable-container hpbx-also-horizontal">
      <?php print render($variables['page']['content']['system_main']['main']); ?>
      <div class="hpbx-bulk-upload-custom-select-expand-space"></div>
  </div>
</div>
<div class="hpbx-full-screen-footer">
    <div class="hpbx-full-screen-footer-buttons">
        <div class="hpbx-full-screen-footer-button">
            <a class="hpbx-full-screen-footer-link-button" href="<?php print url('hpbx/dashboard');?>"><?php print t('Cancel');?></a>
        </div>
        <div class="hpbx-full-screen-footer-button">
            <input type="button" class="sg-btn sg-without-icon sg-btn-primary hpbx-full-screen-footer-button-start" value="<?php print t('Start');?>" />
        </div>
    </div>
</div>
<div class="hpbx-loader" data-title="<?php print t('Processing');?>..." data-subtitle="<?php print t("This can take some time. Please do not close your browser.");?>">
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
<div class="hpbx-alert hpbx-alert-small hpbx-notification-info">
    <div class="hpbx-alert-box">
        <div class="hpbx-notification-icon"></div>
        <div class="hpbx-notification-title"><?php print t('Upload finished');?></div>
        <div class="hpbx-notification-message" data-message="<?php print t('###successful### out of ###total### rows were successfully added.');?>"></div>
        <div class="hpbx-notification-button">
            <button class="sg-btn sg-without-icon sg-white hpbx-alert-close-button"><?php print t('OK');?></button>
        </div>
    </div>
</div>
<div class="hpbx-alert hpbx-alert-big hpbx-notification-success">
    <div class="hpbx-alert-box">
        <div class="hpbx-alert-header">
            <div class="hpbx-notification-icon"></div>
            <div class="hpbx-notification-title"><?php print t('Bulk import completed');?></div>
        </div>
        <div class="hpbx-alert-message-section hpbx-first">
            <div class="hpbx-notification-title"><?php print t('Devices');?></div>
            <div class="hpbx-alert-message-content"><?php print t('In case you have added Cisco devices to the portal via the bulk import, you need to sync these devices seperately. In order to do so, please enter the device overview, click on sync and follow the instructions. In case you have added Yealink and Panasonic devices, you should restart the devices.');?></div>
            <ul class="hpbx-alert-message-links">
                <li class="hpbx-alert-message-link"><a href="<?php print url('hpbx/pbxdevices');?>"><?php print t('Device overview');?></a></li>
            </ul>
        </div>
        <div class="hpbx-alert-message-section">
            <div class="hpbx-notification-title"><?php print t('Subscribers');?></div>
            <div class="hpbx-alert-message-content"><?php print t('After succesfully having uploaded your subscribers, you can configure options like call forwards, voicemail or speed dials for each subscriber. In order to do so, please enter the subscriber overview and choose the subscriber you want to edit.');?></div>
            <ul class="hpbx-alert-message-links">
                <li class="hpbx-alert-message-link"><a href="<?php print url('hpbx/subscribers');?>"><?php print t('Subscriber overview');?></a></li>
            </ul>
        </div>
        <div class="hpbx-alert-footer">
            <a href="<?php print url('hpbx/dashboard');?>" class="hpbx-alert-close-button"><?php print t('Exit');?></a>
        </div>
    </div>
</div>
