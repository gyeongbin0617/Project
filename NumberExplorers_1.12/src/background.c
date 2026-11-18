#include "app.h"
#include <math.h>
/* Original code-drawn scenery. The same renderer is used for previews and play. */
static SDL_Color rgb(int r,int g,int b){return (SDL_Color){r,g,b,255};}
static void box(App*a,SDL_Rect v,int x,int y,int w,int h,SDL_Color c){
 SDL_Rect q={v.x+x*v.w/1280,v.y+y*v.h/720,w*v.w/1280+1,h*v.h/720+1};
 SDL_SetRenderDrawColor(a->renderer,c.r,c.g,c.b,c.a);SDL_RenderFillRect(a->renderer,&q);
}
static void oval(App*a,SDL_Rect v,int x,int y,int rx,int ry,SDL_Color c){
 SDL_SetRenderDrawColor(a->renderer,c.r,c.g,c.b,c.a);
 int cy=v.y+y*v.h/720,cx=v.x+x*v.w/1280,hy=ry*v.h/720,wx=rx*v.w/1280;
 if(hy<1)hy=1;
 for(int j=-hy;j<=hy;j++){int d=(int)(wx*sqrt(1.0-(double)j*j/(hy*hy)));SDL_RenderDrawLine(a->renderer,cx-d,cy+j,cx+d,cy+j);}
}
void ui_background(App*a,int theme,SDL_Rect v){
 if(theme==0){SDL_RenderCopy(a->renderer,a->room,NULL,&v);return;}
 SDL_Rect old;SDL_RenderGetClipRect(a->renderer,&old);SDL_bool clipped=SDL_RenderIsClipEnabled(a->renderer);SDL_RenderSetClipRect(a->renderer,&v);
 SDL_Color top=theme==1?rgb(181,221,225):theme==2?rgb(231,169,167):rgb(68,69,112);
 SDL_Color low=theme==1?rgb(246,243,206):theme==2?rgb(255,225,171):rgb(158,143,190);
 for(int y=0;y<720;y+=3)box(a,v,0,y,1280,3,rgb(top.r+(low.r-top.r)*y/720,top.g+(low.g-top.g)*y/720,top.b+(low.b-top.b)*y/720));
 if(theme==1){
  oval(a,v,1050,140,68,68,rgb(255,237,172));
  for(int i=0;i<4;i++){int x=120+i*300,y=100+(i%2)*75;oval(a,v,x,y,80,25,CREAM);oval(a,v,x+25,y-18,43,29,CREAM);}
  oval(a,v,260,650,650,260,rgb(157,194,149));oval(a,v,1100,710,780,270,rgb(190,211,158));
  box(a,v,80,300,22,260,rgb(163,128,103));oval(a,v,90,280,115,140,rgb(130,173,143));
  box(a,v,1170,270,24,285,rgb(163,128,103));oval(a,v,1180,250,125,145,rgb(140,185,150));
  for(int i=0;i<32;i++){int x=(i*193+45)%1280,y=535+(i*37)%180;oval(a,v,x,y,7,7,i%2?CREAM:rgb(238,171,174));oval(a,v,x,y,2,3,rgb(233,189,94));}
  box(a,v,420,610,440,95,rgb(248,225,207));for(int i=0;i<11;i++)box(a,v,420+i*40,610,17,95,rgb(237,187,174));for(int i=0;i<3;i++)box(a,v,420,618+i*32,440,11,rgb(250,239,216));
 }else if(theme==2){
  oval(a,v,965,300,85,85,rgb(255,232,172));box(a,v,0,375,1280,345,rgb(132,179,185));
  for(int i=0;i<12;i++){int w=85+i*16;box(a,v,965-w/2,390+i*15,w,5,rgb(243,212,174));}
  for(int i=0;i<8;i++)box(a,v,(i*213)%1000,410+i*23,150,3,rgb(180,212,209));
  oval(a,v,680,890,1200,340,rgb(244,216,180));oval(a,v,200,810,650,240,rgb(255,229,193));
  box(a,v,175,435,8,205,rgb(155,118,103));oval(a,v,179,438,112,57,rgb(222,150,150));box(a,v,65,438,230,65,rgb(132,179,185));
  for(int i=0;i<8;i++)oval(a,v,330+i*109,637+(i%3)*24,12,6,rgb(219,173,155));
 }else{
  for(int i=0;i<70;i++){int x=(i*197+39)%1280,y=(i*83+30)%480;int size=i%5==0?4:2;oval(a,v,x,y,size,size,CREAM);if(size==4){box(a,v,x-8,y,17,1,CREAM);box(a,v,x,y-8,1,17,CREAM);}}
  oval(a,v,1040,145,65,65,rgb(255,236,191));oval(a,v,1062,126,60,60,rgb(83,81,125));
  oval(a,v,120,815,780,300,rgb(90,105,140));oval(a,v,1200,800,850,290,rgb(107,119,150));
  oval(a,v,620,830,950,250,rgb(69,89,121));
  for(int i=0;i<10;i++){int x=i*145-20;box(a,v,x,585,10,130,rgb(57,76,106));oval(a,v,x+5,590,35,85,rgb(63,84,113));}
 }
 SDL_RenderSetClipRect(a->renderer,clipped?&old:NULL);
}
