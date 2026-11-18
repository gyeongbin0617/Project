#include "mistakes.h"

static int priority(const MistakeNote *n) {
    return n->wrong > n->reviewed ? n->wrong - n->reviewed : 0;
}
int mistake_due(const MistakeBook *book) {
    int total=0;
    for (int i=0;i<book->count;i++) total+=priority(&book->note[i])>0;
    return total;
}
int mistake_next(const MistakeBook *book, int exclude) {
    int best=-1, score=0;
    for (int i=0;i<book->count;i++) {
        int p=priority(&book->note[i]);
        if (i==exclude || p<=0) continue;
        if (p>score || (p==score && best>=0 && book->note[i].wrong>book->note[best].wrong)) {
            best=i;score=p;
        }
    }
    return best>=0?best:exclude>=0 && exclude<book->count && priority(&book->note[exclude])>0?exclude:-1;
}
void mistake_add(MistakeBook *book, int kind, int left, int right, int answer) {
    if (kind<0 || kind>9 || left<0 || right<0 || answer<0) return;
    int i;
    for (i=0;i<book->count;i++) {
        MistakeNote *n=&book->note[i];
        if (n->kind==kind && n->left==left && n->right==right && n->answer==answer) break;
    }
    if (i==book->count) {
        if (book->count<MAX_MISTAKES) book->count++;
        else {
            i=0;
            for (int j=1;j<book->count;j++)
                if (priority(&book->note[j])<priority(&book->note[i])) i=j;
        }
        book->note[i]=(MistakeNote){kind,left,right,answer,0,0};
    }
    if (book->note[i].wrong<999) book->note[i].wrong++;
}
void mistake_review(MistakeBook *book, int index, int correct) {
    if (index<0 || index>=book->count) return;
    MistakeNote *n=&book->note[index];
    if (correct) {if (n->reviewed<n->wrong) n->reviewed++;}
    else if (n->wrong<999) n->wrong++;
}
