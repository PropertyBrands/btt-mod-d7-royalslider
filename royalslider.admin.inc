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

  // Show select boxes for the various image styles (thumbnail, normal, big)
  $image_style = image_style_options(FALSE);
  $form['image_style'] = array(
    '#type' => 'fieldset',
    '#title' => 'Image style',
    '#tree' => TRUE,
  );
  $form['image_style']['normal'] = array(
    '#type' => 'select',
    '#title' => t('Normal image style'),
    '#description' => t('Image style for the main stage images.'),
    '#empty_option' => t('None (original image)'),
    '#options' => $image_style,
    '#default_value' => $optionset->imagestyle_normal,
  );
  $form['image_style']['thumbnail'] = array(
    '#type' => 'select',
    '#title' => t('Thumbnail image style'),
    '#description' => t('Image style for the thumbnail images.'),
    '#empty_option' => t('None (original image)'),
    '#options' => $image_style,
    '#default_value' => $optionset->imagestyle_thumbnail,
    // @todo enable/disable this option based on the type of options selected
    // Ex: Thumbnails should be enabled
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
