#include "game.h"
#include "i18n.h"
#define G(s) tr(g->language, (s))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned random_value(Game *g) {
    g->rng ^= g->rng << 13;
    g->rng ^= g->rng >> 17;
    g->rng ^= g->rng << 5;
    return g->rng;
}
static double random_unit(Game *g) { return (random_value(g) % 10000) / 10000.0; }
const char *mode_name(Mode m) {
    static const char *n[] = {"Classic 369", "Party Mix", "Party Mix", "Tangsuyuk"};
    return n[m];
}
const char *rule_name(Rule r) {
    static const char *n[] = {"Classic rules", "Double clap", "Reverse order", "Lucky seven",
                              "Quick round"};
    return n[r];
}
int game_claps(int n, Rule rule) {
    int c = 0;
    while (n > 0) {
        int d = n % 10;
        if (d == 3 || d == 6 || d == 9 || (rule == SEVEN && d == 7))
            c++;
        n /= 10;
    }
    return rule == DOUBLE_CLAP ? c * 2 : c;
}
int game_expected(const Game *g) {
    return g->mode == TANGSUYUK ? (g->value - 1) % 3 : game_claps(g->value, g->rule);
}
int game_alive(const Game *g) {
    int n = 0;
    for (int i = 0; i < g->count; i++)
        n += g->p[i].hp > 0;
    return n;
}
double game_limit(const Game *g) {
    double base = g->difficulty == 0 ? 4.5 : g->difficulty == 1 ? 3.3 : 2.4;
    base -= (g->turns / g->count) * 0.055;
    if (base < 1.25)
        base = 1.25;
    if (g->rule == RUSH)
        base *= 0.75;
    if (g->typing && g->mode != TANGSUYUK)
        base += 1.2;
    return base;
}
static void begin_turn(Game *g) {
    g->phase = TURN;
    g->started = g->now;
    g->deadline = g->now + game_limit(g);
    g->claps = 0;
    g->last_clap = -1;
    g->typed_len = 0;
    g->typed[0] = 0;
    g->used_item = false;
    g->item_message[0] = 0;
    g->bot_at = g->now + 0.48 + random_unit(g) * 0.62 + game_expected(g) * 0.065;
    if (g->bot_at > g->deadline - 0.1)
        g->bot_at = g->deadline - 0.1;
    if (g->current > 0 && g->mode == ITEMS) {
        Player *p = &g->p[g->current];
        if (!p->shield && p->inventory[0] && random_unit(g) < 0.45)
            game_item(g, 0);
        else if (p->inventory[1] && random_unit(g) < 0.2)
            game_item(g, 1);
        else if (p->inventory[2] && random_unit(g) < 0.12)
            game_item(g, 2);
    }
}
void game_start(Game *g, Mode mode, int count, int difficulty, bool typing, uint32_t seed) {
    memset(g, 0, sizeof(*g));
    g->mode = mode;
    g->count = mode == TANGSUYUK ? 2 : count < 2 ? 2 : count > 8 ? 8 : count;
    g->difficulty = difficulty < 0 ? 0 : difficulty > 2 ? 2 : difficulty;
    g->typing = typing;
    g->rng = seed ? seed : 1234567;
    g->direction = 1;
    g->value = 1;
    g->last_player = -1;
    g->last_answer = -1;
    g->phase = READY;
    g->phase_end = 2.5;
    for (int i = 0; i < g->count; i++) {
        g->p[i].hp = g->difficulty == 0 ? 3 : 2;
        if (mode == ITEMS)
            for (int j = 0; j < 3; j++)
                g->p[i].inventory[j] = 1;
    }
}
static void finish(Game *g) {
    g->phase = FINISHED;
    g->won = g->p[0].hp > 0;
}
void game_submit(Game *g, int answer) {
    if (g->phase != TURN)
        return;
    Player *p = &g->p[g->current];
    int expected = game_expected(g);
    g->success = answer == expected;
    g->blocked = false;
    g->skipped = false;
    g->last_answer = expected;
    g->last_player = g->current;
    const char *who = G(g->current == 0 ? "You" : "Bot");
    if (g->success) {
        p->correct++;
        if (g->current == 0) {
            g->human_correct++;
            g->streak++;
            if (g->streak > g->best_streak)
                g->best_streak = g->streak;
        }
        snprintf(g->message, sizeof(g->message), G("%s got it!"), who);
        if (g->mode == ITEMS && p->correct % 3 == 0) {
            int item = (int)(random_value(g) % 3);
            if (p->inventory[item] < 3)
                p->inventory[item]++;
            snprintf(g->item_message, sizeof(g->item_message), G("New item earned: %s"),
                     item == 0   ? G("Shield")
                     : item == 1 ? G("Extra time")
                                 : G("Skip"));
        }
    } else {
        if (g->current == 0)
            g->streak = 0;
        if (p->shield) {
            p->shield = false;
            g->blocked = true;
            snprintf(g->message, sizeof(g->message), G("Shield saved %s!"),
                     g->current == 0 ? G("you") : G("the bot"));
        } else {
            p->hp--;
            snprintf(g->message, sizeof(g->message), p->hp ? G("%s lost a heart") : G("%s is out!"),
                     who);
        }
    }
    if (g->mode == TANGSUYUK) {
        static const char *n[] = {"TANG", "SU", "YUK"};
        snprintf(g->detail, sizeof(g->detail), G("The answer was %s"), G(n[expected]));
    } else if (expected)
        snprintf(g->detail, sizeof(g->detail), G("%d needs %d clap%s"), g->value, expected,
                 g->language || expected == 1 ? "" : "s");
    else
        snprintf(g->detail, sizeof(g->detail), G("%d is a number turn"), g->value);
    if (answer == -99)
        strncat(g->detail, G(" - time ran out"), sizeof(g->detail) - strlen(g->detail) - 1);
    g->phase = FEEDBACK;
    g->phase_end = g->now + (g->success ? 0.62 : 1.25);
    g->turns++;
}
void game_clap(Game *g) {
    if (g->phase == TURN && g->current == 0) {
        if (g->claps < 12)
            g->claps++;
        g->last_clap = g->now;
    }
}
void game_submit_typed(Game *g) {
    if (g->phase != TURN || g->current != 0)
        return;
    int need = game_expected(g), answer = -1;
    if (g->mode == TANGSUYUK) {
        if (g->typed_len == 1) {
            char c = g->typed[0];
            answer = c == 'T' ? 0 : c == 'S' ? 1 : c == 'Y' ? 2 : -1;
        }
    } else if (need > 0) {
        bool all = true;
        for (int i = 0; i < g->typed_len; i++)
            if (g->typed[i] != 'C')
                all = false;
        if (all)
            answer = g->typed_len;
    } else {
        char expected[24];
        snprintf(expected, sizeof(expected), "%d", g->value);
        if (strcmp(expected, g->typed) == 0)
            answer = 0;
    }
    game_submit(g, answer);
}
bool game_item(Game *g, int item) {
    if (g->mode != ITEMS || g->phase != TURN || g->used_item || item < 0 || item > 2)
        return false;
    Player *p = &g->p[g->current];
    if (p->inventory[item] <= 0 || (item == 0 && p->shield))
        return false;
    p->inventory[item]--;
    g->used_item = true;
    if (item == 0) {
        p->shield = true;
        snprintf(g->item_message, sizeof(g->item_message), "%s",
                 G("Shield ready: blocks one mistake."));
    }
    if (item == 1) {
        g->deadline += 2;
        snprintf(g->item_message, sizeof(g->item_message), "%s",
                 G("A little breathing room: +2 seconds."));
    }
    if (item == 2) {
        g->skipped = true;
        g->success = true;
        g->blocked = false;
        g->last_player = g->current;
        g->last_answer = game_expected(g);
        g->phase = FEEDBACK;
        g->phase_end = g->now + 0.7;
        g->turns++;
        snprintf(g->message, sizeof(g->message), "%s", G("Turn skipped!"));
        snprintf(g->detail, sizeof(g->detail), "%s", G("No heart lost. No score earned."));
    }
    return true;
}
void game_update(Game *g, double dt) {
    if (g->phase == FINISHED)
        return;
    g->now += dt;
    if (g->phase == READY || g->phase == EVENT_NOTICE) {
        if (g->now >= g->phase_end)
            begin_turn(g);
        return;
    }
    if (g->phase == FEEDBACK) {
        if (g->now < g->phase_end)
            return;
        if (g->p[0].hp <= 0 || game_alive(g) <= 1) {
            finish(g);
            return;
        }
        g->value++;
        if (g->mode == ITEMS && g->turns % 12 == 0) {
            Rule old = g->rule;
            do {
                g->rule = (Rule)(1 + random_value(g) % 4);
            } while (g->rule == old);
            g->direction = g->rule == REVERSE ? -1 : 1;
        }
        do {
            g->current = (g->current + g->direction + g->count) % g->count;
        } while (g->p[g->current].hp <= 0);
        if (g->mode == ITEMS && g->turns % 12 == 0) {
            g->phase = EVENT_NOTICE;
            g->phase_end = g->now + 2.7;
            return;
        }
        begin_turn(g);
        return;
    }
    if (g->phase != TURN)
        return;
    if (g->now >= g->deadline) {
        if (g->current == 0 && !g->typing && g->mode != TANGSUYUK && g->claps > 0)
            game_submit(g, g->claps);
        else
            game_submit(g, -99);
        return;
    }
    if (g->current > 0 && g->now >= g->bot_at) {
        double fail = g->difficulty == 0 ? 0.17 : g->difficulty == 1 ? 0.08 : 0.035;
        if (game_expected(g) > 1)
            fail += 0.08;
        if (g->value >= 30)
            fail += 0.035;
        if (g->rule != NORMAL_RULE)
            fail += 0.035;
        if (g->rule == RUSH)
            fail += 0.025;
        fail += g->turns * 0.0007;
        game_submit(g, random_unit(g) < fail ? -1 : game_expected(g));
        return;
    }
    if (g->current == 0 && !g->typing && g->mode != TANGSUYUK && g->claps > 0 &&
        g->now - g->last_clap >= 0.55)
        game_submit(g, g->claps);
}
