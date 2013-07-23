<?php
/**
 * Provides default formatting for a set of images using the Royal Slider library.
 */

$output = "";
$slides = "";
$controls = "";
$link = "";

foreach($element['slides'] as $ind => $slide) {
  $attributes = array(
    'class' => array('rsImg')
  );
  $dimensions = array(
    'width' => $slide['#item']['width'],
    'height' => $slide['#item']['height'],
  );
  image_style_transform_dimensions($slide['#image_style'], $dimensions);

  if ($slide['#image_style']) {
    $path = file_is_scheme_remote($uri)
      ? remote_stream_wrapper_image_style_path($style_name, $uri)
      : image_style_url($slide['#image_style'], $slide['#item']['uri']);
  }
  else {
    $path = file_create_url($slide['#item']['uri']);
  }

  $attributes['data-rsw'] = $dimensions['width'];
  $attributes['data-rsh'] = $dimensions['height'];
  if(isset($element['fullscreen'][$ind])) {
    $attributes['data-rsbigimg'] = $element['fullscreen'][$ind]['#image_style']
      ? image_style_url($element['fullscreen'][$ind]['#image_style'], $element['fullscreen'][$ind]['#item']['uri'])
      : file_create_url($element['fullscreen'][$ind]['#item']['uri']);
  }
  $content = $slide['#item']['title'];
  if(isset($element['controls'][$ind])) {
    $element['controls'][$ind]['#item']['attributes']['class'][] = 'rsTmb';
    $content .= drupal_render($element['controls'][$ind]);
  }
  $output .= l($content, $path, array('attributes' => $attributes, 'html' => TRUE));
}

if(isset($element['#settings']['limit_link']) && $element['#settings']['limit_link'] && isset($element['#settings']['limit']) && $element['#settings']['limit'] > 0 && isset($element['#settings']['items_total']) && $element['#settings']['items_total'] > $element['#settings']['limit']) {
  $more_ct = $element['#settings']['items_total'] - $element['#settings']['limit'];
  $uri = entity_uri($element['#entity_type'], $element['#object']);
  $link = l(format_plural($more_ct, '@ct more photo', '@ct more photos', array('@ct' => $more_ct)), $uri['path'], $uri['options']);
  $link = "<div class=\"royalslider-link\">$link</div>";
}
$output = '<div id="' . $container_id .'" class="' . $variables['classes'] . '"' . $variables['attributes'] . ' style="width:100%;">' . $output . '</div>' . $link;

drupal_add_css(libraries_get_path('royalslider') . "/skins/default/rs-default.css");

drupal_add_js("jQuery(document).ready(function($) {
  $('#$container_id').royalSlider({
    fullscreen: {
      enabled: true,
      nativeFS: true
    },
    controlNavigation: 'thumbnails',
    autoScaleSlider: true,
    autoScaleSliderWidth: {$dimensions['width']},
    autoScaleSliderHeight: {$dimensions['height']},
    loop: false,
    imageScaleMode: 'fit-if-smaller',
    navigateByClick: true,
    numImagesToPreload:2,
    arrowsNav:true,
    arrowsNavAutoHide: true,
    arrowsNavHideOnTouch: true,
    keyboardNavEnabled: true,
    fadeinLoadedSlide: true,
    globalCaption: true,
    globalCaptionInside: false,
    thumbs: {
      appendSpan: true,
      firstMargin: true,
      paddingBottom: 4
    }
  });
});", "inline");

print($output);
