#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <SDL.h>
#include "app.h"

/* 도움말 오버레이 활성화 시 입력 상태를 백업/복원하기 위한 스냅샷 구조체 */
typedef struct {
    bool click;
    bool activate;
    bool moved;
    int focus;
} InputSnapshot;

/* UI 둥근 패널 영역·반경·색상을 묶어 전달하는 스타일 규격 구조체 */
typedef struct {
    SDL_Rect rect;
    int radius;
    SDL_Color color;
} UIBoxSpec;

/* 포커스 순환 이동 범위를 전달하는 내비게이션 구조체 */
typedef struct {
    int *current_focus;
    int total_items;
    int step;
} FocusNavSpec;

/* 씬별 키 입력 및 ESC 뒤로가기 함수 포인터 핸들러 구조체 */
typedef struct {
    bool (*on_key)(App *a, SDL_Keycode key);
    void (*on_escape)(App *a);
} SceneInputHandler;

#endif /* APP_TYPES_H */