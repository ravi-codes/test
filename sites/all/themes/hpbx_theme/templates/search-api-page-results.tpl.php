<?php
/**
 * @file
 * Default theme implementation for displaying search results.
 *
 * This template collects each invocation of theme_search_result(). This and the
 * child template are dependent on one another, sharing the markup for
 * definition lists.
 *
 * Note that modules and themes may implement their own search type and theme
 * function completely bypassing this template.
 *
 * Available variables:
 * - $index: The search index this search is based on.
 * - $result_count: Number of results.
 * - $spellcheck: Possible spelling suggestions from Search spellcheck module.
 * - $search_results: All results rendered as list items in a single HTML
 *   string.
 * - $items: All results as it is rendered through search-result.tpl.php.
 * - $search_performance: The number of results and how long the query took.
 * - $sec: The number of seconds it took to run the query.
 * - $pager: Row of control buttons for navigating between pages of results.
 * - $keys: The keywords of the executed search.
 * - $classes: String of CSS classes for search results.
 * - $page: The current Search API Page object.
 * - $no_results_help: Help text to display under the header if no results were
 *   found.
 *
 * View mode is set in the Search page settings. If you select
 * "Themed as search results", then the child template will be used for
 * theming the individual result. Any other view mode will bypass the
 * child template.
 *
 * @see template_preprocess_search_api_page_results()
 */

?>


<div class="container hpbx-no-menu">
  <div class="row hpbx-help-content">
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
      <div class="col-xs-10 col-xxs-12 col-sm-8 col-md-6">
        <div class="hpbx-help-search-results-amount"><?php print format_plural($result_count,
      '1 search result for @keys', 
      '@result_count search results for @keys',
      array('@result_count' => $result_count, '@keys' => $keys)); ?></div>

        <?php if ($result_count): ?>
          
          <div class="hpbx-help-search-results-container">
              <?php print render($search_results); ?>
          </div>
          <?php endif; ?>
      </div>
      <div class="col-xs-1 col-sm-2 col-md-3 hidden-xxs"></div>
  </div>
</div>
