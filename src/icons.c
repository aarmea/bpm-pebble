#include "icons.h"

GBitmap *ICON_TEMPO_TAP;

void icons_init() {
  ICON_TEMPO_TAP = gbitmap_create_with_resource(RESOURCE_ID_ICON_TEMPO_TAP);
}

void icons_deinit() {
  gbitmap_destroy(ICON_TEMPO_TAP);
}
