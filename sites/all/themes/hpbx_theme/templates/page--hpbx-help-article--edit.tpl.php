<form id="hpbx-help-article-node-form" method="post" enctype="multipart/form-data" action="<?php print url(current_path());?>">
<?php

// Render essential fields, needed for processing.

print render($page['content']['system_main']['field_hpbx_body']);
print render($page['content']['system_main']['form_build_id']);
print render($page['content']['system_main']['form_id']);
print render($page['content']['system_main']['form_token']);
print render($page['content']['system_main']['hpbx_element_weight']);

// Render some elements withouth displaying the rendered version.
render($page['content']['system_main']['options']['status']);
render($page['content']['system_main']['options']['promote']);
//render($page['content']['actions']);

?>
<div class="hpbx-help-header">
    <div class="container hpbx-no-menu">
        <div class="row">
            <div class="col-xs-1 col-sm-2 hidden-xxs"></div>
            <div class="col-xs-5 col-xxs-10 col-sm-4">
                <a class="hpbx-help-header-link" href="<?php print url('hpbx/help/home')?>">
                    <span class="hpbx-help-header-link-icon LGI-iconarrow-left"></span><?php print t('Back to all help articles'); ?>
                </a>
            </div>
            <div class="col-xs-1 col-sm-2 hidden-xxs"></div>
        </div>
    </div>
    <div class="hpbx-help-close-handle needsclick"><span class="LGI-iconcross"></span></div>
</div>
<div class="hpbx-help-edit-tabbed-content">
            <div class="hpbx-tabs hpbx-tabs-inside-content">
              <ul>
                <li class="hpbx-tab-item"><a href="#tab-edit"><span class="hpbx-tab-icon LGI-iconedit"></span><span class="hpbx-tab-title"><?php print t('Edit'); ?></span></a></li>
                <li class="hpbx-tab-item"><a href="#tab-sitemap"><span class="hpbx-tab-icon LGI-iconauto-attendant"></span><span class="hpbx-tab-title"><?php print t('Sitemap'); ?></span></a></li>
                <li class="hpbx-tab-item"><a href="#tab-details"><span class="hpbx-tab-icon LGI-iconsettings"></span><span class="hpbx-tab-title"><?php print t('Details');?></span></a></li>
              </ul>
              <div id="tab-edit">
                <div class="hpbx-scrollable-container hpbx-help-scrollable-content-container">
                  <!-- TITLE -->
                  <div class="hpbx-help-edit-header">
                    <div class="container hpbx-no-menu">
                      <div class="row">
                        <div class="col-xs-1">
                        </div>
                        <div class="col-xs-10"><?php print render($page['content']['system_main']['title']) ?></div>
                        <div class="col-xs-1"></div>
                      </div>
                    </div>
                  </div>
                  <div class="hpbx-help-container">
                    <div class="row hpbx-help-edit-add">
                      <div class="hpbx-help-edit-add-line"></div>
                        <div class="hpbx-help-edit-add-links-container">
                          <span class="hpbx-help-edit-add-links">
                            <span class="hpbx-help-edit-add-link" data-block-type="text"><?php print t('Text block');?></span>
                            <span class="hpbx-help-edit-add-link" data-block-type="photo"><?php print t('Photo');?></span>
                            <span class="hpbx-help-edit-add-link" data-block-type="video"><?php print t('Video');?></span>
                            <span class="hpbx-help-edit-add-link" data-block-type="file"><?php print t('File');?></span>
                            <span class="hpbx-help-edit-add-link" data-block-type="instruction"><?php print t('Instruction');?></span>
                        </span>
                      </div>
                    </div>
                  </div>
    
    
                  <!-- END TITLE -->
  
                  <!-- START BLOCK -->
                  <?php 
                  
                  foreach ($page['content']['system_main']['#node']->field_hpbx_block['und'] as $delta => $item):?>
                  
                    <?php $entity = field_collection_field_get_entity($item); ?>
                    <?php $valid_block = FALSE; ?>
                    <?php if (isset($entity->field_hpbx_file['und']) && count($entity->field_hpbx_file['und'])): ?>
                        <?php $valid_block = TRUE;?>
                        <div class="hpbx-help-container" data-block-type="file" data-fc-revision-id="<?php print $entity->revision_id?>" data-fc-item-id="<?php print $entity->item_id?>" data-fc-delta="<?php print $delta; ?>">
                          <div class="hpbx-help-placeholder-file-block">
                            <?php  foreach ($entity->field_hpbx_file['und'] as $fc_delta => $file):?>
                            <img data-fc-delta="<?php print $fc_delta;?>" data-filesize="<?php print format_size($file['filesize']); ?>"  data-filename="<?php print $file['filename']?>" src="<?php print hpbx_help_get_file_icon_path($file);?>"/>
                            <?php endforeach;?>
                          </div>
                        
                    <?php elseif (isset($entity->field_hpbx_video['und']) && count($entity->field_hpbx_video['und'])): ?>
                      <?php $valid_block = TRUE;?>
                      <div class="hpbx-help-container" data-block-type="video" data-fc-revision-id="<?php print $entity->revision_id?>"  data-fc-item-id="<?php print $entity->item_id?>" data-fc-delta="<?php print $delta; ?>">
                        <div class="hpbx-help-placeholder-video-block" data-video-url="<?php print $entity->field_hpbx_video['und'][0]['video_url']; ?>"></div>
                    
                    <?php elseif (isset($entity->field_hpbx_body['und']) && count($entity->field_hpbx_body['und'])): ?>
                      <?php $valid_block = TRUE;?>
                      <div class="hpbx-help-container" data-block-type="text" data-fc-revision-id="<?php print $entity->revision_id?>"  data-fc-item-id="<?php print $entity->item_id?>" data-fc-delta="<?php print $delta; ?>">
                        <div class="hpbx-help-placeholder-wysiwyg-block">
                          <?php print $entity->field_hpbx_body['und'][0]['value']; ?>
                        </div>
                        
                    <?php elseif (isset($entity->field_hpbx_instruction_photo['und']) && count($entity->field_hpbx_instruction_photo['und'])): ?>
                       <?php $sub_entity = field_collection_field_get_entity($entity->field_hpbx_instruction_photo['und'][0]); ?>
                       <?php $valid_block = TRUE;?>
                        <div class="hpbx-help-container" data-block-type="instruction" data-fc-revision-id="<?php print $entity->revision_id?>" data-fc-item-id="<?php print $entity->item_id?>" data-fc-delta="<?php print $delta; ?>">
                          <div class="hpbx-help-placeholder-wysiwyg-block">
                            <?php print $sub_entity->field_hpbx_body['und'][0]['value']; ?>
                          </div>
                          <div class="hpbx-help-placeholder-photo-block">
                            <?php if (isset($sub_entity->field_hpbx_photo['und'][0]['uri'])):?>
                              <img data-fc-delta="0" data-filesize="<?php print format_size($sub_entity->field_hpbx_photo['und'][0]['filesize']); ?>"  data-filename="<?php print $sub_entity->field_hpbx_photo['und'][0]['filename']?>" src="<?php print file_create_url($sub_entity->field_hpbx_photo['und'][0]['uri'])?>"/>
                            <?php endif ?>
                          </div>
                    <?php elseif (isset($entity->field_hpbx_photo['und']) && count($entity->field_hpbx_photo['und'])): ?>
                        <?php $valid_block = TRUE;?>
                        <div class="hpbx-help-container" data-block-type="photo" data-fc-revision-id="<?php print $entity->revision_id?>" data-fc-item-id="<?php print $entity->item_id?>" data-fc-delta="<?php print $delta; ?>">
                          <div class="hpbx-help-placeholder-photo-block">
                            <?php  foreach ($entity->field_hpbx_photo['und'] as $fc_delta => $file):?>
                            <img data-fc-delta="<?php print $fc_delta;?>" data-filesize="<?php print format_size($file['filesize']); ?>"  data-filename="<?php print $file['filename']?>" src="<?php print file_create_url($file['uri'])?>"/>
                            <?php endforeach;?>
                          </div>
                    <?php endif?>
                          <?php if ($valid_block): ?>
                          <div class="hpbx-help-edit-footer">
                              <div class="container hpbx-no-menu">
                                  <div class="row">
                                      <div class="col-xs-1"></div>
                                      <div class="col-xs-5"></div>
                                      <div class="col-xs-5">
                                          <div class="hpbx-help-edit-footer-right">
                                              <a class="hpbx-help-edit-link-button hpbx-help-block-remove" href="#"><?php print t('Remove block')?></a>
                                          </div>
                                      </div>
                                      <div class="col-xs-1"></div>
                                  </div>
                              </div>
                          </div>
                          <div class="row hpbx-help-edit-add">
                            <div class="hpbx-help-edit-add-line"></div>
                              <div class="hpbx-help-edit-add-links-container">
                                <span class="hpbx-help-edit-add-links">
                                  <span class="hpbx-help-edit-add-link" data-block-type="text"><?php print t('Text block');?></span>
                                  <span class="hpbx-help-edit-add-link" data-block-type="photo"><?php print t('Photo');?></span>
                                  <span class="hpbx-help-edit-add-link" data-block-type="video"><?php print t('Video');?></span>
                                  <span class="hpbx-help-edit-add-link" data-block-type="file"><?php print t('File');?></span>
                                  <span class="hpbx-help-edit-add-link" data-block-type="instruction"><?php print t('Instruction');?></span>
                              </span>
                            </div>
                          </div>
                        
                      </div> <!-- end of hpbx-help-container -->
                      <?php endif;?>
                  <?php endforeach;?>
                    
                  <!-- END BLOCK -->
                      
                  
                  <!-- -->
                  <div class="hpbx-help-edit-footer">
                    <div class="container hpbx-no-menu">
                        <div class="row">
                            <div class="col-xs-1"></div>
                            <?php print render($page['content']['system_main']['actions']); ?>
                            <div class="col-xs-5">
                                <div class="hpbx-help-edit-footer-right">
                                  <?php 
                                  
                                  if (isset($page['content']['system_main']['#node']->nid)) :?>
                                    <?php print l(t('Cancel'), 'node/'. $page['content']['system_main']['#node']->nid, array('attributes' => array('class' => array('hpbx-help-edit-link-button')))); ?>
                                  <?php else:?>
                                    <?php print l(t('Cancel'), 'hpbx/help', array('attributes' => array('class' => array('hpbx-help-edit-link-button')))); ?>
                                  <?php endif;?>
                                </div>
                            </div>
                            <div class="col-xs-1"></div>
                        </div>
                    </div>
                  </div>
                </div>
              </div>
              <div id="tab-sitemap">
                <div class="hpbx-scrollable-container hpbx-help-scrollable-content-container">
                  <div class="container hpbx-no-menu">
                    <div class="row hpbx-help-content">
                        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                        <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
                            <div class="hpbx-help-edit-sitemap">
                              <?php print theme_hpbx_help_get_sitemap(); ?>
                            </div>
                        </div>
                        <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                    </div>
                  </div>
                </div>
                  
              </div>
              <div id="tab-details" class="hpbx-help-edit-details">
                  <div class="container hpbx-no-menu">
                      <?php if (!empty($node->nid)): ?>
                      <div class="row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Node ID'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right"><?php print $node->nid;?></div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <?php endif;?>
                      <?php if (!empty($node->revision_timestamp)): ?>
                      <div class="row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Edited'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right"><?php if (!empty($node->revision_timestamp)){ print date('Y-m-d H:i:s', $node->revision_timestamp);}?></div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <?php endif;?>
                      <?php 
                      if ($account = user_load($node->revision_uid)) {
                        if ($ad_entry = ccs_users_get_user($account->name)) {
                          if (!empty($ad_entry[0]['cn'][0])) {
                            $username = $ad_entry[0]['cn'][0];
                          }
                        }
                        else {
                          $username = $account->name;
                        }
                      }
                      
                      if (!empty($username)):
                      ?>
                      <div class="row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Edited by'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                              <?php  print $username; ?>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <?php endif;?>
                      <div class="row hpbx-help-edit-details-toggle-row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Visibility'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                  <div class="sg-custom-toggle sg-slide-colors">
                                      <label title="">
                                          <input type="checkbox" name="status" id="sg-custom-toggle-sc" value="1" <?php if ($page['content']['system_main']['options']['status']['#default_value']): print "checked=\"checked\""; endif?>">
                                          <div class="sg-alternative-display">
                                              <div class="sg-on"></div>
                                              <div class="sg-off"></div>
                                              <div class="sg-knob">
                                                  <div class="sg-dot"></div>
                                              </div>
                                          </div>
                                          <div class="sg-caption-container">
                                              <div class="sg-caption"></div>
                                          </div>
                                      </label>
                                  </div>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <div class="row hpbx-help-edit-details-toggle-row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Frontpage'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                  <div class="sg-custom-toggle sg-slide-colors">
                                      <label title="">
                                          <input type="checkbox" name="promote" id="sg-custom-toggle-sc" value="1" <?php if ($page['content']['system_main']['options']['promote']['#default_value']): print "checked=\"checked\""; endif?>">
                                          <div class="sg-alternative-display">
                                              <div class="sg-on"></div>
                                              <div class="sg-off"></div>
                                              <div class="sg-knob">
                                                  <div class="sg-dot"></div>
                                              </div>
                                          </div>
                                          <div class="sg-caption-container">
                                              <div class="sg-caption"></div>
                                          </div>
                                      </label>
                                  </div>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <div class="row hpbx-help-edit-details-custom-select-row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Language'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                <?php
                                  $page['content']['system_main']['language']['#title_display'] = 'invisible';
                                  $page['content']['system_main']['language']['#attributes'] = array('class' => array('chzn-select', 'sg-element', 'no-search'));
                                  
                                  unset($page['content']['system_main']['language']['#options']['dm']);
                                  print render($page['content']['system_main']['language']); 
                                ?>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                       <div class="row hpbx-help-edit-details-custom-select-row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Reseller'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                <?php
                                
                                $page['content']['system_main']['field_hpbx_reseller_id']['und']['#attributes'] = array('class' => array('chzn-select', 'sg-element', 'no-search'));
                                print render($page['content']['system_main']['field_hpbx_reseller_id']);
                                ?>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <div class="row hpbx-help-edit-details-custom-select-row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Frontpage type'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                <?php
                                $page['content']['system_main']['field_hpbx_type']['und']['#attributes'] = array('class' => array('chzn-select', 'sg-element', 'no-search'));
                                print render($page['content']['system_main']['field_hpbx_type']);
                                ?>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <div class="row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2">
                              <div class="hpbx-help-edit-details-left"><?php print t('Pages'); ?></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                              <div class="hpbx-help-edit-details-right">
                                <?php
                                
                                print render($page['content']['system_main']['assigned_pages']);
                                ?>
                              </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                      <div class="row">
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                          <div class="col-xs-2 col-xxs-3 col-sm-2 col-md-2"> 
                            <div class="hpbx-help-edit-details-left"></div>
                          </div>
                          <div class="col-xs-8 col-xxs-9 col-sm-6 col-md-4">
                            <div class="hpbx-help-edit-details-right">
                              <?php print render($page['content']['system_main']['actions']['submit']); ?>
                            </div>
                          </div>
                          <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
                      </div>
                  </div>
              </div>
            </div>
        </div>
      </div>
    </div>
    </div>
</form>



<div id="hpbx-help-content-tpl" style="display:none;">
  <div class="hpbx-help-edit-container">
    <div class="hpbx-help-edit-header">
      <div class="container hpbx-no-menu">
        <div class="row">
          <div class="col-xs-1">
          </div>
          <div class="col-xs-10"><div class="hpbx-wysiwyg-toolbar"></div></div>
          <div class="col-xs-1"></div>
        </div>
      </div>
    </div>
    <div class="hpbx-help-edit-main">
      <div class="container hpbx-no-menu">
        <div class="row">
          <div class="col-xs-1"></div>
          <div class="col-xs-10"><div class="hpbx-wysiwyg-container"></div></div>
          <div class="col-xs-1"></div>
        </div>
      </div>
    </div>
  </div>
</div>

<div id="hpbx-help-content-photos-tpl" style="display:none;">
  <div class="hpbx-help-edit-container hpbx-help-edit-photos">
    <div class="hpbx-help-edit-header">
      <div class="container hpbx-no-menu hpbx-help-edit-header-title">
          <div class="row">
              <div class="col-xs-1">
              </div>
              <div class="col-xs-10">
                  <div class="hpbx-help-edit-header-title hpbx-help-edit-header-title-label"><?php print t('Add photos') ?></div>
              </div>
              <div class="col-xs-1"></div>
          </div>
          <div class="hpbx-help-edit-saved-items"></div>
      </div>
      <div class="hpbx-help-edit-main">
          <div class="container hpbx-no-menu">
              <div class="row">
                  <div class="col-xs-1"></div>
                  <div class="col-xs-9">
                      <?php print t('Drag and drop files here or <a href="" class="hpbx-file-upload-link">upload</a> them'); ?>
                    <input type="file" />    
                  </div>
                  <div class="col-xs-1"></div>
              </div>
          </div>
      </div>
    </div>
  </div>
</div>


<div id="hpbx-help-content-video-tpl" style="display:none;">
  <div class="hpbx-help-edit-container hpbx-help-edit-video">
    <div class="hpbx-help-edit-header">
        <div class="container hpbx-no-menu">
            <div class="row">
                <div class="col-xs-1">
                </div>
                <div class="col-xs-10">
                    <div class="hpbx-help-edit-header-title"><?php print t('Add video') ?></div>
                </div>
                <div class="col-xs-1"></div>
            </div>
        </div>
    </div>
    <div class="hpbx-help-edit-main">
        <div class="container hpbx-no-menu">
            <div class="row">
                <div class="col-xs-1"></div>
                <div class="col-xs-10">
                    <div class="hpbx-ie8"><?php print t('URL of your video') ?></div>
                    <input type="text" class="hpbx-help-edit-input-incognito" placeholder="<?php print t('Enter the URL of your video here...'); ?>" />
                </div>
                <div class="col-xs-1"></div>
            </div>
        </div>
    </div>
  </div>
</div>


<div id="hpbx-help-saved-item-tpl" style="display:none;">
  <div class="hpbx-help-edit-saved-item" data-fc-delta="">
      <div class="row">
          <div class="col-xs-1"></div>
          <div class="col-xs-9">
              <div class="hpbx-help-edit-thumbnail-container">
                  <div class="hpbx-help-edit-thumbnail-outer">
                      <div class="hpbx-help-edit-thumbnail-inner" style="background-image: url(/dist/img/wizard/upc-business-1.jpg)"></div>
                  </div>
              </div>
              <div class="hpbx-help-edit-saved-item-info">
                  <div class="hpbx-help-edit-saved-item-title">File name</div>
                  <div class="hpbx-help-edit-saved-item-subtitle">512.8 kb</div>
              </div>
          </div>
          <div class="col-xs-2">
              <div class="LGI-iconcross hpbx-help-edit-saved-item-delete-icon hpbx-delete-saved-item"></div>
          </div>
      </div>
  </div>
</div>

<div id="hpbx-help-block-footer-tpl" style="display:none;">
  <div class="hpbx-help-edit-footer">
      <div class="container hpbx-no-menu">
          <div class="row">
              <div class="col-xs-1"></div>
              <div class="col-xs-5"></div>
              <div class="col-xs-5">
                  <div class="hpbx-help-edit-footer-right">
                      <a class="hpbx-help-edit-link-button hpbx-help-block-remove" href="#"><?php print t('Remove block');?></a>
                  </div>
              </div>
              <div class="col-xs-1"></div>
          </div>
      </div>
  </div>
</div>

<div id="hpbx-help-edit-add-tpl" style="display:none;">
  <div class="row hpbx-help-edit-add">
    <div class="hpbx-help-edit-add-line"></div>
      <div class="hpbx-help-edit-add-links-container">
        <span class="hpbx-help-edit-add-links">
          <span class="hpbx-help-edit-add-link" data-block-type="text"><?php print t('Text block');?></span>
          <span class="hpbx-help-edit-add-link" data-block-type="photo"><?php print t('Photo');?></span>
          <span class="hpbx-help-edit-add-link" data-block-type="video"><?php print t('Video');?></span>
          <span class="hpbx-help-edit-add-link" data-block-type="file"><?php print t('File');?></span>
          <span class="hpbx-help-edit-add-link" data-block-type="instruction"><?php print t('Instruction');?></span>
      </span>
    </div>
  </div>
</div>

<div id="hpbx-wysiwyg-toolbar-dialog-tpl" style="display:none;">
  <div class="hpbx-wysiwyg-toolbar-dialog" data-wysihtml5-dialog="createLink" style="display:none">
      <div class="row">
          <div class="hpbx-help-toolbar-dialog-row">
              <div class="col-xs-3">Url:</div>
              <div class="col-xs-9"><input type="text" class="sg-element" data-wysihtml5-dialog-field="href" value=""></div>
          </div>
      </div>
      <div class="row">
          <div class="hpbx-help-toolbar-dialog-row">
              <div class="col-xs-3">Class:</div>
              <div class="col-xs-9"><input type="text" class="sg-element" data-wysihtml5-dialog-field="class" value=""></div>
          </div>
      </div>
      <div class="row">
          <div class="hpbx-help-toolbar-dialog-row">
              <div class="col-xs-6">
                  <div class="hpbx-help-edit-footer-left">
                      <a class="sg-btn sg-btn-small sg-btn-primary sg-without-icon" data-wysihtml5-dialog-action="save">OK</a>
                  </div>
              </div>
              <div class="col-xs-6">
                  <div class="hpbx-help-edit-footer-right">
                      <a class="hpbx-help-edit-link-button" data-wysihtml5-dialog-action="cancel">Cancel</a>
                  </div>
              </div>
          </div>
      </div>
  </div>
</div>

                                    
                                    
