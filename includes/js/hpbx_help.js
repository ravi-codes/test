
(function($, window, document) {

  function sitemapContextMenuItems(node) {
    var items = {
      'view': {
        'label': Drupal.t('View'),
        'action': function(event){
          location.href = $(event.reference[0]).data('view-url');
          return false; 
        }
      },
      'edit': {
        'label': Drupal.t('Edit'),
        'action':  function(event){
          location.href = $(event.reference[0]).data('edit-url');
          return false; 
        },
        '_disabled': function(event) {
          return $(event.reference[0]).data('disabled');
        }
      },
      'replicate': {
        'label': Drupal.t('Replicate'),
        'action': function(event){
          location.href = $(event.reference[0]).data('replicate-url');
          return false; 
        }
      },
    };
    
    if (node.parents.length < 2) {
      delete items.deleteItem;
    }
    
    return items;
  }
  
  $('.hpbx-help-edit-sitemap').jstree({
    'plugins': ['contextmenu'],
    'contextmenu': {
      'items': sitemapContextMenuItems,
      'show_at_node': false
    }
  });
  
  
  
  
  
  
  window.URLEditSubmit = URLEditSubmit;
  
  window.HelpArticleCount = function(item_id){
    $.ajax({
      type: 'POST',
      url:  Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/help/count',
      data: 'item_id=' + item_id,
      cache: false,
      dataType: 'json'
    });
  };
  
  function URLEditSubmit(){
    
    $.ajax({
      type: 'POST',
      url:  Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/help/url/save',
      data: 'nid=' + $('.hpbx-help-edit-url-popup select option:selected').val(),
           cache: false,
           dataType: 'json'
    })
    .done(function( msg ) {

      $('.hpbx-help-open-handle').data('help-url', Drupal.settings.basePath + Drupal.settings.pathPrefix + 'node/' + $('.hpbx-help-edit-url-popup select option:selected').val());
      $.magnificPopup.close();
      
    })
    .fail(function( jqXHR, textStatus ) {
      alert( "Request failed: " + textStatus );
    });
  }
  
  
  $(document).ready(function(){
  
    var wid = 1;
    if ($('body.node-type-hpbx-help-article').length) {
      $('body.node-type-hpbx-help-article li.hpbx-help-step-substep').each(function(){
        
        if (!$(this).prev().get(0) || $(this).prev().get(0).tagName!='LI') {
          $(this).toggleClass('wid-' + wid, true);
        }
        if ($(this).prev().get(0) && $(this).prev().get(0).tagName=='LI' &&
            $(this).next().get(0) && $(this).next().get(0).tagName=='LI') {

          $(this).toggleClass('wid-' + wid, true);
        }
        if (!$(this).next().get(0) || $(this).next().get(0).tagName!='LI') {
            $(this).toggleClass('wid-' + wid, true);
            wid++;
        }
      });

      for (var i=1;i<wid;i++) {
        $('.wid-' + i).removeClass('wid-' + i).wrapAll('<ul class="hpbx-help-step-substeps"/>');
      }
    }
    
    
    function newToolbarId() {
      // Find new toolbar ID.
      var toolbar_id = Math.max.apply(0, $('.hpbx-wysiwyg-toolbar').map(function(){
        
        if ($(this).attr('id')!==undefined) {
          return parseInt($(this).attr('id').split('-')[3]);
        }
        return -1;
      }))+1;
      return toolbar_id;
    }
    
    function newDelta() {
      // Find new toolbar ID.
      var fc_delta = Math.max.apply(0, $('.hpbx-help-container').map(function(){
        
        if ($(this).data('fc-delta')!==undefined) {
          return parseInt($(this).data('fc-delta'));
        }
        return -1;
      }))+1;
      return fc_delta;
    }
    
    function newPhotoNameId() {
      // Find new toolbar ID.
      var counter = Math.max.apply(0, $('input[type=file]').map(function(){
        
        if ($(this).attr('id')!==undefined) {
          return parseInt($(this).attr('id').split('-')[2]);
        }
        return -1;
      }))+1;
      return counter;
    }
    
    
    function getWysiwygButtons(block_type)  {
      var buttons = [];
      switch (block_type) {
        case 'text':
          // Set buttons.
          buttons = [
          {cmd: 'formatBlock',label: 'H2', value: 'h2'},
          {cmd: 'formatBlock',label: 'H4', value: 'h4'},
          {cmd: 'formatBlock',label: 'P', value: 'p'},
          {cmd: 'customClassParagraph',label: 'Intro', value: 'sg-text-intro'},
          {cmd: 'createLink',label: 'link'},
          {cmd: 'bold',label: 'B'},
          {cmd: 'italic',label: 'I'},
          {cmd: 'underline',label: 'U'},
          {cmd: 'insertUnorderedList',label: 'UL'},
          {cmd: 'insertOrderedList',label: 'OL'},
          {cmd: 'undo',label: 'undo'},
          {cmd: 'redo',label: 'redo'}];
          break;
        case 'instruction':
          buttons = [
          {cmd: 'customClassParagraph', value: 'hpbx-help-step-title', label: 'Title'}, 
          {cmd: 'customClassLi', value: 'hpbx-help-step-substep', label: 'Substep'},
          {cmd: 'createLink', label: 'link'},
          {cmd: 'bold', label: 'B'},
          {cmd: 'italic',label: 'I'},
          {cmd: 'underline',label: 'U'},
          {cmd: 'insertUnorderedList', label: 'UL'},
          {cmd: 'insertOrderedList', label: 'OL'},
          {cmd: 'undo', label: 'undo'},
          {cmd: 'redo', label: 'redo'}];
          break;
      }
      return buttons;
    }
    
    function generateInstruction($element) {
      generateWysiwygEditor($element, 'hpbx-help-edit-instruction');
      generatePhotoEditor($element, false);
    }
    
    function generateVideoEditor($element) {
      
      var block_type    = $element.data('block-type'),
                    $block        = $element.find('.hpbx-help-placeholder-video-block');
                    video_url = $element.find('.hpbx-help-placeholder-video-block').data('video-url');
                    
                    if (video_url == '') {
                      video_url = Drupal.t('Enter the URL of your video here...');
                    }
                    
                    // Clone from template.
                    var $help_content = $('#hpbx-help-content-video-tpl .hpbx-help-edit-container').clone();
                    
                    // Add block. 
                    var $hpbx_block = $('<div>')
                    .addClass('hpbx-block')
                    .append($help_content);
                    
                    
                    $hpbx_block.find('input').val(video_url);
                    
                    $block.replaceWith($hpbx_block);
    }
    
    function generateWysiwygEditor($element, edit_container_class) {
      var block_type    = $element.data('block-type'),
                    $block        = $element.find('.hpbx-help-placeholder-wysiwyg-block');
                    block_content = $element.find('.hpbx-help-placeholder-wysiwyg-block').html();
                    
                    if (block_content == '') {
                      block_content = Drupal.t('Type your text here...');
                    }
                    
                    // Clone from template.
                    var $help_content = $('#hpbx-help-content-tpl .hpbx-help-edit-container').clone();
                    
                    $help_content.addClass(edit_container_class);
                    
                    // Get correct buttons.
                    var buttons = getWysiwygButtons(block_type);
                    
                    // Get a new toolbar identifier.
                    var id = newToolbarId();
                    
                    if (buttons.length) {
                      
                      // Set toolbar id.
                      $help_content.find('.hpbx-wysiwyg-toolbar').attr('id', 'wysiwyg-toolbar-wysiwyg-' + id);
                      
                      // Set buttons.
                      $.each(buttons, function(key, obj){
                        var a = $('<a>')
                        .addClass('sg-btn sg-btn-small sg-btn-secondary sg-without-icon')
                        .attr('data-wysihtml5-command', obj.cmd)
                        .attr('data-wysihtml5-command-value', obj.value)
                        .html(Drupal.t(obj.label));
                        
                        $help_content.find('.hpbx-wysiwyg-toolbar').append(a);
                      });
                      
                      var hpbx_wysiwyg_toolbar_dialog = $('#hpbx-wysiwyg-toolbar-dialog-tpl .hpbx-wysiwyg-toolbar-dialog').clone();
                      $help_content.find('.hpbx-wysiwyg-toolbar').append(hpbx_wysiwyg_toolbar_dialog);
                    }
                    
                    // Editor.
                    var editor = $('<div/>')
                    .addClass('hpbx-wysiwyg')
                    .attr('id','wysiwyg-editor-wysiwyg-' + id)
                    .attr('data-toolbar-id', 'wysiwyg-toolbar-wysiwyg-' + id)
                    .attr('data-toolbar-block-type', block_type)
                    .html(block_content);
                    
                    // Append editor to container.
                    $help_content.find('.hpbx-wysiwyg-container').append(editor);
                    
                    var t = setTimeout(function(){ 
                      $element.find('.hpbx-wysiwyg').each(function() {
                        var editor = $(this);
                        var wysiwyg = new wysihtml5.Editor(editor.attr('id'), 
                                                           {toolbar: editor.data('toolbar-id'), parserRules: wysihtml5ParserRules});
                      });
                    },500);        
                    
                    // Add block. 
                    var $hpbx_block = $('<div>')
                    .addClass('hpbx-block')
                    .append($help_content);
                    
                    $block.replaceWith($hpbx_block);
    }
    
    $('.hpbx-help-container').each(function(){
      
      if ($(this).data('block-type') == 'text') {
        
        generateWysiwygEditor($(this), 'hpbx-help-edit-wysiwyg');
      }
      else if ($(this).data('block-type') == 'instruction') {
        generateInstruction($(this));
      }
      else if ($(this).data('block-type') == 'photo') {
        generatePhotoEditor($(this), true);
      }
      else if ($(this).data('block-type') == 'file') {
        generateFileEditor($(this));
      }
      else if ($(this).data('block-type') == 'video') {
        generateVideoEditor($(this));
      }
    });
    
    function generatePhotoEditor($element, removal_allowed) {
      var block_type    = $element.data('block-type'),
                    $block            = $element.find('.hpbx-help-placeholder-photo-block');
                    $saved_items      = $element.find('.hpbx-help-placeholder-photo-block img');
                    
                    // Clone from template.
                    var $help_content = $('#hpbx-help-content-photos-tpl .hpbx-help-edit-container').clone();
                    var $saved_item   = $('#hpbx-help-saved-item-tpl .hpbx-help-edit-saved-item').clone();
                    
                    $.each($saved_items, function(i, img){
                      
                      $new_saved_item = $saved_item.clone();
                      
                      if (!removal_allowed) {
                        $new_saved_item.find('.hpbx-delete-saved-item').remove();
                      }
                      // Change cloned data.
                      $new_saved_item.attr('data-fc-delta', $(img).data('fc-delta'));
                      $new_saved_item.find('.hpbx-help-edit-thumbnail-inner').css('background-image', 'url(' + $(img).attr('src') + ')');
                      $new_saved_item.find('.hpbx-help-edit-saved-item-title').html($(img).data('filename'));
                      $new_saved_item.find('.hpbx-help-edit-saved-item-subtitle').html($(img).data('filesize'));
                      
                      // Append
                      $help_content.find('.hpbx-help-edit-saved-items').append($new_saved_item);  
                    });
                    
                    // Add block. 
                    var $hpbx_block = $('<div>')
                    .addClass('hpbx-block')
                    .append($help_content);
                    
                    $block.replaceWith($hpbx_block);
                    
                    var photo_id = newPhotoNameId();
                    
                    var $file_input = $hpbx_block.find('.hpbx-file-upload-link + input[type="file"]')
                    .attr('id', 'hpbx-photo-' + photo_id);
                    
                    $hpbx_block.find('.hpbx-file-upload-link').on('click', function(e) {
                      e.preventDefault();
                      $(this).find('+ input[type="file"]').trigger('click');
                    });
                    
                    var inputField = $hpbx_block.find('.hpbx-file-upload-link + input[type="file"]');
                    
                    var label = inputField.parent();
                    
                    inputField.on('change', { inputField: inputField, label: label }, function(event) {
                      
                      var filename = event.data.inputField.val().split('\\').pop();
                      //event.data.label.html(filename);
                      
                    });
    }
    
    
    function generateFileEditor($element) {
      var block_type    = $element.data('block-type'),
                    $block            = $element.find('.hpbx-help-placeholder-file-block');
                    $saved_items      = $element.find('.hpbx-help-placeholder-file-block img');
                    
                    // Clone from photo template.
                    var $help_content = $('#hpbx-help-content-photos-tpl .hpbx-help-edit-container').clone();
                    var $saved_item   = $('#hpbx-help-saved-item-tpl .hpbx-help-edit-saved-item').clone();
                    
                    $help_content.find('.hpbx-help-edit-header-title-label').html(Drupal.t('Add files'));
                    
                    $.each($saved_items, function(i, img){
                      
                      $new_saved_item = $saved_item.clone();
                      
                      $new_saved_item.find('.hpbx-help-edit-thumbnail-container')
                      .removeClass('hpbx-help-edit-thumbnail-container')
                      .addClass('hpbx-help-edit-file-type-container');
                      
                      $new_saved_item.find('.hpbx-help-edit-thumbnail-outer').removeClass('hpbx-help-edit-thumbnail-outer');
                      
                      // Change cloned data.
                      $new_saved_item.find('.hpbx-help-edit-thumbnail-inner')
                      .removeClass('hpbx-help-edit-thumbnail-inner')
                      .addClass('hpbx-help-edit-file-type-image')
                      .css('background-image', 'url(' + $(img).attr('src') + ')');
                      
                      $new_saved_item.find('.hpbx-help-edit-saved-item-title').html($(img).data('filename'));
                      $new_saved_item.find('.hpbx-help-edit-saved-item-subtitle').html($(img).data('filesize'));
                      
                      // Append
                      $help_content.find('.hpbx-help-edit-saved-items').append($new_saved_item);  
                    });
                    
                    // Add block. 
                    var $hpbx_block = $('<div>')
                    .addClass('hpbx-block')
                    .append($help_content);
                    
                    $block.replaceWith($hpbx_block);
                    
                    var photo_id = newPhotoNameId();
                    
                    var $file_input = $hpbx_block.find('.hpbx-file-upload-link + input[type="file"]')
                    .attr('id', 'hpbx-file-' + photo_id);
                    
                    $hpbx_block.find('.hpbx-file-upload-link').on('click', function(e) {
                      e.preventDefault();
                      $(this).find('+ input[type="file"]').trigger('click');
                    });
                    
                    var inputField = $hpbx_block.find('.hpbx-file-upload-link + input[type="file"]');
                    
                    var label = inputField.parent();
                    
                    inputField.on('change', { inputField: inputField, label: label }, function(event) {
                      
                      var filename = event.data.inputField.val().split('\\').pop();
                      //event.data.label.html(filename);
                      
                    });
    }
    
    $(document).on('click', '.hpbx-help-instruction-add-next', function(){
      $(this).parents('.hpbx-help-container').find('.hpbx-help-edit-add-link[data-block-type=instruction]').trigger('click');
      return false;
    });
    
    $(document).on('click', '.hpbx-help-block-remove', function(){
      $(this).parents('.hpbx-help-container').slideUp("normal", function() { $(this).remove(); } );
      return false;
    });

    $(document).on('click', '.hpbx-help-edit-add-link', function(){
      var fc_delta      = newDelta();
      var $footer       = $('#hpbx-help-block-footer-tpl .hpbx-help-edit-footer').clone();
      var $links        = $('#hpbx-help-edit-add-tpl .hpbx-help-edit-add').clone();      
      
      $container        = $('<div class="hpbx-help-container" data-block-type="'+ $(this).data('block-type') +'" data-fc-delta="' + fc_delta+ '">');
      
      switch ($(this).data('block-type')) {
        case 'text':
          $container.append('<div class="hpbx-help-placeholder-wysiwyg-block"></div>');
          generateWysiwygEditor($container, 'hpbx-help-edit-wysiwyg');
          break;
        case 'file': 
          $container.append('<div class="hpbx-help-placeholder-file-block"></div>');
          generateFileEditor($container);
          break;
        case 'photo': 
          $container.append('<div class="hpbx-help-placeholder-photo-block"></div>');
          generatePhotoEditor($container, true);
          break;
        case 'video': 
          $container.append('<div class="hpbx-help-placeholder-video-block"></div>');
          generateVideoEditor($container);
          break;
        case 'instruction':
          $container.append('<div class="hpbx-help-placeholder-wysiwyg-block"></div>');
          $container.append('<div class="hpbx-help-placeholder-photo-block"></div>');
          generateInstruction($container);
          break;
      }
      
      $container.append($footer);
      
      if ($(this).data('block-type') == 'instruction') {
        $footer.find('.row div:eq(1)').append('<div class="hpbx-help-edit-footer-left"><a class="hpbx-help-edit-link-button hpbx-help-instruction-add-next" href="#">' + Drupal.t('Add next step') + '</a></div>');
        
      }
      
      $container.append($links);
      $container.insertAfter($(this).parents('.hpbx-help-container'));
    });
    
    $(document).on('click', '.hpbx-delete-saved-item', function(){
      $(this).parents('.hpbx-help-edit-saved-item').attr("data-is-deleted", "1");
      $(this).parents('.hpbx-help-edit-saved-item').hide();
    });
    
    $(document).on('click', '#hpbx-help-article-node-form input.form-submit', function(event){
      
      $('.hpbx-help-container').each(function(index){
        
        if ($(this).data('block-type')!==undefined) {
          
          if ($(this).data('block-type') == 'video') {
            var video_url = $(this).find('input').val();
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][video_url]').val(video_url);
            $('#hpbx-help-article-node-form').append(input);
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][field]').val('field_hpbx_video');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][item_id]').val($(this).data('fc-item-id'));
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][revision_id]').val($(this).data('fc-revision-id'));
            $('#hpbx-help-article-node-form').append(input);
          }
          else if ($(this).data('block-type') == 'text') {
            var value = $(this).find('.wysihtml5-editor').html();
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][value]').val(value);
            $('#hpbx-help-article-node-form').append(input);
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][field]').val('field_hpbx_body');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][item_id]').val($(this).data('fc-item-id'));
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][revision_id]').val($(this).data('fc-revision-id'));
            $('#hpbx-help-article-node-form').append(input);
          }
          else if ($(this).data('block-type') == 'instruction') {
            var value = $(this).find('.wysihtml5-editor').html();
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][value]').val(value);
            $('#hpbx-help-article-node-form').append(input);
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][field]').val('field_hpbx_instruction_photo');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][item_id]').val($(this).data('fc-item-id'));
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][revision_id]').val($(this).data('fc-revision-id'));
            $('#hpbx-help-article-node-form').append(input);
            
            $(this).find('[type=file]').attr('name', 'files[photo_' + (index-1) + ']');
            
            // Find deleted photo's.
            $(this).find('[data-is-deleted="1"]').each(function(){
              
              var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][deleted][' + $(this).data('fc-delta')+ ']').val('1');
              $('#hpbx-help-article-node-form').append(input);
            });
            
          }
          else if ($(this).data('block-type') == 'photo') {
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + ']').val('file');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][field]').val('field_hpbx_photo');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][item_id]').val($(this).data('fc-item-id'));
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][revision_id]').val($(this).data('fc-revision-id'));
            $('#hpbx-help-article-node-form').append(input);
            
            $(this).find('[type=file]').attr('name', 'files[photo_' + (index-1) + ']');
            
            // Find deleted photo's.
            $(this).find('[data-is-deleted="1"]').each(function(){
              
              var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][deleted][' + $(this).data('fc-delta')+ ']').val('1');
              $('#hpbx-help-article-node-form').append(input);
            });
          }
          else if ($(this).data('block-type') == 'file') {
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + ']').val('file');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][field]').val('field_hpbx_file');
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][item_id]').val($(this).data('fc-item-id'));
            $('#hpbx-help-article-node-form').append(input);  
            
            var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][revision_id]').val($(this).data('fc-revision-id'));
            $('#hpbx-help-article-node-form').append(input);
            
            $(this).find('[type=file]').attr('name', 'files[file_' + (index-1) + ']');
            
            // Find deleted file's.
            $(this).find('[data-is-deleted="1"]').each(function(){
              
              var input = $("<input>").attr('type', 'hidden').attr('name', 'block[' + (index-1) + '][deleted][' + $(this).data('fc-delta')+ ']').val('1');
              $('#hpbx-help-article-node-form').append(input);
            });
          }
        }
      });
      
      // Add the clicked button as op=submit|delete, as the submit buttons are not
      // submitted (form is submitted by jquery not using 'a click').
      var input = $("<input>").attr("type", "hidden").attr("name", "op").val($(event.currentTarget).val());
      $('#hpbx-help-article-node-form').append(input);

      event.preventDefault();
      $('#hpbx-help-article-node-form').submit();      
      
    });
  });
}(window.jQuery, window, document));