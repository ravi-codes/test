<?php
function hpbx_get_stretto_profiles_callback() { 
  if(!empty($_GET['groupname'])){
  	$groupname =  $_GET['groupname'];  
    $obj = new StrettoUser();
    $result = $obj->stretto_get_all('profile', 'groupName', $groupname);  
    drupal_json_output($result);
  }  
}