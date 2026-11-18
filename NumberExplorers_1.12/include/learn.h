#ifndef PARTY_LEARN_H
#define PARTY_LEARN_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int kind, level, operation, table;
    int index, left, right, answer, op, choices[3];
    int attempts, first_correct;
    bool solved, hint, rewarded;
    uint32_t rng;
} Learn;
void learn_start(Learn *l, int kind, int level, int operation, int table, uint32_t seed);
void learn_question(Learn *l);
bool learn_answer(Learn *l, int choice);
#endif
