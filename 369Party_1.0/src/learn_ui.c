#include "app.h"
#include <stdio.h>
#include <time.h>

static void text(App *a, const char *s, int x, int y, int size) {
    ui_text(a, s, x, y, size, PLUM, true);
}
void learn_begin(App *a) {
    learn_start(&a->lesson, a->learn_kind, a->learn_level, a->learn_operation, a->learn_table,
                (uint32_t)time(NULL) ^ (uint32_t)(a->clock * 1000));
    app_go(a, LEARN);
}
void learn_choose(App *a, int choice) {
    if (a->scene != LEARN || a->lesson.index >= 9 || a->lesson.solved)
        return;
    bool ok = learn_answer(&a->lesson, choice);
    audio_cue(a, ok ? 3 : 0);
}
void learn_advance(App *a) {
    Learn *l = &a->lesson;
    if (!l->solved || l->index >= 9)
        return;
    l->index++;
    if (l->index == 9) {
        if (!l->rewarded) {
            l->rewarded = true;
            a->save.coins += 18;
            if (a->save.coins > 999999)
                a->save.coins = 999999;
            if (!save_write(a))
                app_message(a, "Could not save progress. Check folder access.");
            audio_cue(a, 3);
        }
    } else
        learn_question(l);
}
static void setup(App *a) {
    bool times = a->learn_kind == 1;

    ui_panel(a, 45, 128, 482, 523, CREAM);
    text(a, times ? "Multiply & divide" : "Add & subtract", 286, 153, 30);
    ui_avatar(a, times ? 7 : 6, 151, 285, 270, true, true);
    ui_text(a, "Learn with circles, at your own pace.", 286, 214, 22, PLUM, true);
    ui_text(a, "9 questions / no time limit", 286, 584, 20, PLUM, true);

    ui_panel(a, 560, 128, 675, 523, CREAM);
    text(a, times ? "Table" : "Range", 897, 199, 24);
    char label[100];
    if (times) {
        for (int i = 0; i < 9; i++) {
            int value = i == 0 ? 0 : i + 1;
            if (value == 0)
                snprintf(label, sizeof(label), "%s", L(a, "Mix"));
            else
                snprintf(label, sizeof(label), L(a, "Table %d"), value);
            if (ui_button(a, label, 588 + (i % 5) * 128, 248 + (i / 5) * 61, 116, 47,
                          a->learn_table == value ? CORAL : CREAM, true))
                a->learn_table = value;
        }
    } else {
        const char *ranges[] = {"Up to 5", "Up to 10", "Up to 20"};
        for (int i = 0; i < 3; i++)
            if (ui_button(a, ranges[i], 588 + i * 210, 256, 196, 63,
                          a->learn_level == i ? CORAL : CREAM, true))
                a->learn_level = i;
    }
    text(a, "Operation", 897, 380, 23);
    const char *ops[] = {"Both", times ? "Multiply" : "Add", times ? "Divide" : "Subtract"};
    for (int i = 0; i < 3; i++)
        if (ui_button(a, ops[i], 588 + i * 210, 426, 196, 55,
                      a->learn_operation == i ? MINT : CREAM, true))
            a->learn_operation = i;
    if (times && ui_button(a, "Times tables", 588, 495, 618, 54, LAVENDER, true))
        a->show_table = true;
    if (ui_button(a, "START", 588, 570, 618, 54, CORAL, true)) {
        learn_begin(a);
        return;
    }
}
static void table_sheet(App *a) {
    ui_panel(a, 145, 115, 990, 550, CREAM);
    int t = a->learn_table ? a->learn_table : 2;
    char label[120];
    snprintf(label, sizeof(label), L(a, "Table %d"), t);
    text(a, label, 640, 139, 32);

    for (int i = 1; i <= 9; i++) {
        int x = 320 + ((i - 1) / 3) * 320, y = 253 + ((i - 1) % 3) * 81;
        snprintf(label, sizeof(label), "%d × %d = %d", t, i, t * i);
        text(a, label, x, y, 30);
    }
    if (ui_button(a, "<", 200, 538, 80, 49, LAVENDER, true))
        a->learn_table = t == 2 ? 9 : t - 1;
    if (ui_button(a, ">", 1000, 538, 80, 49, LAVENDER, true))
        a->learn_table = t == 9 ? 2 : t + 1;
    if (ui_button(a, "DONE", 390, 574, 500, 57, CORAL, true))
        a->show_table = false;
}

static void dot(App *a, int x, int y, int size, SDL_Color c, bool crossed) {
    int border = size >= 24 ? 2 : 1;
    ui_round(a, x, y, size, size, size / 2, (SDL_Color){0, 0, 0, 255});
    ui_round(a, x + border, y + border, size - 2 * border, size - 2 * border,
             (size - 2 * border) / 2, c);
    if (crossed) {
        SDL_SetRenderDrawColor(a->renderer, PLUM.r, PLUM.g, PLUM.b, 255);
        SDL_RenderDrawLine(a->renderer, x - 2, y + size + 2, x + size + 2, y - 2);
    }
}
static void pictures(App *a) {
    Learn *l = &a->lesson;
    if (l->kind == 0) {
        int total = l->op == 0 ? l->left + l->right : l->left;
        for (int i = 0; i < total; i++) {
            bool crossed = l->op == 1 && i >= l->answer;
            SDL_Color c = l->op == 0 ? (i < l->left ? CORAL : MINT) : (crossed ? LAVENDER : CORAL);
            int x = 397 + (i % 10) * 50, y = 297 + (i / 10) * 55;
            dot(a, x, y, 34, c, crossed);
            if (l->hint || l->solved) {
                char b[12];
                snprintf(b, sizeof(b), "%d", i + 1);
                text(a, b, x + 17, y + 4, 16);
            }
        }

    } else {
        int groups = l->right;
        int each = l->op == 2 ? l->left : l->answer;
        for (int g = 0; g < groups; g++) {
            int x = 355 + (g % 5) * 116, y = 270 + (g / 5) * 86;
            ui_round(a, x, y, 105, 75, 12, (SDL_Color){240, 231, 215, 255});
            for (int i = 0; i < each; i++)
                dot(a, x + 10 + (i % 3) * 29, y + 8 + (i / 3) * 21, 15, g % 2 ? MINT : CORAL,
                    false);
        }
    }
}
void learn_render(App *a) {
    if (ui_header(a, a->learn_kind ? "Multiply & divide" : "Add & subtract")) {
        if (a->show_table)
            a->show_table = false;
        else
            app_go(a, a->scene == LEARN ? LEARN_SETUP : SINGLE);
        return;
    }

    if (a->scene == LEARN_SETUP) {
        if (a->show_table)
            table_sheet(a);
        else
            setup(a);
        return;
    }
    Learn *l = &a->lesson;
    char b[200];
    if (l->index < 9) {
        snprintf(b, sizeof(b), L(a, "%d / 9"), l->index + 1);
        text(a, b, 810, 36, 18);
    }
    if (l->index >= 9) {
        ui_panel(a, 280, 130, 720, 515, CREAM);
        text(a, "Finished", 640, 164, 30);
        ui_avatar(a, a->save.skin, 545, 226, 190, true, true);
        snprintf(b, sizeof(b), L(a, "%d / 9"), l->first_correct);
        text(a, b, 640, 437, 24);
        text(a, "+18 coins", 640, 481, 21);
        if (ui_button(a, "PLAY AGAIN", 330, 560, 295, 57, CORAL, true)) {
            learn_begin(a);
            return;
        }
        if (ui_button(a, "SETUP", 655, 560, 295, 57, MINT, true))
            app_go(a, LEARN_SETUP);
        return;
    }
    for (int i = 0; i < 9; i++)
        ui_round(a, 437 + i * 47, 111, 29, 15, 7,
                 i < l->index    ? MINT
                 : i == l->index ? CORAL
                                 : CREAM);
    ui_avatar(a, a->save.skin, 44, 265, 230, true, true);
    ui_avatar(a, l->kind ? 7 : 6, 1000, 265, 230, l->solved, true);
    ui_panel(a, 320, 139, 640, 350, CREAM);

    const char *symbols[] = {"+", "−", "×", "÷"};
    if (l->solved)
        snprintf(b, sizeof(b), "%d %s %d = %d", l->left, symbols[l->op], l->right, l->answer);
    else
        snprintf(b, sizeof(b), "%d %s %d = ?", l->left, symbols[l->op], l->right);
    text(a, b, 640, 191, 48);
    pictures(a);
    const char *hints[] = {"Count all the circles.", "Count circles without a slash.",
                           "Count circles in all groups.", "Count circles in one group."};
    text(a, hints[l->op], 640, 446, 20);
    if (!l->solved && !l->attempts)
        text(a, "Click an answer or press 1 / 2 / 3.", 640, 502, 19);
    if (l->solved) {
        text(a, "Correct!", 640, 516, 24);
        if (ui_button(a, l->index == 8 ? "DONE" : "NEXT", 415, 577, 450, 67, MINT, true))
            learn_advance(a);
    } else {
        for (int i = 0; i < 3; i++) {
            snprintf(b, sizeof(b), "%d", l->choices[i]);
            if (ui_button(a, b, 350 + i * 200, 547, 180, 82,
                          i == 0   ? CORAL
                          : i == 1 ? MINT
                                   : LAVENDER,
                          true)) {
                learn_choose(a, i);
                return;
            }
        }

        if (l->attempts)
            text(a, "Try again", 640, 502, 20);
        if (!l->hint && ui_button(a, "Hint", 490, 652, 300, 43, CREAM, true))
            l->hint = true;
    }
    if (l->hint && !l->solved) {
        snprintf(b, sizeof(b), "= %d", l->answer);
        text(a, b, 640, 659, 19);
    }
}
