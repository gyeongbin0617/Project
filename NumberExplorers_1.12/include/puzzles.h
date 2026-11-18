#ifndef NUMBER_PUZZLES_H
#define NUMBER_PUZZLES_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int disks, rods, peg[5][7], count[5], selected, moves;
    int history[512][2], history_count;
    int hint_from, hint_to;
    bool done, rewarded;
} Hanoi;
typedef struct {
    int value, turn, winner, difficulty, last_start, last_count, last_player;
    bool done, rewarded;
    uint32_t rng;
    double bot_at;
} Count31;
typedef struct { int x, y; } DotPoint;
typedef struct {
    const char *name;
    int count;
    DotPoint point[24];
} DotPicture;
typedef struct { int picture, next, mistakes; bool hint, done, rewarded; } DotGame;

void hanoi_start(Hanoi *h, int disks);
void hanoi_start_ex(Hanoi *h, int disks, int rods);
int hanoi_minimum(int disks, int rods);
bool hanoi_move(Hanoi *h, int from, int to);
bool hanoi_undo(Hanoi *h);
bool hanoi_hint(const Hanoi *h, int *from, int *to);
void count31_start(Count31 *g, int difficulty, int first, uint32_t seed);
bool count31_play(Count31 *g, int count);
int count31_bot(Count31 *g);
extern const DotPicture dot_pictures[];
extern const int dot_picture_count;
void dots_start(DotGame *g, int picture);
bool dots_pick(DotGame *g, int point);
#endif
