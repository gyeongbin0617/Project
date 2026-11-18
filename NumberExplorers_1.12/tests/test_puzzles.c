#include "puzzles.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void invariant(const Hanoi *h) {
    int seen[8]={0}, total=0;
    for(int p=0;p<h->rods;p++) for(int i=0;i<h->count[p];i++) {
        int d=h->peg[p][i]; assert(d>=1&&d<=h->disks); assert(!seen[d]);seen[d]=1;total++;
        if(i) assert(h->peg[p][i-1]>d);
    }
    assert(total==h->disks);
}
int main(void) {
    int states=0;
    for(int disks=3;disks<=7;disks+=2) {
        Hanoi h; hanoi_start(&h,disks); assert(!hanoi_move(&h,0,0));assert(!hanoi_move(&h,1,2));
        assert(hanoi_move(&h,0,1)); assert(!hanoi_move(&h,0,1)); assert(hanoi_undo(&h));assert(h.moves==0&&h.count[0]==disks);
        while(!h.done) {int f,t; assert(hanoi_hint(&h,&f,&t)); assert(hanoi_move(&h,f,t)); invariant(&h);}
        assert(h.moves==(1<<disks)-1);assert(!hanoi_undo(&h));
        int combinations=1;for(int i=0;i<disks;i++)combinations*=3;
        for(int code=0;code<combinations;code++) {
            hanoi_start(&h,disks); memset(h.count,0,sizeof(h.count));
            int loc[7],v=code;for(int i=0;i<disks;i++){loc[i]=v%3;v/=3;}
            for(int d=disks;d>=1;d--){int p=loc[d-1];h.peg[p][h.count[p]++]=d;}
            h.done=h.count[2]==disks; int guard=0;
            while(!h.done) {int f,t;assert(hanoi_hint(&h,&f,&t));assert(hanoi_move(&h,f,t));invariant(&h);assert(++guard<=127);}
            states++;
        }
    }
    for(int rods=4;rods<=5;rods++) for(int disks=3;disks<=7;disks+=2) {
        Hanoi h;hanoi_start_ex(&h,disks,rods);
        assert(h.rods==rods&&hanoi_minimum(disks,rods)>0);
        int guard=0;
        while(!h.done) {
            int from,to;assert(hanoi_hint(&h,&from,&to));
            assert(from>=0&&from<rods&&to>=0&&to<rods);
            assert(hanoi_move(&h,from,to));invariant(&h);
            assert(++guard<=127);
        }
        assert(h.moves==hanoi_minimum(disks,rods));
        assert(h.count[rods-1]==disks);
    }
    Count31 g;
    for(int value=0;value<31;value++) for(int count=1;count<=3;count++) for(int turn=0;turn<2;turn++) {
        count31_start(&g,1,turn,7);g.value=value;
        bool legal=value+count<=31; assert(count31_play(&g,count)==legal);
        if(legal){assert(g.value==value+count);assert(g.done==(g.value==31));if(g.done)assert(g.winner==1-turn);else assert(g.turn==1-turn);}
    }
    for(int seed=1;seed<=100;seed++) for(int level=0;level<2;level++) for(int first=0;first<2;first++) {
        count31_start(&g,level,first,(unsigned)seed);int guard=0;
        while(!g.done){int count=g.turn?count31_bot(&g):1+(seed+g.value)%3;if(count>31-g.value)count=31-g.value;assert(count31_play(&g,count));assert(++guard<=127);}
        assert(g.value==31&&g.winner!=g.last_player);assert(!count31_play(&g,1));
    }
    for(int value=0;value<30;value++){count31_start(&g,1,1,2);g.value=value;int c=count31_bot(&g);assert(c>=1&&c<=3);if((30-value)%4)assert((value+c)%4==2);}
    for(int p=0;p<dot_picture_count;p++) {
        DotGame d;dots_start(&d,p);assert(!dots_pick(&d,1));assert(d.next==0&&d.mistakes==1);
        for(int i=0;i<dot_pictures[p].count;i++) {assert(dots_pick(&d,i)); assert(d.next==i+1);}
        assert(d.done);assert(!dots_pick(&d,0));
    }
    printf("PASS: Hanoi %d three-rod arrangements; 3/4/5 rods × 3/5/7 disks, shortest hints, undo and invalid moves\n",states);
    puts("PASS: 31 boundaries, both turns, 400 complete games, challenge strategy");
    puts("PASS: six dot pictures, wrong order, completion lock");
}
