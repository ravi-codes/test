(function ($) {
  $('document').ready(function(){
    if ($('form#hpbx-resellers-overview-form').length) {
    
      $('form#hpbx-resellers-overview-form').append('<div id="hpbx-resellers-stats-calls-per-day"/>');
      
      var pbx_chart = new Highcharts.Chart({
        chart: {
          renderTo: 'hpbx-resellers-stats-calls-per-day',
          type: 'column'
        },
        title: {
          text: 'Calls per day'
        },
        xAxis: {
          categories: [
          '16 jan',
          '17 jan',
          '18 jan',
          '19 jan',
          '20 jan',
          '21 jan',
          '22 jan'
          ]
        },
        yAxis: {
          min: 0,
          title: {
            text: 'Calls'
          }
        },
        tooltip: {
          headerFormat: '<span style="font-size:10px">{point.key}</span><table>',
          pointFormat: '<tr><td style="color:{series.color};padding:0">{series.name}: </td>' +
          '<td style="padding:0"><b>{point.y:.1f} calls</b></td></tr>',
          footerFormat: '</table>',
          shared: true,
          useHTML: true
        },
        plotOptions: {
          column: {
            pointPadding: 0.2,
            borderWidth: 0
          }
        },
        series: [
        {
          name: 'UPC Nederland',
          color: '#F48C00',
          //data: [49.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6]
          
        }, {
          name: 'UPC Ireland',
          color: '#509BCB',
          //data: [83.6, 78.8, 98.5, 93.4, 106.0, 84.5, 105.0]
        }
        ]
      });
      
      function get_stats() {
        
        setTimeout(function() {
          
          $.ajax({
            type: 'GET',
            url: 'hpbx/affiliates/stats',
            success:function(data){
              pbx_chart.series[0].setData([129.2, 144.0, 176.0, 135.6, 148.5, 216.4]);
              pbx_chart.series[1].setData([129.2, 144.0, 176.0, 135.6, 148.5, 216.4]);
              
              get_stats();
            },
            error:function(){
              alert(Drupal.t('Failed to retrieve stats'));
            }
          });
          
        }, 5000);
      }

      get_stats();
    };
    
  });
})(jQuery);





