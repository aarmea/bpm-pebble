#include "metronome.h"

#include "../config.h"
#include "../icons.h"

static Window *s_metronome_window;

static void metronome_window_load(Window *window) {
  // TODO
}

static void metronome_window_unload(Window *window) {
  // TODO
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
