#ifndef PARTY_GAME_H
#define PARTY_GAME_H
#include <stdbool.h>
#include <stdint.h>
#define PLAYERS 8
typedef enum { CLASSIC = 0, ITEMS = 1, TANGSUYUK = 3 } Mode;
typedef enum { READY, TURN, FEEDBACK, EVENT_NOTICE, FINISHED } Phase;
typedef enum { NORMAL_RULE, DOUBLE_CLAP, REVERSE, SEVEN, RUSH } Rule;
typedef struct {
    int hp, correct, inventory[3];
    bool shield;
} Player;
typedef struct {
    Mode mode;
    Phase phase;
    Rule rule;
    Player p[PLAYERS];
    int language;
    int count, difficulty, current, value, turns, direction, streak, best_streak;
    int human_correct, claps, last_answer, last_player;
    bool typing, used_item, won, success, blocked, skipped;
    uint32_t rng;
    double now, started, deadline, phase_end, bot_at, last_clap;
    char typed[24], message[256], detail[256], item_message[256];
    int typed_len;
} Game;
const char *mode_name(Mode m);
const char *rule_name(Rule r);
int game_claps(int number, Rule r);
int game_expected(const Game *g);
double game_limit(const Game *g);
void game_start(Game *g, Mode mode, int count, int difficulty, bool typing, uint32_t seed);
void game_update(Game *g, double dt);
void game_submit(Game *g, int answer);
void game_clap(Game *g);
void game_submit_typed(Game *g);
bool game_item(Game *g, int item);
int game_alive(const Game *g);
#endif
