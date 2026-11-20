#ifndef APP_HELP_H
#define APP_HELP_H

#include "app_types.h"

/* [F1] 도움말 오버레이 열림 시 배경 UI 입력 차단 및 복원 */
void app_help_suspend_input(App *a, InputSnapshot *out_snapshot);
void app_help_restore_input(App *a, const InputSnapshot *snapshot);

/* 도움말 오버레이 전용 방향키/TAB 포커스 이동 처리 */
void app_help_handle_key(App *a, SDL_Keycode key);

#endif /* APP_HELP_H */