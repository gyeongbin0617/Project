#include "app.h"

/* Save an incorrect game answer for later review. */

void app_note_feedback(App *a) {
    Game *g=&a->game;
    if (g->last_player!=0 || g->success) return;
    mistake_add(&a->save.mistakes,0,g->value,g->rule,g->last_answer);
    if (!save_write(a)) app_message(a,"Could not save progress. Check folder access.");
}
