<?php

/**
 * Implementation of hook_css_alter().
 */

function hpbx_theme_css_alter(&$css){

  // Clean up unwanted system css files.
  unset($css['modules/system/system.theme.css']);
  unset($css['modules/system/system.menus.css']);
  unset($css['modules/system/system.base.css']);
  unset($css['modules/system/system.messages.css']);
  unset($css['modules/filter/filter.css']);
  unset($css[drupal_get_path('module','views') . '/css/views.css']);
  unset($css[drupal_get_path('module','adm') . '/includes/css/adm.css']);
  unset($css[drupal_get_path('module','superfish') . '/css/superfish.css']);
  unset($css[drupal_get_path('module','superfish') . '/css/superfish-vertical.css']);
  unset($css[drupal_get_path('module','superfish') . '/css/superfish-navbar.css']);
  unset($css[drupal_get_path('module','field_collection') . '/field_collection.theme.css']);

  // Due to an problem was a bug in the cache manager of IE9. The workaround is to
  // make sure that the URL of the CSS file in the main document is a different
  // one than the URL of the CSS file in the iframe. This will be done by adding 
  // "?version=iframe” to the URL of the CSS in all Help files, because those are
  // the files that are loaded in the iframe.
  if ((arg(0) == 'hpbx' && arg(1) == 'help') || arg(0) == 'node') {
    foreach ($css as $key => &$values) {
      // Change the hpbx css file from file to external; This so we can use a
      // query string (@see common.inc 3537).
      $values['data'] = url($values['data'], array('query' => array('version' => 'iframe'), 'absolute' => TRUE, 'language' => FALSE));
      $values['type'] = 'external';
    }
  }
}

/**
 * Implements hook_preprocess_search_results().
 */
function hpbx_theme_preprocess_search_results(&$vars) {
  // Get the total number of results from the global pager
  $total = $GLOBALS['pager_total_items'][0];

  // If there is more than one page of results:
  $vars['search_totals'] = t('!total search results for !keys', array('!total' => $total, '!keys' => arg(3)));
}

function hpbx_theme_preprocess_page(&$variables) {
  
  if ($_GET['q'] == 'hpbx/subscriber/bulk') {
    $variables['theme_hook_suggestions'][] = 'page__hpbx_subscriber_bulk';
  }
  
  if (in_array('page__node__add__hpbx_help_article', $variables['theme_hook_suggestions'])) {
    $variables['theme_hook_suggestions'][] = 'page__hpbx_help_article__edit';
  }
  
  if (isset($variables['node']->type) && ($variables['node']->type == 'hpbx_help_article')) {


    if (arg(2) == 'edit' || arg(2) == 'add') {
      $variables['theme_hook_suggestions'][] = 'page__hpbx_help_article__edit';
    }
    else {
      $variables['theme_hook_suggestions'][] = 'page__hpbx_help_article';
    }
  }

  require_once(drupal_get_path('module', 'hpbx') . '/includes/hpbx.help.inc');
  $variables['search_form'] = drupal_get_form('hpbx_help_search_form');
  
  // Pass messages to html.
  $var = &drupal_static('hpbx_messages');
  if (!isset($var)) {
    if (!isset($variables['messages'])) {
      $var = $variables['show_messages'] ? theme('status_messages') : '';
    }
  }

  global $title_suffix;
  
  if (isset($title_suffix)) {
    $variables['title_suffix'] = $title_suffix;
  }
  
}

// remove a tag from the head for Drupal 7
function hpbx_theme_html_head_alter(&$head_elements) {
  
  unset($head_elements['system_meta_content_type']);
  unset($head_elements['system_meta_generator']);
}

function hpbx_theme_preprocess_html(&$variables) {
  
$js_foot = <<<EOF
  <!--[if lt IE 9]>
  <script src="/sites/all/themes/hpbx_theme/dependencies/jquery-legacy/dist/jquery.min.js"></script>
  <![endif]-->
  <!--[if gte IE 9]><!-->
  <script src="/sites/all/themes/hpbx_theme/dependencies/jquery/dist/jquery.min.js"></script>
  <!--<![endif]-->
<script>
  (function ($) {
    $('document').ready(function(){
    $('.hpbx-disable-target-when-value').on('change',disableTargetWhenValue).trigger('change');
    
      function disableTargetWhenValue(event) {
        var \$element = $(this),
         target   = \$element.data('target'),
         value    = \$element.data('value');
         
        if (!target || !value) return;
         
        if (\$element.val() != value) {
          $('.' + target).removeClass('sg-disabled').prop('disabled', false);
        } else {
          $('.' + target).addClass('sg-disabled').prop('disabled', 'disabled').val('');
        }
      };
    });
  })(jQuery);
</script>
EOF;


if (arg(0) == 'hpbx' && arg(1) == 'call-log') {
  
  $js_foot .= <<<EOF
<script type="text/javascript" src="/sites/all/themes/hpbx_theme/dependencies/pickadate/lib/compressed/legacy.js"></script>
EOF;
}

$js_foot .= <<<EOF
<script type="text/javascript" src="/sites/all/themes/hpbx_theme/dist/js/all.js"></script>
EOF;

$headers = drupal_get_http_header();

if (isset($headers['status']) && $headers['status'] == '404 Not Found') {
  $variables['classes_array'][] = 'hpbx-page-404';
}


if (in_array('html__node__add__hpbx_help_article', $variables['theme_hook_suggestions'])) {
  $variables['classes_array'][] = 'hpbx-page-help';
  $variables['classes_array'][] = 'hpbx-page-help-edit';
}

if ((arg(1) == 'password' || arg(1) == 'terms')) {
  $variables['classes_array'][] = 'hpbx-no-menu';
}

if (arg(0) == 'hpbx' && arg(1) == 'dashboard') {
  $variables['classes_array'][] = 'hpbx-page-dashboard';
}
if (arg(0) == 'hpbx' && arg(1) == 'help') {
  $variables['classes_array'][] = 'hpbx-page-help';
}

if (arg(0) == 'node' && 
  
    (isset($variables['page']['content']['system_main']['nodes']) && 
  current($variables['page']['content']['system_main']['nodes'])['#node']->type == 'hpbx_help_article') || 
    
    (isset($variables['page']['content']['system_main']['#node']) && 
  $variables['page']['content']['system_main']['#node']->type == 'hpbx_help_article') ) {
    
  // Add help body class.
  $variables['classes_array'][] = 'hpbx-page-help';

  // Add help edit body class.
  if (arg(2) == 'edit') {
    $variables['classes_array'][] = 'hpbx-page-help-edit';
  }       
}

if (arg(0) == 'hpbx' && arg(1) == 'wizard') {
  $variables['classes_array'][] = 'hpbx-page-wizard';
}

  if (arg(0) == 'hpbx' && arg(1) == 'help' && arg(2) == 'sitemap') {
    $variables['classes_array'][] = 'hpbx-page-help';
    $variables['classes_array'][] = 'hpbx-page-help-edit';
  }



if (arg(0) == 'hpbx' && arg(1) == 'help' && arg(2) == 'search') {
  $variables['classes_array'][] = 'hpbx-show-menu-over-menu';
}

if (arg(0) == 'hpbx' && arg(1) == 'subscriber' && arg(2) == 'bulk' ) {
  $variables['classes_array'][] = 'hpbx-show-menu-over-menu';
  $variables['classes_array'][] = 'hpbx-page-bulk-upload';
}


$messsages = &drupal_static('hpbx_messages');
  
$device = '';
  if (arg(0) == 'hpbx' && arg(1) == 'pbxdevice' && (arg(2) == 'add' || arg(3) == 'edit')) {
  
    $device .= '
      <div class="hpbx-device-info container-fluid hidden-xs" style="display:none;">
        <div class="row">
          <div class="col-sm-5">
            <span class="hpbx-device-profile-label">'. t('Device profile'). ':</span>
            <span class="hpbx-device-profile">&nbsp;</span>
          </div>
          <div class="col-sm-7">
            <span class="hpbx-device-identifier-label">'. t('MAC address'). ':</span>
            <input class="hpbx-device-footer-input-incognito" type="text"/>
            <span class="hpbx-device-identifier-edit-icon LGI-iconedit"></span>
          </div>
        </div>
      </div>';
    
    $device .= '
        <div class="hpbx-menu hpbx-menu-over-menu col-xs-12 hpbx-main-content" style="display:none;">
          <div class="hpbx-device-header">
            <div class="hpbx-device-icon"></div>
            <div class="hpbx-menu-handle hpbx-menu-handle-open hpbx-menu-over-menu-menu-handle-open"></div>
            <div class="hpbx-device-title"></div>
          </div>
          
          <table id="hpbx-device-subscriber-table" class="hpbx-device-subscriber-table"  style="display:none;" ></table>
          <div class="hpbx-device-footer">
            <form>
              <input type="submit" class="sg-btn sg-without-icon sg-btn-primary" value="'. t('Save') . '"/>
              <a class="hpbx-tab-form-link-button" href="'. url('hpbx/pbxdevices') . '">'. t('Cancel').'</a>
            </form>
          </div>
        </div>';
    
    
    $device .= '
        <div id="hpbx-device-subscriber-dialog" class="hpbx-popup mfp-hide">
            <div class="hpbx-popup-content">
                <h4 class="hpbx-popup-title"></h4>
                <form class="hpbx-popup-form">

                    <div class="row hpbx-popup-form-row">
                        <div class="hpbx-tab-form-label-field-row hpbx-required">
                            <div class="col-xs-12">
                                <div class="hpbx-tab-form-label">'. t('Subscriber') .'</div>
                            </div>
                            <div class="col-xs-12">
                                <div class="hpbx-tab-form-field">
                                    <select id="subscriber_id" class="chzn-select sg-element no-search hpbx-enable-target-when-value" data-target="hpbx-timeout-destination" data-value="uri" data-placeholder="'. t('Choose an option') . '"></select>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="row">
                        <div class="hpbx-tab-form-label-field-row hpbx-required">
                            <div class="col-xs-12">
                                <div class="hpbx-tab-form-label">'. t('Type') .'</div>
                            </div>
                            <div class="col-xs-12">
                                <div class="hpbx-tab-form-field">
                                    <select id="line_type" class="chzn-select sg-element no-search hpbx-enable-target-when-value" data-target="hpbx-timeout-destination" data-value="uri" data-placeholder="'. t('Choose an option') . '"></select>
                                </div>
                            </div>
                        </div>
                    </div>
                    <input type="hidden" name="linerange-id" value=""/>
                    <input type="hidden" name="linerange-key" value=""/>

                    <div class="hpbx-popup-form-buttons-row">
                        <a class="hpbx-popup-form-link-button" href="#">'.t('Remove').'</a>
                        <input type="submit" class="sg-btn sg-without-icon sg-btn-primary" value="'.t('Set').'">
                    </div>
                </form>
            </div>
        </div>';

    if (arg(3)=='edit') {
      $variables['classes_array'][] = 'hpbx-device-page';
      $variables['classes_array'][] = 'page-hpbx-pbxdevices';
    }
  }
  else {

    $pos = array_search('page-hpbx-pbxdevices', $variables['classes_array']);
    if ($pos!==FALSE) {
      unset($variables['classes_array'][$pos]);
    }
  }

  $footer = $device . $messsages . $js_foot;
  $variables['page']['page_bottom']['hpbx_footer'] = array('#markup' => $footer);  
}

function hpbx_theme_process_views_view_list(&$vars) {

  if ($vars['view']->name == 'hpbx_help') {

    switch ($vars['title']) {
      case 'Call scenario':
        $vars['options']['class'] = 'hpbx-help-category-call-scenario';
        break;
      case 'Device help':
        $vars['options']['class'] = 'hpbx-help-category-device-help';
        break;
      case 'First time user':
        $vars['options']['class'] = 'hpbx-help-category-first-time-user';
        break;
      case 'F.A.Q.':
        $vars['options']['class'] = 'hpbx-help-category-faq';
        break;
      case 'Installation':
        $vars['options']['class'] = 'hpbx-help-category-installation';
        break;
    }
  }
}

function hpbx_theme_select($variables) {
  $element = $variables['element'];
  element_set_attributes($element, array('id', 'name', 'size'));
  _form_set_class($element, array('form-select'));

  if (isset($element['#parents']) && form_get_error($element) !== NULL && !empty($element['#validated'])) {
    $element['#attributes']['class'][] = 'sg-error';
  }
  
  return '<select' . drupal_attributes($element['#attributes']) . '>' . hpbx_form_select_options($element) . '</select>';
}

function hpbx_form_select_options($element, $choices = NULL) {
  if (!isset($choices)) {
    $choices = $element['#options'];
  }
  
  // Ensure #option_attributes is set.
  $element += array('#option_attributes' => array());
  
  
  // array_key_exists() accommodates the rare event where $element['#value'] is NULL.
  // isset() fails in this situation.
  $value_valid = isset($element['#value']) || array_key_exists('#value', $element);
  $value_is_array = $value_valid && is_array($element['#value']);
  $options = '';
  foreach ($choices as $key => $choice) {
    if (is_array($choice)) {
      $options .= '<optgroup label="' . check_plain($key) . '">';
      $options .= hpbx_form_select_options($element, $choice);
      $options .= '</optgroup>';
    }
    elseif (is_object($choice)) {
      $options .= hpbx_form_select_options($element, $choice->option);
    }
    else {
      
      $option_attributes = array();
      if (isset($element['#option_attributes'][$key])) {
        $option_attributes = $element['#option_attributes'][$key];
      }
      $key = (string) $key;
      if ($value_valid && (!$value_is_array && (string) $element['#value'] === $key || ($value_is_array && in_array($key, $element['#value'])))) {
        $option_attributes['selected'] = 'selected';
      }
      $option_attributes['value'] = check_plain($key);
      
      $options .= '<option '.drupal_attributes($option_attributes) .'>' . check_plain($choice) . '</option>';
    }
  }
  return $options;
}


/**
 * Implements hook_theme().
 */
function hpbx_theme_theme() {
  return array(
    'user_login' => array(
      'render element' => 'form',
      'path' => drupal_get_path('theme', 'hpbx_theme') . '/templates',
      'template' => 'user-login',
      'title' => 'log in',
      'preprocess functions' => array(
         'hpbx_theme_preprocess_user_login'
      ),
    ),
    'user_pass' => array(
      'render element' => 'form',
      'path' => drupal_get_path('theme', 'hpbx_theme') . '/templates',
      'template' => 'user-pass',
      'title' => 'password recovery',
      'preprocess functions' => array(
          'hpbx_theme_preprocess_user_pass'
      ),
    ),
    'hpbx_user_pass_reset_form' => array(
      'render element' => 'form',
      'path' => drupal_get_path('theme', 'hpbx_theme') . '/templates',
      'template' => 'user-pass-reset-form',
      'title' => 'password reset',
      'preprocess functions' => array(
          'hpbx_theme_preprocess_user_pass_reset_form'
      ),
    ),
  );
}



function hpbx_theme_form_hpbx_terms_page_node_form_alter(&$form, &$form_state) {
  
  $form['#prefix'] = '<div class="hpbx-main-content-inner hpbx-music-on-hold">';
  $form['#suffix'] = '</div>';
  $form['#attributes'] = array('class' => array('hpbx-tab-form'));
  
  $form['actions']['#attributes'] = array('class' => array('hpbx-tab-form-buttons-row'));
  $form['actions']['#prefix'] = '<div class="row">';
  $form['actions']['#suffix'] = '</div>';
  
  $form['actions']['submit']['#prefix'] = '<span class="submit-button">';
  $form['actions']['submit']['#suffix'] = '</span>';
  $form['actions']['submit']['#attributes'] = array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary'));
  
  $form['actions']['preview']['#access'] = FALSE;
  $form['additional_settings']['#access'] = FALSE;
  
}

function hpbx_theme_form_user_pass_reset_alter(&$form, &$form_state) {
  
  $form['#prefix'] = '<div class="hpbx-main-content-inner hpbx-music-on-hold">';
  $form['#suffix'] = '</div>';
  $form['#attributes'] = array('class' => array('hpbx-tab-form'));
  
  $form['message']['#prefix'] = '<div class="col-xs-12">';
  $form['help']['#suffix'] = '<br/></div>';
          
          
  $form['actions']['#attributes'] = array('class' => array('hpbx-main-content-buttons-row'));
  $form['actions']['#prefix'] = '<div class="row">';
  $form['actions']['#suffix'] = '</div>';

  $form['actions']['submit']['#prefix'] = '<span class="col-xs-12">';
  $form['actions']['submit']['#suffix'] = '</span>';
  $form['actions']['submit']['#attributes'] = array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary'));
}

function hpbx_theme_preprocess_user_login(&$variables) {
  
  $variables['form']['name']['#attributes']['class'][] = 'sg-element';
  $variables['form']['name']['#attributes']['placeholder']= t('Username');
  unset($variables['form']['name']['#title']);

  $variables['form']['pass']['#attributes']['class'][] = 'sg-element';
  $variables['form']['pass']['#attributes']['placeholder'] = t('Password');
  unset($variables['form']['pass']['#title']);

  $variables['form']['actions']['submit']['#attributes']['class'] = array('sg-btn', 'sg-without-icon', 'sg-btn-primary', 'sg-element');
}

function hpbx_theme_preprocess_user_pass(&$variables) {

  unset($variables['form']['name']['#title']);
}

function hpbx_theme_status_messages($variables) {
  $display = $variables ['display'];
  $output = '';

  $status_heading = array(
      'status' => t('Status message'),
      'error' => t('Error message'),
      'warning' => t('Warning message'),
  );
  foreach (drupal_get_messages($display) as $type => $messages) {
    
    if ($type == 'status') {
      $type = 'success';  
    }
    $output .= '<div class="hpbx-notification hpbx-notification-'. $type .'">';
    $output .= '  <div class="hpbx-notification-icon"></div>';
    $output .= '  <div class="hpbx-notification-title"></div>';
    
    $output .= '  <div class="hpbx-notification-message">';
    
    
    
    if (count($messages) > 1) {
      $output .= " <ul>\n";
      foreach ($messages as $message) {
        $output .= '  <li>' . $message . "</li>\n";
      }
      $output .= " </ul>\n";
    }
    else {
      $output .= reset($messages);
    }
    
    
    $output .= "</div>\n";
    if ($type == 'error') {
      $output .= '<div class="hpbx-notification-button">';
      $output .= '<button class="sg-btn sg-without-icon sg-white hpbx-notification-close-button">'. t('Got it!'). '</button>';
      $output .= '</div>';
    }
    
    $output .= '</div>';
  }
  return $output;
}

function hpbx_theme_textfield($variables) {
  $element = $variables['element'];
  
  $element['#attributes']['type'] = 'text';
  
  element_set_attributes($element, array('id', 'name', 'value', 'size', 'maxlength'));
  
  _form_set_class($element, array('form-text'));


  if (isset($element['#parents']) && form_get_error($element) !== NULL && !empty($element['#validated'])) {
    $element['#attributes']['class'][] = 'sg-error';
  }
  
  $extra = '';
  if ($element['#autocomplete_path'] && drupal_valid_path($element['#autocomplete_path'])) {
    drupal_add_library('system', 'drupal.autocomplete');
    $element['#attributes']['class'][] = 'form-autocomplete';

    $attributes = array();
    $attributes['type'] = 'hidden';
    $attributes['id'] = $element['#attributes']['id'] . '-autocomplete';
    $attributes['value'] = url($element['#autocomplete_path'], array('absolute' => TRUE));
    $attributes['disabled'] = 'disabled';
    $attributes['class'][] = 'autocomplete';
    $extra = '<input' . drupal_attributes($attributes) . ' />';
  }

  
  $output = '<input' . drupal_attributes($element['#attributes']) . ' />';

  return $output . $extra;
}

function hpbx_theme_checkbox($variables) {
  $element = $variables['element'];
  
  $element['#attributes']['type'] = 'checkbox';
  element_set_attributes($element, array('id', 'name', '#return_value' => 'value'));

  // Unchecked checkbox has #value of integer 0.
  if (!empty($element['#checked'])) {
    $element['#attributes']['checked'] = 'checked';
  }
  //_form_set_class($element, array(''));


  if (isset($element['#parents']) && form_get_error($element) !== NULL && !empty($element['#validated'])) {
    $element['#attributes']['class'][] = 'sg-error';
  }
  
  if (isset($element['#wrapper_attributes'])) {
    $wrapper_attributes = $element['#wrapper_attributes'];
  }
  $wrapper_attributes['class'][] = 'sg-custom-check-and-radio';
  $wrapper_attributes['class'][] = 'sg-custom-check';

  $output = '<div '. drupal_attributes($wrapper_attributes) .'>';
  $output .= '<label title="">';
  $output .= '<input' . drupal_attributes($element['#attributes']) . ' />';
  
  if (!empty($element['#caption'])) {
    $output .= '<span class="sg-caption">';
    $output .= $element['#caption'];    
    $output .= '</span>';
  }
  $output .= '  <div class="sg-alternative-display">';
  $output .= '    <div class="sg-icon-check"></div>';
  $output .= '  </div>';
  $output .= '</label>';
  $output .= '</div>';
  
  return $output;
}

function hpbx_theme_form_element($variables) {
  $element = &$variables['element'];

  $t = get_t();
  
  // This function is invoked as theme wrapper, but the rendered form element
  // may not necessarily have been processed by form_builder().
  $element += array(
      '#title_display' => 'before',
  );

  // $attributes['class'] = $element['#attributes']['class'];
  
  // Add element #id for #type 'item'.
  if (isset($element['#markup']) && !empty($element['#id'])) {
    $attributes['id'] = $element['#id'];
  }
  // Add element's #type and #name as class to aid with JS/CSS selectors.
  
  if (!empty($element['#type'])) {
//     /$attributes['class'][] = 'form-type-' . strtr($element['#type'], '_', '-');
  }
  if (!empty($element['#name'])) {
    $attributes['class'][] = 'form-item-' . strtr($element['#name'], array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));
  }
  // Add a class for disabled elements to facilitate cross-browser styling.
  if (!empty($element['#attributes']['disabled'])) {
    $attributes['class'][] = 'form-disabled';
  }
 
  $row_attributes = array();
  if (isset($element['#row_attributes'])) {
    $row_attributes += $element['#row_attributes'];
    // var_dump($row_attributes);
  }

  $row_attributes['class'][] = 'row';
  if (!empty($element['#deletable_row'])) {
    $row_attributes['class'][] = 'hpbx-deletable-row';
  }
  
  if (!empty($element['#data_show_type'])) {
    $row_attributes['data-show-type'] = $element['#data_show_type'];
  }
  if (!empty($element['#data_hide_type'])) {
    $row_attributes['data-hide-type'] = $element['#data_hide_type'];
  }
  
  
  $output = '';
  if (!isset($element['#row_wrapper']) || $element['#row_wrapper']) {
    $output = '<div ' . drupal_attributes($row_attributes). '>' . "\n";
  }
  
  //$variables['element'];
  
  $attributes['class'][] = 'hpbx-tab-form-label-field-row';

  if ($element['#required']) {
    $attributes['class'][] = 'hpbx-required';
  }
  //$output .= "\t". '<div ' . drupal_attributes($attributes). '>' . "\n";
  
  
  $title = '';
  if (isset($element['#title'])) {
    $title = filter_xss_admin($element['#title']);
  }
  $prefix = isset($element['#field_prefix']) ? ' <span'.(!empty($element['#field_prefix']['#attributes']) ? drupal_attributes($element['#field_prefix']['#attributes']) : '') .'">' . drupal_render($element['#field_prefix']) . '</span>' : '';
  $suffix = isset($element['#field_suffix']) ? ' <span'.(!empty($element['#field_suffix']['#attributes']) ? drupal_attributes($element['#field_suffix']['#attributes']) : '') .'">' . drupal_render($element['#field_suffix']) . '</span>' : '';

  if (!isset($element['#label_attributes'])) {
    $element['#label_attributes'] = array('class' => array('col-xs-12', 'col-sm-3'));
  }

  if ($element['#title_display']=='before' && isset($element['#title']) && $element['#title'] !== '') {
    if (!isset($element['#label_row_attributes'])) {
      $element['#label_row_attributes'] = array('class' => array('hpbx-tab-form-label-field-row'));
    }
    $output .= '<div '. drupal_attributes($element['#label_row_attributes']). '>';
    $output .= "\t\t". '<div '.drupal_attributes($element['#label_attributes']).'>' . "\n";
    $output .= "\t\t\t". '<div class="hpbx-tab-form-label hpbx-tab-form-label-'. $element['#type'] .'">'. $t($title).'</div>' . "\n";
    $output .= "\t\t". '</div>' ."\n";
  }

  // Set default col size.
  if (!isset($element['#field_size_class'])) {
    $element['#field_size_class'] = array('col-xs-12', 'col-sm-8', 'col-md-7');
  }
  $output .= "\t\t". '<div class="'. implode(' ', $element['#field_size_class'])  .'">' . "\n";
  //$output .= "\t\t\t". '<div class="hpbx-tab-form-field">' . "\n";
  
  $tab_form_field_classes = array();
  if (isset($element['#tab_form_field_classes'])) {
    $tab_form_field_classes = $element['#tab_form_field_classes'];
    
  }
  else {
    $tab_form_field_classes[] = 'hpbx-tab-form-field';
    if ($element['#type']=='item') {
      $tab_form_field_classes[] = 'hpbx-tab-form-field-text';
    }
  }
  
  $output .= '<div class="'. implode(' ', $tab_form_field_classes) .'">';
  $output .= "\t\t\t\t". ' ' . $prefix . $element['#children'] . $suffix . "\n";
  $output .= '</div>' . "\n";
  //$output .= "\t\t\t". '</div>'. "\n";
  $output .= "\t\t". '</div>' . "\n";

  if ($element['#title_display']=='after' && isset($element['#title']) && $element['#title'] !== '') {
    $output .= "\t\t". '<div '.drupal_attributes($element['#label_attributes']).'>' . "\n";

    if (!isset($element['#label_row_attributes'])) {
      $element['#label_row_attributes'] = array('class' => array('hpbx-tab-form-label', 'hpbx-tab-form-label-'. $element['#type']));
    }

    $output .= "\t\t\t". '<div  '. drupal_attributes($element['#label_row_attributes']). '>'. $t($title).'</div>' . "\n";
    $output .= "\t\t". '</div>' ."\n";
  }
  
  if (!empty($element['#description'])) {
    $output .= "\t\t". '<div class="col-xs-12 col-sm-9 col-sm-offset-3">';
    $output .= "\t\t\t".   '<div class="hpbx-tab-form-explanation">' . $element['#description'] . "</div>\n";
    $output .= "\t\t". '</div>'. "\n";
  }

  if ($element['#title_display']=='before' && isset($element['#title']) && $element['#title'] !== '') {
    $output .= "\t\t". '</div>' ."\n";
  }
  //$output .= "\t". '</div>' . "\n";
  
  if (!isset($element['#row_wrapper']) || $element['#row_wrapper']) { 
    $output .= '</div>' . "\n";
  }

  return $output;
}

function hpbx_theme_container($variables) {
  $element = $variables['element'];
  
  // Ensure #attributes is set.
  $element += array('#attributes' => array());

  // Special handling for form elements.
  if (isset($element['#array_parents'])) {
    // Assign an html ID.
    if (!isset($element['#attributes']['id'])) {
      $element['#attributes']['id'] = $element['#id'];
    }
    // Add the 'form-wrapper' class.
    $element['#attributes']['class'][] = 'form-wrapper';
  }

  if (isset($element['#tabform']) && $element['#tabform']) {
    return '<div' . drupal_attributes($element['#attributes']) . '><div class="hpbx-tab-form">' . $element['#children'] . '</div></div>';
  }
  else {
    return '<div' . drupal_attributes($element['#attributes']) . '>'. $element['#children'] . '</div>';
  }
}



function hpbx_theme_menu_tree($variables){

  $output = <<<EOF
  <div class="hpbx-menu">
  <div class="hpbx-menu-handle hpbx-menu-handle-open"></div>
  <ul class="hpbx-menu-items">
    <li class="hpbx-menu-item hpbx-menu-item-user">
      <div class="hpbx-menu-item-icon-container">
          <div class="hpbx-menu-item-icon hpbx-menu-handle hpbx-menu-handle-close"></div>
          <div class="hpbx-menu-item-count"></div>
      </div>
      <div class="hpbx-menu-item-title-container">
          <span class="hpbx-menu-item-title">{$variables['username']}</span>
      </div>
      <div class="hpbx-menu-item-subtitle-container">
EOF;

  if (!empty($variables['role'])) {
    $output .= '<span class="hpbx-menu-item-subtitle-addition">'. $variables['role'] . '</span>';
  }

  $output .= <<<EOF
        <span class="hpbx-menu-item-subtitle">{$variables['company']}</span>
      </div>
  </li>
EOF;
  
  $output .= $variables['tree'];
  $output .= hpbx_theme_language_list();
  $output .= '</ul></div>';
  return $output;
}

/**
 * Returns HTML for a radio button form element.
 *
 * Note: The input "name" attribute needs to be sanitized before output, which
 *       is currently done by passing all attributes to drupal_attributes().
 *
 * @param $variables
 *   An associative array containing:
 *   - element: An associative array containing the properties of the element.
 *     Properties used: #required, #return_value, #value, #attributes, #title,
 *     #description
 *
 * @ingroup themeable
 */
function hpbx_theme_radio($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'radio';
  element_set_attributes($element, array('id', 'name', '#return_value' => 'value'));

  if (isset($element['#return_value']) && $element['#value'] !== FALSE && $element['#value'] == $element['#return_value']) {
    $element['#attributes']['checked'] = 'checked';
  }
  _form_set_class($element, array('form-radio'));
  
  if (isset($element['#parents']) && form_get_error($element) !== NULL && !empty($element['#validated'])) {
    $element['#attributes']['class'][] = 'sg-error';
  }
  
  $output = '<div class="sg-custom-check-and-radio custom-radio">';
  $output .= '<label title="">';
  $output .= '<input' . drupal_attributes($element['#attributes']) . ' />';
  
  
  if (!empty($element['#caption'])) {
    $output .= '<span class="sg-caption">';
    $output .= $element['#caption'];
    $output .= '</span>';
  }
  $output .= '  <div class="sg-alternative-display">';
  $output .= '    <div class="sg-outer"><div class="sg-inner"></div></div>';
  $output .= '  </div>';
  $output .= '</label>';
  $output .= '</div>';
  return $output;
}

/**
 * Returns HTML for the "previous page" link in a query pager.
 *
 * @param $variables
 *   An associative array containing:
 *   - text: The name (or image) of the link.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - interval: The number of pages to move backward when the link is clicked.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager links.
 *
 * @ingroup themeable
 */
function hpbx_theme_pager_previous($variables) {
  $text = $variables['text'];
  $element = $variables['element'];
  $interval = $variables['interval'];
  $parameters = $variables['parameters'];
  global $pager_page_array;
  $output = '';

  // If we are anywhere but the first page
  if ($pager_page_array[$element] > 0) {
    $page_new = pager_load_array($pager_page_array[$element] - $interval, $element, $pager_page_array);

    // If the previous page is the first page, mark the link as such.
    if ($page_new[$element] == 0) {
      $output = hpbx_theme_pager_first(array('text' => $text, 'element' => $element, 'parameters' => $parameters));
    }
    // The previous page is not the first page.
    else {
      $output = hpbx_theme_pager_link(array('text' => $text, 'page_new' => $page_new, 'element' => $element, 'parameters' => $parameters));
    }
  }

  return $output;
}

/**
 * Returns HTML for the "next page" link in a query pager.
 *
 * @param $variables
 *   An associative array containing:
 *   - text: The name (or image) of the link.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - interval: The number of pages to move forward when the link is clicked.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager links.
 *
 * @ingroup themeable
 */
function hpbx_theme_pager_next($variables) {
  $text = $variables['text'];
  $element = $variables['element'];
  $interval = $variables['interval'];
  $parameters = $variables['parameters'];
  global $pager_page_array, $pager_total;
  $output = '';

  // If we are anywhere but the last page
  if ($pager_page_array[$element] < ($pager_total[$element] - 1)) {
    $page_new = pager_load_array($pager_page_array[$element] + $interval, $element, $pager_page_array);
    // If the next page is the last page, mark the link as such.
    if ($page_new[$element] == ($pager_total[$element] - 1)) {
      $output = hpbx_theme_pager_last(array('text' => $text, 'element' => $element, 'parameters' => $parameters));
    }
    // The next page is not the last page.
    else {
      $output = hpbx_theme_pager_link(array('text' => $text, 'page_new' => $page_new, 'element' => $element, 'parameters' => $parameters));
    }
  }

  return $output;
}


function hpbx_theme_pager_last($variables) {
  $text = $variables['text'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  global $pager_page_array, $pager_total;
  $output = '';

  // If we are anywhere but the last page
  if ($pager_page_array[$element] < ($pager_total[$element] - 1)) {
    $output = hpbx_theme_pager_link(array('text' => $text, 'page_new' => pager_load_array($pager_total[$element] - 1, $element, $pager_page_array), 'element' => $element, 'parameters' => $parameters));
  }
  else {
    $output = '<span class="hpbx-pagination-item-content LGI-iconb-end-bold"></span>';
  }

  return $output;
}

function hpbx_theme_pager_first($variables) {

  $text = $variables['text'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  global $pager_page_array;
  $output = '';

  // If we are anywhere but the first page
  if ($pager_page_array[$element] > 0) {
    
    $output = hpbx_theme_pager_link(array('text' => $text, 'page_new' => pager_load_array(0, $element, $pager_page_array), 'element' => $element, 'parameters' => $parameters));
  }
  else {
    $output = '<span class="hpbx-pagination-item-content LGI-iconb-begin-bold"></span>';
  }

  return $output;
}

/**
 * Returns HTML for a link to a specific query result page.
 *
 * @param $variables
 *   An associative array containing:
 *   - text: The link text. Also used to figure out the title attribute of the
 *     link, if it is not provided in $variables['attributes']['title']; in
 *     this case, $variables['text'] must be one of the standard pager link
 *     text strings that would be generated by the pager theme functions, such
 *     as a number or t('« first').
 *   - page_new: The first result to display on the linked page.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager link.
 *   - attributes: An associative array of HTML attributes to apply to the
 *     pager link.
 *
 * @see theme_pager()
 *
 * @ingroup themeable
 */
function hpbx_theme_pager_link($variables) {

  $text = $variables['text'];
  $page_new = $variables['page_new'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  $attributes = $variables['attributes'];

  $attributes['class'][] = 'hpbx-pagination-item-content';
  
  $page = isset($_GET['page']) ? $_GET['page'] : '';
  if ($new_page = implode(',', pager_load_array($page_new[$element], $element, explode(',', $page)))) {
    $parameters['page'] = $new_page;
  }

  $query = array();
  if (count($parameters)) {
    $query = drupal_get_query_parameters($parameters, array());
  }
  if ($query_pager = pager_get_query_parameters()) {
    $query = array_merge($query, $query_pager);
  }

  // Set each pager link title
  if (!isset($attributes['title'])) {
    static $titles = NULL;
    if (!isset($titles)) {
      $titles = array(
        t('« first') => t('Go to first page'),
        t('‹ previous') => t('Go to previous page'),
        t('next ›') => t('Go to next page'),
        t('last »') => t('Go to last page'),
      );
    }
    if (isset($titles[$text])) {
      $attributes['title'] = $titles[$text];
    }
    elseif (is_numeric($text)) {
      $attributes['title'] = t('Go to page @number', array('@number' => $text));
    }
  }

  // @todo l() cannot be used here, since it adds an 'active' class based on the
  //   path only (which is always the current path for pager links). Apparently,
  //   none of the pager links is active at any time - but it should still be
  //   possible to use l() here.
  // @see http://drupal.org/node/1410574
  $attributes['href'] = url($_GET['q'], array('query' => $query));
  return '<a' . drupal_attributes($attributes) . '>' . $text . '</a>';
}

/**
 * Returns HTML for a query pager.
 *
 * Menu callbacks that display paged query results should call theme('pager') to
 * retrieve a pager control so that users can view other results. Format a list
 * of nearby pages with additional query results.
 *
 * @param $variables
 *   An associative array containing:
 *   - tags: An array of labels for the controls in the pager.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager links.
 *   - quantity: The number of pages in the list.
 *
 * @ingroup themeable
 */
function hpbx_theme_pager($variables) {
  
  $tags = $variables['tags'];
  $element = $variables['element'];
  $parameters = $variables['parameters'];
  $quantity = $variables['quantity'];
  global $pager_page_array, $pager_total;

  // Calculate various markers within this pager piece:
  // Middle is used to "center" pages around the current page.
  $pager_middle = ceil($quantity / 2);
  // current is the page we are currently paged to
  $pager_current = $pager_page_array[$element] + 1;
  // first is the first page listed by this pager piece (re quantity)
  $pager_first = $pager_current - $pager_middle + 1;
  // last is the last page listed by this pager piece (re quantity)
  $pager_last = $pager_current + $quantity - $pager_middle;
  // max is the maximum page number
  $pager_max = $pager_total[$element];
  // End of marker calculations.

  // Prepare for generation loop.
  $i = $pager_first;
  if ($pager_last > $pager_max) {
    // Adjust "center" if at end of query.
    $i = $i + ($pager_max - $pager_last);
    $pager_last = $pager_max;
  }
  if ($i <= 0) {
    // Adjust "center" if at start of query.
    $pager_last = $pager_last + (1 - $i);
    $i = 1;
  }
  // End of generation loop preparation.

  $li_first = theme('pager_first', array('text' => (isset($tags[0]) ? $tags[0] : '<span class="hpbx-pagination-item-content LGI-iconb-begin-bold"></span>'), 'element' => $element, 'parameters' => $parameters));
  
  //$li_previous = theme('pager_previous', array('text' => (isset($tags[1]) ? $tags[1] : ($i-1)), 'element' => $element, 'interval' => 1, 'parameters' => $parameters));
  //$li_next = theme('pager_next', array('text' => (isset($tags[3]) ? $tags[3] :  ($i+1)), 'element' => $element, 'interval' => 1, 'parameters' => $parameters));
  $li_last = theme('pager_last', array('text' => (isset($tags[4]) ? $tags[4] : '<span class="LGI-iconb-end-bold"></span>'), 'element' => $element, 'parameters' => $parameters));

  if ($pager_total[$element] > 1) {
    if ($li_first) {
      
      if ($pager_current == 1) {
        $items[] = array(
          'class' => array('hpbx-pagination-item hpbx-pagination-item-start hpbx-pagination-item-unavailable'),
          'data' => $li_first,
        );
      }
      else {
        $items[] = array(
          'class' => array('hpbx-pagination-item hpbx-pagination-item-start'),
          'data' => $li_first,
        );
      }
    }
    
    if ($li_previous) {
      $items[] = array(
          'class' => array('hpbx-pagination-item-before-current'),
          'data' => $li_previous,
      );
    }

    // When there is more than one page, create the pager list.
    if ($i != $pager_max) {
      if ($i > 1) {
        //$items[] = array(
        //    'class' => array('pager-ellipsis'),
        //    'data' => '…',
        //);
      }
      // Now generate the actual pager piece.
      for (; $i <= $pager_last && $i <= $pager_max; $i++) {
        if ($i<$pager_current-1 || $i > $pager_current +3) {
          continue;
        }
        if ($i < $pager_current) {
          $items[] = array(
              'class' => array('hpbx-pagination-item hpbx-pagination-item-before-current'),
              'data' => theme('pager_previous', array('text' => $i, 'element' => $element, 'interval' => ($pager_current - $i), 'parameters' => $parameters)),
          );
        }
        if ($i == $pager_current) {
          $items[] = array(
              'class' => array('hpbx-pagination-item hpbx-pagination-item-current'),
              'data' => '<span class="hpbx-pagination-item-content">'. $i . '</span>',
          );
        }
        if ($i > $pager_current) {
          $items[] = array(
              'class' => array('hpbx-pagination-item hpbx-pagination-item-after-current'),
              'data' => theme('pager_next', array('text' => $i, 'element' => $element, 'interval' => ($i - $pager_current), 'parameters' => $parameters)),
          );
        }
      }
      if ($i < $pager_max) {
        /*
        $items[] = array(
            'class' => array('pager-ellipsis'),
            'data' => '…',
        );
        */
      }
    }
    // End generation.
    if ($li_next) {
      $items[] = array(
          'class' => array('hpbx-pagination-item'),
          'data' => $li_next,
      );
    }
    if ($li_last) {
      $items[] = array(
          'class' => array('hpbx-pagination-item hpbx-pagination-item-end'),
          'data' => $li_last,
      );
    }
    
    $attributes = array('class' => array('hpbx-pagination'));
        
    if (!empty($variables['data-pagination-id'])) {
      $attributes['data-pagination-id'] = $variables['data-pagination-id'];
    }
    
    return '<div class="row"><div class="col-xs-12"><div class="hpbx-pagination-container">'. theme('item_list', array(
        'items' => $items,
        'attributes' => $attributes,
    )) . '</div></div></div>';
  }
}


function hpbx_theme_table($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  $attributes = $variables['attributes'];
  $caption = $variables['caption'];
  $colgroups = $variables['colgroups'];
  $sticky = $variables['sticky'];
  $empty = $variables['empty'];

  $output = '';
  if (isset($caption)) {
    $output .= '<div '. (!empty($caption['attributes']) ? drupal_attributes($caption['attributes']) : '').'>'. $caption['data'] . '</div>';
  }

  $output .= '<table' . drupal_attributes($attributes) . ">\n";

  // Format the table columns:
  if (count($colgroups)) {
    foreach ($colgroups as $number => $colgroup) {
      $attributes = array();

      // Check if we're dealing with a simple or complex column
      if (isset($colgroup['data'])) {
        foreach ($colgroup as $key => $value) {
          if ($key == 'data') {
            $cols = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $cols = $colgroup;
      }

      // Build colgroup
      if (is_array($cols) && count($cols)) {
        $output .= ' <colgroup' . drupal_attributes($attributes) . '>';
        $i = 0;
        foreach ($cols as $col) {
          $output .= ' <col' . drupal_attributes($col) . ' />';
        }
        $output .= " </colgroup>\n";
      }
      else {
        $output .= ' <colgroup' . drupal_attributes($attributes) . " />\n";
      }
    }
  }

  // Add the 'empty' row message if available.
  if (!count($rows) && $empty) {
    $header_count = 0;
    foreach ($header as $header_cell) {
      if (is_array($header_cell)) {
        $header_count += isset($header_cell['colspan']) ? $header_cell['colspan'] : 1;
      }
      else {
        $header_count++;
      }
    }
    $rows[] = array(array('data' => $empty, 'colspan' => $header_count, 'class' => array('empty', 'message')));
  }

  // Format the table header:
  if (count($header)) {
    $ts = tablesort_init($header);
    // HTML requires that the thead tag has tr tags in it followed by tbody
    // tags. Using ternary operator to check and see if we have any rows.
    $output .= (count($rows) ? ' <thead class="sg-text-small"><tr>' : ' <tr>');
    foreach ($header as $cell) {
      $cell = tablesort_header($cell, $header, $ts);
      $output .= _theme_table_cell($cell, TRUE);
    }
    // Using ternary operator to close the tags based on whether or not there are rows
    $output .= (count($rows) ? " </tr></thead>\n" : "</tr>\n");
  }
  else {
    $ts = array();
  }

  // Format the table rows:
  if (count($rows)) {
    $output .= "<tbody>\n";
    $flip = array('even' => 'odd', 'odd' => 'even');
    $class = 'even';
    foreach ($rows as $number => $row) {
      // Check if we're dealing with a simple or complex row
      if (isset($row['data'])) {
        $cells = $row['data'];
        $no_striping = isset($row['no_striping']) ? $row['no_striping'] : FALSE;

        // Set the attributes array and exclude 'data' and 'no_striping'.
        $attributes = $row;
        unset($attributes['data']);
        unset($attributes['no_striping']);
      }
      else {
        $cells = $row;
        $attributes = array();
        $no_striping = FALSE;
      }
      if (count($cells)) {
        // Add odd/even class
        if (!$no_striping) {
          $class = $flip[$class];
          $attributes['class'][] = $class;
        }

        // Build row
        $output .= ' <tr' . drupal_attributes($attributes) . '>';
        $i = 0;
        foreach ($cells as $cell) {
          $cell = tablesort_cell($cell, $header, $ts, $i++);
          
          $output .= _theme_table_cell($cell);
        }
        $output .= " </tr>\n";
      }
    }
    $output .= "</tbody>\n";
  }

  $output .= "</table>\n";
  return $output;
}

function hpbx_theme_language_list() {
  $languages = array();
  global $language,$base_url;
  
  $default = language_default();
  
  $query = '';
  if (!empty($_GET['pass-reset-token'])) {
    $query = '?pass-reset-token='. $_GET['pass-reset-token'];
  }

  // Retrieve all languages in use.
  $language_list = language_list();
  
  if (function_exists('domain_locale_get_domain_languages') ) {

    // Retrieve languages assigned to active domain.
    $domain_languages = domain_locale_get_domain_languages();
    
    if (count($domain_languages)) {
      $language_list = array_intersect_key($language_list, $domain_languages);
    }
  }

  // In case only 1 language is active, do not show the menu item.
  if (count($language_list) <= 1) {
    return '';
  } 
  foreach ($language_list  as $key => $lang) {
  
    $path = $base_url  . '/'. $key . '/'. $_GET['q'] . $query;

    if ($lang->enabled == '1') {
  
      $attributes = array();
      $attributes['class'] = array('hpbx-menu-item hpbx-menu-item-sub');
      
      if ($key == $language->language) {
        $attributes['class'][] = 'hpbx-menu-item-language';
      }
      if ($key == $language->language) {
        $attributes['data-item-id'] = 'language';
      }
      else {
        $attributes['data-parent-id'] = 'language';
      }
  
      $attributes = drupal_attributes($attributes);
      $languages[$key] = <<<EOF
        <li {$attributes}>
        <a href="{$path}" class="hpbx-menu-item-link">
        <div class="hpbx-menu-item-icon-container">
EOF;
      if ($key == $language->language) {
        $languages[$key] .= '<div class="hpbx-menu-item-icon"></div>';
      }
      else {
        $languages[$key] .= '<div class="hpbx-menu-item-icon">'. drupal_strtoupper($lang->language) . '</div>';
      }

$languages[$key] .= <<<EOF
        </div>
        <div class="hpbx-menu-item-title-container">
        <span class="hpbx-menu-item-title">{$lang->native}</span>
        </div>
EOF;

      if ($key == $language->language) {
        $languages[$key] .= <<<EOF
        <div class="hpbx-menu-item-handle-container">
        <div class="hpbx-menu-item-handle"></div>
        </div>
EOF;
      }

$languages[$key] .= <<<EOF
        </a>
        </li>
EOF;
    }
  }

  // Prepend current language to the beginning of the array.
  array_unshift($languages, $languages[$language->language]);
  unset($languages[$language->language]);

  return implode('', $languages);
}

function hpbx_theme_menu_link($variables) {
  $help_item = FALSE;
  // In case the current logged in user is still an pilot user, we will not display the 'personal settings' and 'voicemail'
  if (($variables['element']['#href'] == 'hpbx/settings' || $variables['element']['#href'] == 'hpbx/voicemails')) {
    $Subscriber = hpbx_get_active_subscriber();
    if ($Subscriber->is_pbx_pilot) {
      return '';
    }
  }
  elseif (($variables['element']['#href'] == 'hpbx/subscriber/bulk' || $variables['element']['#href'] == 'hpbx/wizard/employee')
      && (arg(1) != 'dashboard' && arg(1) != 'subscribers' && arg(1)!='subscriber')) {
    return '';
  }

  $variables['element']['#localized_options']['html'] = TRUE;
  $variables['element']['#localized_options']['attributes']['class'] = array('hpbx-menu-item-link');
  
  if ($variables['element']['#href'] == 'hpbx/help') {
    $variables['element']['#attributes']['class'][] = 'hpbx-menu-item hpbx-menu-item-help hpbx-menu-item-sub';
    $variables['element']['#localized_options']['attributes']['class'][] = 'hpbx-menu-help-open-handle';
    $variables['element']['#localized_options']['fragment'] = FALSE;
    $variables['element']['#localized_options']['language'] = FALSE;
    $variables['element']['#href'] = '';
    $help_item = TRUE;
  }
  
  if (!empty($_GET['pass-reset-token'])) {
    $variables['element']['#localized_options']['query'] = drupal_get_query_parameters();
  }
  
  
  $title = '<div class="hpbx-menu-item-icon-container"><div class="hpbx-menu-item-icon"></div>';
  
  if (isset($variables['element']['#icon_suffix'])) {
    $title .= $variables['element']['#icon_suffix'];
  }
  
  $title .= '</div><div class="hpbx-menu-item-title-container">
  <span class="hpbx-menu-item-title">'. $variables['element']['#title'] . '</span>
</div>';
  
  if ($help_item) {
    // Help item 1; Redirect to the main help page.
    $var_lo1 = $variables['element']['#localized_options'];
    $var_lo1['attributes']['data-help-url'] = url('hpbx/help/home');
    $var_lo1['attributes']['class'][] = 'hpbx-hidden-xxs';
    $output = l($title, $variables['element']['#href'], $var_lo1);
    
    // Help item 2; Redirect to the page configured help page.
    $help_url = hpbx_help_get_help_url();
    if (empty($help_url)) {
      $help_url = url('hpbx/help/home');
    }
    
    $var_lo2 = $variables['element']['#localized_options'];
    $var_lo2['attributes']['data-help-url'] = $help_url;
    $var_lo2['attributes']['class'][] = 'hpbx-visible-xxs-inline';
    $output .= l($title, $variables['element']['#href'], $var_lo2);
  }
  else {
    $output = l($title, $variables['element']['#href'], $variables['element']['#localized_options']);
  }
  
  $output = str_replace('/#', '#', $output);
  
  $variables['element']['#attributes']['class'][] = 'hpbx-menu-item';
  
  if (arg(0) . '/'. arg(1) == substr($variables['element']['#href'], 0, -1)) {
    $variables['element']['#attributes']['class'][] = 'active-trail';
  }
  
  // @todo remove after Trimm fix.
  if (in_array('active-trail', $variables['element']['#attributes']['class'])) {
    $variables['element']['#attributes']['class'][] = 'hpbx-menu-item-active';
  }
  
  $ret = '';
  if (isset($variables['element']['#prefix'])) {
    $ret .= $variables['element']['#prefix'];
  }
  $ret .= '<li' . drupal_attributes($variables['element']['#attributes']) . '>' . $output  . "</li>\n";
  return $ret;
}

