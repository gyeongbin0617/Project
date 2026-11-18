#include "puzzles.h"
#include <string.h>
#include <stdlib.h>

void hanoi_start_ex(Hanoi *h,int disks,int rods) {
    memset(h,0,sizeof(*h));
    h->disks=disks>=7?7:disks>=5?5:3;
    h->rods=rods>=5?5:rods>=4?4:3;
    h->count[0]=h->disks;
    for(int i=0;i<h->disks;i++)h->peg[0][i]=h->disks-i;
    h->selected=h->hint_from=h->hint_to=-1;
}
void hanoi_start(Hanoi *h,int disks) {hanoi_start_ex(h,disks,3);}
int hanoi_minimum(int disks,int rods) {
    if(disks<1 || disks>7 || rods<3 || rods>5)return 0;
    int best[6][8]={0};
    for(int n=1;n<=7;n++)best[3][n]=(1<<n)-1;
    for(int r=4;r<=5;r++) for(int n=1;n<=7;n++) {
        best[r][n]=best[r-1][n];
        for(int k=1;k<n;k++) {
            int moves=2*best[r][k]+best[r-1][n-k];
            if(moves<best[r][n])best[r][n]=moves;
        }
    }
    return best[rods][disks];
}
bool hanoi_move(Hanoi *h,int from,int to) {
    if(h->done || from<0 || from>=h->rods || to<0 || to>=h->rods || from==to || !h->count[from])return false;
    int disk=h->peg[from][h->count[from]-1];
    if(h->count[to] && h->peg[to][h->count[to]-1]<disk)return false;
    if(h->history_count==512){memmove(h->history,h->history+1,511*sizeof(h->history[0]));h->history_count--;}
    h->history[h->history_count][0]=from;h->history[h->history_count++][1]=to;
    h->count[from]--;h->peg[to][h->count[to]++]=disk;
    h->moves++;h->selected=h->hint_from=h->hint_to=-1;
    h->done=h->count[h->rods-1]==h->disks;
    return true;
}
bool hanoi_undo(Hanoi *h) {
    if(h->done || !h->history_count)return false;
    int from=h->history[--h->history_count][0],to=h->history[h->history_count][1];
    h->peg[from][h->count[from]++]=h->peg[to][--h->count[to]];
    h->moves--;h->selected=h->hint_from=h->hint_to=-1;return true;
}
/* Shortest path from any legal state, including detours, for three to five rods. */
bool hanoi_hint(const Hanoi *h,int *from,int *to) {
    if(h->done || h->rods<3 || h->rods>5)return false;
    int power[8]={1},capacity=1,start=0,goal=0;
    for(int i=1;i<=h->disks;i++)power[i]=power[i-1]*h->rods;
    capacity=power[h->disks];
    for(int p=0;p<h->rods;p++)for(int i=0;i<h->count[p];i++)start+=p*power[h->peg[p][i]-1];
    for(int i=0;i<h->disks;i++)goal+=(h->rods-1)*power[i];
    if(start==goal)return false;
    int *queue=malloc((size_t)capacity*sizeof(int));
    int *first_from=malloc((size_t)capacity*sizeof(int));
    int *first_to=malloc((size_t)capacity*sizeof(int));
    unsigned char *seen=calloc((size_t)capacity,1);
    if(!queue || !first_from || !first_to || !seen) {
        free(queue);free(first_from);free(first_to);free(seen);return false;
    }
    int head=0,tail=0;bool found=false;
    queue[tail++]=start;seen[start]=1;
    while(head<tail && !found) {
        int state=queue[head++],top[5]={0},v=state;
        for(int disk=1;disk<=h->disks;disk++){int p=v%h->rods;v/=h->rods;if(!top[p])top[p]=disk;}
        for(int a=0;a<h->rods && !found;a++)for(int b=0;b<h->rods;b++) {
            if(a==b || !top[a] || (top[b] && top[b]<top[a]))continue;
            int next=state+(b-a)*power[top[a]-1];
            if(seen[next])continue;
            seen[next]=1;queue[tail++]=next;
            first_from[next]=state==start?a:first_from[state];
            first_to[next]=state==start?b:first_to[state];
            if(next==goal){*from=first_from[next];*to=first_to[next];found=true;break;}
        }
    }
    free(queue);free(first_from);free(first_to);free(seen);return found;
}
void count31_start(Count31 *g, int difficulty, int first, uint32_t seed) {
    memset(g,0,sizeof(*g)); g->difficulty=!!difficulty; g->turn=!!first;
    g->winner=-1; g->last_player=-1; g->rng=seed?seed:1234567;
}
bool count31_play(Count31 *g, int count) {
    if(g->done || count<1 || count>3 || g->value+count>31) return false;
    g->last_start=g->value+1; g->last_count=count; g->last_player=g->turn;
    g->value+=count;
    if(g->value==31) { g->done=true; g->winner=1-g->turn; }
    else g->turn=1-g->turn;
    return true;
}
int count31_bot(Count31 *g) {
    if(g->done || g->turn!=1) return 0;
    int available=31-g->value;
    if(g->difficulty) {
        int best=(30-g->value)%4;
        if(best>=1 && best<=3) return best;
    }
    g->rng^=g->rng<<13; g->rng^=g->rng>>17; g->rng^=g->rng<<5;
    int max=available<3?available:3;
    return 1+(int)(g->rng%(unsigned)max);
}
/* Original point coordinates: no external picture files are needed. */
const DotPicture dot_pictures[] = {
 {"Little house", 11, {{220,470},{220,300},{150,300},{390,170},{630,300},{560,300},{560,470},{440,470},{440,365},{340,365},{340,470}}},
 {"Twinkle star", 10, {{390,160},{440,280},{580,280},{470,365},{510,490},{390,415},{270,490},{310,365},{200,280},{340,280}}},
 {"Little fish", 10, {{200,330},{290,235},{430,230},{540,295},{630,220},{630,440},{540,375},{430,440},{290,425},{245,380}}},
 {"Rocket", 13, {{390,150},{470,250},{470,370},{550,450},{470,430},{450,490},{390,455},{330,490},{310,430},{230,450},{310,370},{310,250},{350,195}}},
 {"Lovely heart", 14, {{390,250},{445,195},{520,185},{585,240},{600,310},{560,370},{490,425},{390,495},{290,425},{220,370},{180,310},{195,240},{260,185},{335,195}}},
 {"Sailboat", 12, {{160,380},{345,380},{345,190},{210,335},{390,335},{390,170},{570,350},{440,350},{440,380},{620,380},{535,485},{245,485}}}
};
const int dot_picture_count=(int)(sizeof(dot_pictures)/sizeof(dot_pictures[0]));
void dots_start(DotGame *g, int picture) {
    memset(g,0,sizeof(*g)); g->picture=picture>=0 && picture<dot_picture_count?picture:0;
}
bool dots_pick(DotGame *g, int point) {
    if(g->done || point<0 || point>=dot_pictures[g->picture].count) return false;
    if(point!=g->next) { g->mistakes++; return false; }
    g->next++; g->done=g->next==dot_pictures[g->picture].count;
    return true;
}
