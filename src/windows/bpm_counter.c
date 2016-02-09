#include "bpm_counter.h"
#include "metronome.h"

#include "../bpm.h"
#include "../icons.h"

static Window *s_bpm_counter_window;
static char s_bpm_text_buffer[BPM_TEXT_BUFFER_SIZE] = BPM_DEFAULT;
static TextLayer *s_bpm_text_layer;
static TextLayer *s_bpm_hint_text_layer;
static ActionBarLayer *s_action_bar_layer;
static GPoint s_center;

static void bpm_counter_window_update_bpm(void *context) {
  bpm_manual_tap();
  bpm10_t bpm = bpm_get_bpm10();
  if (bpm == 0)
    snprintf(s_bpm_text_buffer, BPM_TEXT_BUFFER_SIZE, "%s", BPM_DEFAULT);
  else
    snprintf(s_bpm_text_buffer, BPM_TEXT_BUFFER_SIZE, "%u.%u", bpm/10, bpm%10);
  layer_mark_dirty((Layer *) s_bpm_text_layer);
}

static void bpm_counter_window_click_config() {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)
      metronome_window_push);
  window_raw_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)
      bpm_counter_window_update_bpm, NULL /*up_handler*/, NULL /*context*/);
}

static void bpm_counter_window_load(Window *window) {
  Layer *root_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root_layer);
  s_center = grect_center_point(&bounds);

  s_bpm_text_layer = text_layer_create((GRect) {
    .origin = (GPoint) {
      .x = 0,
      .y = s_center.y - BPM_FONT_HEIGHT / 2
    },
    .size = (GSize) {
      .w = bounds.size.w - ACTION_BAR_WIDTH_WITH_GAP,
      .h = BPM_FONT_HEIGHT
    }
  });
  text_layer_set_overflow_mode(s_bpm_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_text_alignment(s_bpm_text_layer, GTextAlignmentRight);
  text_layer_set_font(s_bpm_text_layer, fonts_get_system_font(BPM_FONT));
  text_layer_set_text(s_bpm_text_layer, s_bpm_text_buffer);
  layer_add_child(root_layer, (Layer *) s_bpm_text_layer);

  s_bpm_hint_text_layer = text_layer_create((GRect) {
    .origin = (GPoint) {
      .x = 0,
      .y = s_center.y - BPM_FONT_HEIGHT / 2 - BPM_HINT_FONT_HEIGHT
    },
    .size = (GSize) {
      .w = bounds.size.w - ACTION_BAR_WIDTH_WITH_GAP,
      .h = BPM_HINT_FONT_HEIGHT
    }
  });
  text_layer_set_text_alignment(s_bpm_hint_text_layer, GTextAlignmentRight);
  text_layer_set_font(s_bpm_hint_text_layer,
      fonts_get_system_font(BPM_HINT_FONT));
  text_layer_set_text(s_bpm_hint_text_layer, BPM_HINT_TEXT);
  layer_add_child(root_layer, (Layer *) s_bpm_hint_text_layer);

  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, ICON_PLAY);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT,
      ICON_TEMPO_TAP);
  action_bar_layer_set_click_config_provider(s_action_bar_layer,
      bpm_counter_window_click_config);
  action_bar_layer_add_to_window(s_action_bar_layer, window);
}

static void bpm_counter_window_unload(Window *window) {
  text_layer_destroy(s_bpm_text_layer);
  text_layer_destroy(s_bpm_hint_text_layer);
  action_bar_layer_destroy(s_action_bar_layer);
  window_destroy(s_bpm_counter_window);
}

void bpm_counter_window_push() {
  s_bpm_counter_window = window_create();
  window_set_background_color(s_bpm_counter_window, BG_COLOR);
  window_set_window_handlers(s_bpm_counter_window, (WindowHandlers) {
    .load = bpm_counter_window_load,
    .unload = bpm_counter_window_unload,
  });
  window_stack_push(s_bpm_counter_window, true /*animated*/);
}
