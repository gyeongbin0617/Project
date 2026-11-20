#include "app.h"

/* Render the help overlay for the current scene. */

void help_render(App *a) {
    if (a->scene == PUZZLE) { puzzle_help(a); return; }
    if (a->scene == TRAIN) {
        SDL_SetRenderDrawColor(a->renderer,47,30,51,160);
        SDL_RenderFillRect(a->renderer,NULL);
        a->button_count=0;
        ui_panel(a,180,155,920,425,CREAM);
        ui_text(a,a->save.language?"숫자 기차 타는 법":"HOW TO RIDE THE NUMBER TRAIN",640,192,33,PLUM,true);
        ui_text(a,"2  →  4  →  6  →  8  →  ?",640,293,38,PLUM,true);
        ui_text(a,a->save.language?"2씩 커져요. 다음은 10!":"Each step adds 2. The next is 10!",640,378,24,PLUM,true);
        ui_text(a,a->save.language?"어려움은 +1, +2, +3처럼 변화량도 바뀌어요.":
                    "Hard mode changes the gap: +1, +2, +3...",640,425,20,MUTED,true);
        ui_text(a,a->save.language?"틀려도 다시 풀 수 있고, 숫자 사이의 힌트가 보여요.":
                    "Retry freely; a wrong answer reveals the gaps.",640,463,19,MUTED,true);
        if(ui_button(a,"CLOSE",470,512,340,54,MINT,true)) help_toggle(a);
        return;
    }
    if (a->scene == REVIEW) {
        SDL_SetRenderDrawColor(a->renderer,47,30,51,160);
        SDL_RenderFillRect(a->renderer,NULL);
        a->button_count=0;
        ui_panel(a,190,155,900,425,CREAM);
        ui_text(a,"REVIEW MISTAKES",640,192,34,PLUM,true);
        const char *en[]={"Wrong answers are saved here automatically.",
            "Questions missed more often come first.",
            "Choose 1, 2 or 3. Correct answers clear one retry."};
        const char *ko[]={"게임에서 틀린 문제가 여기에 자동으로 모여요.",
            "자주 틀린 문제부터 다시 풀어요.",
            "1·2·3 중 답을 골라요. 맞히면 복습 횟수가 올라가요."};
        for (int i=0;i<3;i++) {
            ui_round(a,260,270+i*68,760,54,16,i==1?MINT:LAVENDER);
            ui_text(a,a->save.language?ko[i]:en[i],640,280+i*68,21,PLUM,true);
        }
        if (ui_button(a,"CLOSE",470,514,340,52,CORAL,true)) help_toggle(a);
        return;
    }
    SDL_SetRenderDrawColor(a->renderer,47,30,51,160);
    SDL_RenderFillRect(a->renderer,NULL);
    a->button_count=0;
    ui_panel(a,100,90,1080,580,CREAM);
    ui_text(a,"HOW TO PLAY",640,112,30,PLUM,true);
    const char *tabs[]={"Classic 369","Party Mix","Add / subtract","Times / divide"};
    for(int i=0;i<4;i++)
        if(ui_button(a,tabs[i],150+i*246,170,235,48,a->help_page==i?CORAL:LAVENDER,true)) a->help_page=i;
    static const char *examples[4][3]={{"12","13","33"},{"Q","W","E"},{"3 + 2","5 - 2","?"},{"2 x 3","6 / 2","?"}};
    static const char *captions[4][3]={{"Say 12","Clap once","Clap twice"},{"Shield","More time","Skip turn"},{"Count both groups","Cross out circles","Choose an answer"},{"Count groups","Share equally","Choose an answer"}};
    static const char *ko[4][3]={{"12 말하기","박수 한 번","박수 두 번"},{"실수 막기","시간 추가","차례 넘기기"},{"원을 모두 세요","원을 지워 봐요","답을 골라요"},{"묶음을 세요","똑같이 나눠요","답을 골라요"}};
    for(int j=0;j<3;j++) {
        int x=180+j*324;
        ui_round(a,x,270,265,202,30,j==1?MINT:LAVENDER);
        ui_round(a,x+83,292,100,100,50,j==1?CORAL:CREAM);
        ui_text(a,examples[a->help_page][j],x+133,317,31,PLUM,true);
        ui_text(a,a->save.language?ko[a->help_page][j]:captions[a->help_page][j],x+133,420,19,PLUM,true);
    }
    const char *en[]={"Space = say number    C = clap    Esc = pause",
        "Use Q / W / E once per turn. Watch the event!",
        "Tap answer 1 / 2 / 3. No timer; try again.",
        "Choose a times table. Tap 1 / 2 / 3 to answer."};
    const char *kr[]={"Space 숫자 · C 박수 · Esc 일시정지",
        "Q / W / E 아이템 · 이벤트 그림도 확인해요",
        "답 1 / 2 / 3 · 시간 제한 없이 다시 풀어요",
        "구구단을 고르고 답 1 / 2 / 3을 눌러요"};
    ui_text(a,a->save.language?kr[a->help_page]:en[a->help_page],640,512,21,PLUM,true);
    if(ui_button(a,"CLOSE",465,590,350,52,MINT,true)) help_toggle(a);
}
