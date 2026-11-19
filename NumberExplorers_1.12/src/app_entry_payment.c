#include "app.h"

/* Deduct the entry fee for paid game modes. */

bool app_pay_entry(App *a, int mode) {
    if (mode == 0) return true;
    if (a->save.coins < 5) { app_message(a, "Not enough coins - play Classic 369"); return false; }
    Save before = a->save;
    a->save.coins -= 5;
    if (!save_write(a)) { a->save = before; app_message(a, "Could not save progress. Check folder access."); return false; }
    return true;
}
