#include "app.h"

/* Change the active scene and reset transient input state. */

void app_go(App *a, Scene s) {
    a->scene = s;
    a->help_open = false;
    a->focus = 0;
    a->click = false;
    a->activate = false;
    a->quit_confirm = false;
    a->reset_confirm = false;
    audio_cue(a, 0);
}
