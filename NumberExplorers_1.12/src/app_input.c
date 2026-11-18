#include "app.h"
void app_event(App *a, SDL_Event *e) {
    if (e->type == SDL_QUIT) {
        if (a->scene == MATCH) {
            a->paused = true;
            a->quit_confirm = true;
        } else
            a->running = false;
        return;
    }
    if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_FOCUS_LOST &&
        a->scene == MATCH)
        a->paused = true;
    if (e->type == SDL_MOUSEMOTION) {
        a->mx = e->motion.x;
        a->my = e->motion.y;
        a->moved = true;
    }
    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        a->mx = e->button.x;
        a->my = e->button.y;
        a->click = true;
    }
    if (a->scene == TITLE && (e->type == SDL_KEYDOWN || e->type == SDL_MOUSEBUTTONUP)) {
        if (e->type == SDL_KEYDOWN && e->key.repeat)
            return;
        app_go(a, LOBBY);
        return;
    }
    if(a->scene==SINGLE && !a->help_open && e->type==SDL_MOUSEWHEEL) {
        int direction=e->wheel.x ? e->wheel.x : -e->wheel.y;
        if(e->wheel.direction==SDL_MOUSEWHEEL_FLIPPED) direction=-direction;
        if(direction>0 && a->mode_page<2) a->mode_page++;
        if(direction<0 && a->mode_page>0) a->mode_page--;
        a->focus=0;
        return;
    }
    if (e->type != SDL_KEYDOWN || e->key.repeat)
        return;
    SDL_Keycode k = e->key.keysym.sym;
    if (k == SDLK_F11) {
        a->save.fullscreen = !a->save.fullscreen;
        SDL_SetWindowFullscreen(a->window, a->save.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        save_write(a);
        return;
    }
    if (k == SDLK_F1 || (k == SDLK_ESCAPE && a->help_open)) {
        help_toggle(a);
        return;
    }
    if (a->help_open) {
        if (k == SDLK_TAB || k == SDLK_RIGHT || k == SDLK_DOWN)
            a->focus = a->scene == PUZZLE ? 0 : (a->focus + 1) % 6;
        if (k == SDLK_LEFT || k == SDLK_UP)
            a->focus = a->scene == PUZZLE ? 0 : (a->focus + 5) % 6;
        if (k == SDLK_RETURN || k == SDLK_SPACE)
            a->activate = true;
        return;
    }
    if(a->scene==SINGLE && (k==SDLK_LEFT || k==SDLK_RIGHT)) {
        if(k==SDLK_LEFT && a->mode_page>0) a->mode_page--;
        if(k==SDLK_RIGHT && a->mode_page<2) a->mode_page++;
        a->focus=0;return;
    }
    if (k == SDLK_ESCAPE) {
        a->click = false;
        a->activate = false;
        if (a->scene == MATCH) {
            a->paused = !a->paused;
            a->quit_confirm = false;
            a->focus = 0;
        } else if (a->scene == LOBBY)
            app_go(a, TITLE);
        else if (a->scene == LEARN_SETUP && a->show_table)
            a->show_table = false;
        else if (a->scene == SETUP || a->scene == LEARN_SETUP || a->scene == TUTORIAL)
            app_go(a, SINGLE);
        else if (a->scene == PUZZLE || a->scene == TRAIN)
            app_go(a, SINGLE);
        else if (a->scene == REVIEW)
            app_go(a, LOBBY);
        else if (a->scene == LEARN) {
            app_go(a, LEARN_SETUP);
        } else if (a->scene == SETTINGS) {
            save_write(a);
            app_go(a, a->settings_back);
        } else
            app_go(a, LOBBY);
        return;
    }
    if (a->scene == LEARN) {
        if (k >= SDLK_1 && k <= SDLK_3) {
            learn_choose(a, (int)(k - SDLK_1));
            return;
        }
        if ((k == SDLK_RETURN || k == SDLK_SPACE) && a->lesson.solved) {
            learn_advance(a);
            return;
        }
    }
    if (a->scene == REVIEW) {
        if (k>=SDLK_1 && k<=SDLK_3) {review_answer(a,(int)(k-SDLK_1));return;}
        if (a->review_feedback && (k==SDLK_RETURN || k==SDLK_SPACE)) {
            a->review_index=mistake_next(&a->save.mistakes,a->review_index);
            a->review_feedback=false;return;
        }
    }
    if (a->scene == TRAIN) {
        if (k>=SDLK_1 && k<=SDLK_3) {train_answer(a,(int)(k-SDLK_1));return;}
        if (a->train.solved && (k==SDLK_RETURN || k==SDLK_SPACE)) {
            train_advance(a);return;
        }
    }
    if (a->scene == PUZZLE && puzzle_key(a, k)) return;
    if (a->scene == MATCH && !a->paused) {
        app_game_key(a, k);
        return;
    }
    if (k == SDLK_TAB || k == SDLK_DOWN || k == SDLK_RIGHT) {
        if (a->previous_buttons)
            a->focus = (a->focus + 1) % a->previous_buttons;
    }
    if (k == SDLK_UP || k == SDLK_LEFT) {
        if (a->previous_buttons)
            a->focus = (a->focus + a->previous_buttons - 1) % a->previous_buttons;
    }
    if (k == SDLK_RETURN || k == SDLK_KP_ENTER || k == SDLK_SPACE)
        a->activate = true;
}
