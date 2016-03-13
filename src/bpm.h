#pragma once

#include <pebble.h>

typedef uint16_t bpm10_t;

struct time_ms_t {
  time_t s;
  uint16_t ms;
};

struct time_ms_t difftime_time_ms_t(struct time_ms_t a, struct time_ms_t b);

void bpm_manual_tap();

bpm10_t bpm_get_bpm10();
void bpm_set_bpm10(bpm10_t bpm);

uint16_t bpm_bpm10_to_period_ms(bpm10_t bpm);
