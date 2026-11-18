#include "learn.h"
#include <string.h>
static unsigned random_number(Learn *l) {
    l->rng ^= l->rng << 13;
    l->rng ^= l->rng >> 17;
    l->rng ^= l->rng << 5;
    return l->rng;
}

void learn_question(Learn *l) {
    l->solved = false;
    l->hint = false;
    l->attempts = 0;
    l->op = l->kind * 2 + (l->operation == 0 ? l->index % 2 : l->operation - 1);
    if (l->kind == 0) {
        int limit = l->level == 0 ? 5 : l->level == 1 ? 10 : 20;
        int total = 1 + (int)(random_number(l) % (unsigned)limit);
        int part = (int)(random_number(l) % (unsigned)(total + 1));
        if (l->op == 0) {
            l->left = part;
            l->right = total - part;
            l->answer = total;
        } else {
            l->left = total;
            l->right = part;
            l->answer = total - part;
        }
    } else {
        int table = l->table ? l->table : 2 + (int)(random_number(l) % 8);
        int factor = l->index + 1;
        if (l->op == 2) {
            l->left = table;
            l->right = factor;
            l->answer = table * factor;
        } else {
            l->left = table * factor;
            l->right = table;
            l->answer = factor;
        }
    }
    l->choices[0] = l->answer;
    for (int i = 1; i < 3; i++) {
        int n;
        bool duplicate;
        do {
            int offset = (int)(random_number(l) % 7) - 3;
            n = l->answer + offset;
            duplicate = n < 0;
            for (int j = 0; j < i; j++)
                if (n == l->choices[j])
                    duplicate = true;
        } while (duplicate);
        l->choices[i] = n;
    }
    for (int i = 2; i > 0; i--) {
        int j = (int)(random_number(l) % (unsigned)(i + 1));
        int t = l->choices[i];
        l->choices[i] = l->choices[j];
        l->choices[j] = t;
    }
}
void learn_start(Learn *l, int kind, int level, int operation, int table, uint32_t seed) {
    memset(l, 0, sizeof(*l));
    l->kind = kind == 1;
    l->level = level < 0 ? 0 : level > 2 ? 2 : level;
    l->operation = operation < 0 ? 0 : operation > 2 ? 2 : operation;
    l->table = table >= 2 && table <= 9 ? table : 0;
    l->rng = seed ? seed : 1234567;
    learn_question(l);
}
bool learn_answer(Learn *l, int choice) {
    if (l->solved || l->index >= 9 || choice < 0 || choice > 2)
        return false;
    bool correct = l->choices[choice] == l->answer;
    if (correct) {
        if (l->attempts == 0 && !l->hint)
            l->first_correct++;
        l->solved = true;
    } else
        l->hint = true;
    l->attempts++;
    return correct;
}
