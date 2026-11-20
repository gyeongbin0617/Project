#ifndef APP_INPUT_H
#define APP_INPUT_H

#include <SDL.h>
#include "app_types.h"

/* SDL 이벤트 폴링 루프에서 호출되는 최상위 입력 라우터 */
void app_event(App *a, SDL_Event *e);

/* 씬별 입력 핸들러 테이블 조회 (scene_handlers.c 구현) */
const SceneInputHandler *scene_get_input_handler(Scene scene);

#endif /* APP_INPUT_H */