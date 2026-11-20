#include "app.h"
#include "app_input.h"

/* --- 각 씬별 키 입력 처리 핸들러 --- */

static bool handle_single_key(App *a, SDL_Keycode k) {
    if (k == SDLK_LEFT && a->mode_page > 0) {
        a->mode_page--;
        a->focus = 0;
        return true;
    }
    if (k == SDLK_RIGHT && a->mode_page < 2) {
        a->mode_page++;
        a->focus = 0;
        return true;
    }
    return false;
}

static bool handle_learn_key(App *a, SDL_Keycode k) {
    if (k >= SDLK_1 && k <= SDLK_3) {
        learn_choose(a, (int)(k - SDLK_1));
        return true;
    }
    if ((k == SDLK_RETURN || k == SDLK_SPACE) && a->lesson.solved) {
        learn_advance(a);
        return true;
    }
    return false;
}

static bool handle_review_key(App *a, SDL_Keycode k) {
    if (k >= SDLK_1 && k <= SDLK_3) {
        review_answer(a, (int)(k - SDLK_1));
        return true;
    }
    if (a->review_feedback && (k == SDLK_RETURN || k == SDLK_SPACE)) {
        a->review_index = mistake_next(&a->save.mistakes, a->review_index);
        a->review_feedback = false;
        return true;
    }
    return false;
}

static bool handle_train_key(App *a, SDL_Keycode k) {
    if (k >= SDLK_1 && k <= SDLK_3) {
        train_answer(a, (int)(k - SDLK_1));
        return true;
    }
    if (a->train.solved && (k == SDLK_RETURN || k == SDLK_SPACE)) {
        train_advance(a);
        return true;
    }
    return false;
}

static bool handle_puzzle_key(App *a, SDL_Keycode k) {
    return puzzle_key(a, k);
}

static bool handle_match_key(App *a, SDL_Keycode k) {
    if (!a->paused) {
        app_game_key(a, k);
        return true;
    }
    return false;
}

/* --- 각 씬별 ESC 뒤로가기 핸들러 --- */

static void esc_lobby(App *a)     { app_go(a, TITLE); }
static void esc_to_single(App *a) { app_go(a, SINGLE); }
static void esc_to_lobby(App *a)  { app_go(a, LOBBY); }
static void esc_learn(App *a)     { app_go(a, LEARN_SETUP); }

static void esc_match(App *a) {
    a->paused = !a->paused;
    a->quit_confirm = false;
    a->focus = 0;
}

static void esc_learn_setup(App *a) {
    if (a->show_table) a->show_table = false;
    else app_go(a, SINGLE);
}

static void esc_settings(App *a) {
    save_write(a);
    app_go(a, a->settings_back);
}

/* --- 씬별 함수 포인터 핸들러 테이블 --- */
const SceneInputHandler *scene_get_input_handler(Scene scene) {
    static const SceneInputHandler k_handlers[] = {
        [TITLE]       = { NULL,              NULL },
        [LOBBY]       = { NULL,              esc_lobby },
        [SINGLE]      = { handle_single_key, esc_to_lobby },
        [SETUP]       = { NULL,              esc_to_single },
        [LEARN_SETUP] = { NULL,              esc_learn_setup },
        [TUTORIAL]    = { NULL,              esc_to_single },
        [PUZZLE]      = { handle_puzzle_key, esc_to_single },
        [TRAIN]       = { handle_train_key,  esc_to_single },
        [REVIEW]      = { handle_review_key, esc_to_lobby },
        [LEARN]       = { handle_learn_key,  esc_learn },
        [MATCH]       = { handle_match_key,  esc_match },
        [SETTINGS]    = { NULL,              esc_settings },
    };

    const int count = (int)(sizeof(k_handlers) / sizeof(k_handlers[0]));
    if ((int)scene >= 0 && (int)scene < count) {
        return &k_handlers[scene];
    }
    return NULL;
}