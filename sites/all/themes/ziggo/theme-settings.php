<?php

function ziggo_form_system_theme_settings_alter(&$form, $form_state) {

  // Admin settings
  $form['admin_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Admin'),
    '#description' => t('Control how madless\'s admin features behave'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['admin_settings']['madless_move_sidebar'] = array(
    '#type' => 'checkbox',
    '#title' => t('Append second sidebar to first when displaying admin pages'),
    '#default_value' => theme_get_setting('madless_move_sidebar'),
  );
  $form['admin_settings']['breadcrumb_display_admin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always show breadcrumb on admin pages'),
    '#default_value' => theme_get_setting('breadcrumb_display_admin'),
    '#description' => t('This overwrites the general breadcrumb setting for all admin pages'),
  );


  // Breadcrumb
  $form['general_settings']['breadcrumb'] = array(
    '#type' => 'fieldset',
    '#title' => t('Breadcrumb'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['general_settings']['breadcrumb']['breadcrumb_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display breadcrumb'),
    '#default_value' => theme_get_setting('breadcrumb_display'),
  );
  $form['general_settings']['breadcrumb']['breadcrumb_with_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display page title in the breadcrumb'),
    '#default_value' => theme_get_setting('breadcrumb_with_title'),
  );

  // Page titles
  $form['seo']['page_format_titles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page titles'),
    '#description'   => t('This is the title that displays in the title bar of your web browser. Your site title, slogan, and mission can all be set on your Site Information page'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // front page title
  $form['seo']['page_format_titles']['front_page_format_titles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Front page title'),
    '#description'   => t('Your front page in particular should have important keywords for your site in the page title'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['seo']['page_format_titles']['front_page_format_titles']['front_page_title_display'] = array(
    '#type' => 'select',
    '#title' => t('Set text of front page title'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#default_value' => theme_get_setting('front_page_title_display'),
    '#options' => array(
                    'stitle' => t('Site title'),
                    'stitle_slogan'=> t('Site title | slogan'),
                    'default' => t('Drupal default'),
                    'custom' => t('Custom (below)'),
                  ),
  );
  $form['seo']['page_format_titles']['front_page_format_titles']['page_title_display_custom'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom'),
    '#size' => 60,
    '#default_value' => theme_get_setting('page_title_display_custom'),
    '#description'   => t('Enter a custom page title for your front page, tokens can be used'),
  );
  // other pages title
  $form['seo']['page_format_titles']['other_page_format_titles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Other page titles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['seo']['page_format_titles']['other_page_format_titles']['other_page_title_display'] = array(
    '#type' => 'select',
    '#title' => t('Set text of other page titles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#default_value' => theme_get_setting('other_page_title_display'),
    '#options' => array(
                    'ptitle' => t('Page title'),
                    'ptitle_stitle' => t('Page title | Site title'),
                    'ptitle_slogan' => t('Page title | Slogan'),
                    'ptitle_custom' => t('Page title | Custom (below)'),
                    'default' => t('Drupal default'),
                    'custom' => t('Custom (below)'),
                  ),
  );
  $form['seo']['page_format_titles']['other_page_format_titles']['other_page_title_display_custom'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom'),
    '#size' => 60,
    '#default_value' => theme_get_setting('other_page_title_display_custom'),
    '#description'   => t('Enter a custom page title for all other pages, tokens can be used.'),
  );

  // Tokens replacement help list
  $form['seo']['page_format_titles']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('node'),
  );
}
