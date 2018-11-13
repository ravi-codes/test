(function($, window, document) {

  $(document).ready(function () {



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

      console.log();
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

}(window.jQuery, window, document));