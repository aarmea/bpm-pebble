#include <pebble.h>

#include "icons.h"
#include "windows/bpm_counter.h"

static void init() {
  icons_init();
  bpm_counter_window_push();
}

static void deinit() {
  icons_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
