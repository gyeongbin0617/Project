#include "app.h"
#include <stdio.h>

/* Display a localized temporary message. */

void app_message(App *a, const char *s) {
    snprintf(a->toast, sizeof(a->toast), "%s", L(a, s));
    a->toast_end = a->clock + 3.0;
}
