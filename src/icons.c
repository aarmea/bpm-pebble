#include "icons.h"

GBitmap *ICON_TEMPO_TAP;
GBitmap *ICON_PLAY;
GBitmap *ICON_PAUSE;

void icons_init() {
  ICON_TEMPO_TAP = gbitmap_create_with_resource(RESOURCE_ID_ICON_TEMPO_TAP);
  ICON_PLAY = gbitmap_create_with_resource(RESOURCE_ID_ICON_PLAY);
  ICON_PAUSE = gbitmap_create_with_resource(RESOURCE_ID_ICON_PAUSE);
}

void icons_deinit() {
  gbitmap_destroy(ICON_TEMPO_TAP);
}
