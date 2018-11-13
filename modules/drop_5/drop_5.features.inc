<?php
/**
 * @file
 * drop_5.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function drop_5_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "file_entity" && $api == "file_type") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_default_search_api_index().
 */
function drop_5_default_search_api_index() {
  $items = array();
  $items['hpbx_articles'] = entity_import('search_api_index', '{
    "name" : "hpbx articles",
    "machine_name" : "hpbx_articles",
    "description" : null,
    "server" : "database",
    "item_type" : "node",
    "options" : {
      "datasource" : { "bundles" : [ "hpbx_help_article" ] },
      "index_directly" : 1,
      "cron_limit" : "50",
      "fields" : {
        "author" : { "type" : "integer", "entity_type" : "user" },
        "field_hpbx_block" : {
          "type" : "list\\u003Cinteger\\u003E",
          "entity_type" : "field_collection_item"
        },
        "field_hpbx_block:field_hpbx_body" : { "type" : "list\\u003Ctext\\u003E" },
        "field_hpbx_block:field_hpbx_instruction_photo:field_hpbx_body" : { "type" : "list\\u003Clist\\u003Ctext\\u003E\\u003E" },
        "field_hpbx_reseller_id" : { "type" : "integer" },
        "language" : { "type" : "string" },
        "nid" : { "type" : "integer" },
        "search_api_access_node" : { "type" : "list\\u003Cstring\\u003E" },
        "search_api_language" : { "type" : "string" },
        "status" : { "type" : "boolean" },
        "title" : { "type" : "text" }
      },
      "data_alter_callbacks" : {
        "search_api_alter_bundle_filter" : {
          "status" : 0,
          "weight" : "-10",
          "settings" : { "default" : "1", "bundles" : [] }
        },
        "search_api_alter_node_access" : { "status" : 1, "weight" : "0", "settings" : [] },
        "search_api_alter_node_status" : { "status" : 1, "weight" : "0", "settings" : [] },
        "search_api_alter_language_control" : {
          "status" : 0,
          "weight" : "0",
          "settings" : { "lang_field" : "", "languages" : [] }
        },
        "search_api_alter_add_viewed_entity" : { "status" : 0, "weight" : "0", "settings" : { "mode" : "full" } },
        "search_api_alter_add_url" : { "status" : 0, "weight" : "0", "settings" : [] },
        "search_api_alter_add_aggregation" : { "status" : 0, "weight" : "0", "settings" : [] },
        "search_api_alter_add_hierarchy" : { "status" : 0, "weight" : "0", "settings" : { "fields" : [] } }
      },
      "processors" : {
        "search_api_case_ignore" : {
          "status" : 1,
          "weight" : "0",
          "settings" : { "fields" : {
              "title" : true,
              "field_hpbx_block" : true,
              "field_hpbx_block:field_hpbx_body" : true,
              "field_hpbx_block:field_hpbx_instruction_photo:field_hpbx_body" : true
            }
          }
        },
        "search_api_html_filter" : {
          "status" : 0,
          "weight" : "10",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_block:field_hpbx_body" : true,
              "field_hpbx_block:field_hpbx_instruction_photo:field_hpbx_body" : true
            },
            "title" : 0,
            "alt" : 1,
            "tags" : "h1 = 5\\r\\nh2 = 3\\r\\nh3 = 2\\r\\nstrong = 2\\r\\nb = 2\\r\\nem = 1.5\\r\\nu = 1.5"
          }
        },
        "search_api_tokenizer" : {
          "status" : 0,
          "weight" : "20",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_block:field_hpbx_body" : true,
              "field_hpbx_block:field_hpbx_instruction_photo:field_hpbx_body" : true
            },
            "spaces" : "[^[:alnum:]]",
            "ignorable" : "[\\u0027]"
          }
        },
        "search_api_stopwords" : {
          "status" : 0,
          "weight" : "30",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_block:field_hpbx_body" : true,
              "field_hpbx_block:field_hpbx_instruction_photo:field_hpbx_body" : true
            },
            "file" : "",
            "stopwords" : "but\\r\\ndid\\r\\nthe this that those\\r\\netc"
          }
        },
        "search_api_highlighting" : {
          "status" : 1,
          "weight" : "35",
          "settings" : {
            "prefix" : "\\u003Cstrong\\u003E",
            "suffix" : "\\u003C\\/strong\\u003E",
            "excerpt" : 1,
            "excerpt_length" : "140",
            "exclude_fields" : { "title" : "title" },
            "highlight" : "always"
          }
        }
      }
    },
    "enabled" : "1",
    "read_only" : "0"
  }');
  return $items;
}

/**
 * Implements hook_default_search_api_page().
 */
function drop_5_default_search_api_page() {
  $items = array();
  $items['hpbx_search_page'] = entity_import('search_api_page', '{
    "index_id" : "hpbx_articles",
    "path" : "hpbx\\/help\\/search",
    "name" : "hpbx search page",
    "machine_name" : "hpbx_search_page",
    "description" : null,
    "options" : {
      "mode" : "terms",
      "fields" : [],
      "per_page" : "50",
      "get_per_page" : 0,
      "result_page_search_form" : 0,
      "view_mode" : "search_api_page_result",
      "language_filter" : { "current" : "current" }
    },
    "enabled" : "1"
  }');
  return $items;
}
