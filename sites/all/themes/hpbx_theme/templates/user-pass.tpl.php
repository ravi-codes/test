<?php drupal_render($form['name']);?>

<div class="row">
    <div class="col-xs-12 col-sm-3 col-md-4"></div>
    <div class="col-xs-12 col-sm-6 col-md-4">
        <div class="hpbx-page-title">
            <span class="sg-icon LGI-iconreset-password"></span>
            <h2 class="sg-header-2"><?php print t('Password recovery'); ?></h2>
        </div>
        <div class="hpbx-content-container">
          <div class="hpbx-form-login">
                <div class="hpbx-form-login-row">
                    <?php  print t('A one time login link will be sent to your email address.'); ?>
                </div>
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('Email or username'); ?></div>
                    <input type="text" id="edit-name" name="name" class="sg-element" size="60" maxlength="254"  placeholder="<?php print t('Email or username'); ?>" />
                </div>
                <div class="hpbx-form-login-row">
                    <input type="submit" class="sg-btn sg-without-icon sg-btn-primary sg-element" value="<?php print t('Request'); ?>">
                </div>
              </div>
        </div>
        <div class="hpbx-form-login-addition">
          <?php print l(t('Back to login'), 'user'); ?>
        </div>
    </div>
    <div class="col-xs-12 col-sm-3 col-md-4"></div>
</div>
    <?php
    print drupal_render($form['form_build_id']);
    print drupal_render($form['form_id']);
    
    ?>
