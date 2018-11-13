(function($, window, document) {
  
  
  $(document).ready(function(){
    
    var inputField = $('input[type="file"]');
    var label      = $('.sg-btn-file-input-label');

    inputField.on('change', { inputField: inputField}, function(event) {
      inputField.parents('form').submit();
      
    });

    // Handle device list select box (split up as vendor and type) 
	//Commented since the functionality is implemented in Trimm - For Tp Issue 9530
    $('select[data-device-list]').each(function (index, element) {

      var $source = $(element),
        $target = $source.closest('.hpbx-bulk-upload-data-row').find('.hpbx-bulk-upload-field-device-type'),
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
            $target.append($('<option value="' + type.value + '">' + type.label + '</option>').prop('selected', selected_device != '' && type.label == selected_device));
          });
          $target.data('isset', true);
          $target.trigger('liszt:updated');
        }
      });
    });
  });
  

}(window.jQuery, window, document));


function hpbxBulkUploadSendNextRow() {
  if (hpbxBulkUploadSetupNextRow()) {
    
    $.ajax({
      type: 'POST',
      url: Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/subscriber/bulk/submit',
      dataType: 'json',
      data: hpbxBulkUploadCurrentRowData,
    })
    .done(function(response) {

      var isSuccessful = response.isSuccessful;    
      var message = '';
      if (response.errors.length) {
        isSuccessful = false;
        $.each(response.errors, function(key, error){
          message = message + error.message + '<br/>';
        });
      }
      hpbxBulkUploadSetRowStatus(isSuccessful, message); 
      hpbxBulkUploadSendNextRow();

    })
    .fail(function(jqXHR, message) {
      
      var isSuccessful = false;
      var message = Drupal.t('Uploading data failed for an unknown reason. Please try again.');

      hpbxBulkUploadSetRowStatus(isSuccessful, message);
    });
  }
  else {
    hpbxBulkUploadEndOfProcess();
  }
}