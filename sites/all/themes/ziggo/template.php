<?php

/**
 * Implementation of hook_css_alter().
 */

function ziggo_css_alter(&$css){
  // Clean up unwanted system css files.
  unset($css['modules/system/system.theme.css']);
  unset($css['modules/system/system.menus.css']);
  unset($css['modules/system/system.messages.css']);
  unset($css['modules/filter/filter.css']);
  $views = drupal_get_path('module','views');
  unset($css[$views . '/css/views.css']);
  //var_dump($css);
}
/**
 * Implements template_preprocess_html().
 */
function ziggo_preprocess_html(&$variables) {

  // Add conditional CSS for IE's
  $theme_path = drupal_get_path('theme', 'ziggo');
  $title = t(variable_get('site_name', ''));
  $slogan = t(variable_get('site_slogan', ''));
  $page_title = t(drupal_get_title());
  $title_separator = ' ' . theme_get_setting('configurable_separator') . ' ';
  $head_title = '';

  // Add mobile meta tags
  $mobile_meta = array(
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'viewport',
      'content' => 'width=device-width, initial-scale=1, maximum-scale=1'
    )
  );
  drupal_add_html_head($mobile_meta, 'meta_viewport');

  // Add the choosen display suite layout as a class to the body.
  $node = menu_get_object();

  // (Front) page title settings.
  if (drupal_is_front_page()) {
    switch (theme_get_setting('front_page_title_display')) {
      case 'stitle':
        $head_title = $title;
        break;
      case 'stitle_slogan':
        $head_title = $title . $title_separator . $slogan;
        break;
      case 'custom':
        if (theme_get_setting('page_title_display_custom') !== '') {
          $head_title = token_replace(theme_get_setting('page_title_display_custom'));
        }
    }
    if (theme_get_setting('front_page_title_display') != 'default'){
      $variables['head_title'] = drupal_set_title(decode_entities($head_title));
    }
  }
  else { // Non-front page title settings.
    $head_title = $page_title;
    switch (theme_get_setting('other_page_title_display')) {
      case 'ptitle':
        break;
      case 'ptitle_slogan':
        $head_title .= ($slogan) ? $title_separator . $slogan : '';
        break;
      case 'ptitle_stitle':
        $head_title .= $title_separator . $title;
        break;
      case 'ptitle_custom':
        if (theme_get_setting('other_page_title_display_custom') !== '') {
          $head_title .= $title_separator . token_replace(theme_get_setting('other_page_title_display_custom'));
        }
        break;
      case 'custom':
        if (theme_get_setting('other_page_title_display_custom') !== '') {
          $head_title = token_replace(theme_get_setting('other_page_title_display_custom'));
        }
    }
    if (theme_get_setting('other_page_title_display') != 'default'){
      $variables['head_title'] = drupal_set_title(decode_entities($head_title));
    }
    }
  if (arg(0) == 'user' && !arg(2)) {
    $variables['classes_array'][] = 'page-user-profile';
  }
}

/**
 * Implements theme_menu_local_tasks().
 * Added wrapper div for better tab styling
 */
function ziggo_menu_local_tasks(&$variables) {
  $output = '';
  $secondary = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="tabs primary">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs secondary">';
    $variables['secondary']['#suffix'] = '</ul>';
    $secondary .= drupal_render($variables['secondary']);
  }
  if (!empty($output)){
    $output = '<div class="tabs">' . $output . '</div>';
  }
  if (!empty($secondary)){
    $output .= '<div class="secondary-tabs">' . $secondary . '</div>';
  }
  return $output;
}

/**
 * Implements theme_menu_local_task().
 * Added extra span for better tab styling
 */
function ziggo_menu_local_task($variables) {
  $link = $variables['element']['#link'];
  $link_text = $link['title'];

  // If the link does not contain HTML already, check_plain() it now.
  // After we set 'html'=TRUE the link will not be sanitized by l().
  if (empty($link['localized_options']['html'])) {
    $link['title'] = check_plain($link['title']);
  }
  $link['localized_options']['html'] = TRUE;

  if (!empty($variables['element']['#active'])) {
    // Add text to indicate active tab for non-visual users.
    $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';
    $link_text = t('!local-task-title !active', array('!local-task-title' => $link['title'], '!active' => $active));
  }

  return '<li' . (!empty($variables['element']['#active']) ? ' class="active"' : '') . '>' . l('<span class="inner">'. $link_text .'</span>', $link['href'], $link['localized_options']) . "</li>\n";
}


/**
 * Implements theme_breadcrumb().
 *
 * Allow the title to be parsed in the breadcrumb &
 * unset the breadcrumb on the frontpage.
 * @todo hide breadcrumb on frontpage if set so in theme
 *
 */
function ziggo_breadcrumb($variables) {
  $title = drupal_get_title();
  $breadcrumb = $variables['breadcrumb'];
  if (theme_get_setting('breadcrumb_display') && !empty($breadcrumb) && (count($breadcrumb)>0)) {
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    $output .= implode(' » ', $breadcrumb);
    if (theme_get_setting('breadcrumb_with_title')) {
      $output .=  ' » <span class="title">'. $title .'</span>';
    }
    $output = '<div class="breadcrumb">'. $output .'</div>';
  }
  else $output = '';

  return $output;
}

/**
 * Implements theme_select()
 *
 * Add span wrapper to help styling
 */

function ziggo_select($variables) {
  $element = $variables ['element'];
  element_set_attributes($element, array('id', 'name', 'size'));
  _form_set_class($element, array('form-select'));
  //var_dump($variables);
  if ($element['#multiple']){
    _form_set_class($element, array('form-select form-select-multiple'));
    return '<select' . drupal_attributes($element ['#attributes']) . '>' . form_select_options($element) . '</select>';
  }
  else {
    return '<span class="select-wrapper"><select' . drupal_attributes($element ['#attributes']) . '>' . form_select_options($element) . '</select></span>';
  }
}

/**
 * Implements theme_checkbox()
 *
 * Add an empy label when there is none, to help with checkbox styling.
 */

function ziggo_checkbox($variables) {
  $element = $variables ['element'];
  $element ['#attributes']['type'] = 'checkbox';
  element_set_attributes($element, array('id', 'name', '#return_value' => 'value'));

  // Unchecked checkbox has #value of integer 0.
  if (!empty($element ['#checked'])) {
    $element ['#attributes']['checked'] = 'checked';
  }
  _form_set_class($element, array('form-checkbox'));

  if (!isset($element['#title'])){
    // Add our empty label.
    _form_set_class($element, array('form-checkbox-no-label'));
    return '<input' . drupal_attributes($element ['#attributes']) . ' /><label />';
  }
  else {
    return '<input' . drupal_attributes($element ['#attributes']) . ' />';
  }
}

/**
 * Implements template_preprocess_block()
 */

function ziggo_preprocess_block(&$variables){

  // Add column numbers to the blocks. For example if you want a 3 column grid;
  // you can set a clear: left on class block-column-after-3
  $columns = find_dividers($variables['block_id']);
  $variables['classes_array'] = array_merge($variables['classes_array'],$columns);

  // add a class if the block is first in it's region.
  if ($variables['block_id'] == 1) {
    $variables['classes_array'][] = 'first-block';
  }


  if ($variables['block']->region == 'header'){
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  };
  if (isset($variables['blocktheme'])){
    $variables['classes_array'][] = $variables['blocktheme'];
  };
}

/**
* Implements template_preprocess_field().
*/
function mvo_preprocess_field(&$variables, $hook){
  if ($variables['element']['#field_name'] == 'title_field'){
    if (empty($variables['element']['#object']->title)){
      // if the title is empty, unset the item, so that we don't end up with an
      // empty H2.
      unset($variables['items']);
    };
  };
}

/**
 * Implements template_preprocess_node().
 */
function ziggo_preprocess_node(&$variables) {
  // Add view-mode classes
  $variables['classes_array'][] = 'node-view-' . $variables['view_mode'];
}

/**
 * Implements template_preprocess_page().
 */
function ziggo_preprocess_page(&$variables) {

  global $base_root, $base_path;

  // Manually flush cache when in developer mode, so that variables are inserted
  if (module_exists('less') && variable_get('less_devel')==true){
    less_flush_caches();
  }

  // Set up logo variable.
  $path = substr($variables['logo'], strlen($base_root . $base_path), strlen($variables['logo']));
  $info = image_get_info($path);
  $site_name   = filter_xss_admin(variable_get('site_name', 'Drupal'));
  $variables['logo'] = theme('image', array(
    'path' => $path,
    'alt' => $site_name,
    'title' => $site_name,
    'width' => !empty($info['width']) ? $info['width'] : '',
    'height' => !empty($info['height']) ? $info['height'] : '',
  ));

  $variables['logo'] = l($variables['logo'], "<front>", array(
    'html' => 'true',
    'attributes' => array(
      'title' => $site_name,
    ),
  ));

    // Set up main_menu variable when main_menu has items.
  if (!empty($variables['main_menu'])) {
    $variables['main_menu'] = theme('links', array(
      'links' => $variables['main_menu'],
      'attributes' => array('class' => 'menu primary-links'),
    ));
  }

  // Set up secondary_menu variable when secondary_menu has items.
  if (!empty($variables['secondary_menu'])) {
    $variables['secondary_menu'] = theme('links', array(
      'links' => $variables['secondary_menu'],
      'attributes' => array('class' => 'menu secondary-links'),
    ));
  }
}


/**
 * Preprocessor for menu_link.
 * Adds the required HTML attributes and loads subtrees if necessary.
 */
function ziggo_preprocess_menu_link(&$variables) {
  if($variables['element']['#href']=='<front>'){
    $variables['element']['#attributes']['class'][] = 'home-link';
  };
  $variables['element']['#attributes']['class'][] =  strtolower(drupal_clean_css_identifier('menu-' . $variables['element']['#title']));
}

function ziggo_process_page(&$variables) {
  if (theme_get_setting('hide_front_page_title') && drupal_is_front_page()){
    $variables['title'] = null;
  }
}

function ziggo_preprocess_region(&$variables) {
  if ($variables['region'] == 'sidebar_first' || $variables['region'] == 'sidebar_second'){
    $variables['classes_array'][] = 'sidebar';
  }
}

function ziggo_status_messages($variables) {
  $display = $variables['display'];
  $output = '';

  $status_heading = array(
    'status' => t('Status message'),
    'error' => t('Error message'),
    'warning' => t('Warning message'),
  );
  foreach (drupal_get_messages($display) as $type => $messages) {
    $output .= "<div class=\"messages $type\">\n<div class=\"messages-inner\">\n";
    if (!empty($status_heading[$type])) {
      $output .= '<h2 class="element-invisible">' . $status_heading[$type] . "</h2>\n";
    }
    if (count($messages) > 1) {
      $output .= " <ul>\n";
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . "</li>\n";
      }
      $output .= " </ul>\n";
    }
    else {
      $output .= $messages[0];
    }
    $output .= "</div>\n</div>\n";
  }
  return $output;
}

/**
 * Implements theme_form_alter().
 */

function ziggo_form_alter(&$form, $form_state, $form_id){

  //var_dump($form_id);

  if ($form_id == 'adm_search_form'){
    //var_dump($form);
    $form['#attributes']['class'][] = 'test';
  }

  // Modify search form, show label and use an image button.
  if ($form_id=='search_block_form'){
    $theme_path = drupal_get_path('theme', 'ziggo');

    $form['search_block_form']['#title'] = t('Search this site');
    $form['search_block_form']['#title_display'] = 'before';

    unset($form['actions']['submit']);
    $form['submit']['#type'] = 'image_button';
    $form['submit']['#attributes'] = array( 'alt' => t('Search'));
    $form['submit']['#src'] = $theme_path . '/images/i_search.png';
  }
  return $form;
}

/*
 * Helper function:
 * Find the numbers that the input can be divided by
 */

function find_dividers($number){
  $results = array();
  // we start with 2, because every number can be divided by one
  for ($i = 2; $i <= $number; $i++) {
    if ( $number % $i == 0 ){
      $results[] = 'block-column-'.$i;
    }
    if ( $number % $i == 1 ){
      $results[] = 'block-column-after-'.$i;
    }
  }
  return $results;
}