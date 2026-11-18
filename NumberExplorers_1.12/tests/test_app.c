#include "app.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static void key(App*a,SDL_Keycode k){SDL_Event e={0};e.type=SDL_KEYDOWN;e.key.keysym.sym=k;app_event(a,&e);}
static void click(App*a,int x,int y){SDL_Event e={0};e.type=SDL_MOUSEBUTTONUP;e.button.button=SDL_BUTTON_LEFT;e.button.x=x;e.button.y=y;app_event(a,&e);app_render(a);}
static void shot(App*a,const char *name){
 app_render(a);SDL_Surface*s=SDL_CreateRGBSurfaceWithFormat(0,WIDTH,HEIGHT,32,SDL_PIXELFORMAT_RGBA32);assert(s);
 assert(SDL_RenderReadPixels(a->renderer,NULL,SDL_PIXELFORMAT_RGBA32,s->pixels,s->pitch)==0);
 char p[256];snprintf(p,sizeof(p),"../qa_numberplay/%s_%s.png",a->save.language?"ko":"en",name);assert(IMG_SavePNG(s,p)==0);SDL_FreeSurface(s);
}
int main(void){
 App*a=calloc(1,sizeof(*a));assert(a&&app_init(a,true));snprintf(a->save_path,sizeof(a->save_path),"../qa_numberplay/test_save.txt");
 for(int lang=0;lang<2;lang++){
  a->save.language=lang;
  a->save.mistakes=(MistakeBook){0};
  app_go(a,TITLE);shot(a,"title");app_go(a,LOBBY);shot(a,"lobby");app_go(a,SINGLE);a->mode_page=0;shot(a,"menu");
  click(a,620,475);assert(a->train_level==2);shot(a,"train_hard_menu");
  click(a,300,550);assert(a->scene==TRAIN&&a->train.level==2);shot(a,"train");
  help_toggle(a);shot(a,"train_help");help_toggle(a);
  int initial=a->save.mistakes.count;
  int incorrect=0;while(a->train.options[incorrect]==a->train.answer) incorrect++;
  key(a,SDLK_1+incorrect);assert(a->train.missed==1&&a->save.mistakes.count==initial+1);
  shot(a,"train_hint");
  int answer=0;while(a->train.options[answer]!=a->train.answer) answer++;
  key(a,SDLK_1+answer);assert(a->train.solved);shot(a,"train_correct");
  key(a,SDLK_RETURN);assert(a->train.stage==1);
  int old_coins=a->save.coins;
  while(a->train.stage<TRAIN_STAGES) {
   int slot=0;while(a->train.options[slot]!=a->train.answer)slot++;
   key(a,SDLK_1+slot);key(a,SDLK_RETURN);
  }
  assert(a->save.coins==old_coins+30);shot(a,"train_finish");
  key(a,SDLK_RETURN);assert(a->save.coins==old_coins+30);
  int train_note=-1;
  for(int n=0;n<a->save.mistakes.count;n++)
   if(a->save.mistakes.note[n].kind>=6)train_note=n;
  assert(train_note>=0);
  a->review_index=train_note;a->review_feedback=false;app_go(a,REVIEW);shot(a,"train_review");
  int review_slot=(a->save.mistakes.note[train_note].left+a->save.mistakes.note[train_note].right)%3;
  key(a,SDLK_1+review_slot);assert(a->review_feedback);
  app_go(a,SINGLE);a->mode_page=0;click(a,1000,550);assert(a->mode_page==1);shot(a,"other_games_1");
  a->save.tutorial_seen_mask=0;
  click(a,160,490);assert(a->scene==TUTORIAL);shot(a,"tutorial");
  click(a,640,580);assert(a->scene==SETUP && (a->save.tutorial_seen_mask&1));
  click(a,1070,490);assert(a->chosen==ITEMS);shot(a,"setup_items");
  click(a,860,490);assert(a->chosen==CLASSIC);shot(a,"setup_basic");
  a->save.tutorial_seen_mask=255;
  const int pages[2][3]={{0,1,2},{3,4,-1}};
  for(int page=1;page<=2;page++)for(int slot=0;slot<3;slot++) {
   int i=pages[page-1][slot];if(i<0)continue;
   app_go(a,SINGLE);a->mode_page=page;app_render(a);click(a,160+slot*410,490);
   assert(a->scene==(i==0?SETUP:i==1?LEARN_SETUP:PUZZLE));
   if(i>=2)assert(a->puzzle_kind==i-2);
   if(i==1) {
    shot(a,"math_combined");click(a,1070,165);assert(a->learn_kind==1);shot(a,"math_multiply");
    click(a,710,165);assert(a->learn_kind==0);
   }
  }
  app_go(a,SINGLE);a->mode_page=2;shot(a,"other_games_2");
  key(a,SDLK_LEFT);assert(a->mode_page==1);key(a,SDLK_RIGHT);assert(a->mode_page==2);
  puzzle_begin(a,0);click(a,205,310);assert(a->hanoi.disks==5);click(a,305,310);assert(a->hanoi.disks==7);
  click(a,305,365);assert(a->hanoi.rods==5);shot(a,"hanoi7_rods5");
  while(!a->hanoi.done){int f,t;assert(hanoi_hint(&a->hanoi,&f,&t));key(a,SDLK_1+f);key(a,SDLK_1+t);}assert(a->hanoi.moves==hanoi_minimum(7,5));
  shot(a,"hanoi7_done");click(a,105,310);assert(a->hanoi.disks==3);shot(a,"hanoi");key(a,SDLK_h);shot(a,"hanoi_hint");
  help_toggle(a);shot(a,"hanoi_help");key(a,SDLK_ESCAPE);assert(!a->help_open);
  key(a,SDLK_1);key(a,SDLK_2);assert(a->hanoi.moves==1);key(a,SDLK_u);assert(a->hanoi.moves==0);
  int coins=a->save.coins;
  while(!a->hanoi.done){int f,t;assert(hanoi_hint(&a->hanoi,&f,&t));key(a,SDLK_1+f);key(a,SDLK_1+t);}
  assert(a->save.coins==coins+18);shot(a,"hanoi_done");key(a,SDLK_1);assert(a->save.coins==coins+18);
  key(a,SDLK_ESCAPE);assert(a->scene==SINGLE);
  puzzle_begin(a,1);shot(a,"count31");key(a,SDLK_3);assert(a->count31.value==3&&a->count31.turn==1);
  a->help_open=true;app_update(a,2);assert(a->count31.value==3);a->help_open=false;app_update(a,.1);assert(a->count31.value>3);
  shot(a,"count31_play");help_toggle(a);shot(a,"count31_help");help_toggle(a);
  coins=a->save.coins;while(!a->count31.done){if(a->count31.turn==0)key(a,SDLK_1);else app_update(a,2);}
  assert(a->save.coins==coins+(a->count31.winner==0?25:5));app_update(a,2);assert(a->save.coins==coins+(a->count31.winner==0?25:5));shot(a,"count31_done");
  puzzle_begin(a,2);shot(a,"dots");help_toggle(a);shot(a,"dots_help");help_toggle(a);
  for(int p=0;p<dot_picture_count;p++){
   dots_start(&a->dots,p);const DotPicture*d=&dot_pictures[p];coins=a->save.coins;
   click(a,410+d->point[1].x,70+d->point[1].y);assert(a->dots.next==0);
   for(int i=0;i<d->count;i++)click(a,410+d->point[i].x,70+d->point[i].y);
   assert(a->dots.done&&a->save.coins==coins+18);char n[40];snprintf(n,sizeof(n),"dots_done_%d",p);shot(a,n);
   app_render(a);assert(a->save.coins==coins+18);
  }
  app_go(a,LOBBY);app_render(a);click(a,800,490);assert(a->scene==LOBBY);assert(a->toast_end>a->clock);shot(a,"multiplayer_notice");
  a->toast_end=0;app_go(a,SHOP);a->shop_category=0;shot(a,"shop");
  click(a,780,140);assert(a->shop_category==1);a->save.bg_owned=1;a->save.background=0;a->save.coins=79;
  click(a,450,570);assert(a->save.coins==79&&a->save.background==0);
  a->save.coins=500;click(a,450,570);assert(a->save.coins==420&&a->save.background==1&&(a->save.bg_owned&2));
  click(a,150,570);assert(a->save.background==0&&a->save.coins==420);
  click(a,450,570);assert(a->save.background==1&&a->save.coins==420);
  click(a,750,570);assert(a->save.background==2&&a->save.coins==300);
  click(a,1050,570);assert(a->save.background==3&&a->save.coins==140);
  a->toast_end=0;shot(a,"background_shop");app_go(a,TITLE);shot(a,"night_title");
  app_go(a,SHOP);click(a,490,140);assert(a->shop_category==0);shot(a,"character_shop");
  int before_notes=a->save.mistakes.count;
  a->learn_kind=0;a->learn_level=0;a->learn_operation=1;learn_begin(a);
  int wrong=0;while(a->lesson.choices[wrong]==a->lesson.answer) wrong++;
  learn_choose(a,wrong);assert(a->save.mistakes.count==before_notes+1);
  game_start(&a->game,CLASSIC,2,0,false,1234);a->game.phase=TURN;a->game.current=0;a->game.value=33;
  app_go(a,MATCH);key(a,SDLK_SPACE);assert(a->game.phase==FEEDBACK);
  assert(a->save.mistakes.count==before_notes+2);
  mistake_add(&a->save.mistakes,0,33,0,2);
  app_go(a,LOBBY);shot(a,"review_button");click(a,800,560);assert(a->scene==REVIEW);shot(a,"review");
  help_toggle(a);shot(a,"review_help");help_toggle(a);
  int pick=(33+0)%3;key(a,SDLK_1+pick);assert(a->review_feedback);shot(a,"review_correct");
  key(a,SDLK_RETURN);assert(!a->review_feedback && a->review_index>=0);
app_go(a,SETTINGS);shot(a,"settings");
  int keep_coins=a->save.coins,keep_bg=a->save.background,keep_notes=a->save.mistakes.count;
  click(a,410,590);assert(a->reset_confirm);shot(a,"settings_reset");
  click(a,620,490);assert(!a->reset_confirm);
  assert(a->save.coins==keep_coins&&a->save.background==keep_bg&&a->save.mistakes.count==keep_notes);
 }
 puts("PASS: three train difficulties, grouped side games, Hanoi input/undo/hint/finish, 31 bot/pause/finish, six dot pictures, one-time rewards, Korean/English renders");
 app_destroy(a);free(a);return 0;
}
