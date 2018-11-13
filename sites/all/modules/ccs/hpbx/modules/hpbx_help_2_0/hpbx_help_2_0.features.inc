<?php
/**
 * @file
 * hpbx_help_2_0.features.inc
 */

/**
 * Implements hook_views_api().
 */
function hpbx_help_2_0_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function hpbx_help_2_0_node_info() {
  $items = array(
    'hpbx_help_article' => array(
      'name' => t('HPBX Help article'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Titel'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_default_search_api_index().
 */
function hpbx_help_2_0_default_search_api_index() {
  $items = array();
  $items['default_node_index'] = entity_import('search_api_index', '{
    "name" : "Default node index",
    "machine_name" : "default_node_index",
    "description" : "An automatically created search index for indexing node data. Might be configured to specific needs.",
    "server" : null,
    "item_type" : "node",
    "options" : {
      "index_directly" : 1,
      "cron_limit" : "50",
      "data_alter_callbacks" : {
        "search_api_alter_bundle_filter" : {
          "status" : 0,
          "weight" : "-10",
          "settings" : { "default" : "1", "bundles" : [] }
        },
        "search_api_alter_node_access" : { "status" : 1, "weight" : "0", "settings" : [] },
        "search_api_alter_node_status" : { "status" : 1, "weight" : "0", "settings" : [] },
        "search_api_alter_language_control" : {
          "status" : 1,
          "weight" : "0",
          "settings" : {
            "lang_field" : "language",
            "languages" : {
              "und" : "und",
              "nl" : "nl",
              "dm" : "dm",
              "en" : "en",
              "de" : "de",
              "ro" : "ro"
            }
          }
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
              "field_hpbx_body" : true,
              "field_step:field_hpbx_body" : true,
              "field_step:field_hpbx_instruction_photo:field_hpbx_body" : true
            }
          }
        },
        "search_api_html_filter" : {
          "status" : 1,
          "weight" : "10",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_body" : true,
              "field_step:field_hpbx_body" : true,
              "field_step:field_hpbx_instruction_photo:field_hpbx_body" : true
            },
            "title" : 0,
            "alt" : 1,
            "tags" : "h1 = 5\\r\\nh2 = 3\\r\\nh3 = 2\\r\\nstrong = 2\\r\\nb = 2\\r\\nem = 1.5\\r\\nu = 1.5"
          }
        },
        "search_api_tokenizer" : {
          "status" : 1,
          "weight" : "20",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_body" : true,
              "field_step:field_hpbx_body" : true,
              "field_step:field_hpbx_instruction_photo:field_hpbx_body" : true
            },
            "spaces" : "[^\\\\p{L}\\\\p{N}]",
            "ignorable" : "[-]"
          }
        },
        "search_api_stopwords" : {
          "status" : 0,
          "weight" : "30",
          "settings" : {
            "fields" : {
              "title" : true,
              "field_hpbx_body" : true,
              "field_step:field_hpbx_body" : true,
              "field_step:field_hpbx_instruction_photo:field_hpbx_body" : true
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
            "excerpt_length" : "256",
            "exclude_fields" : [],
            "highlight" : "always"
          }
        }
      },
      "fields" : {
        "author" : { "type" : "integer", "entity_type" : "user" },
        "changed" : { "type" : "date" },
        "created" : { "type" : "date" },
        "field_hpbx_block" : {
          "type" : "list\\u003Cinteger\\u003E",
          "entity_type" : "field_collection_item"
        },
        "promote" : { "type" : "boolean" },
        "search_api_language" : { "type" : "string" },
        "sticky" : { "type" : "boolean" },
        "title" : { "type" : "text", "boost" : "5.0" },
        "type" : { "type" : "string" }
      }
    },
    "enabled" : "0",
    "read_only" : "0"
  }');
  return $items;
}
