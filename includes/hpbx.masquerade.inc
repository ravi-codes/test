<?php


function hpbx_masquerade_return() {
  global $user;

  // Load the user.
  if ($account = user_load($_SESSION['ccs']['hpbx']['masquerading_from'])) {

    $destination = $_SESSION['ccs']['hpbx']['masquerading_from_destination'];

    unset($_SESSION['ccs']['hpbx']['masquerading_from']);
    unset($_SESSION['ccs']['hpbx']['masquerading_from_destination']);

    // Call logout hooks when switching from masquerading user.
    module_invoke_all('user_logout', $user);
    drupal_session_regenerate();

    $user = $account;

    // Call all login hooks when switching back to original user.
    $edit = array(); // Passed by reference.
    user_module_invoke('login', $edit, $user);


    if (user_access('hpbx view reseller')) {
      drupal_goto($destination);
    }
    else {
      // Redirect.
      drupal_goto('hpbx/customers');
    }
  }
}

function hpbx_masquerade_page($customer_id, $reseller_id = NULL, $subscriber_id = NULL) {
  global $user;

  if (!is_null($reseller_id)) {

    // Temporary set the reseller_id.
    sipwise_api_activate_reseller($reseller_id);
  }

  // No subscriber_id passed.
  if (is_null($subscriber_id)) {
    $query = array(
      'customer_id' => $customer_id,
      'is_admin' => 1
    );
    $subscribers = sipwise_api_get_all('subscribers', $query);
  }

  // Subscriber_id passed. Try to load this specific user.
  else {
    $query = array(
      'customer_id' => $customer_id,
      'subscriber_id' => (int)$subscriber_id,
    );

    $subscribers = array(
      $subscriber_id => SipwiseEntity::load('subscribers', (int)$subscriber_id)
    );
  }

  // Get an admin subscriber drupal uid. Doesn't matter which one, as all
  // admins have the same functionality.
  foreach ($subscribers as $Subscriber) {


    if (!empty($Subscriber->webusername)) {

      if ($Subscriber->get_active_directory_entry()) {


        // Load the user.
        if ($account = user_load($Subscriber->webusername)) {

          //if (in_array('HostedPBX Customer Administrator', $account->roles)) {

            // Set the currently loaded uid.
            $_SESSION['ccs']['hpbx']['masquerading_from'] = $user->uid;
            $_SESSION['ccs']['hpbx']['masquerading_from_destination'] = $_SERVER['HTTP_REFERER'];

            // Call logout hooks when switching from original user.
            module_invoke_all('user_logout', $user);
            drupal_session_regenerate();

            // Assign the admin account to the global $user.
            $user = $account;

            // Call all login hooks when switching to masquerading user.
            $edit = array(); // Passed by reference.
            user_module_invoke('login', $edit, $user);

            // Redirect.
            if (in_array('HostedPBX Customer Administrator', $account->roles)) {
              drupal_goto('hpbx/dashboard');
            }
            elseif (in_array('HostedPBX Subscriber', $account->roles)) {
              drupal_goto('hpbx/settings');
            }
            else {
              watchdog('hpbx_masquerade', 'Incorrect role assigned for '. $Subscriber->id . ' with webusername '. $Subscriber->webusername);
            }
        }
        else {
          watchdog('hpbx_masquerade', 'Drupal user not found for Subscriber '. $Subscriber->id . ' with webusername '. $Subscriber->webusername);
        }
      }
      else {
        watchdog('hpbx_masquerade', 'AD Entry not found for Subscriber '. $Subscriber->id . ' with webusername '. $Subscriber->webusername);
      }
    }
    else {
      watchdog('hpbx_masquerade', 'Webusername not found for Subscriber '. $Subscriber->id);
    }
  }

  watchdog('hpbx_masquerade', 'Could not find administrative user for customer '. $customer_id);
  drupal_set_message(t('Could not find administrative user for selected customer'), 'error');

  if (user_access('hpbx view reseller') && !empty($_SERVER['HTTP_REFERER'])) {
    drupal_goto($_SERVER['HTTP_REFERER']);
  }
  else {
    // Redirect.
    drupal_goto('');
  }
  return '';
}
