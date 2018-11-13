

<div class="row">
    <div class="col-xs-12 col-sm-3 col-md-4 col-lg-3"></div>
    <div class="col-xs-12 col-sm-6 col-md-4 col-lg-6">
        <div class="hpbx-page-title">
            <span class="sg-icon LGI-iconreset-password"></span>
            <h2 class="sg-header-2"><?php print t('Password reset'); ?></h2>
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
                <div class="hpbx-form-login-row">
                    <input type="submit" class="sg-btn sg-without-icon sg-btn-primary sg-element" value="<?php print t('Save'); ?>">
                </div>
            </div>
        </div>
    </div>
    <div class="col-xs-12 col-sm-3 col-md-4 col-lg-3"></div>
</div>
