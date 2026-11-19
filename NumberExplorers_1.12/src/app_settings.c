#include "app.h"

/* Restore default application settings. */

void app_reset_settings(Save *s) {
    s->music=50; s->sfx=70; s->motion=1; s->fullscreen=0; s->language=1;
}
