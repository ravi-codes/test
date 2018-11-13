<?php drupal_render($form['name']);?>
<?php drupal_render($form['pass']);?>


<div class="row">
    <div class="col-xs-12 col-sm-3 col-md-4"></div>
    <div class="col-xs-12 col-sm-6 col-md-4">
        <div class="hpbx-page-title">
            <span class="sg-icon LGI-iconb-subscriber-large"></span>
            <h2 class="sg-header-2"><?php print t('Log in'); ?></h2>
        </div>
        <div class="hpbx-content-container hpbx-form-login">
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('Email or username'); ?></div>
                    <input class="sg-element form-text required" placeholder="<?php print t('Email or username'); ?>" type="text" id="edit-name" name="name" value="" size="60" maxlength="60">
                </div>
                <div class="hpbx-form-login-row">
                    <div class="hpbx-form-login-caption"><?php print t('Password'); ?></div>
                    <input class="sg-element form-text required" placeholder="<?php print t('Password'); ?>" type="password" id="edit-pass" name="pass" size="60" maxlength="128">
                </div>
                <div class="hpbx-form-login-row">
                    <?php print drupal_render($form['actions']); ?>
                </div>
        </div>
        <div class="hpbx-form-login-addition">
            <?php print l(t('Forgot password?'), 'user/password'); ?>
        </div>
    </div>
    <div class="col-xs-12 col-sm-3 col-md-4"></div>
</div>
    <?php
    print drupal_render($form['form_build_id']);
    print drupal_render($form['form_id']);
    
    ?>
