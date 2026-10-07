#include "app.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static const char *friends[] = {"BUNNY", "MISO",  "BIBI",   "BOMO",
                                "RUBY",  "PANDA", "FROGGY", "KOALA"};
static const char *bots[] = {"YOU", "MISO", "BIBI", "BOMO", "COCO", "LULU", "POPO", "MOMO"};
static SDL_Color mode_color(int m) {
    return m == 0 ? CORAL : m == 1 ? MINT : m == 2 ? LAVENDER : (SDL_Color){240, 199, 122, 255};
}
static bool header(App *a, const char *title, Scene back) {
    if (ui_header(a, title)) {
        app_go(a, back);
        return true;
    }
    return false;
}
static void title(App *a) {
    ui_panel(a, 345, 47, 590, 258, CREAM);
    ui_text(a, "369 PARTY", 640, 66, 68, PLUM, true);
    ui_round(a, 477, 166, 326, 39, 18, LAVENDER);
    ui_text(a, "NUMBER PICNIC", 640, 174, 20, PLUM, true);
    const int ids[] = {0, 4, 1, 5, 6, 7};
    for (int i = 0; i < 6; i++)
        ui_avatar(a, ids[i], 57 + i * 198, 342, 180, i == 0, true);
    ui_panel(a, 395, 570, 490, 80, CREAM);
    ui_text(a, "Press any key or click to start", 640, 594, 22, PLUM, true);
    ui_text(a, "VERSION 1.0", 640, 671, 16, MUTED, true);
}
static void lobby(App *a) {
    if (header(a, "369 PARTY", TITLE))
        return;
    ui_panel(a, 45, 127, 430, 525, CREAM);
    ui_round(a, 79, 157, 362, 43, 20, LAVENDER);
    ui_text(a, "CHARACTER", 260, 167, 22, PLUM, true);
    ui_avatar(a, a->save.skin, 120, 225, 280, true, true);
    ui_text(a, friends[a->save.skin], 260, 533, 27, PLUM, true);
    char stats[160];
    snprintf(stats, sizeof(stats), L(a, "%d games played  /  %d wins"), a->save.played,
             a->save.wins);
    ui_text(a, stats, 260, 599, 18, MUTED, true);
    ui_panel(a, 510, 127, 725, 190, CREAM);
    ui_text(a, "SINGLE PLAYER", 540, 150, 29, PLUM, false);
    if (ui_button(a, "START", 540, 206, 665, 66, CORAL, true)) {
        app_go(a, SINGLE);
        return;
    }
    ui_panel(a, 510, 341, 725, 311, CREAM);
    if (ui_button(a, "SHOP", 540, 382, 319, 64, MINT, true)) {
        app_go(a, SHOP);
        return;
    }
    if (ui_button(a, "SETTINGS", 884, 382, 319, 64, LAVENDER, true)) {
        a->settings_back = LOBBY;
        app_go(a, SETTINGS);
        return;
    }
    if (ui_button(a, "MULTIPLAYER", 540, 468, 663, 55, CREAM, true)) {
        app_go(a, MULTI);
        return;
    }
    if (ui_button(a, "QUIT", 1020, 586, 184, 43, CREAM, true))
        a->running = false;
}
static void single(App *a) {
    if (header(a, "SINGLE PLAYER", LOBBY))
        return;
    ui_text(a, "PARTY", 42, 113, 18, MUTED, false);
    const char *names[] = {"Classic 369", "Party Mix", "Tangsuyuk", "Add & subtract",
                           "Multiply & divide"};
    for (int i = 0; i < 5; i++) {
        int x = i < 3 ? 40 + i * 405 : 40 + (i - 3) * 610, y = i < 3 ? 150 : 443,
            w = i < 3 ? 390 : 595;
        ui_panel(a, x, y, w, 222, CREAM);
        ui_round(a, x + 16, y + 17, 8, 43, 4, mode_color(i % 4));
        ui_text(a, names[i], x + 39, y + 23, 27, PLUM, false);
        ui_avatar(a, i < 3 ? i : i + 1, x + w - 151, y + 66, 129, false, true);
        if (ui_button(a, "START", x + 24, y + 145, w - 192, 52, mode_color(i % 4), true)) {
            if (i < 3) {
                a->chosen = i == 0 ? CLASSIC : i == 1 ? ITEMS : TANGSUYUK;
                if (i == 2)
                    a->players = 2;
                app_go(a, SETUP);
            } else {
                a->learn_kind = i - 3;
                a->learn_operation = 0;
                a->show_table = false;
                app_go(a, LEARN_SETUP);
            }
            return;
        }
    }
    ui_text(a, "LEARNING", 42, 397, 18, MUTED, false);
}
static void setup(App *a) {
    if (header(a, "SETUP", SINGLE))
        return;
    if (a->chosen == TANGSUYUK)
        a->players = 2;
    ui_panel(a, 45, 128, 482, 523, CREAM);
    ui_text(a, mode_name(a->chosen), 286, 153, 33, PLUM, true);
    ui_avatar(a, a->chosen, 151, 285, 270, true, true);
    ui_text(a,
            a->chosen == TANGSUYUK ? "Take turns: TANG > SU > YUK" : "Clap for every 3, 6 and 9!",
            286, 214, 22, PLUM, true);
    ui_text(a,
            a->chosen == ITEMS       ? "Items + an event every 12 turns"
            : a->chosen == TANGSUYUK ? "One-on-one with a bot"
                                     : "13: one clap / 33: two claps",
            286, 584, 20, PLUM, true);
    ui_panel(a, 560, 128, 675, 523, CREAM);
    ui_text(a, "SETUP", 595, 154, 18, MUTED, false);
    ui_text(a, "Bot friends", 595, 207, 24, PLUM, false);
    if (ui_button(a, "-", 955, 200, 55, 47, CREAM, a->chosen != TANGSUYUK))
        a->players = a->players > 2 ? a->players - 1 : 8;
    char text[70];
    snprintf(text, sizeof(text), "%d", a->players - 1);
    ui_text(a, text, 1070, 207, 25, PLUM, true);
    if (ui_button(a, "+", 1130, 200, 55, 47, CREAM, a->chosen != TANGSUYUK))
        a->players = a->players < 8 ? a->players + 1 : 2;
    ui_text(a, "Difficulty", 595, 301, 24, PLUM, false);
    const char *d[] = {"EASY", "NORMAL", "HARD"};
    if (ui_button(a, d[a->difficulty], 953, 294, 233, 52, LAVENDER, true))
        a->difficulty = (a->difficulty + 1) % 3;
    ui_text(a, "Input", 595, 399, 24, PLUM, false);
    if (ui_button(a, a->input ? "TYPE + ENTER" : "QUICK BUTTONS", 953, 393, 233, 52, MINT, true))
        a->input = !a->input;

    if (ui_button(a, "START", 595, 560, 591, 56, CORAL, true))
        app_start(a);
}
static void multi(App *a) {
    if (header(a, "MULTIPLAYER", LOBBY))
        return;
    ui_panel(a, 246, 139, 788, 505, CREAM);
    ui_round(a, 480, 165, 320, 38, 18, LAVENDER);
    ui_text(a, "Coming soon", 640, 175, 14, PLUM, true);
    ui_avatar(a, 0, 400, 297, 210, false, true);
    ui_avatar(a, 1, 660, 297, 210, false, true);
    if (ui_button(a, "PLAY WITH BOTS", 417, 565, 446, 54, CORAL, true))
        app_go(a, SINGLE);
}
static void shop(App *a) {
    if (header(a, "SHOP", LOBBY))
        return;
    int prices[] = {0, 100, 160, 220, 60, 80, 100, 120};
    for (int slot = 0; slot < 4; slot++) {
        int i = slot + a->shop_tab * 4;
        int x = 42 + slot * 304;
        ui_panel(a, x, 155, 284, 469, CREAM);
        ui_round(a, x + 20, 178, 244, 244, 110, i % 2 ? LAVENDER : MINT);
        ui_avatar(a, i, x + 28, 187, 228, a->save.skin == i, true);
        ui_text(a, friends[i], x + 142, 435, 28, PLUM, true);
        bool owned = (a->save.owned & (1 << i)) != 0;
        char price[100];
        snprintf(price, sizeof(price), owned ? L(a, "Owned") : L(a, "%d coins"), prices[i]);
        ui_text(a, price, x + 142, 485, 17, MUTED, true);
        const char *label = a->save.skin == i ? "EQUIPPED" : owned ? "WEAR THIS" : "BUY & WEAR";
        if (ui_button(a, label, x + 22, 547, 240, 53, a->save.skin == i ? LAVENDER : MINT,
                      a->save.skin != i)) {
            if (owned || a->save.coins >= prices[i]) {
                if (!owned) {
                    a->save.coins -= prices[i];
                    a->save.owned |= 1 << i;
                }
                a->save.skin = i;
                if (!save_write(a))
                    app_message(a, "Could not save your purchase.");
                else
                    app_message(a, "Equipped");
            } else
                app_message(a, "Not enough coins");
        }
    }
    if (ui_button(a, "1 / 2", 295, 653, 320, 45, a->shop_tab == 0 ? CORAL : CREAM, true))
        a->shop_tab = 0;
    if (ui_button(a, "2 / 2", 665, 653, 320, 45, a->shop_tab == 1 ? MINT : CREAM, true))
        a->shop_tab = 1;
}
static void settings(App *a) {
    if (header(a, "SETTINGS", a->settings_back))
        return;
    ui_panel(a, 220, 126, 840, 529, CREAM);
    if (a->reset_confirm) {
        ui_text(a, "Reset?", 640, 220, 38, PLUM, true);
        if (ui_button(a, "CANCEL", 340, 380, 600, 64, MINT, true))
            a->reset_confirm = false;
        if (ui_button(a, "RESET", 340, 468, 600, 58, CORAL, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            a->save = (Save){.coins = 100,
                             .owned = 1,
                             .music = 50,
                             .sfx = 70,
                             .motion = 1,
                             .legacy = 1,
                             .language = a->save.language};
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            SDL_SetWindowFullscreen(a->window, 0);
            save_write(a);
            a->reset_confirm = false;
            app_message(a, "Reset complete");
        }
        return;
    }
    for (int i = 0; i < 2; i++) {
        int y = 158 + i * 73;
        ui_text(a, i == 0 ? "Music" : "Sound effects", 260, y + 11, 23, PLUM, false);
        int *v = i == 0 ? &a->save.music : &a->save.sfx;
        if (ui_button(a, "-", 731, y, 56, 47, CREAM, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            *v = *v >= 10 ? *v - 10 : 0;
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            save_write(a);
        }
        char value[30];
        snprintf(value, sizeof(value), "%d%%", *v);
        ui_text(a, value, 863, y + 10, 23, PLUM, true);
        if (ui_button(a, "+", 945, y, 56, 47, CREAM, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            *v = *v <= 90 ? *v + 10 : 100;
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            save_write(a);
        }
    }
    ui_text(a, "Fullscreen", 260, 318, 23, PLUM, false);
    if (ui_button(a, a->save.fullscreen ? "ON  /  F11" : "OFF  /  F11", 731, 305, 270, 48, LAVENDER,
                  true)) {
        a->save.fullscreen = !a->save.fullscreen;
        SDL_SetWindowFullscreen(a->window, a->save.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        save_write(a);
    }
    ui_text(a, "Motion", 260, 391, 23, PLUM, false);
    if (ui_button(a, a->save.motion ? "ON" : "REDUCED", 731, 378, 270, 48, MINT, true)) {
        a->save.motion = !a->save.motion;
        save_write(a);
    }
    ui_text(a, "Language / 언어", 260, 464, 23, PLUM, false);
    if (ui_button(a, a->save.language ? "한국어 >" : "English >", 731, 451, 270, 48, LAVENDER,
                  true)) {
        a->save.language = !a->save.language;
        if (!save_write(a))
            app_message(a, "Could not save progress. Check folder access.");
    }
    if (ui_button(a, "RESET PROGRESS", 260, 570, 334, 54, CREAM, true)) {
        a->reset_confirm = true;
        a->focus = 0;
    }
    if (ui_button(a, "DONE", 670, 570, 331, 54, CORAL, true)) {
        save_write(a);
        app_go(a, a->settings_back);
    }
}
static void player_slot(App *a, int player, int x, int y, int size) {
    Game *g = &a->game;
    bool active = g->current == player;
    bool alive = g->p[player].hp > 0;
    int skin = player == 0 ? a->save.skin : player % 4;
    ui_avatar(a, skin, x, y, size, active, alive);
    ui_round(a, x - 2, y + size - 8, size + 4, 44, 15, active ? CORAL : CREAM);
    ui_text(a, bots[player], x + size / 2, y + size - 4, 15, PLUM, true);
    for (int i = 0; i < 3; i++)
        ui_round(a, x + size / 2 - 20 + i * 16, y + size + 20, 10, 10, 5,
                 i < g->p[player].hp ? (active ? PLUM : CORAL) : (SDL_Color){194, 178, 184, 255});
    if (!alive)
        ui_text(a, "OUT", x + size / 2, y + size + 39, 14, PLUM, true);
    if (g->p[player].shield) {
        ui_round(a, x + size - 30, y + size - 20, 38, 25, 10, MINT);
        ui_text(a, "S", x + size - 11, y + size - 18, 15, PLUM, true);
    }
}
static void pause_screen(App *a) {
    SDL_SetRenderDrawColor(a->renderer, 47, 30, 51, 145);
    SDL_RenderFillRect(a->renderer, NULL);
    a->button_count = 0;
    ui_panel(a, 385, 148, 510, 370, CREAM);
    ui_text(a, a->quit_confirm ? "Leave match?" : "PAUSE", 640, 188, 32, PLUM, true);
    if (ui_button(a, "RESUME", 439, 314, 402, 60, MINT, true)) {
        a->paused = false;
        a->quit_confirm = false;
        a->focus = 0;
        return;
    }
    if (ui_button(a, a->quit_confirm ? "LEAVE MATCH" : "LEAVE MATCH", 439, 399, 402, 54, CREAM,
                  true)) {
        if (a->quit_confirm) {
            a->paused = false;
            app_go(a, SINGLE);
        } else
            a->quit_confirm = true;
        return;
    }
}
static void match(App *a) {
    Game *g = &a->game;
    bool can = g->phase == TURN && g->current == 0 && !a->paused;
    ui_text(a, "369 PARTY", 34, 26, 29, PLUM, false);
    char label[256];
    snprintf(label, sizeof(label), L(a, "%s  /  TURN %02d"), L(a, mode_name(g->mode)),
             g->turns + 1);
    ui_round(a, 400, 23, 480, 45, 19, CREAM);
    ui_text(a, label, 640, 35, 18, PLUM, true);

    bool saved_click = a->click, saved_activate = a->activate;
    if (a->paused) {
        a->click = false;
        a->activate = false;
    }
    if (ui_button(a, "PAUSE", 1121, 23, 126, 45, CREAM, !a->paused)) {
        a->paused = true;
        a->focus = 0;
    }
    if (g->mode == ITEMS) {
        ui_round(a, 365, 83, 550, 41, 18, LAVENDER);
        ui_text(a, rule_name(g->rule), 640, 93, 20, PLUM, true);
    }

    if (g->count <= 4) {
        const int pos[][2] = {{66, 345}, {99, 104}, {967, 104}, {1000, 345}};
        for (int i = 0; i < g->count; i++)
            player_slot(a, i, pos[i][0], pos[i][1], 210);
    } else {
        const int pos[][2] = {{61, 375}, {51, 166},  {259, 110},  {468, 98},
                              {683, 98}, {900, 110}, {1103, 166}, {1103, 375}};
        for (int i = 0; i < g->count; i++)
            player_slot(a, i, pos[i][0], pos[i][1], 115);
    }
    ui_panel(a, 389, 285, 502, 249, CREAM);
    snprintf(label, sizeof(label), g->current == 0 ? L(a, "YOUR TURN") : L(a, "%s'S TURN"),
             L(a, bots[g->current]));
    ui_round(a, 477, 260, 326, 42, 18, g->current == 0 ? CORAL : LAVENDER);
    ui_text(a, label, 640, 270, 18, PLUM, true);
    if (g->mode == TANGSUYUK) {
        ui_text(a, "What comes next?", 640, 315, 29, PLUM, true);
        static const char *words[] = {"TANG", "SU", "YUK"};
        snprintf(label, sizeof(label),
                 g->last_player < 0 ? L(a, "Start with TANG") : L(a, "Previous: %s"),
                 g->last_answer < 0 ? "-" : L(a, words[g->last_answer % 3]));
        ui_text(a, label, 640, 367, 24, MUTED, true);
        snprintf(label, sizeof(label), L(a, "STEP %d"), g->value);
        ui_text(a, label, 640, 411, 19, MUTED, true);
    } else {
        snprintf(label, sizeof(label), "%d", g->value);
        ui_text(a, label, 640, 306, g->value < 100 ? 100 : 82, PLUM, true);
    }
    double duration = g->deadline - g->started;
    double remaining = g->phase == TURN ? g->deadline - g->now : duration;
    if (remaining < 0)
        remaining = 0;
    float ratio = duration > 0 ? (float)(remaining / duration) : 1;
    if (ratio > 1)
        ratio = 1;
    ui_round(a, 435, 452, 410, 13, 6, (SDL_Color){223, 211, 220, 255});
    ui_round(a, 435, 452, (int)(410 * ratio), 13, 6, ratio < 0.3 ? CORAL : LAVENDER);
    snprintf(label, sizeof(label), L(a, "%.1fs"), remaining);
    ui_text(a, label, 640, 473, 21, PLUM, true);
    if (g->phase == FEEDBACK) {
        ui_round(a, 358, 541, 564, 57, 17, g->success ? MINT : g->blocked ? LAVENDER : CORAL);
        ui_text(a, g->message, 640, 547, 20, PLUM, true);
        ui_text(a, g->detail, 640, 575, 15, PLUM, true);
    } else if (g->typing) {
        ui_round(a, 365, 547, 400, 48, 14, CREAM);
        snprintf(label, sizeof(label), "%s", g->typed_len ? g->typed : L(a, "Type your answer..."));
        ui_text(a, label, 565, 557, 22, PLUM, true);
        if (ui_button(a, "ENTER", 780, 547, 136, 48, MINT, can && g->typed_len > 0))
            app_game_key(a, SDLK_RETURN);
    } else if (g->mode == TANGSUYUK) {
        const char *words[] = {"TANG", "SU", "YUK"};
        for (int i = 0; i < 3; i++)
            if (ui_button(a, words[i], 355 + i * 195, 550, 180, 51, mode_color(i), can)) {
                app_game_key(a, i == 0 ? SDLK_t : i == 1 ? SDLK_s : SDLK_y);
                break;
            }
    } else {
        if (ui_button(a, "NUMBER", 355, 550, 276, 55, CORAL, can))
            app_game_key(a, SDLK_SPACE);
        if (ui_button(a, "CLAP", 650, 550, 276, 55, MINT, can))
            app_game_key(a, SDLK_c);
        if (g->claps > 0) {
            snprintf(label, sizeof(label), L(a, "%d clap%s"), g->claps,
                     a->save.language || g->claps == 1 ? "" : "s");
            ui_text(a, label, 640, 614, 16, PLUM, true);
        }
    }
    if (g->phase == TURN || g->phase == READY) {
        const char *tip = g->typing ? "Type answer + Enter / C for each clap" :
            g->mode == TANGSUYUK ? "T: TANG / S: SU / Y: YUK" :
            "Space: number / C: clap / Enter: submit claps";
        if (g->typing && g->mode == TANGSUYUK)
            tip = "Type T / S / Y, then Enter";
        if (g->claps == 0)
            ui_text(a, tip, 640, 609, 16, MUTED, true);
    }
    if (g->mode == ITEMS) {
        const char *names[] = {"SHIELD [Q]", "+2 SEC [W]", "SKIP [E]"};
        for (int i = 0; i < 3; i++) {
            snprintf(label, sizeof(label), "%s x%d", L(a, names[i]), g->p[0].inventory[i]);
            if (ui_button(a, label, 336 + i * 207, 650, 194, 43, LAVENDER,
                          can && !g->used_item && g->p[0].inventory[i] > 0 &&
                              !(i == 0 && g->p[0].shield)))
                app_game_key(a, i == 0 ? SDLK_q : i == 1 ? SDLK_w : SDLK_e);
        }
        if (g->item_message[0] && g->current == 0)
            ui_text(a, g->item_message, 640, 625, 15, PLUM, true);
    }
    if (g->phase == READY || g->phase == EVENT_NOTICE) {
        SDL_SetRenderDrawColor(a->renderer, 47, 30, 51, 105);
        SDL_RenderFillRect(a->renderer, NULL);
        ui_panel(a, 298, 244, 684, 234, CREAM);
        if (g->phase == READY) {
            snprintf(label, sizeof(label), "%d", (int)ceil(g->phase_end - g->now));
            ui_text(a, label, 640, 278, 50, PLUM, true);
            ui_text(a, L(a, mode_name(g->mode)), 640, 360, 26, MUTED, true);
        } else {
            ui_text(a, "EVENT", 640, 271, 20, MUTED, true);
            ui_text(a, rule_name(g->rule), 640, 319, 39, PLUM, true);
            const char *rules[] = {"Clap for every 3, 6 and 9!", "Clap twice for each 3, 6 or 9.",
                "Turns now go the other way.", "Clap for 7 too!", "You have 75% of the usual time."};
            ui_text(a, rules[g->rule], 640, 400, 22, PLUM, true);
        }
    }
    if (a->paused) {
        a->click = saved_click;
        a->activate = saved_activate;
        pause_screen(a);
    }
}
static void result(App *a) {
    Game *g = &a->game;
    ui_panel(a, 282, 91, 716, 565, CREAM);
    ui_text(a, g->won ? "Victory" : "Finished", 640, 124, 39, PLUM, true);
    ui_text(a, L(a, mode_name(g->mode)), 640, 183, 21, MUTED, true);
    ui_avatar(a, a->save.skin, 544, 227, 192, g->won, true);
    char label[160];
    snprintf(label, sizeof(label), L(a, "%d correct  /  %d streak"), g->human_correct,
             g->best_streak);
    ui_text(a, label, 640, 430, 23, PLUM, true);
    snprintf(label, sizeof(label), L(a, "+%d coins"), a->reward);
    ui_round(a, 514, 479, 252, 44, 19, MINT);
    ui_text(a, label, 640, 488, 24, PLUM, true);
    if (ui_button(a, "PLAY AGAIN", 324, 564, 293, 59, CORAL, true)) {
        app_start(a);
        return;
    }
    if (ui_button(a, "PICK A MODE", 648, 564, 306, 59, LAVENDER, true)) {
        app_go(a, SINGLE);
        return;
    }
}
void render_scene(App *a) {
    switch (a->scene) {
    case TITLE:
        title(a);
        break;
    case LOBBY:
        lobby(a);
        break;
    case SINGLE:
        single(a);
        break;
    case SETUP:
        setup(a);
        break;
    case MULTI:
        multi(a);
        break;
    case SHOP:
        shop(a);
        break;
    case SETTINGS:
        settings(a);
        break;
    case MATCH:
        match(a);
        break;
    case RESULT:
        result(a);
        break;
    case LEARN_SETUP:
    case LEARN:
        learn_render(a);
        break;
    }
}

void help_toggle(App *a) {
    if (a->help_open) {
        a->help_open = false;
        a->focus = a->help_focus;
    } else {
        a->help_focus = a->focus;
        a->help_page = (a->scene == LEARN || a->scene == LEARN_SETUP) ? 3 + a->learn_kind
                       : (a->scene == MATCH || a->scene == SETUP)
                           ? (a->chosen == TANGSUYUK ? 2 : (int)a->chosen)
                           : 0;
        a->help_open = true;
        a->focus = 0;
    }
    a->click = a->activate = false;
}
void help_render(App *a) {
    SDL_SetRenderDrawColor(a->renderer, 47, 30, 51, 160);
    SDL_RenderFillRect(a->renderer, NULL);
    a->button_count = 0;
    ui_panel(a, 100, 90, 1080, 580, CREAM);
    ui_text(a, "HOW TO PLAY", 640, 112, 30, PLUM, true);
    const char *tabs[] = {"Classic 369", "Party Mix", "Tangsuyuk", "Add & subtract",
                          "Multiply & divide"};
    for (int i = 0; i < 5; i++)
        if (ui_button(a, tabs[i], 125 + i * 207, 170, 199, 48, a->help_page == i ? CORAL : LAVENDER,
                      true))
            a->help_page = i;
    static const char *lines[5][8] = {
        {"Take turns counting from 1. Keep your 3 hearts to win.",
         "Clap once for each 3, 6 or 9 in the number.",
         "Examples: 12 = number / 13 = 1 clap / 33 = 2 claps",
         "Quick input: NUMBER or Space / CLAP or C.",
         "For multiple claps, press C again within 0.55 seconds.",
         "Claps submit automatically; Enter submits immediately.",
         "Typing: enter the number or C / CC, then press Enter.",
         "A wrong answer or timeout costs 1 heart. Esc pauses."},
        {"Classic 369 with items and a new event every 12 turns.",
         "Q: shield blocks one mistake / W: add 2 seconds.",
         "E: skip your turn safely. Use one item per turn.",
         "Start with one of each; earn a random item every 3 correct.",
         "Double clap: 13 = 2 claps, 33 = 4 claps.",
         "Lucky seven: clap for 7 as well as 3, 6 and 9.", "Reverse order: turns go the other way.",
         "Quick round: time is reduced to 75%. Check the event label."},
        {"Play one-on-one against one bot.", "Take turns saying TANG > SU > YUK > TANG...",
         "You start with TANG. Check the previous word.",
         "Click the word, or press T / S / Y in quick input.",
         "Typing input: type T, S or Y, then press Enter.",
         "A wrong word or timeout costs 1 heart.", "Keep your hearts until the bot is out to win.",
         "Esc pauses the match."},
        {"Choose a range up to 5, 10 or 20, then an operation.",
         "Addition: count all the pink and green circles.",
         "Subtraction: count only circles without a slash.",
         "Choose an answer by clicking, or use keys 1 / 2 / 3.",
         "No time limit. Try again if you make a mistake.",
         "Hint shows the answer and circle numbers.",
         "After a correct answer, click NEXT or press Enter / Space.",
         "Finish 9 questions to earn 18 coins for the shop."},
        {"Choose a table from 2 to 9, or Mix for a random table.",
         "Open Times tables in setup to practice before playing.",
         "Multiply: count the circles across all groups.",
         "Divide: count the circles in one equal group.",
         "Click an answer, or use keys 1 / 2 / 3 from left to right.",
         "No time limit. Retry freely or use Hint.",
         "After a correct answer, click NEXT or press Enter / Space.",
         "Finish 9 questions for 18 coins. Score counts first tries."},
    };
    for (int i = 0; i < 8; i++)
        ui_text(a, lines[a->help_page][i], 150, 244 + i * 40, 22, PLUM, false);
    if (ui_button(a, "CLOSE", 465, 590, 350, 52, MINT, true))
        help_toggle(a);
}
