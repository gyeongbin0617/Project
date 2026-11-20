#include "app.h"
#include "app_modules.h"
#include <stdio.h>
#include <time.h>

static int local_day(time_t timestamp) {
    struct tm day;
#ifdef _WIN32
    if (localtime_s(&day, &timestamp)) return 0;
#else
    struct tm *found = localtime(&timestamp);
    if (!found) return 0;
    day = *found;
#endif
    return (day.tm_year + 1900) * 1000 + day.tm_yday + 1;
}

void app_process_daily_attendance(App *a) {
    Save before = a->save;
    time_t now = time(NULL);
    int bonus = attendance_claim(&a->save, local_day(now), local_day(now - 86400));
    if (bonus && save_write(a)) {
        snprintf(a->toast, sizeof(a->toast), a->save.language ?
            "연속 출석 %d일! +%d코인" : "Day %d bonus! +%d coins",
            a->save.attendance_streak, bonus);
        a->toast_end = 7.0;
    } else if (bonus) {
        a->save = before;
    }
}
