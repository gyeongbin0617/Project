#include "app.h"
#include "app_input.h"
#include "app_help.h"
#include "shop_settings.h"

/* [내부 캡슐화] FocusNavSpec 구조체 포인터를 사용한 포커스 순환 이동 */
static void step_focus(const FocusNavSpec *nav) {
    if (!nav || !nav->current_focus || nav->total_items <= 0) return;

    int next = (*nav->current_focus + nav->step) % nav->total_items;
    if (next < 0) next += nav->total_items;
    *nav->current_focus = next;
}

void app_help_handle_key(App *a, SDL_Keycode k) {
    if (!a) return;

    if (k == SDLK_TAB || k == SDLK_RIGHT || k == SDLK_DOWN) {
        if (a->scene == PUZZLE) {
            a->focus = 0;
        } else {
            FocusNavSpec nav = { &a->focus, 6, 1 };
            step_focus(&nav);
        }
    } else if (k == SDLK_LEFT || k == SDLK_UP) {
        if (a->scene == PUZZLE) {
            a->focus = 0;
        } else {
            FocusNavSpec nav = { &a->focus, 6, -1 };
            step_focus(&nav);
        }
    } else if (k == SDLK_RETURN || k == SDLK_SPACE) {
        a->activate = true;
    }
}

/* [내부 캡슐화] 마우스 이동·클릭·휠 이벤트 처리 */
static bool process_mouse_event(App *a, const SDL_Event *e) {
    if (e->type == SDL_MOUSEMOTION) {
        const SDL_Point pos = { e->motion.x, e->motion.y };
        a->mx = pos.x;
        a->my = pos.y;
        a->moved = true;
        return false;
    }
    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        const SDL_Point pos = { e->button.x, e->button.y };
        a->mx = pos.x;
        a->my = pos.y;
        a->click = true;
        return false;
    }
    if (a->scene == SINGLE && !a->help_open && e->type == SDL_MOUSEWHEEL) {
        int direction = e->wheel.x ? e->wheel.x : -e->wheel.y;
        if (e->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) direction = -direction;
        if (direction > 0 && a->mode_page < 2) a->mode_page++;
        if (direction < 0 && a->mode_page > 0) a->mode_page--;
        a->focus = 0;
        return true;
    }
    return false;
}

/* [내부 캡슐화] 기본 UI 포커스 이동 및 확인 키 처리 */
static void handle_default_ui_nav(App *a, SDL_Keycode k) {
    if (k == SDLK_TAB || k == SDLK_DOWN || k == SDLK_RIGHT) {
        FocusNavSpec nav = { &a->focus, a->previous_buttons, 1 };
        step_focus(&nav);
    } else if (k == SDLK_UP || k == SDLK_LEFT) {
        FocusNavSpec nav = { &a->focus, a->previous_buttons, -1 };
        step_focus(&nav);
    } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_SPACE) {
        a->activate = true;
    }
}

void app_event(App *a, SDL_Event *e) {
    if (!a || !e) return;

    if (e->type == SDL_QUIT) {
        if (a->scene == MATCH) {
            a->paused = true;
            a->quit_confirm = true;
        } else {
            a->running = false;
        }
        return;
    }

    if (e->type == SDL_WINDOWEVENT &&
        e->window.event == SDL_WINDOWEVENT_FOCUS_LOST &&
        a->scene == MATCH) {
        a->paused = true;
    }

    if (process_mouse_event(a, e)) return;

    if (a->scene == TITLE && (e->type == SDL_KEYDOWN || e->type == SDL_MOUSEBUTTONUP)) {
        if (e->type == SDL_KEYDOWN && e->key.repeat) return;
        app_go(a, LOBBY);
        return;
    }

    if (e->type != SDL_KEYDOWN || e->key.repeat) return;

    SDL_Keycode k = e->key.keysym.sym;

    if (k == SDLK_F11) {
        settings_toggle_fullscreen(a->window, &a->save, a);
        return;
    }
    if (k == SDLK_F1 || (k == SDLK_ESCAPE && a->help_open)) {
        help_toggle(a);
        return;
    }
    if (a->help_open) {
        app_help_handle_key(a, k);
        return;
    }

    const SceneInputHandler *handler = scene_get_input_handler(a->scene);

    if (k == SDLK_ESCAPE) {
        a->click = false;
        a->activate = false;
        if (handler && handler->on_escape) {
            handler->on_escape(a);
        } else {
            app_go(a, LOBBY);
        }
        return;
    }

    if (handler && handler->on_key && handler->on_key(a, k)) {
        return;
    }

    handle_default_ui_nav(a, k);
}