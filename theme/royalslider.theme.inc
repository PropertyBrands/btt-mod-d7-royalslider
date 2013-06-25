<?php
/**
 * Implements HOOK_preprocess() for royalslider_formatter
 */
function royalslider_preprocess_royalslider_formatter(&$vars) {
  drupal_add_js(libraries_get_path('royalslider') . "/jquery.royalslider.min.js");
  drupal_add_css(libraries_get_path('royalslider') . "/royalslider.css");
  $vars['container_id'] = 'royalslider-' . $vars['element']['#entity_type'] . "-" . implode(entity_extract_ids($vars['element']['#entity_type'], $vars['element']['#object']), '-');
  $vars['classes_array'][] = 'royalSlider';
  $vars['classes_array'][] = 'rsDefault';
}
