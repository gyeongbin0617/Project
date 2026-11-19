#ifndef REVIEW_INTERNAL_H
#define REVIEW_INTERNAL_H
#include "app.h"

static inline void review_choices(const MistakeNote *n, int answer[3]) {
    int slot=(n->left+n->right)%3;
    answer[slot]=n->answer;
    answer[(slot+1)%3]=n->answer+1;
    answer[(slot+2)%3]=n->answer>0?n->answer-1:n->answer+2;
}
#endif
