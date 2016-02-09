#include "metronome.h"

#include "../config.h"
#include "../bpm.h"
#include "../icons.h"

static Window *s_metronome_window;
static char s_bpm_text_buffer[BPM_TEXT_BUFFER_SIZE] = BPM_DEFAULT;
static TextLayer *s_bpm_text_layer;
static TextLayer *s_metronome_hint_text_layer;
static ActionBarLayer *s_action_bar_layer;
static GPoint s_center;

static AppTimer *s_metronome_timer = NULL;

static VibePattern METRONOME_VIBE_PATTERN = {
  .durations = (const uint32_t const []) { METRONOME_VIBE_DURATION_MS },
  .num_segments = 1
};

static void metronome_window_update_bpm() {
  bpm10_t bpm = bpm_get_bpm10();
  if (bpm == 0)
    snprintf(s_bpm_text_buffer, BPM_TEXT_BUFFER_SIZE, "%s", BPM_DEFAULT);
  else
    snprintf(s_bpm_text_buffer, BPM_TEXT_BUFFER_SIZE, "%u.%u", bpm/10, bpm%10);
  layer_mark_dirty((Layer *) s_bpm_text_layer);
}

static void metronome_tick(void *callback_data) {
  vibes_enqueue_custom_pattern(METRONOME_VIBE_PATTERN);
  s_metronome_timer =
    app_timer_register(bpm_bpm10_to_period_ms(bpm_get_bpm10()),
        (AppTimerCallback) metronome_tick, NULL /*callback_data*/);
}

static void metronome_window_start() {
  metronome_tick(NULL /*callback_data*/);
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, ICON_PAUSE);
}

static void metronome_window_stop() {
  if (s_metronome_timer) {
    app_timer_cancel(s_metronome_timer);
    s_metronome_timer = NULL;
  }
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, ICON_PLAY);
}

static void metronome_window_start_stop(void *context) {
  if (s_metronome_timer)
    metronome_window_stop();
  else
    metronome_window_start();
}

static void metronome_window_click_config() {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)
      metronome_window_start_stop);
}

static void metronome_window_load(Window *window) {
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

  s_metronome_hint_text_layer = text_layer_create((GRect) {
    .origin = (GPoint) {
      .x = 0,
      .y = s_center.y - BPM_FONT_HEIGHT / 2 - METRONOME_HINT_FONT_HEIGHT
    },
    .size = (GSize) {
      .w = bounds.size.w - ACTION_BAR_WIDTH_WITH_GAP,
      .h = METRONOME_HINT_FONT_HEIGHT
    }
  });
  text_layer_set_text_alignment(s_metronome_hint_text_layer, GTextAlignmentRight);
  text_layer_set_font(s_metronome_hint_text_layer,
      fonts_get_system_font(METRONOME_HINT_FONT));
  text_layer_set_text(s_metronome_hint_text_layer, METRONOME_HINT_TEXT);
  layer_add_child(root_layer, (Layer *) s_metronome_hint_text_layer);

  s_action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_SELECT, ICON_PLAY);
  action_bar_layer_set_click_config_provider(s_action_bar_layer,
      metronome_window_click_config);
  action_bar_layer_add_to_window(s_action_bar_layer, window);

  metronome_window_update_bpm(NULL /*context*/);
}

static void metronome_window_unload(Window *window) {
  metronome_window_stop();

  text_layer_destroy(s_bpm_text_layer);
  text_layer_destroy(s_metronome_hint_text_layer);
  action_bar_layer_destroy(s_action_bar_layer);
  window_destroy(s_metronome_window);
}

void metronome_window_push() {
  s_metronome_window = window_create();
  window_set_background_color(s_metronome_window, BG_COLOR);
  window_set_window_handlers(s_metronome_window, (WindowHandlers) {
    .load = metronome_window_load,
    .unload = metronome_window_unload,
  });
  window_stack_push(s_metronome_window, true /*animated*/);
}
