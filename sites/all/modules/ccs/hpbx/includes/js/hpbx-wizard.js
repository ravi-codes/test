(function($, window, document) {

  $( document ).ready(function(){
    // Verify if we are at a result page of the device syncing callback.
    var url_state =  window.location.pathname.substring(window.location.pathname.lastIndexOf('/') + 1);
    if (url_state == 'failed') {
      window.wizardGoToStep(6);
    }
    else if (url_state == 'success') {
      window.wizardGoToStep(6);
    }
  });

  window.wizardSubmit = wizardSubmit;
  window.wizardBackToStartCustom = wizardBackToStartCustom;

  function setErrors(form_errors) {

    var html = '';
    html += '<div class="hpbx-notification hpbx-notification-error">';
    html += '   <div class="hpbx-notification-icon"></div>';
    html += '   <div class="hpbx-notification-title"></div>';
    html += '   <div class="hpbx-notification-message"><ul>';
    $(form_errors).each(function(index, obj) {
      html += '<li>' + obj.message + '</li>';
    });
    html += '</u></div></div>';
    $('.container').append(html);
    
    $(".hpbx-notification").each(function() {
      var $notification = $(this),delay = 5000;
      $notification.delay(delay).slideUp();
    });
  }
  
  function wizardBackToStartCustom() {
    $('#hpbx-wizard-employee-form')[0].reset();
    window.wizardBackToStart();
  }
  
  function wizardSubmit(type) {

    $('.sg-element').removeClass('sg-error');
    $('.hpbx-notification').remove();
    $('[name=current_step]').val(type);

    if (type == 'subscriber') {
      url = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/wizard/callback/subscriber/submit';
      var translation_type = Drupal.t('Subscriber');
    }
    else if (type == 'device') {
      url = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/wizard/callback/device/submit';
      var translation_type = Drupal.t('Device');
    }
    else if (type == 'sync') {
      //console.log(window.sync_url);
      //console.log(window.sync_ip_val);
      //console.log($(window.sync_ip_val).val());
      window.location = window.sync_url + $(window.sync_ip_val).val();
      return false;
    }

    hpbxShowLoader(Drupal.t('Please wait'), Drupal.t('The @type is currently being created.', {'@type': translation_type}));

    $.ajax({
      method: 'POST',
      dataType: 'json',
      data: $('#hpbx-wizard-employee-form').serialize(),
      url: url
    })
    .done(function( data ) {
      
      hpbxHideLoader();
      
      if (data['success']) {
        
        var handler = data['handler']!=undefined ? data['handler'] : false;
        var handler_argument = data['handler_argument']!=undefined ? data['handler_argument'] : null;

        var handlerFunction = window[handler];
        
        if(typeof handlerFunction === 'function') {
          handlerFunction(handler_argument);
        }
 
        if (type == 'device') {
          window.sync_url = data['sync_url'];
          window.sync_ip_val = data['sync_ip_val'];
        }
      }
      else {
        
        var step_back = false;
        $(data.form_errors).each(function(index, obj) {
          
          var error_type = type;
          
          if (type == 'device') {
            error_type = 'pbxdevice';
          }
          $('#edit-' + error_type + '-' + obj.element.replace('_', '-')).addClass('sg-error');
          
          // Need to go back to previous step?
          if (type == 'subscriber' && (obj.element == 'firstname' || obj.element == 'lastname' || obj.element == 'email' || obj.element == 'external_id')) {
            step_back = true;
          }
        });
        
        // Set error messages.
        setErrors(data.form_errors);
        
        // Need to go back to previous step?
        if (step_back) {
          window.wizardPreviousStep();
        }
      }
    })
    .fail(function( jqXHR, textStatus ) {
      console.log( "Request failed: " + textStatus );
    });
  }
  
}(window.jQuery, window, document));