<?php
/**
 * @file
 * Admin forms for RoyalSlider.
 */

/**
 * Submit handler for adding a new option set.
 */
function royalslider_form_optionset_add_submit($form, &$form_state) {
  $optionset = royalslider_optionset_create(array('name' => $form_state['values']['name'], 'title' => $form_state['values']['title']));

  $saved = royalslider_optionset_save($optionset, TRUE);

  if ($saved) {
    drupal_set_message(t('Option set %name was created.', array('%name' => $optionset->name)));
    $form_state['redirect'] = 'admin/config/media/royalslider/edit/' . $optionset->name;
  }
  else {
    drupal_set_message(t('Failed to create option set. Please verify your settings.'), 'error');
  }
}


/**
 * Defines the form elements used to edit the RoyalSlider library options
 *
 * @param array $options [optional]
 *  Pass in a set of default values for the options
 * @return array
 *  Returns the option set array
 */
function royalslider_option_elements($options = array()) {
  $form = array();

  // Load available image formatters.
  module_load_include('inc', 'field_ui', 'field_ui.admin');
  $formatters = field_ui_formatter_options('image');
  // But not RoyalSlider, that would be inception.
  unset($formatters['royalslider']);

  // Full-screen settings.
  $form['fullscreen'] = array(
    '#type' => 'fieldset',
    '#title' => t('Full Screen settings'),
    // For whatever reason this isn't working properly here so let's just
    // prefix all nested setting names.
//    '#tree' => TRUE,
  );

  $fullscreen_formatter = isset($form_state['values']['fullscreen_formatter'])
    ? $form_state['values']['fullscreen_formatter']
    : '';

  $form['fullscreen']['fullscreen_formatter'] = array(
    '#type' => 'select',
    '#title' => t('Full Screen Formatter'),
    '#options' => array('' => t('None')) + $formatters,
    '#default_value' => $fullscreen_formatter,
    '#ajax' => array(
      'callback' => 'royalslider_ajax_fullscreen_formatter',
      'wrapper' => 'fullscreen-formatter-settings',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form['fullscreen']['formatter_settings'] = array(
    '#prefix' => '<div id="fullscreen-formatter-settings">',
    '#suffix' => '</div>',
  );

  // General Slideshow and Animiation Settings
  $form['animation_slideshow'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Slideshow and Animation Settings'),
  );

  $form['animation_slideshow']['animation'] = array(
    '#type' => 'select',
    '#title' => t('Animation'),
    '#description' => t('Select your animation type'),
    '#options' => array(
      'fade'   => t('Fade'),
      'slide'  => t('Slide'),
    ),
    '#default_value' => isset($options['animation']) ? $options['animation'] : _royalslider_optionset_defaults('animation'),
  );

  // Build in support for easing plugin
  $easing_options = array('swing' => t('Swing'), 'linear' => t('Linear'));
  if (module_exists('jqeasing')) {
    $easing_options = array_merge($easing_options, _royalslider_jqeasing_options());
  }

  $form['animation_slideshow']['easing'] = array(
    '#type' => 'select',
    '#title' => t('Easing'),
    '#multiple' => FALSE,
    '#description' => t('The description appears usually below the item.'),
    '#options' => $easing_options,
    '#default_value' => isset($options['easing']) ? $options['easing'] : _royalslider_optionset_defaults('easing'),
  );

  return $form;
}

/**
 * Selects just the second dropdown to be returned for re-rendering
 *
 * Since the controlling logic for populating the form is in the form builder
 * function, all we do here is select the element and return it to be updated.
 *
 * @return renderable array
 */
function royalslider_ajax_fullscreen_formatter($form, $form_state) {
  $response = '';
  $fullscreen_formatter = isset($form_state['values']['fullscreen_formatter'])
    ? $form_state['values']['fullscreen_formatter']
    : '';

  if (!empty($fullscreen_formatter)) {
    if ($formatter = field_info_formatter_types($fullscreen_formatter)) {
      $function = $formatter['module'] . '_field_formatter_settings_form';

      if (function_exists($function)) {
        $new_form = call_user_func($function, NULL);
        dfb($new_form);
      }
    }
  }

  return $response;
}

/**
 * Form builder; Form to edit a given option set.
 */
function royalslider_form_optionset_edit($form, &$form_state) {

  if (empty($form_state['optionset'])) {
    $optionset = royalslider_optionset_create();
  }
  else {
    $optionset = $form_state['optionset'];
  }

  // Title
  $form['title'] = array(
    '#type' => 'textfield',
    '#maxlength' => '255',
    '#title' => t('Title'),
    '#description' => t('A human-readable title for this option set.'),
    '#required' => TRUE,
    '#default_value' => $optionset->title,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => '255',
    '#machine_name' => array(
      'source' => array('title'),
      'exists' => 'royalslider_optionset_exists',
    ),
    '#required' => TRUE,
    '#default_value' => $optionset->name,
  );

  // Options Vertical Tab Group table
  $form['options'] = array(
    '#type' => 'vertical_tabs',
  );

  $default_options = royalslider_option_elements($optionset->options);
  // Add the options to the vertical tabs section
  foreach ($default_options as $key => $value) {
    $form['options'][] = $value;
  }

  return $form;
}

/**
 * Form builder; Form to delete a given option set.
 */
function royalslider_optionset_form_delete($form, &$form_state, $optionset) {
  $form_state['optionset'] = &$optionset;

  // Deleting an export in code will revert it.
  $op = ($optionset->export_type & EXPORT_IN_CODE) ? 'Revert' : 'Delete';

  return confirm_form(
    $form,
    t('Are you sure you want to @action the option set %name?', array('@action' => t(drupal_strtolower($op)), '%name' => $optionset->name)),
    'admin/config/media/royalslider',
    NULL,
    t($op),  t('Cancel')
  );
}

/**
 * Submit handler for deleting an option set.
 */
function royalslider_optionset_form_delete_submit($form, &$form_state) {
  $optionset = &$form_state['optionset'];
  $op = ($optionset->export_type & EXPORT_IN_CODE) ? 'reverted' : 'deleted';

  ctools_include('export');
  ctools_export_crud_delete('royalslider_optionset', $optionset);

  drupal_set_message(t('Option set %name was ' . $op . '.', array('%name' => $optionset->name)));
  $form_state['redirect'] = 'admin/config/media/royalslider';
}
