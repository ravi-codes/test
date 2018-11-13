<?php

class StrettoUser extends Stretto {
	
  /**
   * Check an Stretto connectivity in API
   * @param string $table
   *
   * @return array
   */
	
   public function stretto_connect($table){
    
      $response = Stretto::exec('/stretto/prov/'.$table);
  	  return $response;  	
    }
	
  /**
   * Create an entity in Stretto API
   * @param array $query
   * @param string $table
   * @param array $data
   *
   * @return array
   */	
	 
   public function create($query, $table, $customrequest, $data=NULL){
      
     $result = Stretto::exec('/stretto/prov/'.$table.'?method='.$customrequest.'&'.http_build_query($query),$data);
     return $result;
     
   }	
	
   /**
    * Get all entities from Stretto API
    * @param string $table
    * @param string $attribute
    * @param string $value
    *
    * @return array 
    */
	 
  public function stretto_get_all($table, $attribute, $value){
	  
    $response = Stretto::exec('/stretto/prov/'.$table.'?method=GET&'.$attribute.'='.$value);
	  
    if($table == 'usergroup'){
        
      foreach($response->CcsUserGroup as $groups){
        $group = json_decode(json_encode($groups),true);
  	$result[$group["@attributes"]['groupName']] = $group["@attributes"]['groupName'];
      }
    }
    elseif($table == 'profile'){
	    
      foreach($response->CcsProfile as $CcsProfile){
        $profiles = json_decode(json_encode($CcsProfile),true);
  	$result[$profiles["@attributes"]['profileName']] = $profiles["@attributes"]['profileName'];
      }
    }
	  
    return $result;
  }
    
  /**
   * Get particular entity from Stretto
   * @param string $table
   * @param array $query
   *
   * @return array
   */
	 
  public function stretto_get($table, $query){
    $response = Stretto::exec('/stretto/prov/'.$table.'?method=GET&'.http_build_query($query));
	return $response;  	
  }
  
  /**
   * Delete an entity in Stretto
   * @param string $table
   * @param array $query
   * 
   * @return string
   */
   
   public function stretto_delete($table, $query){
     $response = Stretto::exec('/stretto/prov/'.$table.'?method=DELETE&'.http_build_query($query));
     return $response;
   }
   	
}