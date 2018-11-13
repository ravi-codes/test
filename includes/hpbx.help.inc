<?php

function hpbx_help_import_load_file($entity_id, $revision_id, &$files, $delta) {
  // Look for a photo.
  $query = db_select('field_data_field_hpbx_photo', 'b');
  $query->join('file_managed', 'f', 'f.fid = b.field_hpbx_photo_fid');
  $query->fields('f');
  $query->condition('b.entity_id', $entity_id);
  $query->condition('b.revision_id', $revision_id);
  $query->condition('b.deleted', 0);

  if ($file_remote = $query->execute()->fetchAssoc()) {

    if (!empty($file_remote['uri'])) {
	  $remoteURI = strtolower($file_remote['uri']);
      $path = './sites/default/files/Lab/'. rawurlencode((basename($remoteURI)));     
      if ($contents = file_get_contents(urldecode($path))) {
		$filename = str_replace('private://','',$remoteURI);
        $destination = 'private://'. uniqid() . $filename;
        db_set_active();
        if ($file = file_save_data($contents, $destination, FILE_EXISTS_RENAME)) {
          $files[$delta]['fid'] = $file->fid;
        }
        db_set_active('hpbx_node_import');
      }
      else {
        echo 'could not get images for '.$remoteURI;
      }
    }
  }
}

function hpbx_help_load_fc_item_content($fc_item, &$data, &$files) {
  
  // Look for a video url.
  $query = db_select('field_data_field_hpbx_video', 'b');
  $query->fields('b', array('field_hpbx_video_video_url'));
  $query->condition('b.entity_id', $fc_item->field_hpbx_block_value);
  $query->condition('b.revision_id',$fc_item->field_hpbx_block_revision_id);
  $query->condition('b.deleted', 0);
  
  if ($value = $query->execute()->fetchField()) {
    $data['block'][$fc_item->delta]['field'] = 'field_hpbx_video';
    $data['block'][$fc_item->delta]['video_url'] = $value;
  }
  
  // Look for a normal body.
  $query = db_select('field_data_field_hpbx_body', 'b');
  $query->fields('b', array('field_hpbx_body_value'));
  $query->condition('b.entity_id', $fc_item->field_hpbx_block_value);
  $query->condition('b.revision_id',$fc_item->field_hpbx_block_revision_id);
  $query->condition('b.deleted', 0);
  
  if ($value = $query->execute()->fetchField()) {
    $data['block'][$fc_item->delta]['field'] = 'field_hpbx_body';
    $data['block'][$fc_item->delta]['value'] = $value;
  }
  
  hpbx_help_import_load_file($fc_item->field_hpbx_block_value, $fc_item->field_hpbx_block_revision_id, $files, $fc_item->delta);
  
  // Look for photo instruction.
  $query = db_select('field_data_field_hpbx_instruction_photo', 'b');
  $query->fields('b', array('field_hpbx_instruction_photo_value', 'field_hpbx_instruction_photo_revision_id'));
  $query->condition('b.entity_id', $fc_item->field_hpbx_block_value);
  $query->condition('b.revision_id',$fc_item->field_hpbx_block_revision_id);
  $query->condition('b.deleted', 0);
  
  if ($values = $query->execute()->fetchAssoc()) {
    
    // Get the main node.
    $query = db_select('field_collection_item', 'i');
    $query->fields('i');
    $query->condition('i.archived', 0);
    $query->condition('i.item_id', $values['field_hpbx_instruction_photo_value']);
    $query->condition('i.revision_id', $values['field_hpbx_instruction_photo_revision_id']);
    
    $fc_items = $query->execute()->fetchAllAssoc('item_id');
    
    foreach ($fc_items as $item_id => $fc_item_sub) {
      
      // Look for a normal body.
      $query = db_select('field_data_field_hpbx_body', 'b');
      $query->fields('b', array('field_hpbx_body_value'));
      $query->condition('b.entity_id', $fc_item_sub->item_id);
      $query->condition('b.revision_id', $fc_item_sub->revision_id);
      $query->condition('b.deleted', 0);
      
      if ($value = $query->execute()->fetchField()) {
        $data['block'][$fc_item->delta]['field'] = 'field_hpbx_instruction_photo';
        $data['block'][$fc_item->delta]['value'] = $value;
      }

      hpbx_help_import_load_file($fc_item_sub->item_id, $fc_item_sub->revision_id, $files, $fc_item->delta);
    }
  }
}


/**
 * 
 * @param unknown $form
 * @param unknown $form_state
 * @return multitype:
 */
function hpbx_help_node_create_form($form, &$form_state) {
  $form = array();
  /*$form['nid'] = array(
    '#type' => 'textfield',
    '#title' => 'Remote node ID that needs to be imported',
    '#required' => TRUE,
    '#attributes' => array('class' => array('sg-element')),
  );

  $options = array();
  foreach (hpbx_get_resellers() as $Reseller) {
    $options[$Reseller->id] = $Reseller->name;
  }
  $options['3'] = 'VTR';
  $form['reseller_id'] = array(
    '#type' => 'select',
    '#title' => 'Local reseller ID to which the node must be assigned',
    '#required' => TRUE,
    '#options' => $options,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );

  $form['env'] = array(
    '#type' => 'select',
    '#title' => 'Remote environment from which the node needs to be imported',
    '#options' => array(
      'LAB-PE' => 'LAB-PE',
      'DEV-PE' => 'DEV-PE',
    ),
    '#required' => TRUE,
    '#attributes' => array('class' => array('chzn-select', 'sg-element', 'no-search')),
  );*/
  
  // Submit button.
  $form['container']['actions'] = array(
      '#type' => 'actions',
      '#attributes' => array('class' => array('hpbx-tab-form-buttons-row')),
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',
  );
  
  $form['container']['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Import',
      '#prefix' => '<span class="submit-button">',
      '#suffix' => '</span></div>',
      '#attributes' => array('class' => array('sg-btn', 'sg-without-icon', 'sg-btn-primary')),
  );
  
  
  return $form;
}

function hpbx_help_node_create_form_validate($form, &$form_state) {
  
  global $databases;
  
  try {
    // Verify if nid does exist.
    $database_info = array(
      'username' => 'voice_portal',
      'password' => 'voice_portal',
      'host' => 'silala.vtr.cl',
      'database' => 'voice_portal_lg',
      'port' => '3306',
      'driver' => 'mysql',
      'prefix' => 'vp_',
	  'pdo' => array(
            PDO::MYSQL_ATTR_SSL_CA => '/etc/pki/tls/server/ca.crt',
      ),
    );
    
    // Verify if nid does exist.
    /*$database_info = array(
      'username' => 'rgraat',
      'password' => 'z5e6wAxnsYFLebfa',
      'host' => 'mysql-noro-nd4.netops.local',
      'port' => '3306',
      'driver' => 'mysql',
      'prefix' => 'vp_',
    );
    
    if ($form_state['values']['env'] == 'LAB-PE') {
      $database_info['database'] = 'voice_portal_sipwise_lgi_look_feel';
    }
    elseif ($form_state['values']['env'] == 'DEV-PE') {
      $database_info['database'] = 'voice_portal_hpbx_pe_development';
    }
    else {
      form_set_error('', 'invalid environment');
      return;
    }*/
    
    /*Database::addConnectionInfo('hpbx_node_import', 'default', $database_info);

    db_set_active('hpbx_node_import');

    // Get the main node.
    $query = db_select('node', 'n');
    $query->fields('n');
    $query->condition('n.nid', $form_state['values']['nid']);
    $query->condition('n.type', 'hpbx_help_article');
    
    if (!$result = $query->execute()->fetchAssoc()) {
      db_set_active();
      form_set_error('nid', 'failed to retrieve remote node');
      return;
    }*/
  }
  catch (Exception $e) {
    db_set_active();
    form_set_error('nid', $e->getMessage());
    return;
  }
  db_set_active();
}


/**
 * 
 * @param unknown $form
 * @param unknown $form_state
 */
function hpbx_help_node_create_form_submit($form, &$form_state) {

  global $user;
  
  /*$nid_remote = $form_state['values']['nid']; // 615
  $env_remote = $form_state['values']['env']; // lab / dev / prod.*/
      
  try {
    $database_info = array(
      'username' => 'voice_portal',
      'password' => 'voice_portal',
      'host' => 'silala.vtr.cl',
      'database' => 'voice_portal_lg',
      'port' => '3306',
      'driver' => 'mysql',
      'prefix' => 'vp_',
	  'pdo' => array(
            PDO::MYSQL_ATTR_SSL_CA => '/etc/pki/tls/server/ca.crt',
      ),
    );
    
    //Create a temporary table to store old and new id to replace internal links.
    try{
    	db_set_active();
	    $schema['temp_nodeid'] = array(
	        'description' => t ('Temporarily store old and new node id'),
	        'fields' => array(
	           'id' => array(
	               'type' => 'int',
	               'not null' => TRUE,
	           ),
	           'old_nodeId' => array(
	               'type' => 'int',
	               'not null' => TRUE,
	           ),
	           'new_nodeId' => array(
	               'type' => 'int',
	               'not null' => TRUE,
	           )
	        )
	    );
    	db_create_table('temp_nodeid',$schema['temp_nodeid']);
    }catch(Exception $e){
    	echo 'Unable to create temp table, please check if already exists';
    	die;
    }

    // Verify if nid does exist.
    /*$database_info = array(
        'username' => 'rgraat',
        'password' => 'z5e6wAxnsYFLebfa',
        'host' => 'mysql-noro-nd4.netops.local',
        'port' => '3306',
        'driver' => 'mysql',
        'prefix' => 'vp_',
    );
    
    if ($form_state['values']['env'] == 'LAB-PE') {
      $database_info['database'] = 'voice_portal_sipwise_lgi_look_feel';
    }
    elseif ($form_state['values']['env'] == 'DEV-PE') {
      $database_info['database'] = 'voice_portal_hpbx_pe_development';
    }
    elseif ($form_state['values']['env'] == 'PROD-PE') {
      $database_info['database'] = 'voice_portal_hpbx_pe';
    }
    else {
      die('invalid environment');
    }*/

    Database::addConnectionInfo('hpbx_node_import', 'default', $database_info);    
    db_set_active('hpbx_node_import');
    
    // Get the main node.
    $query = db_select('node', 'n');
    $query->fields('n');
    //$query->condition('n.nid', $nid_remote);
    $query->condition('n.type', 'hpbx_help_article');
    //$query->range(501,50);
    $result = $query->execute()->fetchAll();
    $failedNodes=array();
    $id=0;
    foreach($result as $record){
    	sleep(3);
    	$data = array();
  		$data['reseller_id_local']= 3;//$form_state['values']['reseller_id']; // 3.
    	$data['title'] = $record->title;
	    $data['status'] = $record->status;
	    $data['promote'] = $record->promote;
	    $data['language'] = $record->language;
	    
	    // Get the main node.
	    $query = db_select('field_data_field_hpbx_type', 't');
	    $query->join('taxonomy_term_data', 'd', 'd.tid = t.field_hpbx_type_tid');
	    $query->condition('t.entity_id', $record->nid);  
	    $query->fields('d', array('Name'));
	    
	    if ($term_name = $query->execute()->fetchField()) {
	
	      // No get the correct local tid for the retrieved name.
	      db_set_active();
	      if (!$terms = taxonomy_get_term_by_name($term_name, 'hpbx_help_type')) {
	        db_set_active();
	        echo 'failed to retrieve local tid for help type with the name '. $term_name . PHP_EOL;
	        array_push($failedNodes,$record->nid);
	        continue;
	      }
	      $data['hpbx_help_type_tid_local'] = (int)current($terms)->tid;
	      
	      
	      echo 'Var set hpbx_help_type_tid='.  $data['hpbx_help_type_tid_local'] . PHP_EOL;
	    }
	    else {
	      echo 'No remote tid set for hpbx_help_type for node ' .$record->nid. PHP_EOL;
	    }
	    
	    db_set_active('hpbx_node_import');
	    
	     // Get all attached blocks.
	    $query = db_select('field_data_field_hpbx_block', 'b');
	    $query->condition('b.entity_id', $record->nid);
	    $query->join('field_collection_item', 'i', 'i.item_id=b.field_hpbx_block_value AND i.revision_id=field_hpbx_block_revision_id');
	    $query->fields('b');
	    $query->fields('i');
	    $query->condition('b.deleted', 0);
	    $query->condition('i.archived', 0);
	    
	    $fc_items = $query->execute()->fetchAllAssoc('delta');
	    $files = array();
	    foreach ($fc_items as $delta => $fc_item) {
	      hpbx_help_load_fc_item_content($fc_item, $data, $files);
	    }
	    
	    // Point to local db.
	    db_set_active();
	    
	    $node = hpbx_help_create_fc_item($data, $files);
	
	    if (!empty($node->nid)) {
	      $id++;
	      $nid = db_insert('temp_nodeid')
				->fields(array(
				  'id' => $id,	
				  'old_nodeId' => $record->nid,
				  'new_nodeId' => $node->nid,
				  ))
			->execute();
	    }
    }
  }
  catch (Exception $e) {
    db_set_active();
    echo $e->getMessage();
    die();
  }
  $replacedLinks = hpbx_replace_internal_node_links();
  if($replacedLinks){
  	if(empty($failedNodes)){
  		echo 'Nodes been created and internal links been modified successfully';
  	}else{
  		echo "Nodes were created and internal links been modified successfully apart from the ones ".implode(',',$failedNodes);
  	}
  	die;
  }
}

function hpbx_help_create_fc_item($data, $files) {

  ///////////////
  // Create new node.
  $node = new stdClass();
  $node->type = 'hpbx_help_article';
  
  node_object_prepare($node);
  
  // Set simple node values.
  $node->title = $data['title'];
  $node->status = $data['status'];
  $node->promote = $data['promote'];
  $node->language = $data['language'];
  
  // Set type taxonomy term.
  if (isset($data['hpbx_help_type_tid_local'])) {
    $node->field_hpbx_type['und'][0]['tid'] = $data['hpbx_help_type_tid_local'];
  }

  // Set reseller id.
  $node->field_hpbx_reseller_id['und'][0]['value'] = $data['reseller_id_local'];
  
  // Process all posted blocks.
  $fcs = array();
  
  // Verify if the blocks are posted.
  if (isset($data['block'])) {
  
    // Loop through the posted blocks.
    foreach ($data['block'] as $delta => $item) {
  
      // Create entity item.
      $fc_item = entity_create('field_collection_item', array('field_name' => 'field_hpbx_block'));
      watchdog('hpbx_help', 'Created new field_hpbx_block field_collection item');
  
      // Assign block (field collection item) to the node.
      $fc_item->setHostEntity('node', $node);
  
      // Update the field collection item.
      hpbx_help_update_field_collection_item($fc_item, $item, $delta, $files);
  
      // Save the collection item.
      $fc_item->save(TRUE);
  
      // Set delta.
      $fcs[$delta] = array('value' => $fc_item->item_id, 'revision_id' => $fc_item->revision_id);
    }
  }
  
  // Set correct order based on filled array with collection items.
  $node->field_hpbx_block['und'] = $fcs;
  
  // Save the node to apply the order.
  node_save($node);
  
  return $node;
}

function hpbx_replace_internal_node_links(){
  //Point to local db.
  db_set_active();
  
  //Fetch each block content of help articles been created.
  $query = db_select('node', 'n');
  $query->join('field_data_field_hpbx_block', 'b', 'n.nid = b.entity_id');
  $query->join('field_data_field_hpbx_body', 'f', 'b.field_hpbx_block_value = f.entity_id');
  $query->fields('f',array('entity_id','field_hpbx_body_value'));
  $query->condition('n.type', 'hpbx_help_article');
  try{
  	$result = $query->execute()->fetchAll();
  }catch(exception $e){
  	var_dump($e);
  }
  
  //Fetch the old and new id of help articles to replace internal links.
  $query = db_select('temp_nodeid', 't');
  $query->fields('t',array('old_nodeId','new_nodeId'));
  try{
  	$tempNodes = $query->execute()->fetchAll();
  }catch(exception $e){
  	var_dump($e);
  }
  
  /*//Get all old node ids as an array.
  $oldIdGroup =array();  
  foreach($tempNodes as $nodeIds){
  	array_push($oldIdGroup,$nodeIds->old_nodeid);  	
  }
  
  foreach($tempNodes as $nodeIds){
  	if(in_array($nodeIds->new_nodeid,$oldIdGroup)){
  		$query = db_select('node');
  		$query->addExpression('MAX(nid)');
  		echo $max= $query->execute()->fetchField();
  	}
  }*/
  
  //Loop through the contents to replace old node id's of internal links with new node id's been created.
  foreach($result as $record){
  	//Check if the content block has any internal links.
  	$regularExpression='/href=["\']?([^"\'>]+)["\']?/iU'; 
  	$linkCount=preg_match_all($regularExpression, $record->field_hpbx_body_value, $matches);
  	if($linkCount){
	  	sleep(3);
	  	$count=0;
	  	$linksModified =0;
	  	$oldContent = $record->field_hpbx_body_value;
	  	$newContent='';  	
		  	foreach($tempNodes as $nodeIds){
		  		$oldId = (string) '#(?<!\w^[a-z])/node/'.$nodeIds->old_nodeid.'(?!\w)#i';
		  		$newId = (string) '/node/'.$nodeIds->new_nodeid;
		  		$newContent = preg_replace($oldId,$newId,$oldContent,-1,$count);
		  		if($count){
		  			$linksModified++;
		  			$oldContent = $newContent;
		  		}
		  	}  	
		  	if($linksModified){
		  		//Update newly modified content in the table field_data_field_hpbx_body.
		  		$contentDataUpdate = db_update('field_data_field_hpbx_body')
							     ->fields(array(
							      'field_hpbx_body_value' => $newContent,
							      ))
							     ->condition('entity_id',$record->entity_id)
							     ->execute();
							     
				//Update newly modified content in the table field_revision_field_hpbx_body.
				$contentRevisionUpdate = db_update('field_revision_field_hpbx_body')
							     ->fields(array(
							      'field_hpbx_body_value' => $newContent,
							      ))
							     ->condition('entity_id',$record->entity_id)
							     ->execute();					     
		  	}
  }else{
  	continue;
  }
 }
 drupal_flush_all_caches();     
 //$result = db_truncate('temp_nodeid')->execute(); 
 //Drop the temp table been created to replace internal links to nodes within the articles.
 try{
 	db_set_active();
	db_drop_table('temp_nodeid',$schema['temp_nodeid']);
 }catch(Exception $e){
	echo 'Unable to drop table temp_nodeid, please remove manually';
 }
 return TRUE;
}

function hpbx_help_node_replicate_callback($node) {

  // Load entity.
  $entity = array_shift(entity_load('node', array($node->nid)));

  // Duplicate the entity and save the replica.
  if ($nid = replicate_entity('node', $entity)) {

    // Change reseller.
    $entity = array_shift(entity_load('node', array($nid)));

    global $user;

    $account_h = $user;

    // Only reseller of the logged in or 'masquerading from' user.
    if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
      $account_h = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
    }

    if (isset($account_h->data['sipwise']['reseller_id'])) {
      $entity->field_hpbx_reseller_id['und'][0]['value'] = $account_h->data['sipwise']['reseller_id'];

    }

    $entity->title = t('Copy - @title', array('@title' => $entity->title));
    entity_save('node', $entity);

    drupal_goto('node/'. $entity->nid . '/edit');
  }
  else {
    drupal_set_messsage(t('Failed to replicate the help article'), 'error');
  }
}

/**
 * Callback for hpbx/help/count
 */
function hpbx_help_article_counter_callback() {
  if (!empty($_POST['item_id'])) {
    $item_id = (int)$_POST['item_id'];
    
    // Verify if this user - within this session - not already increased the counter for this f.a.q. item.
    if (!is_array($_SESSION['ccs']['hpbx']['hpbx_help_faq_counted_items']) || 
      !in_array($item_id, $_SESSION['ccs']['hpbx']['hpbx_help_faq_counted_items'])) {
      
      $fc_item = current(entity_load('field_collection_item', array($item_id)));
      watchdog('hpbx_help_count', 'Loaded existing field_hpbx_block field_collection item with id ', $item_id);
    
      if (isset($fc_item->field_hpbx_view_counter)) {
        if (!isset($fc_item->field_hpbx_view_counter[LANGUAGE_NONE][0]['value'])) {
          $fc_item->field_hpbx_view_counter[LANGUAGE_NONE][0]['value'] = 0;
        }
        $fc_item->field_hpbx_view_counter[LANGUAGE_NONE][0]['value']++;
        
        $fc_item->save(TRUE);
        
        $_SESSION['ccs']['hpbx']['hpbx_help_faq_counted_items'][] = $item_id;
      }
    }
  }
}

function hpbx_help_delete_confirm_redirect($form, &$form_state) { 
  $form_state['redirect'] = 'hpbx/help';
}

/**
 * Callback for hpbx/help
 */
function hpbx_help_page() {
  drupal_goto('hpbx/help/home');
}

function hpbx_help_get_file_icon_path($file) {
  $prefix = drupal_get_path('theme', 'hpbx_theme') . '/dist/img/help/file-types/';
  global $base_path;
  
  $info = pathinfo($prefix . $file['filename']);
  if (!empty($info['extension']) && file_exists($prefix . $info['extension'] . '.png')) {
    return $base_path . $prefix . $info['extension'] . '.png';
  } 
  return $base_path . $prefix .  'blank.png';
}

function hpbx_help_article_node_form_alter(&$form, &$form_state, $form_id) {
  global $ActiveReseller, $language;
  
  
  $form_state['build_info']['files'] = array(
    'hpbx_help' => drupal_get_path('module', 'hpbx'). '/includes/hpbx.help.inc',
    'node_pages' => drupal_get_path('module', 'node'). '/node.pages.inc'
  );

  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_help.js', array('scope' => 'footer'));
  drupal_add_js(drupal_get_path('theme', 'hpbx_theme') . '/dependencies/wysihtml/dist/wysihtml5x-toolbar.min.js', array('scope' => 'footer'));
  drupal_add_js(drupal_get_path('theme', 'hpbx_theme') . '/dependencies/wysihtml/parser_rules/advanced_and_extended.js', array('scope' => 'footer'));

  drupal_add_css('https://cdnjs.cloudflare.com/ajax/libs/jstree/3.0.9/themes/default/style.min.css', 'external');
  
  $form['title']['#title_display'] = 'none';
  $form['title']['#attributes'] = array('placeholder' => t('Title'));
  
  
  $form['options']['#access'] = TRUE;
  $form['options']['sticky']['#access'] = FALSE;

  $form['#submit'] = array('hpbx_help_article_node_form_submit');
  
  $form['actions']['#prefix'] = '<div class="col-xs-5"><div class="hpbx-help-edit-footer-left">';
  $form['actions']['#suffix'] = '</div></div>';

  // Submit button.
  $form['actions']['submit']['#attributes'] = array('class' => array('sg-btn', 'sg-btn-small', 'sg-btn-primary', 'sg-without-icon'));
  $form['actions']['submit']['#weight'] = 101;

  // Delete button.
  $form['actions']['delete']['#attributes'] = array('class' => array('sg-btn', 'sg-btn-small', 'sg-btn-primary', 'sg-without-icon'));
  $form['actions']['delete']['#weight'] = 103;
  
  $form['field_hpbx_reseller_id']['und']['#title_display'] = 'none';
  $form['field_hpbx_type']['und']['#title_display'] = 'none';
  
  
  global $user;
  
  $account_h = $user;
  // Only reseller of the logged in or 'masquerading from' user.
  if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
    $account_h = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
  }
  
  if (isset($account_h->data['sipwise']['reseller_id'])) {
    foreach ($form['field_hpbx_reseller_id']['und']['#options'] as $opt_value => $option) {
      if ($opt_value != $account_h->data['sipwise']['reseller_id']) {
        unset($form['field_hpbx_reseller_id']['und']['#options'][$opt_value]);
      }
    }
  }
  
  // Default values.
  if (empty($form['nid']['#value'])) {
    if (!empty($ActiveReseller->id)) {
     $form['field_hpbx_reseller_id']['und']['#default_value'][0] = $ActiveReseller->id;
    }
  }
  
  $items = array();
  if (isset($form['#node']->nid)) {
    $paths = hpbx_help_get_assigned_paths($form['#node']->nid);
    foreach ($paths as $item) {
      $items[] = $item;
    }
  }
  $vars = array('items' => $items);
  $form['assigned_pages'] = array('#markup' => theme('item_list', $vars));
}

function hpbx_help_get_assigned_paths($nid) {
  
  $resellers = array(0 => (object)array('name' => 'Liberty Global', 'id' => 0));
  $res = hpbx_get_resellers();
  $resellers = $resellers + $res;
  

  $paths = array();
  
  if ($result = db_select('hpbx_help', 'h')
    ->fields('h')
    ->condition('h.nid', $nid)
    ->execute()
    ->fetchAssoc()) {
      $paths[] = '['. $resellers[$result['reseller_id']]->name . '] ['. strtoupper($result['lang']) .'] '.$result['hpbx_url'];
  }
  return $paths;
}


function theme_hpbx_help_get_sitemap() {
  $language_list = language_list();
  
  $resellers = array(0 => (object)array('name' => 'Liberty Global', 'id' => 0));
  $res = hpbx_get_resellers();
  
  $resellers = $resellers + $res;
  
  
  global $user;
  $account_h = $user;
  // Only reseller of the logged in or 'masquerading from' user.
  if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
    $account_h = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
  }
  
  
  $nodes = array();
  
  // Retrieve all languages in use.
  $language_list = language_list();
  
  if (function_exists('domain_locale_get_domain_languages') ) {
  
    // Retrieve languages assigned to active domain.
    $domain_languages = domain_locale_get_domain_languages();
  
    if (count($domain_languages)) {
      $language_list = array_intersect_key($language_list, $domain_languages);
    }
  }
  
  foreach ($language_list as $key => $lang) {
    $n = node_load_multiple(array(), array('language' => $key, 'type' => 'hpbx_help_article'));
    foreach ($n as $no) {
      $nodes[$no->field_hpbx_reseller_id['und'][0]['value']][$key][] = $no;
    }
  }
  
  $output = '';
  foreach ($resellers as $Reseller) {
    $output .= '<ul>';
    $output .= '  <li>';
    $output .=        $Reseller->name;
    foreach ($language_list  as $key => $lang) {
      if ($key == 'dm') continue;
      
      $output .= '<ul>';
      $output .= '  <li>'. strtoupper($key);
      $output .= '    <ul>';
      
      if (isset($nodes[$Reseller->id][$key])) {
        
        foreach ($nodes[$Reseller->id][$key] as $nid => $node) {
          
          $attributes = array(
            'data-edit-url' => url(base_path() . $key . '/node/'. $node->nid . '/edit', array('language' => FALSE)),
            'data-replicate-url' => url(base_path() . $key . '/hpbx/help/node/'. $node->nid . '/replicate', array('language' => FALSE)),  
            'data-view-url' => url(base_path() . $key . '/node/'. $node->nid, array('language' => FALSE)),
            'href' => '',
            'data-disabled' => (int)!node_access('update', $node, $account_h),
          );

          $output .= '  <li><a '. drupal_attributes($attributes) .'>'. $node->title . '</a></li>';
        }
      }
      $output .= '    </ul>';
      $output .= '  </li>';
      $output .= '</ul>';
    }
    
    $output .= '</li>';
    $output .= '</ul>';
  
    
  }
  return $output;
}
                 

/**
 * Helper function to update the field_collection item.
 *
 * @param unknown $fc_item
 * @param unknown $item
 * @param unknown $delta
 * @param unknown $files
 */
function hpbx_help_update_field_collection_item(&$fc_item, $item, $delta, $files = array()) {

  static $depth = 0;
  watchdog('hpbx_help', 'Update '. $item['field'] . ' on field_collection_item '. $fc_item->item_id . '=> ' . 'depth: '. $depth);
  
  // What kind of block?
  switch ($item['field']) {
    case 'field_hpbx_video':
    
      $old_value = '';
      if (!empty($fc_item->field_hpbx_video[LANGUAGE_NONE][0]['video_url'])) {
        $old_value = $fc_item->field_hpbx_video[LANGUAGE_NONE][0]['video_url'];
      }
      watchdog('hpbx_help', 'update '. $item['field'] . ' with video_url "'. $old_value . '" to new video_url "'. $item['video_url'] . '"');
    
      // Set the new value. Single value, so delta 0.
      $fc_item->field_hpbx_video[LANGUAGE_NONE][0]['video_url'] = $item['video_url'];
      break;
    case 'field_hpbx_body':
      
      $old_value = '';
      if (!empty($fc_item->field_hpbx_body[LANGUAGE_NONE][0]['value'])) {
        $old_value = $fc_item->field_hpbx_body[LANGUAGE_NONE][0]['value'];
      }
      watchdog('hpbx_help', 'update '. $item['field'] . ' with value "'. $old_value . '" to new value "'. $item['value'] . '"');
      
      // Set the new value. Single value, so delta 0.
      $fc_item->field_hpbx_body[LANGUAGE_NONE][0]['value'] = $item['value'];
      break;
    case 'field_hpbx_file':
    case 'field_hpbx_photo':
      
      if (isset($files[$delta]['fid'])) {

        if (isset($item['delta'])) {
          $fc_item->{$item['field']}[LANGUAGE_NONE][$item['delta']] = array('fid' => $files[$delta]['fid'], 'display' => 0);
        }
        else {
          // Add new photo in case a photo was uploaded for this block.
          $fc_item->{$item['field']}[LANGUAGE_NONE][] = array('fid' => $files[$delta]['fid'], 'display' => 0);
        }
        
        
        watchdog('hpbx_help', 'update '. $item['field'] . ' with new uploaded fid '. $files[$delta]['fid']);
      }
      
      // Remove deleted items from the field_collection_item.
      if (isset($item['deleted']) && is_array($item['deleted'])) {
        
        foreach ($item['deleted'] as $deleted_fc_delta => $false) {
          
          if (isset($fc_item->{$item['field']}[LANGUAGE_NONE][$deleted_fc_delta])) {
            $file = (object)$fc_item->{$item['field']}[LANGUAGE_NONE][$deleted_fc_delta];
            
            // Delete the file from file system and database.
            if (file_delete($file)) {
  
              // Remove from fc.
              unset($fc_item->{$item['field']}[LANGUAGE_NONE][$deleted_fc_delta]);
            }
            else {
              watchdog('hpbx_help', 'failed to remove file: '. var_export($fc_item->{$item['field']}[LANGUAGE_NONE][$deleted_fc_delta]->fid));
            }
          }
          else {
            watchdog('hpbx_help', 'failed to remove file, fc_delta not found in fc_item: '. var_export($fc_item->{$item['field']}[LANGUAGE_NONE][$deleted_fc_delta]->fid));
          }
        }
      }
      break;
    case 'field_hpbx_instruction_photo':
      
      // Verify if this was an existing item.
      if (!empty($fc_item->field_hpbx_instruction_photo['und'][0]['value']) && !empty($fc_item->field_hpbx_instruction_photo['und'][0]['revision_id'])) {

        // Load the existing entity.
        $fc_item_sub = current(entity_load('field_collection_item',array($fc_item->field_hpbx_instruction_photo['und'][0]['value'])));
        watchdog('hpbx_help', 'Loaded existing instruction_photo field_collection item with id '. $fc_item->field_hpbx_instruction_photo['und'][0]['value']);
      }
      else {
        // Create entity item.
        $fc_item_sub = entity_create('field_collection_item', array('field_name' => 'field_hpbx_instruction_photo'));
        watchdog('hpbx_help', 'Created new instruction_photo field_collection item');
        
        // Assign block (field collection item) to the node.
        $fc_item_sub->setHostEntity('field_collection_item', $fc_item);
      }
      
      $item_body = array(
        'field' => 'field_hpbx_body',
        'value' => $item['value']
      );
      
      // Update the body within the sub fc item.
      $depth++;
      hpbx_help_update_field_collection_item($fc_item_sub, $item_body, $delta, $files);
      $depth--;
      
      $item_photo = array(
        'field' => 'field_hpbx_photo',
        'value' => 'file',
        'delta' => 0,
      );
      
      if (isset($item['deleted'])) {
        $item_photo['deleted'] = $item['deleted'];
      }
      
      // Update the photo within the sub fc item.
      $depth++;
      hpbx_help_update_field_collection_item($fc_item_sub, $item_photo, $delta, $files);
      $depth--;

      // Save the sub collection item.
      $fc_item_sub->save(TRUE);
      break;
    default:
      watchdog('hpbx_help', 'unknown field "' . $item['field'] . '"');
      break;
  }
}


/**
 * 
 */
function hpbx_help_article_node_form_submit($form, &$form_state) {
  global $user;
  if ($form_state['values']['op'] == t('Save')) {

    // Load or create new node.
    if (!empty($form_state['values']['nid'])) {
      
      // Load existing node.
      $node = node_load($form_state['values']['nid']);
    }
    else {
      
      // Create new node.
      $node = new stdClass();
      $node->type = 'hpbx_help_article';
      
      node_object_prepare($node);
    }
  
    // Set simple node values. 
    $node->title = $form_state['values']['title'];
    $node->status = $form_state['values']['status'];
    $node->promote = $form_state['values']['promote'];
    $node->language = $form_state['values']['language'];
    
    // Set type taxonomy term.
    if (isset($form_state['values']['field_hpbx_type']['und'][0]['tid'])) {
      $node->field_hpbx_type['und'][0]['tid'] = $form_state['values']['field_hpbx_type']['und'][0]['tid'];
    }
    
    // Set reseller id.
    $node->field_hpbx_reseller_id['und'][0]['value'] = $form_state['values']['field_hpbx_reseller_id']['und'][0]['value'];
    
    // Upload newly uploaded photo(s) and/or files.
    $files = array();
    
    
    
    if (isset($_FILES['files']['name']) && count($_FILES['files']['name'])) {
      foreach (array_keys($_FILES['files']['name']) as $name) {
        $file_type = $key = FALSE;
        
        if (preg_match('/([photo|file]+)_(\d+)/', $name, $matches)) {
          
          if ($matches[1] == 'photo') {
            $validators = array(
              'file_validate_is_image' => array(),
              'file_validate_extensions' => array('png jpg jpeg'),
            );
          }
          else { 
            $validators = array(
              'file_validate_extensions' => array('pdf txt xls xlsx doc docx ppt pptx')
            );
          }
          if ($file = file_save_upload($name, $validators, 'private://')) {
            $files[$matches[2]]['fid'] = $file->fid;
          }    
        }
      }
    }
    
    // Process all posted blocks.
    $fcs = array();
    
    // Verify if the blocks are posted.
    if (isset($form_state['input']['block'])) {
      
      // Loop through the posted blocks.
      foreach ($form_state['input']['block'] as $delta => $item) {
        
        // Verify if this was an existing item.
        if (!empty($item['item_id']) && !empty($item['revision_id'])) {
  
          // Load the existing entity.
          $fc_item = current(entity_load('field_collection_item',array($item['item_id'])));
          watchdog('hpbx_help', 'Loaded existing field_hpbx_block field_collection item with id ', $item['item_id']);
        }
        else {
          // Create entity item.
          $fc_item = entity_create('field_collection_item', array('field_name' => 'field_hpbx_block'));
          watchdog('hpbx_help', 'Created new field_hpbx_block field_collection item');
          
          // Assign block (field collection item) to the node.
          $fc_item->setHostEntity('node', $node);
        }

        // Update the field collection item.
        hpbx_help_update_field_collection_item($fc_item, $item, $delta, $files);

        // Save the collection item.
        $fc_item->save(TRUE);

        // Set delta.
        $fcs[$delta] = array('value' => $fc_item->item_id, 'revision_id' => $fc_item->revision_id);
      }
    }

    // Set correct order based on filled array with collection items.
    $node->field_hpbx_block['und'] = $fcs;
    
    $user_tmp = FALSE;
    // To set correct node uid, we need to set $user var to masquerading user.
    if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
    
      if ( $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from'])) {
        $user_tmp = $user;
        $user = $account;
      }
    }
    
    // Save the node to apply the order.
    node_save($node);
    
    // Revert the user variable.
    if ($user_tmp) {
      $user = $user_tmp;
    }
    drupal_goto('node/' . $node->nid);
  }
}

/**
 *
 */
function hpbx_help_sitemap_page() {
  drupal_add_css('https://cdnjs.cloudflare.com/ajax/libs/jstree/3.0.9/themes/default/style.min.css', 'external');
  drupal_add_js(drupal_get_path('module', 'hpbx') . '/includes/js/hpbx_help.js', array('scope' => 'footer'));

  $sitemap = theme_hpbx_help_get_sitemap();
  $t = t('Sitemap');
return <<<EOF
<div class="container hpbx-no-menu">
  <div class="row">
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
        <h2 class="hpbx-sitemap-header">{$t}</h2>
        <div class="hpbx-help-edit-sitemap">{$sitemap}</div>
      </div>
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
  </div>
</div>
EOF;

}


function hpbx_help_url_submit() {
  global $user;
  $account = $user;
  
  if (!empty($_SESSION['ccs']['hpbx']['masquerading_from'])) {
    $account = user_load($_SESSION['ccs']['hpbx']['masquerading_from']);
  }
  
  if (!empty($_SERVER['HTTP_REFERER']) && user_access('edit any hpbx_help_article content', $account)) {
        
    $hpbx_url = '';
    foreach (hpbx_menu() as $path => $items) {
      if (preg_match('/'. str_replace('%', '\d+', str_replace('/', '\/', $path)) . '$/', $_SERVER['HTTP_REFERER'], $matches)) {
        $hpbx_url = $path;
        break;
      }
    }
    global $language;
    
    if (!$reseller_id = sipwise_api_get_active_reseller()) {
      $reseller_id = 0; // LG.
    }
    
    watchdog('hpbx_help', 'hpbx_url: '. $hpbx_url);
    watchdog('hpbx_help', 'nid: '. $_POST['nid']);
    watchdog('hpbx_help', 'reseller_id: '. $reseller_id);

    if (!empty($hpbx_url)) {
      try {
        
         if (!empty($_POST['nid'])) {
          $fields = array(
            'reseller_id' => $reseller_id,
            'lang' => $language->language,
            'hpbx_url' => $hpbx_url,
            'help_url' => 'node/'. (int)$_POST['nid'],
            'nid' => (int)$_POST['nid']
          );
          db_merge('hpbx_help')
          ->key(array('reseller_id' => $reseller_id, 'lang' => $language->language, 'hpbx_url' => $hpbx_url))
          ->fields($fields)
          ->execute();
          
          watchdog('hpbx_help', 'Saved help url: '.var_export($fields, TRUE));
          
        }
        else {
          // empty url. clear record if exists.
          db_delete('hpbx_help')
          ->condition('reseller_id', $reseller_id)
          ->condition('lang', $language->language)
          ->condition('hpbx_url', $hpbx_url)
          ->execute();
          
          watchdog('hpbx_help', 'Removed references to : '. $hpbx_url);
        }
        
        
        echo json_encode(array('success' => TRUE));
        drupal_exit();
      }
      catch(Exception $e) {
        watchdog('hpbx_help', 'Retrieved exception: '. $e->getMessage() .' at '. $e->getLine());
        echo json_encode(array('success' => FALSE));
        drupal_exit();
      }
    }
    else {
      watchdog('hpbx_help', t('No url'));
    }
  }
  else {
    watchdog('hpbx_help', t('No user_access'));
  }
  echo json_encode(array('success' => FALSE));
  drupal_exit();
  
}

function hpbx_help_search_form($form, $form_state) {
  
  $form = array();
  $form['query'] = array(
    '#type' => 'textfield',
    '#attributes' => array(
      'placeholder' => t('Search'),
      'class' => array('sg-element', 'needsclick', 'hpbx-search-input'),
      'autocomplete' => 'off',
    ),
    '#field_size_class' => array('')
  );
  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Search'),
  );
  return $form;
}

function hpbx_help_search_form_validate($form, $form_state) {
  
}

function hpbx_help_search_form_submit($form, $form_state) {
  drupal_goto('hpbx/help/search/'. $form_state['values']['query']);
}

/**
 * Callback for hpbx/help/loader.html.
 */
function hpbx_help_loader() {
echo <<<EOF
  <!DOCTYPE html>
  <!--[if lt IE 8]>
  <html lang="en" class="no-js ltie8"> <![endif]-->
  <!--[if IE 8]>
  <html lang="en" class="no-js ie8"> <![endif]-->
  <!--[if IE 9]>
  <html lang="en" class="no-js ie9"> <![endif]-->
  <!--[if (gt IE 9)|!(IE)]><!-->
  <html class="no-js non-ie" lang="en">
  <!--<![endif]-->
  <head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
      <meta name="viewport" content="width=device-width, initial-scale=1">
        <link href="/sites/all/themes/hpbx_theme/dist/css/all.min.css" media="screen" rel="stylesheet" type="text/css">
        <title>HPBX UPC Business - Help Loader</title>
          </head>
          <body>
          <!-- </div> -->
          <!--[if lt IE 9]>
          <script src="/sites/all/themes/hpbx_theme/dependencies/jquery-legacy/dist/jquery.js"></script>
        <![endif]-->
          <!--[if gte IE 9]><!-->
          <script src="/sites/all/themes/hpbx_theme/dependencies/jquery/dist/jquery.min.js"></script>
        <!--<![endif]-->
          <script type="text/javascript" src="/sites/all/themes/hpbx_theme/dist/js/all.js"></script>
    </body>
</html>
EOF;
die();
}

