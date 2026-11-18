#ifndef NUMBER_MISTAKES_H
#define NUMBER_MISTAKES_H
#define MAX_MISTAKES 32
/* kind: 0 = clap 369, 1..4 = + - x /, 5 = legacy zero, 6/7 = fixed train, 8/9 = growing gaps. */
typedef struct {
    int kind, left, right, answer, wrong, reviewed;
} MistakeNote;
typedef struct {
    int count;
    MistakeNote note[MAX_MISTAKES];
} MistakeBook;
void mistake_add(MistakeBook *book, int kind, int left, int right, int answer);
int mistake_next(const MistakeBook *book, int exclude);
int mistake_due(const MistakeBook *book);
void mistake_review(MistakeBook *book, int index, int correct);
#endif
