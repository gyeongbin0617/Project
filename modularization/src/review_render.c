#include "app.h"
#include "review_internal.h"
#include <stdio.h>

/* Draw the mistake-review screen. */

void review_render(App *a) {
    if (ui_header(a,"REVIEW MISTAKES")) {app_go(a,LOBBY);return;}
    ui_panel(a,170,128,940,522,CREAM);
    char label[128];
    int due=mistake_due(&a->save.mistakes);
    snprintf(label,sizeof(label),a->save.language?"복습할 문제 %d개":"%d questions to review",due);
    ui_text(a,label,640,160,26,PLUM,true);
    if (a->review_index<0 || !due) {
        ui_avatar(a,a->save.skin,540,245,200,true,true);
        ui_text(a,"All caught up!",640,484,30,PLUM,true);
        if (ui_button(a,"< BACK",480,560,320,62,MINT,true)) app_go(a,LOBBY);
        return;
    }
    MistakeNote *n=&a->save.mistakes.note[a->review_index];
    const char *symbol[]={"","+","−","×","÷","+"};
    if (n->kind==0) {
        snprintf(label,sizeof(label),a->save.language?"숫자 %d · 박수 몇 번?":"Number %d · how many claps?",n->left);
        ui_text(a,label,640,264,40,PLUM,true);
        ui_text(a,n->right==1?"Double clap":n->right==3?"Lucky seven":"Classic rules",640,321,20,MUTED,true);
    } else if(n->kind>=6) {
        int step=(n->kind==6 || n->kind==8)?n->right:-n->right;
        int grow=n->kind>=8?(step>0?1:-1):0;
        int old=n->kind<=7 && n->answer==n->left+3*step;
        if(old) snprintf(label,sizeof(label),"%d  →  %d  →  %d  →  ?",n->left,n->left+step,n->left+2*step);
        else snprintf(label,sizeof(label),"%d  →  %d  →  %d  →  %d  →  ?",
                      n->left,n->left+step,n->left+2*step+grow,n->left+3*step+3*grow);
        ui_text(a,label,640,264,38,PLUM,true);
        ui_text(a,a->save.language?"숫자 기차 · 다음 레일을 골라요":"Number train · choose the next rail",640,323,20,MUTED,true);
    } else {
        snprintf(label,sizeof(label),"%d %s %d = ?",n->left,symbol[n->kind],n->right);
        ui_text(a,label,640,264,44,PLUM,true);
        if (n->kind==5) ui_text(a,"Count both hands",640,327,20,MUTED,true);
    }
    snprintf(label,sizeof(label),a->save.language?"이 문제를 %d번 틀렸어요":"Missed this %d times",n->wrong);
    ui_text(a,label,640,391,20,MUTED,true);
    if (a->review_feedback) {
        ui_text(a,"Correct!",640,444,28,PLUM,true);
        if (ui_button(a,"NEXT",456,543,368,69,MINT,true)) {
            a->review_index=mistake_next(&a->save.mistakes,a->review_index);
            a->review_feedback=false;
        }
        return;
    }
    int options[3];review_choices(n,options);
    for (int i=0;i<3;i++) {
        snprintf(label,sizeof(label),n->kind==0?
            (a->save.language?"박수 %d번":"%d claps"):"%d",options[i]);
        if (ui_button(a,label,225+i*285,480,255,78,i==0?CORAL:i==1?MINT:LAVENDER,true)) {
            review_answer(a,i);return;
        }
    }
    ui_text(a,a->save.language?"답을 누르거나 1·2·3을 눌러요":"Tap an answer or press 1 / 2 / 3",640,592,18,MUTED,true);
}
