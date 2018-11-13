<?php

function hpbx_download_page() {

  $output = '<h4 class="hpbx-download-category-title">'.t("For your desktop").'</h4>
                    <div class="row">
                        <div class="col-xs-12 col-sm-6 col-lg-4">
                            <a class="hpbx-download-item" href="download/software/windows", target="">
                                <div class="hpbx-download-item-icon LGI-iconwindows"></div>
                                <div class="hpbx-download-item-caption">
                                    <div class="hpbx-download-item-title">Windows</div>
                                    <div class="hpbx-download-item-subtitle">version 2.3</div>
                                </div>
                            </a>
                        </div>
                        <div class="col-xs-12 col-sm-6 col-lg-4">
                            <a class="hpbx-download-item" href="download/software/mac">
                                <div class="hpbx-download-item-icon LGI-iconapple"></div>
                                <div class="hpbx-download-item-caption">
                                    <div class="hpbx-download-item-title">Apple OX</div>
                                    <div class="hpbx-download-item-subtitle">version 1.17</div>
                                </div>
                            </a>
                        </div>
                        <div class="col-xs-12 col-sm-6 col-lg-4">
                            <a class="hpbx-download-item" href="download/software/linux">
                                <div class="hpbx-download-item-icon LGI-iconubuntu"></div>
                                <div class="hpbx-download-item-caption">
                                    <div class="hpbx-download-item-title">Ubuntu</div>
                                    <div class="hpbx-download-item-subtitle">version 1.8</div>
                                </div>
                            </a>
                        </div>
                    </div>';                  

  return $output;
}
function hpbx_software_download($ostype){ 

  $uri = 'private://';
  $softphone = 'softphone.zip';
  if ($wrapper = file_stream_wrapper_get_instance_by_uri($uri)) {
    $path = $wrapper->realpath();	 
  }
  

    $attachment_location = $path.'/'.$_SERVER['SERVER_NAME'].'/'.$ostype.'_'.$softphone;
 
  
  if (file_exists($attachment_location)) {

    header($_SERVER["SERVER_PROTOCOL"] . " 200 OK");
    header("Cache-Control: public"); 
    header("Content-Type: application/zip");
    header("Content-Transfer-Encoding: Binary");
    header("Content-Length:".filesize($attachment_location));
    header("Content-Disposition: attachment; filename=$softphone");
    readfile($attachment_location);
    die();
  } else {
    drupal_add_http_header('Status', '404 Not Found');
    drupal_set_title('');
    $return = '<div class="hpbx-404">
                  <div class="hpbx-404-image"></div>
                  <div class="hpbx-404-message">
                  <div class="hpbx-404-message-title">';
    $return .= t('Whoops');
    $return .= ',</div>
                  <div class="hpbx-404-message-text">'. t("the page you're trying to reach doesn't exist...") . '</div>
                 </div></div>';
    drupal_set_page_content($return);
    $page = element_info('page');
    print drupal_render_page($page);
  }
}