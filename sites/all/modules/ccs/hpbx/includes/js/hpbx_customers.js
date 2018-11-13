(function($, window, document) {

  $(document).ready(function () {	
	var isAjaxing = false
	$( "select[id^='edit-general-emergency-table-']" ).each(function(){		 
		 $(this).chosen().next(".chzn-container").hover(function (e) {
		 	
		 	var query = {};
		 	
		 	if($(this).attr('id') == 'edit_general_emergency_table_township_value_chzn'){
		 	  var item = 'township';			  
			}else if($(this).attr('id') == 'edit_general_emergency_table_city_value_chzn'){
			  var item = 'city';		  
			}else if($(this).attr('id') == 'edit_general_emergency_table_z_gm_value_chzn'){
			  var item = 'z-gm';				  
			}
			  query.region = $('#edit-general-emergency-table-region-value').val();	
			  query.township = $('#edit-general-emergency-table-township-value').val();
			  query.city = $('#edit-general-emergency-table-city-value').val();	
			  		
		if (Drupal.settings['reseller_id']!=undefined) {
          var url = 'hpbx/affiliate/' + Drupal.settings['reseller_id'] + '/customer/location?';
        }
        else {
          var url = 'hpbx/customer/location?';
        }
        if(isAjaxing) return;
        isAjaxing = true;		
        $.ajax({
          url: Drupal.settings.basePath + Drupal.settings.pathPrefix + url + $.param(query),
          cache: false,
          dataType: 'json'
        })
        .done(function( data ) {
        	isAjaxing = false;        
			// Filter next level		   
            var $select = $('#edit-general-emergency-table-'+ item +'-value');
			var firstEnabled = null;
			var changeEnabled = false;
            $select.children('option').each(function () {
                var $this = $(this);
                if ($this.val() != '') {
                  if (data[item].indexOf($this.val()) > -1) {
                    $this.prop('disabled', false);
                    if (!firstEnabled) firstEnabled = $this;
                  } else {
                     $this.prop('disabled', true);
                     if ($this.is(':selected')) {
                       changeEnabled = true;
                     }
                    }
                  }
                });

                // Change selection for next level
			if (changeEnabled && firstEnabled) {
			  $select.val(firstEnabled.val());
			}

			// Update display
			$select.trigger('liszt:updated').change();  
		 });
	   });
	});
	 
    $(document).on("edit-bundle-settings-profileset-change", function(event, element) {

      var value = $(element).val();
      var profileset_id = $(element).data('profileset-id');
      $('tr[data-profileset-id='+profileset_id + ']').toggle(value=='limit_per_profile');
      $('input[data-profileset-id='+profileset_id + '].hpbx-bundle-settings-profileset-total-amount').toggle(value=='limit_total');
    });

    $('[data-custom-change-event=edit-bundle-settings-profileset-change]').trigger('change');


    $(document).on("edit-bundle-settings-profile-change", function(event, element) {

      var value = $(element).val();
      var profile_id = $(element).data('profile-id');
      $('input[data-profile-id='+profile_id + '].hpbx-bundle-settings-profile-total-amount').toggle(value=='input');
    });

    $('[data-custom-change-event=edit-bundle-settings-profile-change]').trigger('change');

    $('#edit-general-table-derive-extension-from-did-value').bind('change', function () {

      var state = parseInt($(this).val()) || 0;
      $("input[id*='-prefix-data']").toggleClass('sg-disabled', !state).prop('disabled', !state);

      if (!state) {
        $("input[id*='-prefix-data']").val('');
      }
    });

    $('#edit-general-table-derive-extension-from-did-value').trigger('change');
    
    


    if (Drupal.settings['hpbx_emergency_indexes'] !== undefined) {
      
      // On change of the "target" the "next" is filtered
      var hierarchyData = Drupal.settings['hpbx_emergency_indexes'];


      var bindFilter = function (target) {

        var others =  ['region', 'township', 'city', 'z-gm'];
        $('#edit-general-emergency-table-' + target + '-value').bind('change', function (a, b) {

          if (typeof b!='object') {
            return;
          }
          var query = {};
          if ($('#edit-general-emergency-table-region-value').val()!='') {
            query.region = $('#edit-general-emergency-table-region-value').val();
          }
          if ($('#edit-general-emergency-table-township-value').val()!='') {
            query.township = $('#edit-general-emergency-table-township-value').val();
          }	  
          if ($('#edit-general-emergency-table-city-value').val()!='') {
            query.city = $('#edit-general-emergency-table-city-value').val();
          }
          if ($('#edit-general-emergency-table-z-gm-value').val()!='') {
            query.zgm = $('#edit-general-emergency-table-z-gm-value').val();
          }

          var target_value = $('#edit-general-emergency-table-'+ target +'-value').val();

          if (Drupal.settings['reseller_id']!=undefined) {
            var url = 'hpbx/affiliate/' + Drupal.settings['reseller_id'] + '/customer/location?';
          }
          else {
            var url = 'hpbx/customer/location?';
          }
          			
          $.ajax({
            url: Drupal.settings.basePath + Drupal.settings.pathPrefix + url + $.param(query),
            cache: false,
            dataType: 'json'
          })
          .done(function( data ) {
            $(others).each(function(i, item) {
              if (target_value == '' || item != target) {

                // Filter next level
                var $select = $('#edit-general-emergency-table-' + item + '-value');
                var firstEnabled = null;
                var changeEnabled = false;
                $select.children('option').each(function () {
                  var $this = $(this);
                  if ($this.val() != '') {
                    if (data[item].indexOf($this.val()) > -1) {
                      $this.prop('disabled', false);
                      if (!firstEnabled) firstEnabled = $this;
                    } else {
                      $this.prop('disabled', true);
                      if ($this.is(':selected')) {
                        changeEnabled = true;
                      }
                    }
                  }
                });


                // Change selection for next level
                if (changeEnabled && firstEnabled) {
                  $select.val(firstEnabled.val());
                }

                // Update display
                $select.trigger('liszt:updated').change();
              }
            });
          });
        });        
      };

      // Bind filtering to all dropdowns
      bindFilter('region');
      bindFilter('township');
      bindFilter('city');
      bindFilter('z-gm');
    }
  });
  
 // Modified for #6769 - Bundling: Create/update Customer screen: Only show the profile set that is selected for the customer
 $('#edit-general-table-profile-set-id-value').on('change', function(e){
 	var profile_id = $('#edit-general-table-profile-set-id-value').val();  	 	
 	$("span[id^='profile_set_']").each(function(i, el){ 		     	
		$(this).closest('tr').show();				
 		if(this.id != 'profile_set_'+profile_id && profile_id != '' && i>0){
 			$('tr[data-profileset-id='+ $(this).attr('profileSet_id') + ']').hide(); 			
 			$('select[id^="edit-general-bundle-settings-profile-set-'+$(this).attr('profileSet_id') +'-setting-value"]').val('unlimited').trigger("liszt:updated");
 			$('[data-custom-change-event=edit-bundle-settings-profileset-change]').trigger('change');
			$(this).closest('tr').hide();				
		}		
 	});	
  });
  
  $(document).ready(function () {
  	if ($('#edit-general-table-multi-site-value').val() == 1){
	  $('#edit-general-table-allowed-ips-value').parentsUntil( ".row" ).hide();
	  $('span.hpbx-foldable-heading-title').each(function() {	   
        if($(this).html()=='Customer Location'){
          $(this).parent().next(".hpbx-foldable-content").hide();
	      $(this).closest("dt").hide();
		}
		if($(this).html()=='Site Settings'){
          $(this).parent().next(".hpbx-foldable-content").show();
		  $(this).closest("dt").show();
		}
      });      
	}else{
	  $('#edit-general-table-allowed-ips-value').parentsUntil( ".row" ).show();		  
	  $('span.hpbx-foldable-heading-title').each(function() {	 
        if($(this).html()=='Customer Location'){	  
	      $(this).parent().next(".hpbx-foldable-content").show();
	      $(this).closest("dt").show();
	    }
	    if($(this).html()=='Site Settings'){
          $(this).parent().next(".hpbx-foldable-content").hide();
		  $(this).closest("dt").hide();
		}
      });     
	}
	$('#edit-general-table-multi-site-value').bind('change', function (){	
		if($(this).val() == 1){
		  $('#edit-general-table-allowed-ips-value').parentsUntil( ".row" ).hide();
		  $('span.hpbx-foldable-heading-title').each(function() {	   
            if($(this).html()=='Customer Location'){			
			  $(this).parent().next(".hpbx-foldable-content").hide();
			  $(this).closest("dt").hide();
		    }
		    if($(this).html()=='Site Settings'){
              $(this).parent().next(".hpbx-foldable-content").show();
		      $(this).closest("dt").show();
		    }
          });
          
          if($('#edit-general-table-allowed-ips-value').val() != ''){
		  	var $allowed_ips = $('#edit-general-table-allowed-ips-value').val();
		  	$('#edit-general-multisite-table-add-ips-data').val($allowed_ips);
		  }
          
          var custlocations = ['region', 'township', 'city', 'z-gm'];        
   
          $(custlocations).each(function(i, item) {
            if($('#edit-general-emergency-table-'+item+'-value option:selected').text() != ''){
	          var $selected = $('#edit-general-emergency-table-'+item+'-value option:selected').text();	         
		      $('#edit-general-multisite-table-add-'+item+'-data').val($selected);		     
		      $('#edit-general-multisite-table-add-'+item+'-data').trigger('liszt:updated');
	        }
         });                     
		}else{
		  $('#edit-general-table-allowed-ips-value').parentsUntil( ".row" ).show();
		  $('span.hpbx-foldable-heading-title').each(function() {
	   	    if($(this).html()=='Customer Location'){
			  $(this).parent().next(".hpbx-foldable-content").show();
			  $(this).closest("dt").show();
		    }
		    if($(this).html()=='Site Settings'){
              $(this).parent().next(".hpbx-foldable-content").hide();
		      $(this).closest("dt").hide();
		    }
          });
		}
	});	
  });
  
  Drupal.ajax.prototype.commands.multisite =  function(ajax, response, status) {
    window.initDropdowns($('.chzn-select'));
    var bindFilter = function (target) {
    var others =  ['region', 'township', 'city', 'z-gm'];
    $('#hpbx-multisite-table-wrapper .chzn-select').each(function(){    	
    	$(this).bind('change', function (a, b){
    		var query = {};
    	    var $result = $(this).attr('data-multisite-id').split('-');
    	    var $uniqueId = $result[0];
    	    var element = $result[1];
    		if (typeof b!='object') {
              return;
            }
            $.each(others, function (key, value) {
            	query[value] = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ value +'-data]').val();
            });
		   
		   var target_value = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ target +'-data]').val();		   	
			
           if (Drupal.settings['reseller_id']!=undefined) {
             var url = 'hpbx/affiliate/' + Drupal.settings['reseller_id'] + '/customer/location?';
           }
           else {
            var url = 'hpbx/customer/location?';
           }
           console.log(url + $.param(query));
            $.ajax({
            url: Drupal.settings.basePath + Drupal.settings.pathPrefix + url + $.param(query),
            cache: false,
            dataType: 'json'
          })
          .done(function( data ) {
          	console.log(data);
          	$(others).each(function(i, item) {
              if (target_value == '' || item != target) {
                // Filter next level
                var $select = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ item +'-data]');
                var firstEnabled = null;
                var changeEnabled = false;
                $select.children('option').each(function () {
                  var $this = $(this);
                  if ($this.val() != '') {
                    if (data[item].indexOf($this.val()) > -1) {
                      $this.prop('disabled', false);
                      if (!firstEnabled) firstEnabled = $this;
                    } else {
                      $this.prop('disabled', true);
                      if ($this.is(':selected')) {
                        changeEnabled = true;
                      }
                    }
                  }
                });

                // Change selection for next level
                if (changeEnabled && firstEnabled) {
                  $select.val(firstEnabled.val());
                }

                // Update display
                $select.trigger('liszt:updated').change();
              }
            });
          });
    	});
    	
    });
	};
	bindFilter('region');
    bindFilter('township');
    bindFilter('city');
    bindFilter('z-gm');
    
    // this will be executed after the ajax call
    $(".hpbx-notification").each(function() {
      var $notification = $(this),delay = 5000;
      $notification.delay(delay).slideUp();
    });     
  }
  var bindFilter = function (target) {
    var others =  ['region', 'township', 'city', 'z-gm'];
    $('#hpbx-multisite-table-wrapper .chzn-select').each(function(){    	
      $(this).bind('change', function (a, b){
        var query = {};
    	var $result = $(this).attr('data-multisite-id').split('-');
    	var $uniqueId = $result[0];
    	var element = $result[1];
    	if (typeof b!='object') {
          return;
        }
    	$.each(others, function (key, value) {
          query[value] = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ value +'-data]').val();
        });
           
		var target_value = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ target +'-data]').val();		   	
			
        if (Drupal.settings['reseller_id']!=undefined) {
          var url = 'hpbx/affiliate/' + Drupal.settings['reseller_id'] + '/customer/location?';
        }
        else {
          var url = 'hpbx/customer/location?';
        }        
        $.ajax({
          url: Drupal.settings.basePath + Drupal.settings.pathPrefix + url + $.param(query),
          cache: false,
          dataType: 'json'
        })
        .done(function( data ) {          
          $(others).each(function(i, item) {
            if (target_value == '' || item != target) {
              // Filter next level
              var $select = $('select[id^=edit-general-multisite-table-'+ $uniqueId +'-'+ item +'-data]');
              var firstEnabled = null;
              var changeEnabled = false;
              $select.children('option').each(function () {
                var $this = $(this);
                if ($this.val() != '') {
                  if (data[item].indexOf($this.val()) > -1) {
                    $this.prop('disabled', false);
                      if (!firstEnabled) firstEnabled = $this;
                    } else {
                      $this.prop('disabled', true);
                      if ($this.is(':selected')) {
                        changeEnabled = true;
                      }
                    }
                  }
                });

                // Change selection for next level
                if (changeEnabled && firstEnabled) {
                  $select.val(firstEnabled.val());
                }

                // Update display
                $select.trigger('liszt:updated').change();
             }
          });
        });
      });    	
    });
  };
	bindFilter('region');
    bindFilter('township');
    bindFilter('city');
    bindFilter('z-gm');
    
}(window.jQuery, window, document));