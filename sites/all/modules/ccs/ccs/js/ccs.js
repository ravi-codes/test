(function ($) {
  $('document').ready(function(){
    $('div#top-header-region .upc-top-menu ul li.home').click(function(){
      window.location = Drupal.settings.basePath + Drupal.settings.pathPrefix;
    });
  });
})(jQuery);