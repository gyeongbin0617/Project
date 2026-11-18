#include "scene_internal.h"
#include <stdio.h>
static SDL_Color mode_color(int m) {
    return m==0?CORAL:m==1?MINT:m==2?LAVENDER:(SDL_Color){240,199,122,255};
}
void scene_title(App *a) {
    ui_panel(a, 345, 47, 590, 258, CREAM);
    ui_text(a, "NUMBER EXPLORERS", 640, 66, 55, PLUM, true);
    ui_round(a, 477, 166, 326, 39, 18, LAVENDER);
    ui_text(a, "FIND THE NEXT NUMBER", 640, 174, 18, PLUM, true);
    for (int i=0;i<4;i++) {
        int x=291+i*181;
        ui_round(a,x,375,140,122,23,i==3?LAVENDER:i==1?MINT:CORAL);
        ui_round(a,x-9,497,158,13,6,PLUM);
        char n[8];snprintf(n,sizeof(n),i==3?"?":"%d",3+i*3);
        ui_text(a,n,x+70,405,51,PLUM,true);
    }
    ui_avatar(a, a->save.skin, 40, 347, 178, true, true);
    ui_avatar(a, 6, 1074, 347, 178, false, true);
    ui_panel(a, 395, 570, 490, 80, CREAM);
    ui_text(a, "Press any key or click to start", 640, 594, 22, PLUM, true);
    ui_text(a, "VERSION 1.12 NUMBER TRAIN", 640, 671, 16, MUTED, true);
}
void scene_lobby(App *a) {
    if (scene_back_header(a, "NUMBER EXPLORERS", TITLE))
        return;
    ui_panel(a, 45, 127, 430, 525, CREAM);
    ui_round(a, 79, 157, 362, 43, 20, LAVENDER);
    ui_text(a, "CHARACTER", 260, 167, 22, PLUM, true);
    ui_avatar(a, a->save.skin, 120, 225, 280, true, true);
    ui_text(a, scene_friend_name(a->save.skin), 260, 533, 27, PLUM, true);
    char stats[160];
    snprintf(stats, sizeof(stats), L(a, "%d games played  /  %d wins"), a->save.played,
             a->save.wins);
    ui_text(a, stats, 260, 599, 18, MUTED, true);
    ui_panel(a, 510, 127, 725, 190, CREAM);
    ui_text(a, "SINGLE PLAYER", 540, 150, 29, PLUM, false);
    if (ui_button(a, "START", 540, 206, 665, 66, CORAL, true)) {
        app_go(a, SINGLE);
        return;
    }
    ui_panel(a, 510, 341, 725, 311, CREAM);
    if (ui_button(a, "SHOP", 540, 382, 319, 64, MINT, true)) {
        app_go(a, SHOP);
        return;
    }
    if (ui_button(a, "SETTINGS", 884, 382, 319, 64, LAVENDER, true)) {
        a->settings_back = LOBBY;
        app_go(a, SETTINGS);
        return;
    }
    if (ui_button(a, "MULTIPLAYER", 540, 468, 663, 55, CREAM, true)) {
        app_message(a, "Under development");
        return;
    }
    char review_label[80];
    snprintf(review_label,sizeof(review_label),a->save.language?"오답노트 · 복습 %d개":"REVIEW MISTAKES · %d due",mistake_due(&a->save.mistakes));
    if (ui_button(a,review_label,540,536,663,52,LAVENDER,true)) {
        a->review_index=mistake_next(&a->save.mistakes,-1);
        a->review_feedback=false;
        app_go(a,REVIEW);return;
    }
    if (ui_button(a, "QUIT", 1020, 601,184,43, CREAM, true))
        a->running = false;
}
static void enter_mode(App *a,int i) {
    if(i==0) {a->chosen=CLASSIC;app_go(a,SETUP);}
    else if(i==1) {a->learn_kind=0;a->learn_operation=0;a->show_table=false;app_go(a,LEARN_SETUP);}
    else if(app_pay_entry(a,1))puzzle_begin(a,i-2);
}
void scene_tutorial(App *a) {
    int i=a->tutorial_mode;
    static const char *en[]={"Clap for numbers with 3, 6, 9", "Count circles and choose an answer", "Move small disks first", "Say 1 to 3 numbers", "Follow numbered dots"};
    static const char *ko[]={"3·6·9가 들어가면 박수!", "원을 세고 답을 골라요", "작은 원판부터 옮겨요", "숫자를 1~3개 말해요", "숫자 점을 순서대로 이어요"};
    static const int bit[]={0,3,5,6,7};
    if(scene_back_header(a,"EXPLORE TOGETHER",SINGLE))return;
    ui_panel(a,120,139,1040,520,CREAM);
    ui_text(a,a->save.language?ko[i]:en[i],640,170,30,PLUM,true);
    ui_round(a,220,243,840,261,30,LAVENDER);
    for(int j=0;j<3;j++) {
        int x=304+j*277;
        ui_round(a,x,307,116,116,58,j==1?MINT:CORAL);
        char number[12];snprintf(number,sizeof(number),"%d",j+1);
        ui_text(a,number,x+58,334,44,PLUM,true);
        if(i==1)for(int k=0;k<j+1;k++)ui_round(a,x+30+k*24,438,18,18,9,k==j?CORAL:MINT);
        else if(i==2)for(int k=0;k<3-j;k++)ui_round(a,x+17+k*31,438+k*6,29,15,7,k%2?MINT:CORAL);
        else if(i==4)for(int k=0;k<=j;k++)ui_round(a,x+25+k*30,440,14,14,7,CORAL);
        if(j<2)ui_text(a,">",x+194,341,32,PLUM,true);
    }
    ui_text(a,a->save.language?"그림을 보고 천천히 해 봐요":"Look at the picture and try it",640,456,24,PLUM,true);
    if(ui_button(a,"LET'S GO",395,550,490,67,MINT,true)) {
        a->save.tutorial_seen_mask|=1<<bit[i];
        if(!save_write(a))app_message(a,"Could not save progress. Check folder access.");
        enter_mode(a,i);
    }
}
void scene_single(App *a) {
    if(scene_back_header(a,a->mode_page==0?"NUMBER TRAIN":"OTHER GAMES",LOBBY)) {
        if(a->mode_page){a->mode_page=0;app_go(a,SINGLE);}return;
    }
    if(a->mode_page==0) {
        ui_text(a,a->save.language?"규칙을 찾아 숫자 기차를 움직여요":"Find the rule. Move the number train!",640,133,25,MUTED,true);
        ui_panel(a,65,194,720,406,CREAM);
        ui_text(a,"NUMBER TRAIN",425,220,37,PLUM,true);
        ui_text(a,a->save.language?"숨어 있는 규칙을 찾아 다음 레일을 골라요":"Find the hidden rule and choose the next rail",425,285,21,PLUM,true);
        for(int j=0;j<5;j++) {
            int x=119+j*130;
            ui_round(a,x,344,100,87,20,j==4?LAVENDER:j%2?MINT:CORAL);
            char n[8];snprintf(n,sizeof(n),j==4?"?":"%d",2+j*2);
            ui_text(a,n,x+50,357,39,PLUM,true);
        }
        const char *level_en[]={"EASY","NORMAL","HARD"};
        const char *level_ko[]={"쉬움","보통","어려움"};
        for(int j=0;j<3;j++) if(ui_button(a,a->save.language?level_ko[j]:level_en[j],139+j*200,448,183,51,a->train_level==j?CORAL:CREAM,true))a->train_level=j;
        if(ui_button(a,"START · FREE",127,521,596,56,CORAL,true)) {train_begin(a);return;}
        ui_panel(a,817,194,398,406,CREAM);
        ui_text(a,"OTHER GAMES",1016,229,31,PLUM,true);
        ui_text(a,a->save.language?"369 박수전 · 사칙연산 · 퍼즐":"Clap 369 · Math · Puzzles",1016,297,21,MUTED,true);
        ui_avatar(a,6,915,340,185,false,true);
        if(ui_button(a,"EXPLORE",850,521,332,56,MINT,true)){a->mode_page=1;a->focus=0;return;}
        return;
    }
    const int pages[2][3]={{0,1,2},{3,4,-1}};
    const int bits[]={0,3,5,6,7};
    const char *en[]={"369 clap game","Four operations","Tower of Hanoi","Count to 31","Connect the dots"};
    const char *ko[]={"369 박수 게임","사칙연산","하노이탑","배스킨라빈스 31","숫자 그림 그리기"};
    const char *en_sub[]={"Choose basic or items", "Add, subtract, multiply, divide", "Choose disks and rods", "Do not say 31", "Find a hidden picture"};
    const char *ko_sub[]={"기본·아이템전을 골라요", "더하기·빼기·곱하기·나누기", "원판과 막대 수를 골라요", "31을 말하면 져요", "점을 이어 그림을 찾아요"};
    for(int slot=0;slot<3;slot++) {
        int i=pages[a->mode_page-1][slot],x=35+slot*410,y=183;
        if(i<0)continue;
        ui_panel(a,x,y,390,357,CREAM);
        ui_round(a,x+22,y+25,6,32,3,mode_color(slot));
        ui_text(a,a->save.language?ko[i]:en[i],x+42,y+25,27,PLUM,false);
        ui_text(a,a->save.language?ko_sub[i]:en_sub[i],x+25,y+76,18,MUTED,false);
        ui_avatar(a,i==0?0:i==1?3:i+2,x+120,y+141,145,false,true);
        if(ui_button(a,i==0?"START · FREE":"START · 5 COINS",x+25,y+280,340,55,mode_color(slot),true)) {
            if(!(a->save.tutorial_seen_mask&(1<<bits[i]))){a->tutorial_mode=i;app_go(a,TUTORIAL);}
            else enter_mode(a,i);
            return;
        }
    }
    if(ui_button(a,"< BACK",316,583,275,53,LAVENDER,true)){a->mode_page--;return;}
    if(a->mode_page<2 && ui_button(a,"NEXT >",689,583,275,53,MINT,true)){a->mode_page++;return;}
}

void scene_setup(App *a) {
    if (scene_back_header(a, "SETUP", SINGLE))
        return;
    ui_panel(a, 45, 128, 482, 523, CREAM);
    ui_text(a, mode_name(a->chosen), 286, 153, 33, PLUM, true);
    ui_avatar(a, a->chosen, 151, 285, 270, true, true);
    ui_text(a,"Clap for every 3, 6 and 9!",286,214,22,PLUM,true);
    ui_text(a,
            a->chosen == ITEMS ? "Items + an event every 12 turns" : "13: one clap / 33: two claps",
            286, 584, 20, PLUM, true);
    ui_panel(a, 560, 128, 675, 523, CREAM);
    ui_text(a, "SETUP", 595, 154, 18, MUTED, false);
    ui_text(a, "Bot friends", 595, 207, 24, PLUM, false);
    if (ui_button(a, "-", 955, 200, 55, 47, CREAM, true))
        a->players = a->players > 2 ? a->players - 1 : 8;
    char text[70];
    snprintf(text, sizeof(text), "%d", a->players - 1);
    ui_text(a, text, 1070, 207, 25, PLUM, true);
    if (ui_button(a, "+", 1130, 200, 55, 47, CREAM, true))
        a->players = a->players < 8 ? a->players + 1 : 2;
    ui_text(a, "Difficulty", 595, 301, 24, PLUM, false);
    const char *d[] = {"EASY", "NORMAL", "HARD"};
    if (ui_button(a, d[a->difficulty], 953, 294, 233, 52, LAVENDER, true))
        a->difficulty = (a->difficulty + 1) % 3;
    ui_text(a, "Input", 595, 399, 24, PLUM, false);
    if (ui_button(a, a->input ? "TYPE + ENTER" : "QUICK BUTTONS", 953, 393, 233, 52, MINT, true))
        a->input = !a->input;

    ui_text(a,"369 RULES",595,468,22,PLUM,false);
    if(ui_button(a,"BASIC",785,465,180,55,a->chosen==CLASSIC?CORAL:CREAM,true)) a->chosen=CLASSIC;
    if(ui_button(a,"ITEMS + EVENTS",980,465,210,55,a->chosen==ITEMS?MINT:CREAM,true)) a->chosen=ITEMS;

    if (ui_button(a, a->chosen==CLASSIC?"START · FREE":"START · 5 COINS", 595, 560, 591, 56, CORAL, true))
        app_start(a);
}
