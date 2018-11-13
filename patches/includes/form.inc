diff -r d2fd27605cb2 includes/form.inc
--- includes/form.inc	Wed Dec 11 11:00:34 2013 +0100
+++ includes/form.inc	Wed Dec 18 09:16:01 2013 +0100
@@ -3319,8 +3319,22 @@
       // As theme_table only maps header and row columns by order, create the
       // correct order by iterating over the header fields.
       foreach ($element['#header'] as $fieldname => $title) {
+        // A row cell can span over multiple headers, which means less row cells
+        // than headers could be present.
+        if (isset($element['#options'][$key][$fieldname])) {
+          // A header can span over multiple cells and in this case the cells
+          // are passed in an array. The order of this array determines the
+          // order in which they are added.
+          if (!isset($element['#options'][$key][$fieldname]['data']) && is_array($element['#options'][$key][$fieldname])) {
+            foreach ($element['#options'][$key][$fieldname] as $cell) {
+              $row['data'][] = $cell;
+            }
+          }
+          else {
         $row['data'][] = $element['#options'][$key][$fieldname];
       }
+        }
+      }
       $rows[] = $row;
     }
     // Add an empty header or a "Select all" checkbox to provide room for the
