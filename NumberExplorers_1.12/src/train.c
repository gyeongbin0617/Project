#include "train.h"
#include <string.h>
static unsigned random_value(Train *t) {
    t->rng ^= t->rng << 13;
    t->rng ^= t->rng >> 17;
    t->rng ^= t->rng << 5;
    return t->rng;
}
void train_question(Train *t) {
    if(t->stage>=TRAIN_STAGES) return;
    int stage=t->stage, level=t->level;
    int subtract=level==0?stage>=7 && stage%2:stage%2;
    int growing=level==2;
    if(level==0) {
        t->step=1+(stage>=2)+(stage>=5);
        t->start=subtract?15+(int)(random_value(t)%9):1+(int)(random_value(t)%10);
    } else if(level==1) {
        t->step=subtract?3+(int)(random_value(t)%6):2+(int)(random_value(t)%5);
        t->start=subtract?35+(int)(random_value(t)%26):1+(int)(random_value(t)%22);
    } else {
        t->step=subtract?5+(int)(random_value(t)%6):1+(int)(random_value(t)%6);
        t->start=subtract?60+(int)(random_value(t)%40):1+(int)(random_value(t)%23);
    }
    for(int i=0;i<4;i++)
        t->values[i]=t->start+(subtract?-1:1)*(i*t->step+(growing?i*(i-1)/2:0));
    t->answer=t->start+(subtract?-1:1)*(4*t->step+(growing?6:0));
    t->solved=0;t->attempts=0;
    t->options[0]=t->answer;
    t->options[1]=t->answer+1;
    t->options[2]=t->answer>0?t->answer-1:t->answer+2;
    int slot=(int)(random_value(t)%3);
    int tmp=t->options[0];t->options[0]=t->options[slot];t->options[slot]=tmp;
}
void train_start_level(Train *t,int level,unsigned seed) {
    memset(t,0,sizeof(*t));
    t->level=level<0?0:level>2?2:level;
    t->rng=seed?seed:1234567u;
    train_question(t);
}
void train_start(Train *t,unsigned seed) {train_start_level(t,0,seed);}
int train_choose(Train *t,int slot) {
    if(t->stage>=TRAIN_STAGES || t->solved || slot<0 || slot>=3) return -1;
    t->attempts++;
    if(t->options[slot]==t->answer) {t->solved=1;return 1;}
    t->missed++;return 0;
}
void train_next(Train *t) {
    if(!t->solved || t->stage>=TRAIN_STAGES) return;
    t->stage++;train_question(t);
}
