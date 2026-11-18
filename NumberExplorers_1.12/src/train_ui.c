#include "app.h"
#include <stdio.h>
#include <time.h>

void train_begin(App *a) {
    train_start_level(&a->train,a->train_level,(unsigned)time(NULL) ^ (unsigned)SDL_GetTicks());
    app_go(a,TRAIN);
}
void train_answer(App *a, int choice) {
    if(a->scene!=TRAIN) return;
    int result=train_choose(&a->train,choice);
    if(result==0) {
        Train *t=&a->train;
        int decreasing=t->values[1]<t->values[0];
        mistake_add(&a->save.mistakes,6+(t->level==2?2:0)+decreasing,
                    t->start,t->step,t->answer);
        if(!save_write(a)) app_message(a,"Could not save progress. Check folder access.");
        audio_cue(a,2);
    } else if(result==1) audio_cue(a,3);
}
void train_advance(App *a) {
    if(a->scene!=TRAIN || !a->train.solved || a->train.stage>=TRAIN_STAGES) return;
    train_next(&a->train);
    if(a->train.stage==TRAIN_STAGES && !a->train.rewarded) {
        a->train.rewarded=1;
        a->save.coins+=30;
        if(a->save.coins>999999) a->save.coins=999999;
        if(!save_write(a)) app_message(a,"Could not save progress. Check folder access.");
        audio_cue(a,3);
    }
}
static void rail(App *a,int x,int y) {
    ui_round(a,x,y,126,16,7,PLUM);
    for(int i=0;i<4;i++) ui_round(a,x+10+i*34,y-14,8,43,3,MUTED);
}
void train_render(App *a) {
    if(ui_header(a,"NUMBER TRAIN")) {app_go(a,SINGLE);return;}
    Train *t=&a->train;
    ui_panel(a,48,128,1184,524,CREAM);
    if(t->stage==TRAIN_STAGES) {
        ui_text(a,a->save.language?"종착역 도착!":"Destination reached!",640,170,42,PLUM,true);
        ui_avatar(a,a->save.skin,515,256,245,true,true);
        ui_text(a,a->save.language?"모든 숫자 레일을 이었어요":"All number rails connected",640,496,26,MUTED,true);
        ui_text(a,"+30 COINS",640,540,25,PLUM,true);
        if(ui_button(a,"PLAY AGAIN",300,588,310,52,MINT,true)) train_begin(a);
        if(ui_button(a,"< BACK",670,588,310,52,LAVENDER,true)) app_go(a,SINGLE);
        return;
    }
    char b[128];
    snprintf(b,sizeof(b),a->save.language?"숫자 기차 · %d / %d역":"NUMBER TRAIN · STATION %d / %d",t->stage+1,TRAIN_STAGES);
    ui_text(a,b,640,155,27,PLUM,true);
    ui_text(a,a->save.language?"다음 숫자 레일을 골라 기차를 출발시켜요":"Find the next rail to send the train onward",640,211,23,MUTED,true);
    /* Progress is a physical journey; the number is always shown on its own rail. */
    for(int i=0;i<5;i++) {
        int x=119+i*207;
        rail(a,x,399);
        ui_round(a,x-12,286,150,106,28,i==4?(t->solved?MINT:LAVENDER):(i%2?MINT:CORAL));
        if(i<4 || t->solved) {
            snprintf(b,sizeof(b),"%d",i==4?t->answer:t->values[i]);
            ui_text(a,b,x+63,305,45,PLUM,true);
        } else ui_text(a,"?",x+63,305,45,PLUM,true);
        if(i<4 && (t->attempts || t->level==0)) {
            int diff=(i==3?t->answer:t->values[i+1])-t->values[i];
            snprintf(b,sizeof(b),"%+d",diff);
            ui_text(a,b,x+171,335,19,MUTED,true);
        }
    }
    /* The engine travels with the completed stations. */
    int engine_x=83+(t->solved?4:t->stage%4)*207;
    ui_round(a,engine_x,451,142,49,19,CORAL);
    ui_round(a,engine_x+18,427,77,31,10,MINT);
    ui_round(a,engine_x+10,492,28,28,14,PLUM);
    ui_round(a,engine_x+102,492,28,28,14,PLUM);
    if(t->solved) {
        ui_text(a,a->save.language?"정답! 기차가 출발해요":"Great! The train moves on",640,545,24,PLUM,true);
        if(ui_button(a,"NEXT STATION",460,581,360,59,MINT,true)) train_advance(a);
    } else {
        if(t->attempts) {
            snprintf(b,sizeof(b),a->save.language?"힌트: 화살표 사이의 숫자 변화를 봐요":"Hint: look at the changes between the rails");
            ui_text(a,b,640,454,22,PLUM,true);
        }
        for(int i=0;i<3;i++) {
            snprintf(b,sizeof(b),"%d",t->options[i]);
            if(ui_button(a,b,244+i*280,534,232,75,i==0?CORAL:i==1?MINT:LAVENDER,true)) {
                train_answer(a,i);return;
            }
        }
        ui_text(a,a->save.language?"레일을 누르거나 1·2·3을 눌러요":"Tap a rail or press 1 / 2 / 3",640,621,17,MUTED,true);
    }
}
