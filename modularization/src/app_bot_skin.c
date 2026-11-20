#include "app.h"

/* Calculate the bot skin index for a player. */

int app_bot_skin(const App *a, int player) {
    return (a->save.skin + player) % 8;
}
