#include "app.h"
#include "app_core.h"
#include "app_help.h"

/* [내부 캡슐화] UIBoxSpec 구조체 포인터를 받아 둥근 패널을 렌더링 */
static void render_box_spec(App *a, const UIBoxSpec *spec) {
    if (!a || !spec) return;
    ui_round(a, spec->rect.x, spec->rect.y, spec->rect.w, spec->rect.h,
             spec->radius, spec->color);
}

/* [내부 캡슐화] 씬별 배경 이미지 및 반투명 틴트 오버레이 렌더링 */
static void render_backdrop(App *a) {
    const SDL_Rect full_screen = {0, 0, WIDTH, HEIGHT};
    Uint8 tint_alpha = (a->scene == TITLE)   ? 115
                     : (a->scene == MATCH)   ? 170
                     : a->save.background    ? 155 : 222;

    SDL_SetRenderDrawColor(a->renderer, 250, 240, 222, 255);
    SDL_RenderClear(a->renderer);
    ui_background(a, a->save.background, full_screen);

    SDL_SetRenderDrawColor(a->renderer, 255, 249, 239, tint_alpha);
    SDL_RenderFillRect(a->renderer, NULL);
}

/* [내부 캡슐화] 타이틀 외 씬 상단 공통 헤더 카드 및 구분선 렌더링 */
static void render_top_header(App *a) {
    if (a->scene == TITLE) return;

    const UIBoxSpec header_card = {
        .rect = {18, 12, 1244, 89},
        .radius = 23,
        .color = {255, 253, 246, 245}
    };
    const UIBoxSpec divider_line = {
        .rect = {38, 99, 1204, 2},
        .radius = 1,
        .color = {214, 204, 200, 180}
    };

    render_box_spec(a, &header_card);
    render_box_spec(a, &divider_line);
}

/* [내부 캡슐화] 하단 토스트 알림 배너 렌더링 */
static void render_toast_banner(App *a) {
    if (a->clock >= a->toast_end) return;

    const UIBoxSpec toast_box = {
        .rect = {270, 651, 740, 45},
        .radius = 18,
        .color = PLUM
    };
    const SDL_Point text_pos = {640, 662};

    render_box_spec(a, &toast_box);
    ui_text(a, a->toast, text_pos.x, text_pos.y, 18, CREAM, true);
}

/* [내부 캡슐화] 매치(미니게임) 진행 및 피드백·종료 상태 전이 갱신 */
static void update_match_session(App *a, double dt) {
    if (a->paused || a->help_open) return;

    Phase before = a->game.phase;
    game_update(&a->game, dt);

    if (before == TURN && a->game.phase == FEEDBACK) {
        app_note_feedback(a);
        audio_cue(a, a->game.success ? 1 : 2);
    }
    if (a->game.phase == FINISHED) {
        app_finish(a);
    }
}

void app_help_suspend_input(App *a, InputSnapshot *out_snapshot) {
    if (!a || !out_snapshot) return;

    out_snapshot->click = a->click;
    out_snapshot->activate = a->activate;
    out_snapshot->moved = a->moved;
    out_snapshot->focus = a->focus;

    a->click = false;
    a->activate = false;
    a->moved = false;
    a->focus = -1;
}

void app_help_restore_input(App *a, const InputSnapshot *snapshot) {
    if (!a || !snapshot) return;

    a->click = snapshot->click;
    a->activate = snapshot->activate;
    a->moved = snapshot->moved;
    a->focus = snapshot->focus;
}

void app_update(App *a, double dt) {
    if (!a) return;

    a->clock += dt;
    if (a->scene == PUZZLE) {
        puzzle_update(a);
    } else if (a->scene == MATCH) {
        update_match_session(a, dt);
    }
}

void app_render(App *a) {
    if (!a) return;

    a->button_count = 0;
    render_backdrop(a);
    render_top_header(a);

    InputSnapshot input_backup;
    bool help_active = a->help_open;
    if (help_active) {
        app_help_suspend_input(a, &input_backup);
    }

    render_scene(a);

    if (help_active) {
        app_help_restore_input(a, &input_backup);
        help_render(a);
    } else if (a->scene != TITLE) {
        const SDL_Rect help_btn = {920, 30, 124, 48};
        if (ui_button(a, "HELP [F1]", help_btn.x, help_btn.y,
                      help_btn.w, help_btn.h, LAVENDER, true)) {
            help_toggle(a);
        }
    }

    render_toast_banner(a);

    a->previous_buttons = a->button_count;
    if (a->focus >= a->button_count) {
        a->focus = 0;
    }
    a->click = false;
    a->activate = false;
    a->moved = false;
}