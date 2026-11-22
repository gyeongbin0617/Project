#include "app.h"

/* Open or close the help overlay and restore focus state. */

void help_toggle(App *a) {
    if (a->help_open) {
        a->help_open = false;
        a->focus = a->help_focus;
    } else {
        a->help_focus = a->focus;
        a->help_page = (a->scene == LEARN || a->scene == LEARN_SETUP) ? 2 + a->learn_kind
                       : (a->scene == MATCH || a->scene == SETUP)
                           ? (int)a->chosen
                           : 0;
        a->help_open = true;
        a->focus = 0;
    }
    a->click = a->activate = false;
}
