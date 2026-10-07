#include "app.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void app_message(App *a, const char *s) {
    snprintf(a->toast, sizeof(a->toast), "%s", L(a, s));
    a->toast_end = a->clock + 3.0;
}
void app_go(App *a, Scene s) {
    a->scene = s;
    a->help_open = false;
    a->focus = 0;
    a->click = false;
    a->activate = false;
    a->quit_confirm = false;
    a->reset_confirm = false;
    audio_cue(a, 0);
}
bool app_init(App *a, bool headless) {
    memset(a, 0, sizeof(*a));
    a->test_mode = headless;
    a->running = true;
    a->players = 4;
    a->difficulty = 1;
    SDL_SetMainReady();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        return false;
    if (TTF_Init() != 0)
        return false;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        return false;
    char *base = SDL_GetBasePath();
    snprintf(a->base, sizeof(a->base), "%s", base ? base : "./");
    SDL_free(base);
    char *pref = SDL_GetPrefPath("CozyParty", "369PartyLearning");
    const char *save_dir = pref ? pref : a->base;
    if (strlen(save_dir) + 9 >= sizeof(a->save_path)) {
        SDL_free(pref);
        SDL_SetError("Save folder path is too long.");
        return false;
    }
    strcpy(a->save_path, save_dir);
    strcat(a->save_path, "save.txt");
    SDL_free(pref);
    if (!headless) {
        SDL_RWops *existing = SDL_RWFromFile(a->save_path, "rb");
        if (existing) {
            SDL_RWclose(existing);
            save_load(a);
        } else {
            char current_path[1024];
            snprintf(current_path, sizeof(current_path), "%s", a->save_path);
            char *old = SDL_GetPrefPath("CozyParty", "369Party");
            if (old && strlen(old) + 9 < sizeof(a->save_path))
                snprintf(a->save_path, sizeof(a->save_path), "%ssave.txt", old);
            save_load(a);
            SDL_free(old);
            snprintf(a->save_path, sizeof(a->save_path), "%s", current_path);
        }
    } else
        a->save = (Save){.coins = 100, .owned = 1, .motion = 0, .legacy = 1};
    a->window = SDL_CreateWindow(
        "369 Party - Number Picnic 1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE | (headless ? SDL_WINDOW_HIDDEN : SDL_WINDOW_SHOWN));
    if (!a->window)
        return false;
    SDL_SetWindowMinimumSize(a->window, 800, 450);
    a->renderer = SDL_CreateRenderer(
        a->window, -1,
        headless ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!a->renderer)
        a->renderer = SDL_CreateRenderer(a->window, -1, SDL_RENDERER_SOFTWARE);
    if (!a->renderer)
        return false;
    SDL_SetRenderDrawBlendMode(a->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(a->renderer, WIDTH, HEIGHT);
    char path[1200];
    snprintf(path, sizeof(path), "%sAssets/room.png", a->base);
    a->room = IMG_LoadTexture(a->renderer, path);
    snprintf(path, sizeof(path), "%sAssets/friends.png", a->base);
    a->friends = IMG_LoadTexture(a->renderer, path);
    if (!a->room || !a->friends) {
        SDL_SetError(
            "Missing Assets/room.png or Assets/friends.png. Extract the complete ZIP first.");
        return false;
    }
    snprintf(path, sizeof(path), "%sAssets/new_friends.png", a->base);
    a->new_friends = IMG_LoadTexture(a->renderer, path);
    if (!a->new_friends) {
        SDL_SetError("Missing Assets/new_friends.png. Extract the entire ZIP.");
        return false;
    }
    snprintf(path, sizeof(path), "%sAssets/Heading.ttf", a->base);
    a->fonts[24] = TTF_OpenFont(path, 24);
    if (!a->fonts[24]) {
        SDL_SetError("Missing Assets/Heading.ttf. Extract the complete ZIP first.");
        return false;
    }
    snprintf(path, sizeof(path), "%sAssets/Korean.ttf", a->base);
    a->fonts[185] = TTF_OpenFont(path, 24);
    if (!a->fonts[185])
        return false;
    if (a->save.fullscreen && !headless)
        SDL_SetWindowFullscreen(a->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!headless)
        audio_init(a);
    return true;
}
void app_destroy(App *a) {
    if (a->audio)
        SDL_CloseAudioDevice(a->audio);
    for (int i = 0; i < 256; i++)
        if (a->cache[i].texture)
            SDL_DestroyTexture(a->cache[i].texture);
    for (int i = 0; i < 322; i++)
        if (a->fonts[i])
            TTF_CloseFont(a->fonts[i]);
    if (a->room)
        SDL_DestroyTexture(a->room);
    if (a->friends)
        SDL_DestroyTexture(a->friends);
    if (a->new_friends)
        SDL_DestroyTexture(a->new_friends);
    if (a->renderer)
        SDL_DestroyRenderer(a->renderer);
    if (a->window)
        SDL_DestroyWindow(a->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
void app_start(App *a) {
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
    a->reward = g->human_correct * 3 + (g->won ? 50 : 5);
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
            if (g->mode == TANGSUYUK) {
                if (k == SDLK_t)
                    c = 'T';
                if (k == SDLK_s)
                    c = 'S';
                if (k == SDLK_y)
                    c = 'Y';
            } else {
                if (k >= SDLK_0 && k <= SDLK_9)
                    c = (char)k;
                if (k >= SDLK_KP_1 && k <= SDLK_KP_9)
                    c = (char)('1' + k - SDLK_KP_1);
                if (k == SDLK_KP_0)
                    c = '0';
                if (k == SDLK_c)
                    c = 'C';
            }
            if (c && g->typed_len < 20) {
                g->typed[g->typed_len++] = c;
                g->typed[g->typed_len] = 0;
                audio_cue(a, 0);
            }
        }
    } else if (g->mode == TANGSUYUK) {
        if (k == SDLK_t)
            game_submit(g, 0);
        if (k == SDLK_s)
            game_submit(g, 1);
        if (k == SDLK_y)
            game_submit(g, 2);
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
    if (before == TURN && g->phase == FEEDBACK)
        audio_cue(a, g->success ? 1 : 2);
}
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
            a->focus = (a->focus + 1) % 6;
        if (k == SDLK_LEFT || k == SDLK_UP)
            a->focus = (a->focus + 5) % 6;
        if (k == SDLK_RETURN || k == SDLK_SPACE)
            a->activate = true;
        return;
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
        else if (a->scene == SETUP || a->scene == LEARN_SETUP)
            app_go(a, SINGLE);
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
void app_update(App *a, double dt) {
    a->clock += dt;
    if (a->scene == MATCH && !a->paused && !a->help_open) {
        Phase before = a->game.phase;
        game_update(&a->game, dt);
        if (before == TURN && a->game.phase == FEEDBACK)
            audio_cue(a, a->game.success ? 1 : 2);
        if (a->game.phase == FINISHED)
            app_finish(a);
    }
}
void app_render(App *a) {
    a->button_count = 0;
    SDL_SetRenderDrawColor(a->renderer, 250, 240, 222, 255);
    SDL_RenderClear(a->renderer);
    SDL_RenderCopy(a->renderer, a->room, NULL, NULL);
    SDL_SetRenderDrawColor(a->renderer, 255, 249, 239,
                           a->scene == TITLE   ? 115
                           : a->scene == MATCH ? 170
                                               : 222);
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
