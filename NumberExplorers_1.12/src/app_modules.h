#ifndef APP_MODULES_H
#define APP_MODULES_H
#include "app.h"
int attendance_claim(Save *s, int today, int yesterday);
void app_process_daily_attendance(App *a);
#endif
