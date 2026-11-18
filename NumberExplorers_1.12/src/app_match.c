#include "app.h"
#include <stdio.h>
#include <time.h>
void app_start(App *a) {
    if (!app_pay_entry(a, a->chosen == CLASSIC ? 0 : 1)) return;
    game_start(&a->game, a->chosen, a->players, a->difficulty, a->input != 0,
               (uint32_t)time(NULL) ^ (uint32_t)(a->clock * 1000));
    a->game.language = a->save.language;
    a->paused = false;
    a->recorded = false;
    app_go(a, MATCH);
}
void app_finish(App *a) {
    if (a->recorded)
        return;
    a->recorded = true;
    Game *g = &a->game;
    a->reward = g->human_correct * 3 + (g->won ? 20 + 10 * g->p[0].hp : 5);
    if (a->reward > 150)
        a->reward = 150;
    a->save.coins += a->reward;
    if (a->save.coins > 999999)
        a->save.coins = 999999;
    a->save.played++;
    if (g->won)
        a->save.wins++;
    if (g->human_correct > a->save.best[g->mode])
        a->save.best[g->mode] = g->human_correct;
    if (!save_write(a))
        app_message(a, "Could not save progress. Check folder access.");
    app_go(a, RESULT);
    audio_cue(a, g->won ? 3 : 2);
}
void app_game_key(App *a, SDL_Keycode k) {
    Game *g = &a->game;
    if (a->paused || g->phase != TURN || g->current != 0)
        return;
    Phase before = g->phase;
    if (g->mode == ITEMS && (k == SDLK_q || k == SDLK_w || k == SDLK_e)) {
        if (game_item(g, k == SDLK_q ? 0 : k == SDLK_w ? 1 : 2))
            audio_cue(a, 3);
        return;
    }
    if (g->typing) {
        if (k == SDLK_BACKSPACE) {
            if (g->typed_len)
                g->typed[--g->typed_len] = 0;
        } else if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
            if (g->typed_len)
                game_submit_typed(g);
        } else {
            char c = 0;
            if (k >= SDLK_0 && k <= SDLK_9) c=(char)k;
            if (k >= SDLK_KP_1 && k <= SDLK_KP_9) c=(char)('1'+k-SDLK_KP_1);
            if (k == SDLK_KP_0) c='0';
            if (k == SDLK_c) c='C';
            if (c && g->typed_len < 20) {
                g->typed[g->typed_len++] = c;
                g->typed[g->typed_len] = 0;
                audio_cue(a, 0);
            }
        }
    } else {
        if (k == SDLK_SPACE)
            game_submit(g, g->claps ? -1 : 0);
        if (k == SDLK_c) {
            game_clap(g);
            audio_cue(a, 1);
        }
        if ((k == SDLK_RETURN || k == SDLK_KP_ENTER) && g->claps)
            game_submit(g, g->claps);
    }
    if (before == TURN && g->phase == FEEDBACK) {
        app_note_feedback(a);
        audio_cue(a, g->success ? 1 : 2);
    }
}
