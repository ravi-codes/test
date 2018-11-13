<?php

function hpbx_dashboard_page() {
  global $ActiveReseller,$language;

  require_once(drupal_get_path('module', 'hpbx') .'/includes/hpbx.pbxdevice.inc');

  $Customer = hpbx_get_active_customer();

  // Count type of subscribers.
  $subscribers = $Customer->get_subscribers(FALSE, TRUE, TRUE, TRUE, TRUE);
  $sub_counter = array();
  foreach ($subscribers as $Subscriber) {
    if ($Subscriber->is_auto_attendant()) $sub_counter[] = t('Auto attendants');
    elseif ($Subscriber->is_conference_room()) $sub_counter[] = t('Conference rooms');
    elseif ($Subscriber->is_huntgroup()) $sub_counter[] = t('Hunt groups');
    elseif ($Subscriber->is_fax_number() && hpbx_fax_access_callback()) $sub_counter[] = t('Fax numbers');
    elseif ($Subscriber->is_regular()) $sub_counter[] = t('Subscribers');
  }

  $sub_counter = array_count_values($sub_counter);

  $sum = array_sum($sub_counter);
  foreach ($sub_counter as $type => &$val) {
    $val = round (($val / $sum) * 100);
  }

  // Count type of subscribers.
  $devices = sipwise_api_get_all('pbxdevices', array('customer_id' => $Customer->id));

  $dev_counter = array();
  foreach ($devices as $PBXDevice) {
    $PBXDeviceModel = hpbx_pbxdevice_get_model_on_profile($PBXDevice->profile_id);
    $dev_counter[] = $PBXDeviceModel->vendor .' '. $PBXDeviceModel->model;
  }
  $dev_counter = array_count_values($dev_counter);

  $sum = array_sum($dev_counter);
  foreach ($dev_counter as $type => &$val) {
    $val = round (($val / $sum) * 100);
  }

  $form = array();
  $form['#prefix'] = '<div class="hpbx-dashboard-circles"><div class="row">';
  $form['#suffix'] = '</div><div class="hpbx-dashboard-wizard-buttons">
                              <button class="sg-btn sg-btn-wizard" onclick="window.location.href=\''. url('hpbx/wizard/employee') . '\';">'.  t('Start wizard') . '</button>
                                  <button class="sg-btn sg-btn-wizard" onclick="window.location.href=\''. url('hpbx/subscriber/bulk') . '\';">'.  t('Bulk import') . '</button>
                            </div>
                      </div>';

  $free_numbers = $Customer->get_free_numbers();
  if (!count($subscribers)) {
    $percentage = 0;
  }
  elseif (!count($free_numbers)) {
    $percentage = 100;
  }
  else {
    $percentage = (count($subscribers) / (count($subscribers) + count($free_numbers) ))  * 100;
  }

  // Subscribers.
  $variables = array(
    'title' => t('Subscribers'),
    'subtitle' => t('@percentage% of numbers in use', array('@percentage' => round($percentage))),
    'value' => $percentage,
    'percentages' => $sub_counter,
    'inner' => '<span class="LGI-iconb-subscriber-large"></span>',
    'class' => array(),
    'labels' => array_keys($sub_counter),
    'svg_id' => 'hpbx-dashboard-pie-chart-subscribers',
    'percentage_display' => TRUE,
  );

  $form['subscribers'] = array(
    '#type' => 'item',
    '#markup' => theme('hpbx_pie_chart', $variables),
    '#field_size_class' => array('col-xxs-12', 'col-xs-6', 'col-md-3'),
    '#row_wrapper' => FALSE,
  );

  $number = count($devices);

  $subtitle = format_plural($number,
    '@count device',
    '@count devices',
    array('@count' => $number)
  );

  if ($language->language == 'pl') {
    $plural_format = ($number == 1) ? 0 : ((($number % 10 >= 2) && ($number % 10 <= 4) && (($number % 100 < 12) || ($number % 100 > 14))) ? 1 : 2);
    if ($plural_format==2) {
      $subtitle = t('@count devices', array('@count' => $number), array('context' => 'plural2'));
    }
  }

  $variables = array(
    'title' => t('Devices'),
    'subtitle' => $subtitle,
    'value' => count($devices),
    'percentages' => $dev_counter,
    'inner' => '<span class="LGI-iconb-pbx-device-large"></span>',
    'class' => array(),
    'labels' => array_keys($dev_counter),
    'svg_id' => 'hpbx-dashboard-pie-chart-devices',
    'percentage_display' => FALSE,
  );

  $form['pbxdevices'] = array(
    '#type' => 'item',
    '#markup' => theme('hpbx_pie_chart', $variables),
    '#field_size_class' => array('col-xxs-12', 'col-xs-6', 'col-md-3'),
    '#row_wrapper' => FALSE,
  );


  /*
  // Calls.
  $variables = array(
    'title' => t('Calls'),
    'subtitle' => t('@percentage% International', array('@percentage' => 17)),
    'tooltip' => array('30% GER', '15% GB', '10% USA', '45% Other'),
    'value' => 17,
    'percentages' => array(30,15,10,45),
    'inner' => '<span class="LGI-iconb-globe-large"></span>',
    'class' => array(),
    'labels' => array('GER','GB','USA','Other'),
    'svg_id' => 'hpbx-dashboard-pie-chart-calls',
  );
  $form['calls'] = array(
    '#type' => 'item',
    '#markup' => theme('hpbx_pie_chart', $variables),
    '#field_size_class' => array('col-xxs-12', 'col-xs-6', 'col-md-3'),
    '#row_wrapper' => FALSE,
  );
  */
  $form['help'] = array(
    '#type' => 'item',
    '#markup' => '<div class="hpbx-dashboard-info hpbx-hidden-xxs">'. theme('hpbx_dashboard_help') .'</div>',
    '#field_size_class' => array('xxs-12', 'col-xs-6', 'col-md-offset-3', 'col-md-3'),
    '#row_wrapper' => FALSE,
  );

  // Render form.
  $output = drupal_render($form);

  // Render help.
  $help_markup = theme('hpbx_dashboard_help');

  $table_variables = array(
    'attributes' => array(
      'class' => array('hpbx-tabs-inside-content-table', 'hpbx-numbered', 'hpbx-hidden-xxs'),
    ),
    'rows' => array()
  );

  // Render conference rooms.
  $query = array(
    'customer_id' => $Customer->id,
    'profile_id' => $ActiveReseller->_settings->conference_room_subscriber_profile_id
  );

  $data_set = $Customer->get_conference_rooms();
  $table_variables['rows'] = hpbx_dashboard_get_subscriber_rows($data_set);
  $table  = theme('table', $table_variables);
  $conf_markup = $table . '<div class="hpbx-tabs-inside-content-table hpbx-numbered hpbx-visible-xxs">'. hpbx_dashboard_table_to_div($table) . '</div>';

  // Render Hunting groups.
  $data_set = $Customer->get_huntgroups();
  $table_variables['rows'] = hpbx_dashboard_get_subscriber_rows($data_set);
  $table  = theme('table', $table_variables);
  $hg_markup = $table . '<div class="hpbx-tabs-inside-content-table hpbx-numbered hpbx-visible-xxs">'. hpbx_dashboard_table_to_div($table) . '</div>';

  // Render Auto Attendants
  $data_set = $Customer->get_auto_attendants();
  $table_variables['rows'] = hpbx_dashboard_get_subscriber_rows($data_set);
  $table  = theme('table', $table_variables);
  $autoattendant_markup = $table . '<div class="hpbx-tabs-inside-content-table hpbx-numbered hpbx-visible-xxs">'. hpbx_dashboard_table_to_div($table) . '</div>';

  if (hpbx_fax_access_callback()) {

    $data_set = $Customer->get_fax_subscribers();
    $table_variables['rows'] = hpbx_dashboard_get_subscriber_rows($data_set);
    $table  = theme('table', $table_variables);
    $fax_markup = $table . '<div class="hpbx-tabs-inside-content-table hpbx-numbered hpbx-visible-xxs">'. hpbx_dashboard_table_to_div($table) . '</div>';
  }

  $t = get_t();

  $btn_options = array('attributes'  => array('class' => array('sg-btn', 'sg-btn-wizard')));
  $conf_link = l(t('Create conference room'), 'hpbx/conference-room/add', $btn_options);
  $hg_link = l(t('Create Hunt group'), 'hpbx/huntinggroup/add', $btn_options);
  $aa_link = l(t('Create auto attendant'), 'hpbx/auto-attendant/add', $btn_options);
  $fax_link = l(t('Create fax number'), 'hpbx/fax2email/add', $btn_options);

  $query = db_select('node', 'n');
  $query->join('field_data_field_hpbx_block', 'b', 'n.nid = b.entity_id');
  $query->join('field_data_field_hpbx_body', 'bo', 'bo.entity_id = b.field_hpbx_block_value');
  $query->join('field_data_field_hpbx_view_counter', 'c', 'c.entity_id = b.field_hpbx_block_value');
  $query->join('field_data_field_hpbx_type', 't', 't.entity_id = n.nid');
  $query->join('field_data_field_hpbx_reseller_id', 'r', 'r.entity_id = n.nid');

  $query->join('taxonomy_term_data', 'td', 'td.tid = t.field_hpbx_type_tid');

  $query
    ->fields('c', array('field_hpbx_view_counter_value'))
    ->fields('n', array('nid'))
    ->fields('bo', array('field_hpbx_body_value'))
    ->fields('b', array('field_hpbx_block_value'))
    ->condition('c.field_hpbx_view_counter_value', 0, '<>')
    ->condition('n.status', 1)
    ->condition('n.type', 'hpbx_help_article')
    ->condition('td.name', 'F.A.Q.')
    ->condition('n.language', $language->language)
    ->condition('r.field_hpbx_reseller_id_value', $ActiveReseller->id);

  $query
    ->orderBy('field_hpbx_view_counter_value', 'DESC')
    ->range(0, 10);

  // echo $query;
  $result = $query->execute();

  $variables = array();
  $variables['items'] = array();
  $variables['type'] = 'ol';
  $variables['attributes']['class'] = array('hpbx-dashboard-ordered-list');


  foreach ($result as $row) {

    if (preg_match('/<h2>(.+)<\/h2>(.+)/im', trim($row->field_hpbx_body_value), $matches)) {
      $title = strip_tags(trim($matches[1]));

      $options = array();
      $options['attributes']['data-help-url'] = url('node/'. $row->nid, array('query' => array('id' => $row->field_hpbx_block_value)));
      $options['attributes']['class'][] = 'hpbx-menu-help-open-handle';
      $options['fragment'] = FALSE;
      $options['language'] = FALSE;
      $element['#href'] = '';
      $variables['items'][] = str_replace('/#', '#', l($title, '' , $options));
    }
  }
  $faq_markup = theme_item_list($variables);

  if (hpbx_fax_access_callback()) {

    $fax_li_item = <<<EOF
<li class="hpbx-tab-item">
  <a href="#tab-fax-number">
    <span class="hpbx-tab-icon LGI-iconfax"></span>
    <span class="hpbx-tab-title">{$t('Fax numbers')}</span>
  </a>
</li>
EOF;

    $fax_tab = <<<EOF
    <div id="tab-fax-number">
      <h4 class="hpbx-tabs-inside-content-title">{$t('Fax numbers')}</h4>
      {$fax_markup}
      <div class="hpbx-tabs-inside-content-buttons">
        {$fax_link}
      </div>
    </div>
EOF;
  }

  $output .= <<<EOF
</div></div></div></div>

<div class="hpbx-dashboard-middle-section">
  <div class="hpbx-dashboard-middle-content-container">
    <div class="container">
      <div class="row">
        <div class="col-xs-12 hpbx-main-content">
          <div class="row">
            <div class="col-xs-12">
              <div class="hpbx-dashboard-info hpbx-visible-xxs">{$help_markup}</div>
            </div>
          </div>
          <div class="row">
            <div class="col-xs-12">
              <div class="row">
                <div class="col-xs-12 col-md-6">
                  <div class="hpbx-dashboard-faq-container">
                    <h3 class="hpbx-dashboard-info-title">
                      <span class="hpbx-dashboard-info-icon LGI-iconb-help-large"></span> {$t('F.A.Q.')}
                    </h3>
                    {$faq_markup}
                  </div>
                </div>
                <div class="col-xs-12 col-md-6">
                  <div class="hpbx-dashboard-tabs-container">
                    <div class="hpbx-tabs hpbx-tabs-inside-content">
                     <ul>
                        <li class="hpbx-tab-item"><a href="#tab-conference-rooms"><span class="hpbx-tab-icon LGI-iconconference-room"></span><span class="hpbx-tab-title">{$t('Conference rooms')}</span></a></li>
                        <li class="hpbx-tab-item"><a href="#tab-hunting-groups"><span class="hpbx-tab-icon LGI-iconhuntinggroup"></span><span class="hpbx-tab-title">{$t('Hunt groups')}</span></a></li>
                        <li class="hpbx-tab-item"><a href="#tab-auto-attendant"><span class="hpbx-tab-icon LGI-iconauto-attendant"></span><span class="hpbx-tab-title">{$t('Auto attendants')}</span></a></li>
                        {$fax_li_item}
                    </ul>
                    <div id="tab-conference-rooms">
                      <h4 class="hpbx-tabs-inside-content-title">{$t('Conference rooms')}</h4>
                      {$conf_markup}
                      <div class="hpbx-tabs-inside-content-buttons">
                        {$conf_link}
                      </div>
                    </div>

                    <div id="tab-hunting-groups">
                      <h4 class="hpbx-tabs-inside-content-title">{$t('Hunt groups')}</h4>
                      {$hg_markup}
                      <div class="hpbx-tabs-inside-content-buttons">
                        {$hg_link}
                      </div>
                    </div>
                    
                    <div id="tab-auto-attendant">
                      <h4 class="hpbx-tabs-inside-content-title">{$t('Auto attendants')}</h4>
                      {$autoattendant_markup}
                      <div class="hpbx-tabs-inside-content-buttons">
                        {$aa_link}
                      </div>
                    </div>
                    {$fax_tab}
                  </div>
                </div>
              </div>
            </div>
          </div>
      <!--
        </div>
      </div>
    </div>
  </div>-->
EOF;
  return $output;
}

/**
 * Helper function to turn a dashboard table into a div.
 *
 * @param string $table_markup
 * @return string
 */
function hpbx_dashboard_table_to_div($table_markup) {

  // Convert table row/cell to div.
  $div = preg_replace(array('/\<tbody.+[^\>]/', '/\<\/tbody\>/', '/\<table.+[^\>]/', '/\<\/table\>/'), '', $table_markup);
  $div = preg_replace(array('/\<td/', '/\<tr/'), '<div', $div);
  return preg_replace(array('/\<\/td/', '/\<\/tr/'), '</div', $div);
}


/**
 * Helper function to get all table rows for a subscriber type.
 *
 * @param array $query
 *
 * @return array
 */
function hpbx_dashboard_get_subscriber_rows($data_set) {
  global $ActiveReseller;
  $Customer = hpbx_get_active_customer();

  $cell_class = array('hpbx-tabs-inside-content-table-cell');
  $rows = array();
  foreach ($data_set as $Subscriber) {
    if (!empty($Subscriber->external_id)) {
      $row = array();
      $row[] = array('data' => hpbx_truncate_name_element($Subscriber->external_id), 'class' => $cell_class);

      if ((!$Customer->_settings->derive_extension_from_did || hpbx_numberranges_has_prefix($Customer))) {
        $row[] = array('data' => $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN), 'class' => $cell_class);
        $row[] = array('data' => $Subscriber->pbx_extension, 'class' => $cell_class);
      }
      else {
        $number = $Subscriber->get_alias_number(HPBX_NUMBER_FORMAT_GSN);
      	if(isset($number) && $number !=''){
			$number = substr($number, 0, $Customer->_settings->extension_length * -1) . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
		}else{
			$number = "<span style='padding-left:35px'>&nbsp</span>" . '<span class="hpbx-pbx-extension" >' . $Subscriber->pbx_extension . '</span>';
		}
        $row[] = array('data' => $number, 'class' => $cell_class);
      }

      if ($Subscriber->profile_id == $ActiveReseller->_settings->conference_room_subscriber_profile_id &&
        $SubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $Subscriber->id)) {

        if (isset($SubscriberPreferences->conference_pin) && $SubscriberPreferences->conference_pin!='') {
          $conf_pin = $SubscriberPreferences->conference_pin;
        }
        else {
          $conf_pin = '-';
        }
        $row[] = array('data' => $conf_pin, 'class' => $cell_class);

      }
      $rows[] = array('data' => $row, 'class' => array('hpbx-tabs-inside-content-table-row'));
    }
  }
  return $rows;
}

/**
 *
 * @return string
 */
function theme_hpbx_dashboard_help() {
  global $ActiveReseller;

  /*return '
    <h4 class="hpbx-dashboard-info-title">
      <span class="hpbx-dashboard-info-icon LGI-iconb-help-large"></span> '. t('Need help?') . '
    </h4>
    <div class="hpbx-dashboard-info-subtitle">'. $ActiveReseller->_settings->help_name .'</div>
    <div class="hpbx-dashboard-info-content">
      <div class="hpbx-contact-item">
        <span class="hpbx-contact-item-icon LGI-iconcall-log"></span> <a href="tel:'. $ActiveReseller->_settings->help_telnumber .'">'. $ActiveReseller->_settings->help_telnumber . '</a>
      </div>
      <div class="hpbx-contact-item">
        <span class="hpbx-contact-item-icon LGI-iconglobe"></span> <a href="http://'. $ActiveReseller->_settings->help_website .'" target="_blank">'. t('visit website') . '</a>
      </div>
      <div class="hpbx-contact-item">
        <span class="hpbx-contact-item-icon LGI-iconvoicemail"></span> <a href="mailto:'. $ActiveReseller->_settings->help_mail .'">'. t('mail us') . '</a>
      </div>
      <!--
      <div class="hpbx-contact-item">
        <span class="hpbx-contact-item-icon LGI-iconconference-room"></span> <a href="#">'. t('start chat') . '</a>
      </div>
      -->
    </div>';*/

   $output = '
      <h4 class="hpbx-dashboard-info-title">
        <span class="hpbx-dashboard-info-icon LGI-iconb-help-large"></span> '. t("Need help?") . '
      </h4>
      <div class="hpbx-dashboard-info-subtitle">'. $ActiveReseller->_settings->help_name .'</div>
      <div class="hpbx-dashboard-info-content">
        <div class="hpbx-contact-item">
          <span class="hpbx-contact-item-icon LGI-iconcall-log"></span> <a href="tel:'. $ActiveReseller->_settings->help_telnumber .'">'. $ActiveReseller->_settings->help_telnumber . '</a>
        </div>
        <div class="hpbx-contact-item">
          <span class="hpbx-contact-item-icon LGI-iconglobe"></span> <a href="http://'. $ActiveReseller->_settings->help_website .'" target="_blank">'. t('visit website') . '</a>
        </div>';
        if($ActiveReseller->_settings->help_mail === 'No mail support'){
          $output .= 
            '<!--
            <div class="hpbx-contact-item">
              <span class="hpbx-contact-item-icon LGI-iconvoicemail"></span> <a href="mailto:'. $ActiveReseller->_settings->help_mail .'">'. t('mail us') . '</a>
            </div>
            -->';
        }
        else{
          $output .= 
            '<div class="hpbx-contact-item">
              <span class="hpbx-contact-item-icon LGI-iconvoicemail"></span> <a href="mailto:'. $ActiveReseller->_settings->help_mail .'">'. t('mail us') . '</a>
            </div>'; 
        }
        
        /* <div class="hpbx-contact-item">
          <span class="hpbx-contact-item-icon LGI-iconconference-room"></span> <a href="#">'. t('start chat') . '</a>
        </div> */
        
      $output .= '</div>';
      return $output;
}