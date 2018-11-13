(function($, window, document) {
  var offset_x_left = -32;
  var offset_x_right = +10;
  var offset_y = -8;
  var line_width = 14;
  var line_offset_x_left = 22;
  var line_offset_x_right = -12;
  var line_offset_y = 7;


  function hpbx_device_key_set_positions(key, vis_key) {
    key.line_width = line_width;

    if (key.labelpos == 'right') {
      key.x += offset_x_right;
      key.line_height = 1;
    }
    else if (key.labelpos == 'left') {
      key.x += offset_x_left;
      key.line_height = 1;
    }
    else if (key.labelpos == 'top') {
      key.line_height = (14 * 2 - 5);
      key.line_width = 1;
      key.x -= offset_x_right;
      key.y -= (line_offset_y * 4);
    }
    else if (key.labelpos == 'bottom') {
      key.line_height = (14 * 2 - 5);
      key.line_width = 1;
      key.x -= offset_x_right;
      key.y += line_offset_y * 4;
    }

    key.y += offset_y;

    // Odd numbers will be shifted to left or right based on labelpos.
    if (vis_key % 2 == 0) {

      if (key.labelpos == 'right') {
        key.x += 30;
        key.line_width += 30;
      }
      else if (key.labelpos == 'left') {
        key.x -= 30;
        key.line_width += 30;
      }
      else if (key.labelpos == 'top') {

      }
      else if (key.labelpos == 'bottom') {

      }
    }

    // Set line x, line y.
    key.line_x = key.x;
    key.line_y = key.y;

    if (key.labelpos == 'right') {
      key.line_x += line_offset_x_right;
      if (vis_key % 2 == 0) {
        key.line_x += -30;
      }
    }
    else if (key.labelpos == 'left') {
      key.line_x += line_offset_x_left;
    }
    else if (key.labelpos == 'top') {
      key.line_y += 9;
      key.line_x += (line_offset_x_left / 2);
    }
    else if (key.labelpos == 'bottom') {
      key.line_y -= (key.line_height + 5);
      key.line_x += (line_offset_x_left / 2);
    }

    key.line_y += line_offset_y;
    return key;
  }

  function hpbx_device_set_subscriber_table_entry(key_data, subscriber) {

    if (subscriber) {
      var name = subscriber.name;
    }
    else {
      var name = Drupal.t('Unassigned');
    }

    var number = '';
    var alias_number = '';
    var ext = '';

    if (alias_number!='') {
      var number = alias_number + ' [' + ext + ']';
    }

    //var tr = '<tr class="hpbx-device-key-subscriber" can_shared="'+linerange.can_shared+'"  can_blf="'+linerange.can_blf+'"  can_private="'+linerange.can_private+'" hpbx-linerange-vis-key="'+ linerange.vis_key +'"  hpbx-linerange-name="'+ linerange.name +'" hpbx-linerange-id="'+ linerange.id +'" hpbx-linerange-key="'+ linerange.key +'">'+
    var tr = $('<tr class="inactive hpbx-device-key-subscriber" hpbx-linerange-name="'+ key_data.linerange.name +'" hpbx-linerange-id="'+ key_data.linerange.id +'" hpbx-linerange-key="'+ key_data.key +'">');

    $(tr).append('<td class="hpbx-device-key-subscriber-num"/><td class="hpbx-device-key-subscriber-data"/>');

    $(tr).find('.hpbx-device-key-subscriber-num').html(key_data.vis_key);
    $(tr).find('.hpbx-device-key-subscriber-data').append('<p class="hpbx-device-key-subscriber-name"/><p class="hpbx-device-key-subscriber-number"/>');

    $(tr).find('.hpbx-device-key-subscriber-name').html(name);
    $(tr).find('.hpbx-device-key-subscriber-number').html(number);

    $(tr).append('<td class="hpbx-device-key-subscriber-actions"></td>');

    if (key_data.linerange.can_blf) {
      $(tr).find('.hpbx-device-key-subscriber-actions').append('<div class="hpbx-device-capability hpbx-device-capability-blf"/>');
    }
    if (key_data.linerange.can_shared) {
      $(tr).find('.hpbx-device-key-subscriber-actions').append('<div class="hpbx-device-capability hpbx-device-capability-shared"/>');
    }
    if (key_data.linerange.can_private) {
      $(tr).find('.hpbx-device-key-subscriber-actions').append('<div class="hpbx-device-capability hpbx-device-capability-private"/>');
    }

    $(tr).find('.hpbx-device-key-subscriber-actions').append('<a href="#" class="hpbx-device-delete"><div class="hpbx-device-delete-img">&nbsp;</div></a>');

    $('table#hpbx-device-subscriber-table').append(tr);

    if (!subscriber) {
      $(tr).addClass('hpbx-device-key-unassigned');
    }
  };

  function hpbx_device_process_linerange(data) {

    var vis_key = 0;
    var device_keys = new Array();

    // Lineranges.
    $.each(data, function(i, linerange) {

      // Linerange Keys.
      $.each(linerange.keys, function(k, key) {

        var key_data = {} ;

        vis_key++;

        // Clone 
        var key_orig = jQuery.extend(true, {}, key);

        // Make corrections on the keys, based on various variables (labelpos, even, odd etc).
        key = hpbx_device_key_set_positions(key, vis_key);

        // Add key to linerange obj.
        key_data.key = k;
        key_data.vis_key = vis_key;
        key_data.linerange = linerange;

        // Create virtual button.
        var vkey = $('<div class="inactive hpbx-device-key" hpbx-linerange-id="'+ key_data.linerange.id +'" hpbx-linerange-key="'+ key_data.key +'">');
        vkey.html(vis_key);

        vkey.appendTo($('#hpbx-device-model-image-wrapper .hpbx-device-keys')).css({
          'left' : key.x+'px',
          'top' : key.y+'px',
          'z-index' : 2
        });

        // Create a line from virtual button to img button.
        $("<hr noshade size=1>").appendTo($('#hpbx-device-model-image-wrapper .hpbx-device-keys')).addClass('hpbx-device-key-line').css({
          'width': key.line_width+'px',
          'height': key.line_height+'px',
          'left' : key.line_x+'px',
          'top' : key.line_y+'px',
          'z-index' : 1
        });

        // Create a transparant div on top of the 'physical' buttons.// Create virtual button.
        var vkey = $('<div class="inactive hpbx-device-key hpbx-device-key-transparant" hpbx-linerange-id="'+ key_data.linerange.id +'" hpbx-linerange-key="'+ key_data.key +'">');

        if (key_orig.labelpos == 'right') {
          key_orig.x -= 26;
        }
        else if (key_orig.labelpos == 'bottom') {
          key_orig.y -= 13;
          key_orig.x -= 13;
        }
        else if (key_orig.labelpos == 'top') {
          key_orig.y += 13;
          key_orig.x -= 13;
        }
        vkey.appendTo($('#hpbx-device-model-image-wrapper .hpbx-device-keys')).css({
          'left' : key_orig.x + 'px',
          'top' : (key_orig.y - 6)+'px',
          'width': '27px',
          'border-radius': '8px',
          'height': '13px',
          'background-color' : 'rgba(0, 0, 0, 0)',
          'box-shadow': 'none',
        });

        // Create sidebar subscriber table entry.
        hpbx_device_set_subscriber_table_entry(key_data, false);

        // Set entry.
        device_keys[key_data.linerange.id + '-' + key_data.key] = key_data;
      });
    });

    if ($('tr.hpbx-device-key-subscriber[subscriber-id]').length === 0) {
      $('.hpbx-device-submit').addClass('submit-button-disabled');
      $('.hpbx-device-submit').find('input').prop('disabled',true);
    }
    else {
      $('.hpbx-device-submit').removeClass('submit-button-disabled');
      $('.hpbx-device-submit').find('input').prop('disabled', false);
    }

    // Store entries.
    $('body').data('device_keys', device_keys);
  }

  function hpbx_subscriber_set(linerange_id, linerange_key, subscriber_id, line_type) {

    var tr = $('table#hpbx-device-subscriber-table tr.hpbx-device-key-subscriber[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]');
    var div_key = $('div.hpbx-device-key[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]');

    $(tr).removeClass('line-type-blf');
    $(tr).removeClass('line-type-private');
    $(tr).removeClass('line-type-shared');

    $(tr).find('.hpbx-device-capability').addClass('hpbx-device-capability-inactive');


    if (subscriber_id && line_type) {

      var obj = Drupal.settings.hpbx_subscriber_list;
      var subscriber = obj[subscriber_id];

      // Set visible data.
      $(tr).removeClass('hpbx-device-key-unassigned');


      $(tr).find('.hpbx-device-capability-' + line_type).removeClass('hpbx-device-capability-inactive');
      $(tr).addClass('line-type-' + line_type);

      $(tr).attr('line-type', line_type);
      $(div_key).attr('line-type', line_type);
      $(tr).attr('subscriber-id', subscriber_id);
      $(div_key).attr('subscriber-id', subscriber_id);

      $(tr).find('p.hpbx-device-key-subscriber-name').html(subscriber.name);
      $(tr).find('p.hpbx-device-key-subscriber-number').html(subscriber.alias_number + ' [' + subscriber.extension + ']');
    }
    else {
      var subscriber = false;

      // Set visible data.
      $(tr).addClass('hpbx-device-key-unassigned');

      $(tr).removeAttr('line-type');
      $(tr).removeAttr('subscriber-id');
      $(div_key).removeAttr('subscriber-id');
      $(div_key).removeAttr('line-type');


      $(tr).find('p.hpbx-device-key-subscriber-name').html(Drupal.t('Unassigned'));
      $(tr).find('p.hpbx-device-key-subscriber-number').html('');
    }

    if ($('tr.hpbx-device-key-subscriber[subscriber-id]').length === 0) {
      $('.hpbx-device-submit').addClass('submit-button-disabled');
      $('.hpbx-device-submit').find('input').prop('disabled', true);
    }
    else {
      $('.hpbx-device-submit').removeClass('submit-button-disabled');
      $('.hpbx-device-submit').find('input').prop('disabled', false);
    }
  }

  function hpbx_subscriber_dialog_set() {

    var linerange_id = $('input[name=linerange-id]').val();
    var linerange_key = $('input[name=linerange-key]').val();

    subscriber_id = false;
    line_type = false;

    if ($('select#subscriber_id option:selected').val()!='' &&
      $('select#line_type option:selected').val()!='') {

      var subscriber_id = $('select#subscriber_id option:selected').val();
      var line_type = $('select#line_type option:selected').val();
    }

    hpbx_subscriber_set(linerange_id, linerange_key, subscriber_id, line_type);
  }

  $('document').ready(function(){
    var path = window.location.pathname.split('/');
    if (path[path.length-1] == 'add') {
      $('.hpbx-add-device').show();
    }
    $('#hpbx-pbxdevice-edit-form input#edit-table-actions-submit').hide();

    // station name is changed.
    $('input#edit-table-station-name-value').change(function(){
      var args = {};
      args['!station_name'] = $('input#edit-table-station-name-value').val();
      $('div#content-area h1').attr('innerHTML', Drupal.t('Device !station_name', args));
    });

    // Identifier is changed.
    $('input#edit-table-identifier-value').change(function(){

      var mac = $(this).val();
      mac = mac.replace(/[^A-Fa-f0-9]/g, "").toUpperCase();
      var new_mac = '';

      if (mac.length == 12) {
        new_mac = mac.substr(0, 2) + ':';
        new_mac += mac.substr(2, 2) + ':';
        new_mac += mac.substr(4, 2) + ':';
        new_mac += mac.substr(6, 2) + ':';
        new_mac += mac.substr(8, 2) + ':';
        new_mac += mac.substr(10, 2);
      }
      $('.hpbx-device-footer-input-incognito').val(new_mac);
    });

    
    $(document).on("edit-table-profile-id-value", function(event, element) {

      // Get profile id.
      var profile_id = $('select#edit-table-profile-id-value option:selected').val();

      $.ajax({
        url: Drupal.settings.basePath + Drupal.settings.pathPrefix + "hpbx/pbxdevicemodel/" + profile_id,
        cache: false,
        dataType: 'json'
      })
        .done(function( data ) {

          if ($('#edit-table-station-name-value').val()!='') {
            $('.hpbx-device-title').html($('#edit-table-station-name-value').val());
          }
          $('.hpbx-device-profile').html($('select#edit-table-profile-id-value option:selected').html());

          // Set background.
          $('div#hpbx-device-model-image-wrapper').css('background-image', 'url('+ Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevicemodelimage/' +  profile_id + ')');

          // Remove subscriber table.
          $('table#hpbx-device-subscriber-table').empty();

          // Remove content of wrapper.
          $('div#hpbx-device-model-image-wrapper').empty();

          // Need a hidden image to auto adjust the wrapper div.
          var img = $('<img src="'+ Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevicemodelimage/' +  profile_id +'"/>');
          $(img).addClass('hpbx-device-model-image-hidden');
          $('div#hpbx-device-model-image-wrapper').append(img);

          var img = $('<img src="'+ Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevicemodelimage/' +  profile_id +'"/>');
          $(img).addClass('hpbx-device-model-image');
          $('div#hpbx-device-model-image-wrapper').append(img);

          $('div#hpbx-device-model-image-wrapper').append('<div class="hpbx-device-keys"/>');

          hpbx_device_process_linerange(data);

          $('.hpbx-device-key-subscriber, .hpbx-device-key').magnificPopup({
            type:'inline',
            items: {
              src: '#hpbx-device-subscriber-dialog'
            },
            callbacks: {
              beforeOpen: function() {
                initialize_popup(this);
              },
              open: function () {
                var magnific = this;
                var args = {};
                if ($(magnific.st.el).find('.hpbx-device-key-subscriber-num').length) {
                  args['!button'] = parseInt($(magnific.st.el).find('.hpbx-device-key-subscriber-num').html());
                }
                else if ($(magnific.st.el).html()!=='') {
                  args['!button'] = parseInt($(magnific.st.el).html());
                }
                else if ($(magnific.st.el).prev().prev().html()!==''){
                  args['!button'] = parseInt($(magnific.st.el).prev().prev().html());
                }

                $('.hpbx-popup-title').html(Drupal.t('Line selection button !button', args));
              }
            },
            midClick: true
          });


          // Add click event handler to open model box for line editting.
          initialize_popup = function(magnific) {

            var el = magnific.st.el;
            var device_keys = $('body').data('device_keys');
            var key_data = device_keys[$(el).attr('hpbx-linerange-id') + '-' + $(el).attr('hpbx-linerange-key')];

            var dialog, form, tips = $(".validateTips");

            // Populate subscribers.
            $('select#subscriber_id option').remove();
            $.each(Drupal.settings.hpbx_subscriber_list, function(subscriber_id, subscriber) {

              var $option = $('<option value="'+subscriber_id+'"></option>');

              var type_count = 0;
              if (subscriber.can_blf && key_data.linerange.can_blf) {
                $option.attr('can_blf', true);
                type_count++;
              }

              if (subscriber.can_private && key_data.linerange.can_private) {
                $option.attr('can_private', true);
                type_count++;
              }

              if (subscriber.can_shared && key_data.linerange.can_shared) {
                $option.attr('can_shared', true);
                type_count++;
              }


              if (type_count) {
                $option.html(subscriber.name + ' ' + subscriber.alias_number + ' (' + subscriber.extension + ') ' + (subscriber.in_use ? '*' : ''));
              }
              $('select#subscriber_id').append($option);
            });

            $('select#subscriber_id').bind('change', function() {
              // data fill the line-type options.

              var can_count = 0;
              var default_line_type_value = '';

              // Populate line_type.
              $('select#line_type option').remove();
              if (key_data.linerange.can_private && $(this).find('option:selected').attr('can_private') == 'true') {
                var $option = $('<option value="private"></option>');
                $option.html(Drupal.t('Private'));
                $('select#line_type').append($option);
                default_line_type_value = 'private';
                can_count++;
              }

              if (key_data.linerange.can_shared && $(this).find('option:selected').attr('can_shared') == 'true') {
                var $option = $('<option value="shared"></option>');
                $option.html(Drupal.t('Shared'));
                $('select#line_type').append($option);
                default_line_type_value = 'shared';
                can_count++;
              }

              if (key_data.linerange.can_blf && $(this).find('option:selected').attr('can_blf') == 'true') {
                var $option = $('<option value="blf"></option>');
                $option.html(Drupal.t('BLF'));
                $('select#line_type').append($option);
                default_line_type_value = 'blf';
                can_count++;
              }

              if ($(el).attr('line-type')) {
                $('select#line_type').val($(el).attr('line-type'));
              }

              if (can_count===1) {
                $('select#line_type').val(default_line_type_value);
              }
              $('select#line_type').trigger('liszt:updated');
            });

            $('select#subscriber_id').trigger('change');

            // Set default values.
            if ($(el).attr('subscriber-id')) {
              $('select#subscriber_id').val($(el).attr('subscriber-id'));
            }

            $('input[name=linerange-id]').val(key_data.linerange.id);
            $('input[name=linerange-key]').val(key_data.key);

            $('select#subscriber_id').trigger('liszt:updated');

            var args = {};
            args['!vis_key'] = key_data.vis_key;

            $('.hpbx-popup-form-buttons-row [type=submit]').on('click', function(){

              hpbx_subscriber_dialog_set();
              magnific.close();
              return false;
            });

            $('.hpbx-popup-form-link-button').on('click', function(){

              $('select#subscriber_id').val('');
              $('select#line_type').val('');

              hpbx_subscriber_dialog_set();
              magnific.close();
              return false;
            });
            return false;
          };

          $('.hpbx-device-title').on('click', function(event) {
            var $title = $(this),
              $input = $('<input type="text" class="sg-element hpbx-device-title-edit" value="">');

            $title.hide().after($input);

            $input.focus()
              .val($title.text())
              .selectRange(0, $input.val().length)
              .on('blur keyup', _handleGlobalClick);

            event.stopPropagation();

            $(document).off('click.hpbx-device-title')
              .on('click.hpbx-device-title', _handleGlobalClick);

            function _handleGlobalClick(event) {
              if (event.target == $input[0] || (event.type == 'keyup' && event.which != 13)) {
                return;
              }

              var newDeviceTitle = $input.val();

              if ($title.text() != newDeviceTitle ) {
                $title.text(newDeviceTitle);
                $('#edit-table-station-name-value').val(newDeviceTitle);
              }

              $input.remove();
              $title.show();
            };
          });

          // Add mouse-overs.
          $(".hpbx-device-key-subscriber, .hpbx-device-key").hover(
            function() {

              var linerange_id = $(this).attr('hpbx-linerange-id');
              var linerange_key = $(this).attr('hpbx-linerange-key');

              var tr = $('table#hpbx-device-subscriber-table tr.hpbx-device-key-subscriber[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]');
              var div_key = $('div.hpbx-device-key[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]:not(.hpbx-device-key-transparant)');

              $(tr).removeClass('inactive');
              $(tr).addClass('active');
              $(div_key).removeClass('inactive');
              $(div_key).addClass('active');
            },

            function() {
              var linerange_id = $(this).attr('hpbx-linerange-id');
              var linerange_key = $(this).attr('hpbx-linerange-key');

              var tr = $('table#hpbx-device-subscriber-table tr.hpbx-device-key-subscriber[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]');
              var div_key = $('div.hpbx-device-key[hpbx-linerange-id="'+ linerange_id +'"][hpbx-linerange-key="'+ linerange_key +'"]:not(.hpbx-device-key-transparant)');

              $(tr).removeClass('active');
              $(tr).addClass('inactive');
              $(div_key).removeClass('active');
              $(div_key).addClass('inactive');
            }
          );

          var path = window.location.pathname.split('/');
          if (path[path.length-1] == 'edit') {
            $('#hpbx-pbxdevice-edit-form input#edit-table-actions-next').trigger('click');
          }

          $(window).trigger('resize');
        });
    });
    
    //Moved the function to here for Bug 1620 - PE DEV: Edit device - Not possible to edit devices using an iOS device
    // In case of editting a existing device, check if model and profile_id are set. If
    // this is the case trigger the profile-id-value event
    if ($('#edit-table-profile-id-value').data('selected-value') != '') {
      //Added to fix the Bug 1620 - PE DEV: Edit device - Not possible to edit devices using an iOS device
      var sv = $('#edit-table-profile-id-value').data('selected-value');
      $('#edit-table-profile-id-value option[selected="selected"]').each(
        function() {
          $(this).removeAttr('selected');
        }
      );
      $("select#edit-table-profile-id-value").val(sv);
      $('select#edit-table-profile-id-value option[value='+ sv +']').attr('selected', 'selected');
      $('select#edit-table-profile-id-value').trigger('liszt:updated');
      //Commented to fix the Bug 1620 - PE DEV: Edit device - Not possible to edit devices using an iOS device
      //$(document).trigger("edit-table-profile-id-value", $(this));
    }

    $('.hpbx-device-footer-input-incognito').on('change', function(){
      $('#edit-table-identifier-value').val($(this).val());
    });


    $('#hpbx-pbxdevice-edit-form input#edit-table-actions-next').click(function(){

      $(this).hide();

      $('body').addClass('hpbx-contains-menu-over-menu');

      $('.hpbx-menu-over-menu-dummy').addClass('hpbx-menu-over-menu');
      $('.hpbx-menu-over-menu').show();
      $('.hpbx-device-subscriber-table').show();
      $('.messages.error').remove();

      // Initialiy set the lines/keys already configured.
      $.each(Drupal.settings.hpbx_lines, function(i, line) {
        hpbx_subscriber_set(line.linerange_id, line.key_num, line.subscriber_id, line.type);
      });

      $('.hpbx-add-device').show();
      $('#edit-table').hide();
      $('.hpbx-page-title').wrap('<div class="col-xs-12 hidden-md hidden-lg hpbx-main-content"></div>');

      $('#edit-subscribers').show();
      $('div.hpbx-add-device').removeClass('hpbx-main-content-inner');
      $('.hpbx-device-info').show();

      $('body').addClass('hpbx-device-page');
      $('body').addClass('page-hpbx-pbxdevices');
      $('div#hpbx-device-model-image-wrapper').show();

      $('<div class="row"><div class="col-xs-12 hpbx-main-content  hidden-md hidden-lg">' +
        '<div class="hpbx-device-profile-label">' + Drupal.t('Device profile') + ':</div>' +
        '<div class="hpbx-device-profile">&nbsp;</div>'+
        '<div class="hpbx-device-identifier-label">'+ Drupal.t('MAC address') + ':</div>' +
        '<input class="hpbx-device-footer-input-incognito sg-completed" type="text">' +
        '<span class="hpbx-device-identifier-edit-icon LGI-iconedit"></span>' +
        '</div></div>').insertAfter('.container .row');


      $('.hpbx-device-title').attr('innerHTML', $('#edit-table-station-name-value').val());

      $('.hpbx-device-profile').html($('select#edit-table-profile-id-value option:selected').html());

      var mac = $('input#edit-table-identifier-value').val();
      mac = mac.replace(/[^A-Fa-f0-9]/g, "").toUpperCase();
      var new_mac = '';

      if (mac.length == 12) {
        new_mac = mac.substr(0, 2) + ':';
        new_mac += mac.substr(2, 2) + ':';
        new_mac += mac.substr(4, 2) + ':';
        new_mac += mac.substr(6, 2) + ':';
        new_mac += mac.substr(8, 2) + ':';
        new_mac += mac.substr(10, 2);
      }
      $('.hpbx-device-identifier').html(new_mac);
      $('.hpbx-device-footer-input-incognito').val(new_mac);

      $('.region-content').addClass('col-xs-12 hpbx-main-content').removeClass('region').removeClass('region-content').detach().appendTo('.container .row:eq(1)');

      $('.container .row:eq(0)').hide();
      $(this).parent().hide();
      return false;
    });

    $('.hpbx-device-footer input[type=submit]').on('click', function(e){

      if ($('tr.hpbx-device-key-subscriber[subscriber-id]').length > 0) {

        // Set the device name.
        if ($('.hpbx-device-title-edit').length) {
          var newDeviceTitle = $('.hpbx-device-title-edit').val();


          if (newDeviceTitle!='' && $('#edit-table-station-name-value').val() != newDeviceTitle) {
            $('#edit-table-station-name-value').val(newDeviceTitle);
          }
        }
        var lines = new Array();
        $('tr.hpbx-device-key-subscriber').each(function(){

          if (!isNaN($(this).attr('subscriber-id'))) {
            var line = {
              linerange_id: parseInt($(this).attr('hpbx-linerange-id')),
              key_num: parseInt($(this).attr('hpbx-linerange-key')),
              subscriber_id: parseInt($(this).attr('subscriber-id')),
              line_type: $(this).attr('line-type'),
              linerange: $(this).attr('hpbx-linerange-name')
            };
            lines.push(line);
          }
        });

        $('input[name="lines"]').val(JSON.stringify(lines));


        $.ajax({
          type: 'POST',
          url:  Drupal.settings.basePath + Drupal.settings.pathPrefix +  'system/ajax',
          data: $('#hpbx-pbxdevice-edit-form').serialize(),
          dataType:  'json',
          beforeSend: function() {

            $('body').append(
              '<div class="hpbx-loader">' +
              '<div class="hpbx-loader-box">' +
              '<div class="hpbx-loader-animation">' +
              '</div>' +
              '<div class="hpbx-loader-progress">' +
              '<div class="hpbx-loader-progress-line"></div>' +
              '<div class="hpbx-loader-progress-dot"></div>' +
              '</div>' +
              '<div class="hpbx-loader-content">' +
              '<div class="hpbx-loader-title"></div>' +
              '<div class="hpbx-loader-subtitle"></div>' +
              '</div>' +
              '</div>' +
              '</div>'
            );

            hpbxShowLoader(Drupal.t('Processing') + '...', Drupal.t('This can take a while.'));
          },
          success: function(data, textStatus,jqXHR ) {

            if (data.status == 'ok') {

              var title = Drupal.t('Device successfully saved');
              var message = '';
              var buttonText = Drupal.t('Exit');

              if (data.type == 'reboot') {

                var message = Drupal.t('Reboot your phone to provision the device');

                hpbxShowAlert('big', title, message, buttonText, function(){
                  document.location.href = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevices';
                });
              }
              else {
                if (data.type == 'sync') {
                  var message = '<p>' + Drupal.t('Please fill in the IP address of your phone in order to provision it') + '</p>';
                }
                else if (data.type == 'sync_reboot') {
                  var message = '<p>' + Drupal.t('In case the device is already synced, reboot<br/>your phone to provision the device.') + '</p><p>' + Drupal.t('Otherwise, please fill in the IP address of your phone in order to provision it' ) + '</p>';
                }

                message = message +
                  '<p>' +
                  '<div class="row">' +
                  '<input type="text" name="ip" id="edit-ip" maxlength="15" placeholder="' + Drupal.t('IP-Address') + '"/> <a  href="#" class="hpbx-pbxdevice-sync-btn sg-btn sg-btn-small sg-without-icon sg-btn-primary">Sync</a>' +
                  '</div>'
                '</p>';

                hpbxShowAlert('big', title, message, buttonText, function(){
                  document.location.href = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevices';
                });

                $('.hpbx-pbxdevice-sync-btn').on('click', function(){
                  window.location = data.sync_url + $('#edit-ip').val();
                  return false;
                });
              }
            }
            else {
              // failed.
              document.location.href = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevices';
            }
          },
          error: function( jqXHR, textStatus, errorThrown ) {
            document.location.href = Drupal.settings.basePath + Drupal.settings.pathPrefix + 'hpbx/pbxdevices';
          },
          complete: function(){
            hpbxHideLoader();
          }
        });


        return false;
      }
      else {		
        var title = Drupal.t('');
        var message = '';
        var buttonText = Drupal.t('Got it!');          
        var message = Drupal.t('There should be at least one line provisioned on the device. Please adjust and save the configuration.');
        hpbxShowAlert('small', title, message, buttonText);                    	
        return false; 
      }
    });




    // Load all data in case an existing phone is editted.
    if ($('input#edit-table-station-name-value').val()!='') {
      $('input#edit-table-station-name-value').trigger("change");
    }
    if ($('input#edit-table-identifier-value').val()!='') {
      $('input#edit-table-identifier-value').trigger("change");
    }
    if ($('select#edit-table-profile-id-value').val()!='') {
      $('select#edit-table-profile-id-value').trigger("change");
    }

    //Added for the bug #9584 - PE DEV: Create device page - Disable next step button as long as there are fields not filled out yet
   if($('form').attr('id') == 'hpbx-pbxdevice-edit-form'){
      $('#edit-table-actions-next').addClass('sg-disabled').prop('disabled', true);
    }

    $('#hpbx-pbxdevice-edit-form .sg-element').on('change mouseout keyup', function(e){
      $('#edit-table-actions-next').addClass('sg-disabled').prop('disabled', true);
      if($('#edit-table-station-name-value').val() != '' && $('#edit-table-vendor-value').val() != '' && $('#edit-table-profile-id-value').val() != '' && $('#edit-table-identifier-value').val() != ''){
        var isInvalid = 0;
        var $element = $('#edit-table-identifier-value');
        regexData = $element.data('validation-regex'),
        regex = regexData !== undefined ? regexData.split('___') : [],messagesData = $element.data('invalid-messages'), messages = messagesData !== undefined ? messagesData.split('___') : [];
        for (var i = 0; i < regex.length; i++) {
          if ($element.val().match(new RegExp(regex[i], 'i')) == null) {
            isInvalid = 1;
          }
        }
        if(isInvalid == 0){
          $('#edit-table-actions-next').removeClass('sg-disabled').prop('disabled',false);
        }
      }
   });

  });

}(window.jQuery, window, document));
