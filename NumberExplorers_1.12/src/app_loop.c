#include "app.h"
void app_update(App *a, double dt) {
    a->clock += dt;
    if (a->scene == PUZZLE) puzzle_update(a);
    if (a->scene == MATCH && !a->paused && !a->help_open) {
        Phase before = a->game.phase;
        game_update(&a->game, dt);
        if (before == TURN && a->game.phase == FEEDBACK) {
            app_note_feedback(a);
            audio_cue(a, a->game.success ? 1 : 2);
        }
        if (a->game.phase == FINISHED)
            app_finish(a);
    }
}
void app_render(App *a) {
    a->button_count = 0;
    SDL_SetRenderDrawColor(a->renderer, 250, 240, 222, 255);
    SDL_RenderClear(a->renderer);
    ui_background(a,a->save.background,(SDL_Rect){0,0,WIDTH,HEIGHT});
    SDL_SetRenderDrawColor(a->renderer, 255, 249, 239,
                           a->scene == TITLE   ? 115
                           : a->scene == MATCH ? 170
                                               : a->save.background ? 155 : 222);
    SDL_RenderFillRect(a->renderer, NULL);
    if (a->scene != TITLE) {
        ui_round(a, 18, 12, 1244, 89, 23, (SDL_Color){255, 253, 246, 245});
        ui_round(a, 38, 99, 1204, 2, 1, (SDL_Color){214, 204, 200, 180});
    }
    bool help = a->help_open, click = a->click, activate = a->activate, moved = a->moved;
    int focus = a->focus;
    if (help) {
        a->click = a->activate = a->moved = false;
        a->focus = -1;
    }
    render_scene(a);
    if (help) {
        a->click = click;
        a->activate = activate;
        a->moved = moved;
        a->focus = focus;
        help_render(a);
    } else if (a->scene != TITLE && ui_button(a, "HELP [F1]", 920, 30, 124, 48, LAVENDER, true)) {
        help_toggle(a);
    }
    if (a->clock < a->toast_end) {
        ui_round(a, 270, 651, 740, 45, 18, PLUM);
        ui_text(a, a->toast, 640, 662, 18, CREAM, true);
    }
    a->previous_buttons = a->button_count;
    if (a->focus >= a->button_count)
        a->focus = 0;
    a->click = false;
    a->activate = false;
    a->moved = false;
}
