<?php

function hpbx_help_article_block_item($node, $entity) {
  if (isset($node->field_hpbx_type['und'][0]['tid']) &&  taxonomy_term_load($node->field_hpbx_type['und'][0]['tid'])->name == 'F.A.Q.'){

    $entity->field_hpbx_body['und'][0]['value'] = preg_replace('/[\n\r]/', '', $entity->field_hpbx_body['und'][0]['value']);

    $title = '';
    if (preg_match('/<h2>(.+)<\/h2>(.+)/im', trim($entity->field_hpbx_body['und'][0]['value']), $matches)) {
      $title = trim($matches[1]);
      $content = trim($matches[2]);
    }

    $dt_attributes = array();

    $dt_attributes = array('class' => array('hpbx-foldable-heading'));
    if (isset($_GET['id']) && isset($entity->item_id) && $entity->item_id == (int)$_GET['id']) {
      $dt_attributes['class'][] =  'hpbx-foldable-state-open';
    }

    $dt_attributes['data-click-handler'] = 'HelpArticleCount';
    $dt_attributes['data-click-handler-argument'] = $entity->item_id;



    return '
    <div class="hpbx-foldable-list-item">
      <dt '.drupal_attributes($dt_attributes).'>
          <span class="hpbx-foldable-heading-title">'. $title .'</span>
      </dt>
      <dd class="hpbx-foldable-content">'. $content . '</dd>
      <dd class="hpbx-foldable-footer hpbx-foldable-state-close">
        <div class="hpbx-foldable-footer-line"></div>
        <div class="hpbx-foldable-footer-icon"></div>
      </dd>
    </div>';
  }

  return $entity->field_hpbx_body['und'][0]['value'];
}
?>

<?php if (isset($node->field_hpbx_type['und'][0]['tid']) && taxonomy_term_load($node->field_hpbx_type['und'][0]['tid'])->name == 'F.A.Q.'): ?>
<div class="hpbx-help-faq-container">
    <dl class="hpbx-foldable-list hpbx-help-faq-foldable-list hpbx-foldable-list-accordion-mode">
<?php endif; ?>

<?php $previous = ''; ?>
<?php foreach ($content['field_hpbx_block']['#items'] as $delta => $item):?>

  <?php $entity = field_collection_field_get_entity($item); ?>

  <?php if (isset($entity->field_hpbx_instruction_photo['und'])): ?>

    <?php $sub_entity = field_collection_field_get_entity($entity->field_hpbx_instruction_photo['und'][0]);?>


      <?php if ($previous != 'field_hpbx_instruction_photo'): ?>
        <div class="hpbx-help-steps-container">
      <?php endif; ?>

    <div class="hpbx-help-step-container">
        <div class="row">
            <div class="col-xxs-12 col-xs-6 col-xxs-push-0 col-xs-push-6">
              <?php print $sub_entity->field_hpbx_body['und'][0]['value']; ?>
            </div>
            <div class="col-xxs-12 col-xs-6 col-xxs-pull-0 col-xs-pull-6">
                <div class="hpbx-help-step-image" style="background-image: url(<?php print file_create_url($sub_entity->field_hpbx_photo['und'][0]['uri'])?>)" data-image-url="<?php print file_create_url($sub_entity->field_hpbx_photo['und'][0]['uri'])?>"></div>
            </div>
        </div>
    </div>
    <?php $previous = 'field_hpbx_instruction_photo' ?>
  <?php elseif (isset($entity->field_hpbx_body['und'])): ?>

      <?php if ($previous == 'field_hpbx_instruction_photo'): ?>
        </div>
      <?php endif; ?>
      <?php print hpbx_help_article_block_item($node, $entity);?>

  <?php elseif (isset($entity->field_hpbx_video['und'])): ?>

      <?php if ($previous == 'field_hpbx_instruction_photo'): ?>
        </div>
      <?php endif; ?>

      <p>
        <iframe class="hpbx-help-article-media" width="100%" src="<?php print $entity->field_hpbx_video['und'][0]['video_url']?>" frameborder="0" allowfullscreen></iframe>
      </p>
  <?php elseif (isset($entity->field_hpbx_photo['und'])): ?>

    <?php if ($previous == 'field_hpbx_instruction_photo'): ?>
      </div>
    <?php endif; ?>

    <?php foreach ($entity->field_hpbx_photo['und'] as $p_delta => $file): ?>
      <div class="sg-paragraph">
        <div class="hpbx-help-article-media hpbx-help-article-image" style="background-image: url(<?php print file_create_url($file['uri'])?>)" data-image-url="<?php print file_create_url($file['uri'])?>"></div>
      </div>
    <?php endforeach; ?>
  <?php elseif (isset($entity->field_hpbx_file['und'])): ?>
    <div class="hpbx-help-filelist hpbx-help-filelist-files">
      <h5 class="hpbx-help-files-container-title"><?php  print t('Files'); ?></h5>
      <div class="hpbx-help-filelist-items">
        <?php  foreach ($entity->field_hpbx_file['und'] as $fc_delta => $file):?>
          <div class="hpbx-help-filelist-item">
            <div class="row">
              <div class="col-xs-10">
                <div class="hpbx-help-filelist-thumbnail-container">
                  <img class="hpbx-help-filelist-thubnail-image" src="<?php print hpbx_help_get_file_icon_path($file); ?>">
                </div>
                <div class="hpbx-help-filelist-item-info">
                  <div class="hpbx-help-filelist-item-title"><?php print l($file['filename'], file_create_url($file['uri'])); ?></div>
                  <div class="hpbx-help-filelist-item-subtitle"><?php print format_size($file['filesize']); ?></div>
                </div>
              </div>
            </div>
          </div>
        <?php endforeach ?>
      </div>
    </div>
  <?php endif ?>
<?php endforeach ?>

<?php if (isset($node->field_hpbx_type['und'][0]['tid']) &&  taxonomy_term_load($node->field_hpbx_type['und'][0]['tid'])->name == 'F.A.Q.'): ?>
  </dl>
</div>
<?php endif;?>