#include "app.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
    App *a=calloc(1,sizeof(*a));assert(a);
    a->test_mode=true;
    a->save=(Save){.coins=125,.owned=255,.skin=7,.bg_owned=15,.background=3,
        .played=29,.wins=14,.music=0,.sfx=10,.motion=0,.fullscreen=1,.language=0};
    mistake_add(&a->save.mistakes,0,33,0,2);
    mistake_add(&a->save.mistakes,0,33,0,2);
    mistake_add(&a->save.mistakes,6,2,2,8);
    assert(a->save.mistakes.count==2 && mistake_due(&a->save.mistakes)==2);
    assert(mistake_next(&a->save.mistakes,-1)==0);
    mistake_review(&a->save.mistakes,0,1);
    assert(mistake_next(&a->save.mistakes,0)==1);
    mistake_review(&a->save.mistakes,0,1);
    assert(mistake_due(&a->save.mistakes)==1);
    for(int level=0;level<3;level++)for(unsigned seed=1;seed<=100;seed++) {
        Train t;train_start_level(&t,level,seed);
        while(t.stage<TRAIN_STAGES) {
            int d0=t.values[1]-t.values[0];
            for(int i=1;i<4;i++) {
                int diff=(i==3?t.answer:t.values[i+1])-t.values[i];
                assert(diff==d0+(level==2?(d0>0?i:-i):0));
            }
            assert(t.answer>=0&&t.options[0]!=t.options[1]&&t.options[1]!=t.options[2]);
            int wrong=0;while(t.options[wrong]==t.answer)wrong++;
            assert(train_choose(&t,wrong)==0&&t.missed>=1);
            int correct=0;while(t.options[correct]!=t.answer)correct++;
            assert(train_choose(&t,correct)==1);train_next(&t);
        }
        assert(t.stage==TRAIN_STAGES&&train_choose(&t,0)==-1);
    }
    mistake_add(&a->save.mistakes,8,1,1,11);
    mistake_add(&a->save.mistakes,9,50,8,12);
    Save before=a->save;app_reset_settings(&a->save);
    assert(a->save.music==50&&a->save.sfx==70&&a->save.motion==1);
    assert(a->save.coins==before.coins&&a->save.owned==before.owned&&a->save.background==before.background);
    assert(a->save.mistakes.count==before.mistakes.count);
    for(int i=1;i<8;i++) assert(app_bot_skin(a,i)!=a->save.skin);
    assert(app_pay_entry(a,0)&&a->save.coins==125);
    assert(app_pay_entry(a,1)&&a->save.coins==120);
    a->save.coins=4;assert(!app_pay_entry(a,1)&&a->save.coins==4);
    assert(attendance_claim(&a->save,2026313,2026312)==10);
    assert(attendance_claim(&a->save,2026313,2026312)==0);
    assert(attendance_claim(&a->save,2026314,2026313)==12);
    snprintf(a->save_path,sizeof(a->save_path),"/tmp/number_explorers_v112_save.txt");
    a->test_mode=false;assert(save_write(a));Save expected=a->save;
    memset(&a->save,0,sizeof(a->save));save_load(a);
    assert(memcmp(&expected,&a->save,sizeof(Save))==0);
    FILE *f=fopen(a->save_path,"w");assert(f);
    assert(fprintf(f,"369COZY_V7\n73 15 3 50 70 1 0 10 4 1 2 3 4 1 1 7 2 2026312 2 255\n1\n0 33 0 2 3 1\n")>0);
    assert(fclose(f)==0);save_load(a);
    assert(a->save.coins==73&&a->save.mistakes.count==1&&a->save.mistakes.note[0].wrong==3);
    f=fopen(a->save_path,"w");assert(f);
    assert(fprintf(f,"369COZY_V8\n73 15 3 50 70 1 0 10 4 1 2 3 4 1 1 7 2 2026312 2 255\n1\n6 2 2 8 2 0\n")>0);
    assert(fclose(f)==0);save_load(a);
    assert(a->save.mistakes.count==1&&a->save.mistakes.note[0].kind==6);
    f=fopen(a->save_path,"w");assert(f);
    assert(fprintf(f,"369COZY_V5\n73 15 3 50 70 1 0 10 4 1 2 3 4 1 1 7 2 2026312 2 255 1 1 15 3\n")>0);
    assert(fclose(f)==0);save_load(a);
    assert(a->save.coins==403 && a->save.skin==3 && a->save.mistakes.count==0);
    assert(save_write(a));save_load(a);assert(a->save.coins==403);
    f=fopen(a->save_path,"w");assert(f);
    assert(fprintf(f,"369COZY_V4\n73 15 3 50 70 1 0 10 4 1 2 3 4 1 1 7 2 2026312 2 255 1 1\n")>0);
    assert(fclose(f)==0);save_load(a);assert(a->save.coins==163);
    remove(a->save_path);free(a);
    puts("PASS: train sequence and completion, review priority, settings isolation, V9 roundtrip, V4/V5 refund once, entry and attendance");
}
