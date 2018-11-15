

(function ($) {

  var disable_speeddial;
  var disable_queue;
  var disable_callforward;
  var disable_voicemail;

  $('.r-tabs-nav li:eq(1) a.r-tabs-anchor').unbind('click').on('click', function(e){
    if (disable_callforward) e.preventDefault();
  });

  $('.r-tabs-nav li:eq(2) a.r-tabs-anchor').unbind('click').on('click', function(e){
    if (disable_voicemail) e.preventDefault();
  });
  $('.r-tabs-nav li:eq(3) a.r-tabs-anchor').unbind('click').on('click', function(e){
    if (disable_speeddial) e.preventDefault();
  });
  
  var bindProfile = function (target) {
  	var query = {}; 
  	
  	if(target != ''){
	   query.groupname =target;	   	  
   	   var url = 'hpbx/stretto/group?';
   	  
   	   $.ajax({
         url: Drupal.settings.basePath + Drupal.settings.pathPrefix + url + $.param(query),
         cache: false,
         dataType: 'json'
       })
       .done(function( data ) {
         
         $('#edit-general-stretto-desktop-profile-value').find('option[value!=""]').remove();
         $('#edit-general-stretto-mobile-profile-value').find('option[value!=""]').remove();
         $('#edit-general-stretto-mobiledesktop-profile-value').find('option[value!=""]').remove();
          	
         stretto_desktop_selected = $('#edit-general-stretto-desktop-profile-value').data('selected-value');
         stretto_mobile_selected = $('#edit-general-stretto-mobile-profile-value').data('selected-value');
         stretto_mobiledesktop_selected = $('#edit-general-stretto-mobiledesktop-profile-value').data('selected-value');
          	
         $.each(data, function (key, profiles) {
           //profiles = profiles.replace(/\./g,' ');
           $('#edit-general-stretto-desktop-profile-value').append($('<option value="' + key + '">' + profiles + '</option>').prop('selected', key == stretto_desktop_selected));
           $('#edit-general-stretto-mobile-profile-value').append($('<option value="' + key + '">' + profiles + '</option>').prop('selected', key == stretto_mobile_selected));
           $('#edit-general-stretto-mobiledesktop-profile-value').append($('<option value="' + key + '">' + profiles + '</option>').prop('selected', key == stretto_mobiledesktop_selected));
          		
         });
         $('#edit-general-stretto-desktop-profile-value').trigger('liszt:updated');
         $('#edit-general-stretto-mobile-profile-value').trigger('liszt:updated');
         $('#edit-general-stretto-mobiledesktop-profile-value').trigger('liszt:updated');
          	        
       });
	 }
	 else{
	   $('#edit-general-stretto-desktop-profile-value').find('option').remove();
       $('#edit-general-stretto-mobile-profile-value').find('option').remove();
       $('#edit-general-stretto-mobiledesktop-profile-value').find('option').remove();
        
       $('#edit-general-stretto-desktop-profile-value').trigger('liszt:updated');
       $('#edit-general-stretto-mobile-profile-value').trigger('liszt:updated');
       $('#edit-general-stretto-mobiledesktop-profile-value').trigger('liszt:updated');
	 } 
	 
		
  }
  //Softclient show/hide based on profile
  function bindsoftclient(softclient) {
	
  if (Drupal.settings['subscriber_profile']!=undefined) { 
    profile = Drupal.settings['subscriber_profile'];
    if( (profile[softclient]['mobile_softclient'] == true) || (profile[softclient]['desktop_softclient'] == true)) { 
      if($('input.hpbx-subscriber-no-selfcare').is(':checked')){
        $('.hpbx-no-softclient').prop('checked', false).prop('disabled', 'disabled');
      }
      else{
        $('.hpbx-no-softclient').prop('disabled', false);
      }
	  }
	  else{
	    $('.hpbx-no-softclient').prop('checked', false).prop('disabled', 'disabled');
 	  }	    
  }
  	
}


  $('document').ready(function() {
     $('[id$=length-data]').on('change', function () { $(this).trigger('liszt:updated');console.log($(this).val());});
	 
	  if($('#edit-table-alias-number-value').find('option:selected').text()=='None'){
    	  $('#caller_line_identification').show();
      }else{
    	  $('#caller_line_identification').hide();
      }
   //This will toggling the QR Code on "For Your Mobile" section.
   $("div.qr-toggle").click(function(){
     $(this).toggleClass('qr-show');
   });
   
  	
  	bindsoftclient($('#edit-general-settings-profile-id-value').val());
  	$('#edit-general-settings-profile-id-value, input.hpbx-subscriber-no-selfcare').on('change', function (a, b){
	    bindsoftclient($('#edit-general-settings-profile-id-value').val());
	  });
  
    bindProfile($('#edit-general-stretto-group-value').val());
    $('#edit-general-stretto-group-value').bind('change', function (a, b){   	   	 
      bindProfile($(this).val());
    });

    // Add the hash to the page header anchor's
    $('.page-hpbx-fax .hpbx-table-sortable th a, .page-hpbx-fax .hpbx-pagination-item-content').each(function(){
      //Modified to fix the bug #9351 - Fax log pagination not working.
      //$(this).attr('href', $(this).attr('href') + window.location.hash);
      $(this).attr('href', $(this).attr('href') + '#tab-fax-log');
    });

    //Added to fix the bug #9351 - Fax log pagination not working.
    //Optimized solution provide in the above section. So commented this section
    /*$('a.r-tabs-anchor').click(function(event){
	  if($(this).context.text == Drupal.t('Fax log')){
         $('.page-hpbx-fax .hpbx-pagination-item-content').each(function(){
                  $(this).attr('href', $(this).attr('href') + '#tab-fax-log');
         });
      }
     });*/


    if (!$('body').hasClass('page-hpbx-subscriber-bulk')) {

      // Handle device list select box (split up as vendor and type)
      $('select[data-device-list]').each(function (index, element) {

        var $source = $(element),
          $target = $($source.data('device-type-target')),
          devices = $source.data('device-list'),
          options = [],
          selected = $source.data('selected-value');

        $source.find('option[value!=""]').remove();

        // first populate vendor dropdown
        $.each(devices, function (key, vendor) {

          // add to vender dropdown list
          $source.append($('<option value="' + key + '">' + vendor.label + '</option>').prop('selected', key == selected));

          // cache list of all types
          $.each(vendor.types, function (key, type) {
            options.push(type);
          });
        });

        $source.trigger('liszt:updated');

        // update target select on change
        $source.on('change', function () {
        if (!$target.data('isset')){
          var selected = $source.val();
          var selected_device = $target.data('selected-value');

          $target.find('option[value!=""]').remove();
          $.each(devices[selected] !== undefined ? devices[selected].types : options, function (index, type) {
            $target.append($('<option value="' + type.value + '">' + type.label + '</option>').prop('selected', selected_device != '' && type.value == selected_device));
          });
          $target.data('isset', true);
          $target.trigger('liszt:updated');
		}
        });
      });
    }
	

    if ($('.hpbx-subscriber-no-selfcare').length) {
      $('#edit-general-actions-submit').bind('click', function (event) {
        $('.hpbx-subscriber-no-selfcare').prop('disabled', false);
      });
    }

    if ($('[name=customer_id]').length && $('[name=customer_id]').val()!='') {

      $('[id$=-prefix-data]').each(function(){
        var id = $(this).parents().eq(4).find('input[type=hidden]').val();
        $('body').data('orig-value-' +  id, $(this).val());
      });

      $('#hpbx-customer-prefix-change-warning-dialog input').on('click', function(){
        
        $('#edit-general-actions-submit').data('confirmed', true);
        $('#edit-general-actions-submit')[0].click();
        event.preventDefault();
      });

      $('#edit-general-actions-submit').on('click', function(event){
        
        if (!$('#edit-general-actions-submit').data('confirmed')) {

          trigger_prefix_warning = false;
          
          $('input[id*=-prefix-data]').each(function(){
            var id = $(this).parents().eq(4).find('input[type=hidden]').val();
            if ($('body').data('orig-value-' +  id)!==undefined && $(this).val() != $('body').data('orig-value-' +  id)) {
              trigger_prefix_warning = true;
            }
          });

          if (trigger_prefix_warning) {
            $('.hpbx-customer-prefix-change-warning-trigger')[0].click();
            event.preventDefault();
          }
        }
      });
    };

    if ($('.hpbx-menu-item-voicemail .hpbx-menu-item-count').length) {
      function voicemail_counter() {
        $.get( "/hpbx/voicemails/count")
        .done(function( data ) {
          $('.hpbx-menu-item-voicemail .hpbx-menu-item-count').html((data > 0) ? data : '');
        });
        setTimeout(function(){  voicemail_counter() }, 120000);
      }
      voicemail_counter();
    }

    $(document).on("edit-general-move-from-pilot-change", function(event, element) {

      if ($('input.hpbx-subscriber-move-from-pilot').is(':checked')) {

        $('input.hpbx-subscriber-firstname').addClass('hpbx-incognito').prop('readonly', true).val($('input[name=pilot_firstname]').val());
        $('input.hpbx-subscriber-lastname').addClass('hpbx-incognito').prop('readonly', true).val($('input[name=pilot_lastname]').val());
        $('input.hpbx-subscriber-email').addClass('hpbx-incognito').prop('readonly', true).val($('input[name=pilot_email]').val());
        $('input.hpbx-subscriber-administrative').prop('checked', true).prop('disabled', true);
      }
      else {
        $('input.hpbx-subscriber-no-selfcare').removeClass('sg-disabled').prop('disabled', false);
        $('input.hpbx-subscriber-firstname').removeClass('hpbx-incognito').prop('readonly', false).val('');
        $('input.hpbx-subscriber-lastname').removeClass('hpbx-incognito').prop('readonly', false).val('');
        $('input.hpbx-subscriber-email').removeClass('hpbx-incognito').prop('readonly', false).val('');
        $('input.hpbx-subscriber-administrative').removeClass('sg-disabled').prop('checked', false).prop('disabled', false);
      }
    });

    $('input.hpbx-subscriber-move-from-pilot').attr('checked', 'checked').trigger('change');
    
    if ($('#edit-general-settings-profile-id-value').length) {

      function toggle_profile_features_queue() {
        var profile_id = $('#edit-general-settings-profile-id-value option:selected').val();

        if (Drupal.settings.hpbx.profile_features_toggle[profile_id]!=='undefined') {
          $.each(Drupal.settings.hpbx.profile_features_toggle, function(p_id, p_attribute) {
            $.each(p_attribute, function(index2, p_a) {
              if (p_a == 'cloud_pbx_callqueue') disable_queue = true;
            });
          });
        }

        // Enable the attributes for the selected profile.
        $.each(Drupal.settings.hpbx.profile_features_toggle[profile_id], function(index, p_a) {
          if (p_a == 'cloud_pbx_callqueue') disable_queue = false;
        });

        if (disable_queue) {
          $('[data-toggle-type=hpbx-queue-toggle]').prop('checked', false).prop('disabled', true);
        }
        else {
          $('[data-toggle-type=hpbx-queue-toggle]').prop('disabled', false);
        }

        $('[data-toggle-type=hpbx-queue-toggle]').trigger('change');
      }
      
      function toggle_profile_features_speeddials() {
        var profile_id = $('#edit-general-settings-profile-id-value option:selected').val();
        
        if (Drupal.settings.hpbx.profile_features_toggle[profile_id]!=='undefined') {

          disable_callforward = false;
          disable_voicemail = false;
          disable_speeddial = false;

          // Disabled all fields which can be toggle.
          $.each(Drupal.settings.hpbx.profile_features_toggle, function(p_id, p_attribute) {
            $.each(p_attribute, function(index2, p_a) {
              if (p_a == 'callforward') disable_callforward = true;
              if (p_a == 'voice_mail') disable_voicemail = true;
              if (p_a == 'speed_dial') disable_speeddial = true;
            });
          });
        }

        // Enable the attributes for the selected profile.
        $.each(Drupal.settings.hpbx.profile_features_toggle[profile_id], function(index, p_a) {
          if (p_a == 'callforward') disable_callforward = false;
          if (p_a == 'voice_mail') disable_voicemail = false;
          if (p_a == 'speed_dial') disable_speeddial = false;
        });

        $('.r-tabs-nav li:eq(1)').toggleClass('r-tabs-state-disabled', disable_callforward);
        $('.r-tabs-nav li:eq(2)').toggleClass('r-tabs-state-disabled', disable_voicemail);
        $('.r-tabs-nav li:eq(3)').toggleClass('r-tabs-state-disabled', disable_speeddial);
      }
      
      function toggle_profile_features() {
        
        var profile_id = $('#edit-general-settings-profile-id-value option:selected').val();
        
        if (Drupal.settings.hpbx.profile_features_toggle[profile_id]!=='undefined') {
          
          // Disabled all fields which can be toggle.
          $.each(Drupal.settings.hpbx.profile_features_toggle, function(p_id, p_attribute) {
            $.each(p_attribute, function(index2, p_a) {
              p_a = p_a.replace(/_/g, '-');
              if ($('#edit-general-settings-'+ p_a  +'-value').length) {
                $('#edit-general-settings-'+ p_a  +'-value')
                  .addClass('sg-disabled')
                  .prop('disabled', 'disabled')
                  .trigger('liszt:updated'); // Last call only effective in case of a dropdown.
              }
            });
          });

          // Enable the attributes for the selected profile.
          $.each(Drupal.settings.hpbx.profile_features_toggle[profile_id], function(index, p_a) {
            p_a = p_a.replace(/_/g, '-');
            if ($('#edit-general-settings-'+ p_a  +'-value').length) {
              $('#edit-general-settings-'+ p_a  +'-value')
                .removeClass('sg-disabled')
                .prop('disabled', false)
                .trigger('liszt:updated'); // Last call only effective in case of a dropdown.
            }
          });
          
          $('.sg-disabled[type=text]').val('');
          $('.sg-disabled[type=checkbox]').prop('checked', false);
        }
      };
      
      $(document).on("edit-subscriber-general-profile-id-change", function(event, element) {
        toggle_profile_features_queue();
        toggle_profile_features_speeddials();
        toggle_profile_features();
      });
      toggle_profile_features_queue();
      toggle_profile_features_speeddials();
      toggle_profile_features();
    }

    $(document).on("edit-voicemail-attach-change", function(event, element) {
      
      if ($('.hpbx-voicemail-attach').is(":checked")) {
        $('.hpbx-voicemail-delete').removeClass('sg-disabled').prop('disabled', false);
      }
      else {
        $('.hpbx-voicemail-delete').addClass('sg-disabled').prop('checked', false).prop('disabled', 'disabled');
      }
    });
    
    $(document).trigger('edit-voicemail-attach-change');
    
    
    $(document).on("edit-subscriber-general-number-change", function(event, element) {
      
      var extension_from_did = $(element).data('derive-extension-from-did');
      var allow_subscribers_without_did = $(element).data('allow-subscribers-without-did');
      var extension_id = $(element).data('extension-id');
      var extension_value= $(element).data('extension-value');
      var form_id = $(element).data('form-id');
      
      if($(element).find('option:selected').text()=='None' && (form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form')){
    	  $('#caller_line_identification').show();
      }else if($(element).find('option:selected').text()!='None' && (form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form')){
    	  $('#caller_line_identification').hide();
      }
      
      if (extension_from_did) {
        
        var alias_number = parseInt($(element).find('option:selected').text());
        
        if ($.isNumeric(alias_number)) {
          if(form_id=='hpbx_subscriber_edit_form'){
	          var defaultVal = $("select#edit-general-settings-cli-value option[value='default']").text();
	          if(defaultVal==''){
	        	  $("#edit-general-settings-cli-value").append('<option value="default">Own Number</option>');
	        	  $('#edit-general-settings-cli-value')
	                .trigger('liszt:updated');
	          }
          }
          if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form'){
	          var defaultVal = $("select#edit-table-cli-value option[value='default']").text();
	          if(defaultVal==''){
	        	  $("#edit-table-cli-value").append('<option value="default">Own Number</option>');
	        	  $('#edit-table-cli-value')
	                .addClass('sg-disabled')
	                .prop('disabled', true)
	                .trigger('liszt:updated');
	          }
          }
          if(form_id=='hpbx_wizard_employee_form'){
	          var defaultVal = $("select#edit-subscriber-cli option[value='default']").text();
	          if(defaultVal==''){
	        	  $("#edit-subscriber-cli").append('<option value="default">Own Number</option>');
                $('#edit-subscriber-cli')
	                .trigger('liszt:updated');
	          }
          }
          var json = $('#hpbx-numberranges').data('json').replace(/'/g, "\"");
          var numberranges = $.parseJSON(json);
          var extension_prefix = '';
          
          $(numberranges).each(function(idx, range) {
            
            var start = parseInt(range.area + range.start);
            var end = start + range.length - 1;
            if (alias_number >= start && alias_number <= end) {
            	if(range.prefix){ 
	              if (range.prefix.length) {
	                extension_prefix = range.prefix;
	              }
	            }
              return false;
            }
          });

          var extension_length = $(element).data('extension-length');
          var extension_suffix_length = extension_length - extension_prefix.length;
          var extension_id = $(element).data('extension-id');
          var extension = extension_prefix + $(element).find('option:selected').text().slice(extension_suffix_length * -1);
          if(allow_subscribers_without_did){
        	  $(extension_id).attr("readonly", false);
        	  $(extension_id).removeClass().addClass('sg-element form-text');
          }
          $(extension_id).val(extension);
        }
        else if ($(element).find('option:selected').text()=='None'){
        	if(form_id=='hpbx_subscriber_edit_form'){
	        	$("select#edit-general-settings-cli-value option[value='default']").remove();
	        	$('#edit-general-settings-cli-value')
                .trigger('liszt:updated');
        	}
        	if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form'){
	        	$("select#edit-table-cli-value option[value='default']").remove();
	        	$('#edit-table-cli-value')
                .removeClass('sg-disabled')
                .prop('disabled', false)
                .trigger('liszt:updated');
        	}
        	if(form_id=='hpbx_wizard_employee_form'){
	        	$("select#edit-subscriber-cli option[value='default']").remove();
	        	$('#edit-subscriber-cli')
                .removeClass('sg-disabled')
                .prop('disabled', false)
                .trigger('liszt:updated');
        	}
        	$(extension_id).attr("readonly", false);
        	$(extension_id).removeClass().addClass('sg-element form-text');
        	if(extension_value){
        		$(extension_id).val(extension_value);
        	}
        }else{
        	if($(element).find('option:selected').text()=='-Choose Number-'){
        		if(allow_subscribers_without_did){
        			$(extension_id).removeClass().addClass('sg-element form-text');
        		}else{
        			$(extension_id).attr("readonly", true);
        		}
        	}
        	if(extension_value){
        		$(extension_id).val(extension_value);
        	}else{
        		$(extension_id).val('');
        	}
			if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form'){
				$('#edit-table-cli-value')
				.addClass('sg-disabled')
				.prop('disabled', true)
				.trigger('liszt:updated');
			}
        }
      }else if(allow_subscribers_without_did){
    	  var alias_number = parseInt($(element).find('option:selected').text());
    	  if ($.isNumeric(alias_number)) {
              if(form_id=='hpbx_subscriber_edit_form'){
    	          var defaultVal = $("select#edit-general-settings-cli-value option[value='default']").text();
    	          if(defaultVal==''){
    	        	  $("#edit-general-settings-cli-value").append('<option value="default">Own Number</option>');
    	        	  $('#edit-general-settings-cli-value')
    	                .trigger('liszt:updated');
    	          }
              }
              if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form'){
    	          var defaultVal = $("select#edit-table-cli-value option[value='default']").text();
    	          if(defaultVal==''){
    	        	  $("#edit-table-cli-value").append('<option value="default">Own Number</option>');
    	        	  $('#edit-table-cli-value')
    	                .addClass('sg-disabled')
    	                .prop('disabled', true)
    	                .trigger('liszt:updated');
    	          }
              }
              if(form_id=='hpbx_wizard_employee_form'){
    	          var defaultVal = $("select#edit-subscriber-cli option[value='default']").text();
    	          if(defaultVal==''){
    	        	  $("#edit-subscriber-cli").append('<option value="default">Own Number</option>');
    	          }
              }
              $(extension_id).attr("readonly", false);
              $(extension_id).removeClass().addClass('sg-element form-text');
              if(extension_value){
            		$(extension_id).val(extension_value);
              }
            }
            else if ($(element).find('option:selected').text()=='None'){
            	if(form_id=='hpbx_subscriber_edit_form'){
    	        	$("select#edit-general-settings-cli-value option[value='default']").remove();
    	        	$('#edit-general-settings-cli-value')
	                .trigger('liszt:updated');
            	}
            	if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' || form_id=='hpbx_conference_edit_form'){
    	        	$("select#edit-table-cli-value option[value='default']").remove();
    	        	$('#edit-table-cli-value')
                    .removeClass('sg-disabled')
                    .prop('disabled', false)
                    .trigger('liszt:updated');
            	}
            	if(form_id=='hpbx_wizard_employee_form'){
    	        	$("select#edit-subscriber-cli option[value='default']").remove();
    	        	$('#edit-subscriber-cli')
                    .removeClass('sg-disabled')
                    .prop('disabled', false)
                    .trigger('liszt:updated');
            	}
            	$(extension_id).attr("readonly", false);
            	$(extension_id).removeClass().addClass('sg-element form-text');
            	if(extension_value){
            		$(extension_id).val(extension_value);
            	}
                }else{
            	if($(element).find('option:selected').text()=='-Choose Number-'){
            		$(extension_id).removeClass().addClass('sg-element form-text');
            	}
				$(extension_id).val('');
    	            if(form_id=='hpbx_autoattendant_edit_form' || form_id=='hpbx_huntgroup_edit_form' ||form_id=='hpbx_conference_edit_form'){
    	  	          	$('#edit-table-cli-value')
    	  	            .addClass('sg-disabled')
    	  	            .prop('disabled', true)
    	  	            .trigger('liszt:updated');
    	            }
            }
      }
    });
    
    $('form').ajaxStart(function(){
      $('span.submit-button input[type=submit]').prop('disabled', true);
    });
    
    $('form').ajaxSuccess(function(){
      $('span.submit-button input[type=submit]').prop('disabled', false);

      var state = parseInt($('#edit-general-table-derive-extension-from-did-value option:selected').val()) || 0;
      $("input[id*='-prefix-data']").toggleClass('sg-disabled', !state).prop('disabled', !state);
    });

    $('.js-delete').on('click', function(event){
      
      var delete_url = $(this).closest('tr').data('delete-url');
      
      if (delete_url) {
        hpbxShowConfirm(
          Drupal.t('Are you sure you want to remove this item? This action cannot be undone.'),
          Drupal.t('Yes'),
          function() {
            $('.hpbx-alert').removeClass('hpbx-alert-show');
            window.location = delete_url;
          },
          Drupal.t('No'),
          function() {
            $('.hpbx-alert').removeClass('hpbx-alert-show');
            return false;
          }
        );
      }
      return false;
    });
    
    // Based on form errors, switch to correct tab.
    var sel = [];
    $('.sg-error').parents('.r-tabs-panel').each(function(){
      sel.push('#' + $(this).attr('id'));
    });
    
    // Need this to get the correct order (We don't want to switch to speeddials
    // in case there is an error in general tab.
    $(sel.join(', ')).each(function(){
      if ($(this).attr('id') == 'tab-general') {
        return false;
      }
      document.location.href = '#' + $(this).attr('id'); 
      return false;
    });
    
   // Disabled the submit in send fax page
   //Modified for the bug #9571 - PE DEV: Send Fax tab - page is redirected to fax numbers page when user forgets the file upload     
  var form_id = $('form').attr('id');
   if(form_id == 'hpbx-fax-sendfax-form'){
      $('#edit-submit').prop('disabled', true);
    }

   //enabling the submit when the file is uploaded 
   $('input:file').change(function(e){      
      if ($(this).val() != "") {         
         $('#edit-submit').removeAttr('disabled');
         $('#edit-submit').removeClass('sg-disabled');
                  
      } 
    });
  });
  
  //to hide the customer field in script page 
  $('#edit-script-name').on('change',function(){
  	var option = $(this).find('option:selected').val(); 
    if(option == 'reseller_gpp_update'){
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(2)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(3)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(4)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(5)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(6)").show();     			
    }else if(option == 'panasonic_rds_update'){    	
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(2)").show();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(3)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(4)").show();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(5)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(6)").hide();	    			
	}else if(option == 'yealink_rds_update'){
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(3)").show();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(4)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(5)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(6)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(2)").hide();
	}else if((option != 'panasonic_rds_update' || option != 'reseller_gpp_update' || option != 'yealink_rds_update') && option !=''){
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(2)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(3)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(4)").hide();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(5)").show();
	  $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(6)").show();	   
	}else{
	   $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(2)").show();
	   $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(3)").show();
	   $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(4)").show();
	   $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(5)").show();
	   $("#hpbx-sipwise-script-form > div > div:nth-last-of-type(6)").show();	   
	}    
  });
})(jQuery);





