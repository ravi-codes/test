diff --git a/includes/bootstrap.inc b/includes/bootstrap.inc
index 655db6d..880557e 100644
--- a/includes/bootstrap.inc
+++ b/includes/bootstrap.inc
@@ -2632,6 +2632,10 @@ function _drupal_bootstrap_configuration() {
   timer_start('page');
   // Initialize the configuration, including variables from settings.php.
   drupal_settings_initialize();
+
+  // Sanitize unsafe keys from the request.
+  require_once DRUPAL_ROOT . '/includes/request-sanitizer.inc';
+  DrupalRequestSanitizer::sanitize();
 }
 
 
 
diff --git a/includes/bootstrap.inc b/includes/bootstrap.inc
index 06acf93..d5963a0 100644
--- a/includes/bootstrap.inc
+++ b/includes/bootstrap.inc
@@ -2778,6 +2778,11 @@ function _drupal_bootstrap_variables() {
       unset($_GET['destination']);
       unset($_REQUEST['destination']);
     }
+    // Use the DrupalRequestSanitizer to ensure that the destination's query
+    // parameters are not dangerous.
+    if (isset($_GET['destination'])) {
+      DrupalRequestSanitizer::cleanDestination();
+    }
     // If there's still something in $_REQUEST['destination'] that didn't come
     // from $_GET, check it too.
     if (isset($_REQUEST['destination']) && (!isset($_GET['destination']) || $_REQUEST['destination'] != $_GET['destination']) && url_is_external($_REQUEST['destination'])) {