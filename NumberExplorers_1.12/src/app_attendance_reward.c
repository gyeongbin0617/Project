#include "app.h"

/* Calculate and apply the daily attendance reward. */

int attendance_claim(Save *s, int today, int yesterday) {
    if (today <= 0 || s->last_claim_day >= today) return 0;
    s->attendance_streak = s->last_claim_day == yesterday ?
        (s->attendance_streak < 7 ? s->attendance_streak + 1 : 7) : 1;
    s->last_claim_day = today;
    int bonus = 10 + 2 * (s->attendance_streak - 1);
    s->coins = s->coins > 999999 - bonus ? 999999 : s->coins + bonus;
    return bonus;
}
