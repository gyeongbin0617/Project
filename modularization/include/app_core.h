#ifndef APP_CORE_H
#define APP_CORE_H

#include "app_types.h"

/* 메인 프레임 업데이트 및 렌더링 진입점 */
void app_update(App *a, double dt);
void app_render(App *a);

#endif /* APP_CORE_H */