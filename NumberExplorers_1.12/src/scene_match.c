#include "scene_internal.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
static const char *bots[]={"YOU","MISO","BIBI","BOMO","COCO","LULU","POPO","MOMO"};
static void player_slot(App *a, int player, int x, int y, int size) {
    Game *g = &a->game;
    bool active = g->current == player;
    bool alive = g->p[player].hp > 0;
    int skin = player == 0 ? a->save.skin : app_bot_skin(a,player);
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
void scene_match(App *a) {
    Game *g = &a->game;
    bool can = g->phase == TURN && g->current == 0 && !a->paused;
    ui_text(a, "NUMBER EXPLORERS", 34, 26, 25, PLUM, false);
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
    snprintf(label,sizeof(label),"%d",g->value);
    ui_text(a,label,640,306,g->value<100?100:82,PLUM,true);
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
        const char *tip=g->typing?"Type answer + Enter / C for each clap":
            "Space: number / C: clap / Enter: submit claps";
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
void scene_result(App *a) {
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
    if(g->won) {
        snprintf(label,sizeof(label),a->save.language?"남은 하트 %d개 × 10코인 보너스":"%d hearts x 10 bonus coins",g->p[0].hp);
        ui_text(a,label,640,531,17,MUTED,true);
        if(a->save.motion) for(int j=0;j<12;j++) {
            int x=360+(j*73)%560, y=112+(j*97+(int)(a->clock*36))%310;
            ui_round(a,x,y,7,13,3,j%2?CORAL:MINT);
        }
    }
    if (ui_button(a, "PLAY AGAIN", 324, 564, 293, 59, CORAL, true)) {
        app_start(a);
        return;
    }
    if (ui_button(a, "PICK A MODE", 648, 564, 306, 59, LAVENDER, true)) {
        app_go(a, SINGLE);
        return;
    }
}
