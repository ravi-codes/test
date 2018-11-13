<?php

/**
 *
 * @param array $form
 * @param array $form_state
 * @param string $customer_id
 *
 * @return multitype:string Ambigous <The, string, unknown, A, Returns, Optional>
 */
function hpbx_defaultforward_edit_form($form, $form_state) {

  global $user, $ActiveReseller;

  // Load the customer.
  $Customer = hpbx_get_active_customer();

  $PilotSubscriber = $Customer->get_pilot_subscriber();

  drupal_set_title(t('Default forward'));

  $form = array(
    '#prefix' => '<div class="hpbx-main-content-inner hpbx-default-forward">',
    '#suffix' => '</div>',
    '#attributes' => array('class' => array('hpbx-tab-form')),
  );
  $form['general']['table'] = array(
    '#type' => 'container',
    '#collapsible' => FALSE,
    '#tree' => TRUE,
    '#title' => t('Settings'),
  );
  // Set the existing id.
  $form['id'] = array(
    '#type' => 'value',
    '#value' => !empty($Customer->id) ? $Customer->id : FALSE,
  );
  // Set the existing id.
  $form['subscriber_id'] = array(
    '#type' => 'value',
    '#value' => !empty($PilotSubscriber->id) ? $PilotSubscriber->id : FALSE,
  );

  $default_value = FALSE;
  
  // Get all subscribers to fill the options.
  $subscriber_options = hpbx_get_subscriber_options($Customer, NULL, FALSE, FALSE, TRUE, TRUE, FALSE, TRUE, FALSE);
  
  if (!empty($PilotSubscriber->id)) {
    $default_value = FALSE;
    if ($CallForwards = SipwiseEntity::load('callforwards', $PilotSubscriber->id)) {
  
      if (isset($CallForwards->cfu->destinations[0]->destination)) {
  
        if (preg_match('/sip:([\w]+)\@/', $CallForwards->cfu->destinations[0]->destination, $matches)) {
  
          foreach (sipwise_api_get_all('subscribers', array('customer_id' => $Customer->id)) as $s) {
  
            if (hpbx_parse_primary_number($s->primary_number, '', FALSE) == $matches[1]) {
              $default_value = $s->id;
              break;
            }
          }
        }
      }
    }
  }
  
  $form['general']['cfu']['value'] = array(
    '#type' => 'select',
    '#options' => $subscriber_options,
    '#required' => FALSE,
    '#default_value' => $default_value,
    '#title' => t('Forward all numbers not in use to'),
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search'), 'data-placeholder' => t('Choose an option')),
    '#field_size_class' => array('col-xs-12', 'col-md-8'),
    '#empty_option' => t('Choose an option'),
  );
  
  // Submit button.
  $form['general']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('hpbx-main-content-buttons-row')),
    '#prefix' => '<div class="row">',
    '#suffix' => '</div>'
  );
  $form['general']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#prefix' => '<span class="col-xs-12">',
    '#suffix' => '</span>',
    '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  return $form;
}

/**
 *
 * @param unknown $form
 * @param unknown $form_state
 * @param string $customer_id
 */
function hpbx_defaultforward_edit_form_validate($form, &$form_state) {

}


/**
 *
 * @param unknown $form
 * @param unknown $form_state
 *
 * @param string $customer_id
 */
function hpbx_defaultforward_edit_form_submit($form, $form_state) {

  global $ActiveReseller;
  global $user;

  try {

    // Load existing customer.
    $Customer = SipwiseEntity::load('customers', $form_state['values']['id']);
    
      // Load existing pilot subscriber.
    $PilotSubscriber = SipwiseEntity::load('subscribers', $form_state['values']['subscriber_id']);

    // Load pilot subscriber preferences.
    $PilotSubscriberPreferences = SipwiseEntity::load('subscriberpreferences', $PilotSubscriber->id);
    

    // Load CallForwards.
    $Callforwards = SipwiseEntity::load('callforwards', $PilotSubscriber->id);
    
    // Reset cfu.
    if (!empty($form_state['values']['value']) && ($CFSubscriber = SipwiseEntity::load('subscribers', (int)$form_state['values']['value']))) {
      
      $form_state['values']['call-forwards']['cfu'] = array(
        'destination_type' => 'uri',
        'destination_timeout' => 300,
        'destination_uri' => $CFSubscriber->pbx_extension
      );
    
      hpbx_set_simple_callforward_values('cfu', $form_state, $Callforwards, $PilotSubscriber);
    }
    else {
      // Reset.
      $Callforwards->cfu = new stdClass();
    }
    $Callforwards->save();
  }
  catch (Exception $e) {

    // Failed to create customer.
    drupal_set_message(t('Failed to store default forward '), 'error');
    drupal_set_message(t('Exception:'. $e->getMessage() .' in '. $e->getFile() .' at line '. $e->getLine()), 'error');
    $failed = TRUE;
  }

  if (!isset($failed)) {
    drupal_set_message(t('Default forward successfully saved'));
  }
}
