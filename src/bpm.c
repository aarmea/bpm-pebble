#include "bpm.h"

#include "config.h"

static struct time_ms_t s_first_tap = { .s = 0, .ms = 0 };
static struct time_ms_t s_last_tap = { .s = 0, .ms = 0 };
static uint16_t s_tap_count = 0;
static bpm10_t s_bpm = 0;

struct time_ms_t difftime_time_ms_t(struct time_ms_t a, struct time_ms_t b) {
  // The time_ms_t structure is only capable of representing positive times. If
  // we are called with a < b, return the absolute value of the actual result.
  if (a.s < b.s || (a.s == b.s && a.ms < b.ms)) {
    APP_LOG(APP_LOG_LEVEL_INFO, "difftime called with a < b");
    struct time_ms_t temp = b;
    b = a;
    a = temp;
  }

  // Carry milliseconds into seconds if necessary
  if (a.ms < b.ms) {
    a.ms += 1000;
    a.s -= 1;
  }

  struct time_ms_t diff;
  diff.s = a.s - b.s;
  diff.ms = a.ms - b.ms;

  return diff;
}

static void bpm_update(struct time_ms_t time_now) {
  if (s_tap_count == 0) {
    s_bpm = 0;
    return;
  }

  struct time_ms_t total_time = difftime_time_ms_t(time_now, s_first_tap);
  APP_LOG(APP_LOG_LEVEL_INFO, "difftime_time_ms_t returned %d.%.3ds", (int)
      total_time.s, total_time.ms);

  // This representation will overflow if s_first_tap is more than 2^32
  // millisseconds (or 50 days (!)) ago. It's probably safe to say this will
  // never happen.
  uint32_t total_time_ms = total_time.s * 1000 + total_time.ms;
  s_bpm = (uint32_t) 1000 * 60 * 10 * s_tap_count / total_time_ms;
}

void bpm_manual_tap() {
  struct time_ms_t time_now;
  time_ms(&time_now.s, &time_now.ms);
  if ((s_first_tap.s == 0 && s_first_tap.ms == 0) ||
      difftime_time_ms_t(time_now, s_last_tap).s >= TAP_TIMEOUT_SECONDS) {
    // First launch, or too much time has passed since the last press
    APP_LOG(APP_LOG_LEVEL_INFO, "First tap");
    s_first_tap = time_now;
    s_tap_count = 0;
    s_bpm = 0;
  } else {
    // Normal case
    APP_LOG(APP_LOG_LEVEL_INFO, "Normal tap");
    ++s_tap_count;
    bpm_update(time_now);
  }
  s_last_tap = time_now;
}

bpm10_t bpm_get_bpm10() {
  return s_bpm;
}

uint16_t bpm_bpm10_to_period_ms(bpm10_t bpm) {
  return 1000 * 60 * 10 / bpm;
}
