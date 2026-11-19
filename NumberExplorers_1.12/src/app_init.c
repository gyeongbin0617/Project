#include "app.h"
#include "app_modules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Initialize SDL, load saved data, and create application resources. */

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
        a->save = (Save){.coins = 100, .owned = 1, .bg_owned = 1, .motion = 0, .legacy = 1};
    a->window = SDL_CreateWindow(
        "Number Explorers / 숫자 탐험대 1.12", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
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
    if (!headless) app_process_daily_attendance(a);
    return true;
}
