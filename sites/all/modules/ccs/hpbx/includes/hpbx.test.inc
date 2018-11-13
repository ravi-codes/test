<?php
function hpbx_test_comp_db_ldap(){
// Get all drupal user which has email id.
    $query = db_select('users', 'u')
            ->fields('u', array('uid', 'mail','name'));
            // ->range(arg(4),arg(5));
     $result = $query->execute();

    $users_data = $result->fetchAll();

    $ldap_data = [];
    foreach ($users_data as $key => $user_data) {
      $ldap = $ldap_name = '';
      $ldap = ccs_ldap_get_user_on_mail_address($user_data->mail);
      $ldap_name= ccs_ldap_user_loopup($user_data->name);
      $sam_ac_name = $ldap_name['attr']['samaccountname'][0];
      if (($sam_ac_name == 1 || empty($ldap)) && ($user_data->uid != 1 && $user_data->uid != 0)) {
        $ldap_data[] = array(
          'uid' => $user_data->uid,
          'mail' => $user_data->mail,
          'ldap_data' => (!empty($ldap)) ? $ldap : NULL,
          'ldap_data_ac' => ($sam_ac_name == 1) ? $sam_ac_name : NULL,
        );
        //  Drupal User Delete
        user_delete($user_data->uid);
      }
    }
    echo '<pre>' . print_r($ldap_data , 1) . '</pre>';
    // echo '<h4>Last user ID:'.$user_data->uid.'</h4>';
    exit;
}
