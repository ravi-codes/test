<div class="col-xs-6">
  <?php 
  $path = $element['#object']->field_image['und'][0]['uri']; 
  ?>
  <div class="hpbx-help-step-image" style="background-image: url(<?php print file_create_url($path) ;?>)"></div>
</div>