#ifndef NUMBER_TRAIN_H
#define NUMBER_TRAIN_H
#define TRAIN_STAGES 10
typedef struct {
    int stage, start, step, answer, options[3], values[4], level, missed, attempts;
    int solved, rewarded;
    unsigned rng;
} Train;
void train_start(Train *t, unsigned seed);
void train_start_level(Train *t, int level, unsigned seed);
void train_question(Train *t);
/* 0: wrong answer, 1: correct answer, -1: invalid/finished. */
int train_choose(Train *t, int slot);
void train_next(Train *t);
#endif
