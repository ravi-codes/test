<?php drupal_render($form['password_current']);?>
<?php drupal_render($form['password_new1']);?>
<?php drupal_render($form['password_new2']);?>
<?php drupal_render($form['actions']);?>
<div class="row">
    <div class="col-xs-12 col-sm-2 col-md-3 col-lg-4"></div>
    <div class="col-xs-12 col-sm-8 col-md-6 col-lg-4">
        <div class="hpbx-page-title">
            <span class="sg-icon LGI-iconreset-password"></span>
            <h2 class="sg-header-2"><?php print t('Change password'); ?></h2>
        </div>
        <div class="hpbx-content-container">
            <div class="hpbx-form-login">
            
                <?php if ($form['password_current']['#access']):?>
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('Current password'); ?></div>
                    <input name="password_current" type="password" class="sg-element" placeholder="<?php print t('Current password'); ?>" />
                </div>
                <?php endif;?>
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('New password'); ?></div>
                    <input name="password_new1" type="password" class="sg-element" placeholder="<?php print t('New password'); ?>" />
                </div>
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('Repeat password'); ?></div>
                    <input name="password_new2" type="password" class="sg-element" placeholder="<?php print t('Repeat password'); ?>" />
                </div>
                <p><?php if(variable_get('hpbx_password_policy')){ print t('Your password must be at least eight characters, including one uppercase letter, one lowercase letter, one special character and one digit.');}else{print t('Your password must be at least 7 characters long.');}?></p>
                <div class="hpbx-form-login-row">
                  <input type="submit" class="sg-btn sg-without-icon sg-btn-primary sg-element" value="<?php print t('Save'); ?>"/>
                </div>
                <div class="hpbx-form-login-row">
                    <a href="javascript:history.back();" class="sg-btn sg-without-icon sg-btn-secondary sg-element"><?php print t('Cancel'); ?></a>
                </div>
            </div>
        </div>
    </div>
    <div class="col-xs-12 col-sm-2 col-md-3 col-lg-4"></div>
</div>
<?php
print drupal_render($form['form_token']);
print drupal_render($form['form_build_id']);
print drupal_render($form['form_id']);
?>