#include "app.h"
#include <stdio.h>

#define P(a, ko, en) ((a)->save.language ? (ko) : (en))
static const SDL_Color BLUE={163,201,226,255}, GOLD={244,206,128,255};
static void txt(App *a,const char *s,int x,int y,int size) { ui_text(a,s,x,y,size,PLUM,false); }
static void reward(App *a, bool *paid, int amount) {
    if(*paid) return;
    *paid=true; a->save.coins+=amount;
    if(a->save.coins>999999) a->save.coins=999999;
    if(!save_write(a)) app_message(a,"Could not save progress. Check folder access.");
    audio_cue(a,3);
}
void puzzle_begin(App *a,int kind) {
    a->puzzle_kind=kind;
    hanoi_start(&a->hanoi,3);
    count31_start(&a->count31,0,0,SDL_GetTicks());
    a->count31.bot_at=a->clock+1;
    dots_start(&a->dots,0);
    app_go(a,PUZZLE);
}
static void hanoi_select(App *a,int peg) {
    Hanoi *h=&a->hanoi;
    if(h->done) return;
    if(h->selected<0) {
        if(h->count[peg]) { h->selected=peg; audio_cue(a,0); }
        return;
    }
    if(h->selected==peg) { h->selected=-1; return; }
    if(hanoi_move(h,h->selected,peg)) {
        audio_cue(a,1);
        if(h->done) reward(a,&h->rewarded,6*h->disks);
    } else {
        app_message(a,P(a,"작은 원판 위에 큰 원판을 놓을 수 없어요","A large disk cannot go on a small disk."));
        audio_cue(a,2);
    }
}
static void hint(App *a) {
    hanoi_hint(&a->hanoi,&a->hanoi.hint_from,&a->hanoi.hint_to);
    a->hanoi.selected=-1;
}
static void say_numbers(App *a,int amount) {
    Count31 *g=&a->count31;
    if(g->turn!=0 || !count31_play(g,amount)) return;
    audio_cue(a,g->done?2:1);
    if(g->done) reward(a,&g->rewarded,g->winner==0?25:5);
    else g->bot_at=a->clock+1.15;
}
void puzzle_update(App *a) {
    if(a->help_open || a->puzzle_kind!=1) return;
    Count31 *g=&a->count31;
    if(!g->done && g->turn==1 && a->clock>=g->bot_at) {
        count31_play(g,count31_bot(g)); audio_cue(a,1);
        if(g->done) reward(a,&g->rewarded,g->winner==0?25:5);
    }
}
bool puzzle_key(App *a,SDL_Keycode key) {
    if(a->puzzle_kind==0) {
        if(key>=SDLK_1 && key<SDLK_1+a->hanoi.rods) { hanoi_select(a,(int)(key-SDLK_1)); return true; }
        if(key==SDLK_u) { hanoi_undo(&a->hanoi); return true; }
        if(key==SDLK_h) { hint(a); return true; }
    } else if(a->puzzle_kind==1 && key>=SDLK_1 && key<=SDLK_3) {
        say_numbers(a,(int)(key-SDLK_0)); return true;
    }
    return false;
}
static void hanoi_render(App *a) {
    Hanoi *h=&a->hanoi; char s[120];
    txt(a,P(a,"하나씩 옮겨 볼까요?","One disk at a time"),65,150,25);
    txt(a,P(a,"작은 원판은 위로","Small disks go on top"),65,200,21);
    txt(a,P(a,"모두 오른쪽 기둥으로!","Move all disks to the right!"),65,232,20);
    for(int i=0;i<3;i++) {
        int disks=3+i*2;
        snprintf(s,sizeof(s),P(a,"%d개","%d disks"),disks);
        if(ui_button(a,s,65+i*100,285,90,48,h->disks==disks?CORAL:LAVENDER,true)) {
            if(h->disks!=disks) hanoi_start_ex(h,disks,h->rods);
            return;
        }
    }
    for(int i=0;i<3;i++) {
        int rods=3+i;
        snprintf(s,sizeof(s),P(a,"막대 %d개","%d rods"),rods);
        if(ui_button(a,s,65+i*100,344,90,46,h->rods==rods?MINT:LAVENDER,true)) {
            if(h->rods!=rods)hanoi_start_ex(h,h->disks,rods);
            return;
        }
    }
    snprintf(s,sizeof(s),P(a,"이동 %d회 / 최소 %d회","Moves %d / Best %d"),h->moves,hanoi_minimum(h->disks,h->rods));
    txt(a,s,65,412,20);
    if(ui_button(a,P(a,"한 수 되돌리기 [U]","Undo [U]"),65,451,290,46,MINT,!h->done&&h->history_count>0)) hanoi_undo(h);
    if(ui_button(a,P(a,"다음 수 힌트 [H]","Hint [H]"),65,504,290,46,GOLD,!h->done)) hint(a);
    if(ui_button(a,P(a,"처음부터","Restart"),65,562,290,48,CREAM,true)) { hanoi_start_ex(h,h->disks,h->rods); return; }
    txt(a,P(a,"옮길 기둥 → 놓을 기둥 순서로 선택해요","Choose a source tower, then a destination."),445,151,22);
    if(h->done) {
        ui_round(a,455,205,740,74,20,MINT);
        snprintf(s,sizeof(s),P(a,"완성! %d회 이동 · +%d코인","Solved! %d moves / +%d coins"),h->moves,h->disks*6);
        ui_text(a,s,825,228,27,PLUM,true);
    } else if(h->hint_from>=0) {
        snprintf(s,sizeof(s),P(a,"힌트: %d번 → %d번 기둥","Hint: tower %d > tower %d"),h->hint_from+1,h->hint_to+1);
        ui_text(a,s,825,225,25,PLUM,true);
    } else ui_text(a,P(a,"숫자키로 막대를 선택할 수 있어요","Use number keys to choose rods."),825,230,20,MUTED,true);
    const SDL_Color disk_color[]={CORAL,GOLD,MINT,BLUE,LAVENDER,{237,181,199,255},{180,195,150,255}};
    for(int p=0;p<h->rods;p++) {
        int center=h->rods==3?530+p*280:495+p*(660/(h->rods-1));
        int disk_scale=h->rods==3?22:13;
        int base_width=h->rods==3?45:31;
        int pad=h->rods==3?115:72;
        if(h->selected==p || h->hint_from==p || h->hint_to==p)
            ui_round(a,center-pad,305,pad*2,242,24,h->selected==p?LAVENDER:CREAM);
        ui_round(a,center-7,305,14,226,7,(SDL_Color){176,146,126,255});
        ui_round(a,center-pad,530,pad*2,15,7,(SDL_Color){176,146,126,255});
        for(int i=0;i<h->count[p];i++) {
            int disk=h->peg[p][i], width=base_width+disk*disk_scale, y=493-i*28;
            ui_round(a,center-width/2,y,width,26,10,PLUM);
            ui_round(a,center-width/2+2,y+2,width-4,22,9,disk_color[disk-1]);
            snprintf(s,sizeof(s),"%d",disk); ui_text(a,s,center,y+2,18,PLUM,true);
        }
        snprintf(s,sizeof(s),P(a,"%d번 기둥","Tower %d"),p+1);
        int bw=h->rods==3?200:130;
        if(ui_button(a,s,center-bw/2,569,bw,53,h->selected==p?CORAL:CREAM,!h->done)) hanoi_select(a,p);
    }
}
static void count31_render(App *a) {
    Count31 *g=&a->count31; char s[160];
    txt(a,P(a,"31을 말하면 져요!","Do not say 31!"),65,150,25);
    txt(a,P(a,"한 번에 숫자 1~3개","Say 1 to 3 numbers"),65,199,21);
    txt(a,P(a,"봇과 번갈아 이어 말해요","Take turns with a bot"),65,231,20);
    if(ui_button(a,g->difficulty?P(a,"난이도: 도전","Level: Challenge"):P(a,"난이도: 연습","Level: Practice"),65,284,290,48,LAVENDER,g->value==0)) g->difficulty=!g->difficulty;
    if(ui_button(a,g->turn?P(a,"봇 먼저 → 내가 먼저","Bot first > You first"):P(a,"내가 먼저 → 봇 먼저","You first > Bot first"),65,347,290,48,MINT,g->value==0)) {
        g->turn=!g->turn; g->bot_at=a->clock+2.0;
    }
    ui_avatar(a,a->save.skin,75,414,117,g->turn==0&&!g->done,true);
    ui_avatar(a,app_bot_skin(a,1),222,414,117,g->turn==1&&!g->done,true);
    if(ui_button(a,P(a,"다시 하기","Play again"),65,562,290,48,CREAM,true)) {
        int level=g->difficulty; count31_start(g,level,0,SDL_GetTicks()); return;
    }
    ui_text(a,g->done?(g->winner==0?P(a,"내가 이겼어요! +25코인","You win! +25 coins"):P(a,"다음엔 이길 수 있어요! +5코인","Try again! +5 coins")):
                    (g->turn==0?P(a,"내 차례예요","Your turn"):P(a,"봇이 생각하고 있어요…","Bot is thinking...")),825,148,27,PLUM,true);
    for(int n=1;n<=31;n++) {
        int x=462+(n-1)%8*92, y=215+(n-1)/8*67;
        SDL_Color c=n==31?CORAL:n<=g->value?MINT:CREAM;
        if(n>=g->last_start && n<g->last_start+g->last_count && g->last_player>=0) c=g->last_player==0?GOLD:BLUE;
        ui_round(a,x,y,72,52,14,c);
        snprintf(s,sizeof(s),"%d",n); ui_text(a,s,x+36,y+7,26,PLUM,true);
    }
    if(g->last_player>=0) {
        char numbers[64]="", one[16];
        for(int i=0;i<g->last_count;i++) { snprintf(one,sizeof(one),"%s%d",i?"  ":"",g->last_start+i); size_t len=0; while(numbers[len]) len++; snprintf(numbers+len,sizeof(numbers)-len,"%s",one); }
        snprintf(s,sizeof(s),"%s: %s",g->last_player==0?P(a,"나","You"):P(a,"봇","Bot"),numbers);
        ui_text(a,s,825,499,24,PLUM,true);
    } else ui_text(a,P(a,"1부터 시작해요. 몇 개를 말할까요?","Start at 1. How many will you say?"),825,499,21,MUTED,true);
    for(int count=1;count<=3;count++) {
        bool valid=!g->done && g->turn==0 && g->value+count<=31;
        snprintf(s,sizeof(s),P(a,"%d개 말하기 [%d]","Say %d [%d]"),count,count);
        if(ui_button(a,s,455+(count-1)*250,565,236,57,count==1?CORAL:count==2?MINT:LAVENDER,valid)) { say_numbers(a,count); return; }
    }
}
static void line(App *a,int x1,int y1,int x2,int y2,SDL_Color c) {
    SDL_SetRenderDrawColor(a->renderer,c.r,c.g,c.b,c.a);
    for(int d=-2;d<=2;d++) {
        SDL_RenderDrawLine(a->renderer,x1+d,y1,x2+d,y2);
        SDL_RenderDrawLine(a->renderer,x1,y1+d,x2,y2+d);
    }
}
static const char *picture_name(App *a,int n) {
    static const char *ko[]={"작은 집","반짝이는 별","물고기","우주 로켓","사랑의 하트","돛단배"};
    return a->save.language?ko[n]:dot_pictures[n].name;
}
static void dots_render(App *a) {
    DotGame *g=&a->dots; const DotPicture *p=&dot_pictures[g->picture]; char s[120];
    txt(a,P(a,"숫자를 따라 이어요","Connect the numbers"),65,150,25);
    txt(a,P(a,"1부터 순서대로 눌러요","Click in order, starting at 1"),65,200,19);
    txt(a,P(a,"숨어 있는 그림을 찾아요","Discover a hidden picture"),65,232,19);
    snprintf(s,sizeof(s),"%d / %d",g->picture+1,dot_picture_count);
    ui_text(a,s,210,293,20,MUTED,true);
    if(ui_button(a,"<",65,284,60,48,CREAM,true)) { dots_start(g,(g->picture+dot_picture_count-1)%dot_picture_count); return; }
    if(ui_button(a,">",295,284,60,48,CREAM,true)) { dots_start(g,(g->picture+1)%dot_picture_count); return; }
    ui_text(a,picture_name(a,g->picture),210,357,27,PLUM,true);
    if(ui_button(a,g->hint?P(a,"힌트 끄기","Hide hint"):P(a,"다음 숫자 힌트","Show next number"),65,426,290,48,MINT,!g->done)) g->hint=!g->hint;
    snprintf(s,sizeof(s),P(a,"연결한 점: %d / %d","Connected: %d / %d"),g->next,p->count); txt(a,s,65,502,20);
    if(ui_button(a,P(a,"처음부터","Restart"),65,562,290,48,CREAM,true)) { dots_start(g,g->picture); return; }
    snprintf(s,sizeof(s),g->done?P(a,"그림 완성! +18코인","Picture complete! +18 coins"):P(a,"다음 숫자는 %d","Next number: %d"),g->next+1);
    ui_text(a,s,825,149,26,PLUM,true);
    for(int i=1;i<g->next;i++) line(a,410+p->point[i-1].x,p->point[i-1].y+70,410+p->point[i].x,p->point[i].y+70,BLUE);
    if(g->done) line(a,410+p->point[p->count-1].x,p->point[p->count-1].y+70,410+p->point[0].x,p->point[0].y+70,CORAL);
    for(int i=0;i<p->count;i++) {
        int x=410+p->point[i].x, y=p->point[i].y+70;
        snprintf(s,sizeof(s),"%d",i+1);
        if(i<g->next) {
            ui_round(a,x-19,y-19,38,38,19,MINT); ui_text(a,s,x,y-11,17,PLUM,true);
        } else {
            if(g->hint&&i==g->next) ui_round(a,x-29,y-29,58,58,29,GOLD);
            if(ui_button(a,s,x-22,y-22,44,44,CREAM,true)) {
                if(dots_pick(g,i)) { audio_cue(a,1); if(g->done) reward(a,&g->rewarded,18); }
                else { audio_cue(a,2); app_message(a,P(a,"다음 숫자를 찾아서 눌러 주세요","Find and click the next number.")); }
                return;
            }
        }
    }
    ui_text(a,P(a,"틀려도 괜찮아요. 천천히 이어 보세요","Take your time. It is okay to try again."),825,610,19,MUTED,true);
}
void puzzle_render(App *a) {
    const char *ko[]={"하노이탑","배스킨라빈스 31","숫자 따라 그림 그리기"};
    const char *en[]={"Tower of Hanoi","Count to 31","Connect the dots"};
    if(ui_header(a,a->save.language?ko[a->puzzle_kind]:en[a->puzzle_kind])) { app_go(a,SINGLE); return; }
    ui_panel(a,40,128,350,520,CREAM);
    ui_panel(a,415,128,820,520,CREAM);
    if(a->puzzle_kind==0) hanoi_render(a);
    else if(a->puzzle_kind==1) count31_render(a);
    else dots_render(a);
}
void puzzle_help(App *a) {
    static const char *ko[3][6]={
        {"목표: 모든 원판을 맨 오른쪽 기둥으로 옮겨요","한 번에 맨 위 원판 하나만 옮길 수 있어요","큰 원판을 작은 원판 위에 놓을 수 없어요","원판 3·5·7개, 막대 3·4·5개를 선택해요","U: 한 수 되돌리기 / H: 현재 위치의 최단 힌트","완성하면 실제 이동 횟수와 최소 이동 횟수를 비교해요"},
        {"목표: 31을 말하지 않고 봇과 대결해요","1부터 이어서 한 번에 1~3개의 숫자를 말해요","예: 5까지 나왔다면 6 또는 6·7 또는 6·7·8을 말해요","31을 말한 사람이 져요. 31을 넘길 수는 없어요","숫자키 1~3 또는 버튼으로 말할 개수를 골라요","연습: 무작위 봇 / 도전: 전략 봇 · 승리 25코인, 패배 5코인"},
        {"목표: 1부터 마지막 숫자까지 순서대로 점을 이어요","점을 누르면 이전 점과 선으로 연결돼요","잘못 누른 점은 연결되지 않아요. 다시 골라 보세요","힌트 버튼을 누르면 다음 점이 강조돼요","작은 집, 별, 물고기, 로켓, 하트, 돛단배 6가지 그림","그림 하나를 완성하면 18코인 · 시간 제한은 없어요"}
    };
    static const char *en[3][6]={
        {"Goal: move all disks to the far-right rod.","Move only the top disk, one at a time.","A large disk cannot rest on a smaller disk.","Choose 3 / 5 / 7 disks and 3 / 4 / 5 rods.","U: undo / H: shortest hint from your current position.","Compare your moves with the minimum at the finish."},
        {"Goal: avoid saying 31 in a one-on-one match with a bot.","Start at 1. Say 1, 2 or 3 consecutive numbers per turn.","After 5, you can say 6, or 6-7, or 6-7-8.","The player who says 31 loses. You cannot go beyond 31.","Use keys 1 / 2 / 3 or the buttons to choose how many.","Practice: random bot / Challenge: strategic bot. Win 25, lose 5 coins."},
        {"Goal: connect every numbered dot in order, starting at 1.","Click a dot to draw a line from the previous dot.","Wrong dots are not connected. You can try again.","Hint highlights the next dot.","Six pictures: house, star, fish, rocket, heart and sailboat.","Each completed picture earns 18 coins. No time limit."}
    };
    SDL_SetRenderDrawColor(a->renderer,47,30,51,160); SDL_RenderFillRect(a->renderer,NULL);
    a->button_count=0; ui_panel(a,100,110,1080,530,CREAM);
    ui_text(a,P(a,"게임 방법","HOW TO PLAY"),640,140,30,PLUM,true);
    for(int i=0;i<6;i++) txt(a,a->save.language?ko[a->puzzle_kind][i]:en[a->puzzle_kind][i],145,215+i*49,22);
    if(ui_button(a,"CLOSE",465,560,350,52,MINT,true)) help_toggle(a);
}
