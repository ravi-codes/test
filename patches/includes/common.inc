diff --git a/includes/common.inc b/includes/common.inc
index d7dc47f..f61d1eb 100644
--- a/includes/common.inc
+++ b/includes/common.inc
@@ -611,8 +611,9 @@ function drupal_parse_url($url) {
   }
   // The 'q' parameter contains the path of the current page if clean URLs are
   // disabled. It overrides the 'path' of the URL when present, even if clean
-  // URLs are enabled, due to how Apache rewriting rules work.
-  if (isset($options['query']['q'])) {
+  // URLs are enabled, due to how Apache rewriting rules work. The path
+  // parameter must be a string.
+  if (isset($options['query']['q']) && is_string($options['query']['q'])) {
     $options['path'] = $options['query']['q'];
     unset($options['query']['q']);
   }