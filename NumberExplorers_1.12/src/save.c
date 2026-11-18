#include "app.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
static FILE *utf8_open(const char *path, const wchar_t *mode) {
    wchar_t wide[1200];
    if (!MultiByteToWideChar(CP_UTF8, 0, path, -1, wide, 1200))
        return NULL;
    return _wfopen(wide, mode);
}
#endif
static int clamp(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
void save_load(App *a) {
    a->save=(Save){.coins=100,.owned=1,.bg_owned=1,.music=50,.sfx=70,.motion=1,.legacy=1,.language=1};
#ifdef _WIN32
    FILE *f=utf8_open(a->save_path,L"r");
#else
    FILE *f=fopen(a->save_path,"r");
#endif
    if (!f) return;
    char magic[32],line[1024];
    if (!fgets(magic,sizeof(magic),f) || !fgets(line,sizeof(line),f)) {fclose(f);return;}
    magic[strcspn(magic,"\r\n")]=0;
    int version=0;
    if (!strcmp(magic,"369COZY_V1")) version=1;
    else if (!strcmp(magic,"369COZY_V2")) version=2;
    else if (!strcmp(magic,"369COZY_V3")) version=3;
    else if (!strcmp(magic,"369COZY_V4")) version=4;
    else if (!strcmp(magic,"369COZY_V5")) version=5;
    else if (!strcmp(magic,"369COZY_V6")) version=6;
    else if (!strcmp(magic,"369COZY_V7")) version=7;
    else if (!strcmp(magic,"369COZY_V8")) version=8;
    else if (!strcmp(magic,"369COZY_V9")) version=9;
    const int count=version==1?14:version==2?15:version==3?17:version==4?22:version==5?24:version>=6?20:0;
    int value[24]={0},read=0;
    char *cursor=line;
    while (read<count) {
        char *end;
        long v=strtol(cursor,&end,10);
        if (end==cursor || v<INT_MIN || v>INT_MAX) break;
        value[read++]=(int)v;cursor=end;
    }
    if (read!=count) {fclose(f);return;}
    Save s={.bg_owned=1,.language=1};
    s.coins=value[0];s.owned=value[1];s.skin=value[2];s.music=value[3];s.sfx=value[4];
    s.motion=value[5];s.fullscreen=value[6];s.played=value[7];s.wins=value[8];
    for (int i=0;i<4;i++) s.best[i]=value[9+i];
    s.legacy=value[13];
    if (version>=2) s.language=value[14];
    if (version>=3) {s.bg_owned=value[15];s.background=value[16];}
    if (version>=4) {s.last_claim_day=value[17];s.attendance_streak=value[18];s.tutorial_seen_mask=value[19];}
    if (version==4 && value[20]) s.coins=clamp(s.coins+90,0,999999);
    if (version==5) {
        int masks=value[22]&15;
        s.coins=clamp(s.coins+(masks&2?90:0)+(masks&4?110:0)+(masks&8?130:0),0,999999);
    }
    if (version>=7) {
        int amount=0;
        if (fscanf(f,"%d",&amount)==1 && amount>=0 && amount<=MAX_MISTAKES)
            for (int i=0;i<amount;i++) {
                MistakeNote n;
                if (fscanf(f,"%d %d %d %d %d %d",&n.kind,&n.left,&n.right,&n.answer,&n.wrong,&n.reviewed)!=6) break;
                if (n.kind<0 || n.kind>(version==7?5:version==8?7:9) || n.left<0 || n.left>9999 || n.right<0 || n.right>9999 ||
                    n.answer<0 || n.answer>9999 || n.wrong<1 || n.wrong>999 || n.reviewed<0 || n.reviewed>n.wrong) break;
                s.mistakes.note[s.mistakes.count++]=n;
            }
    }
    fclose(f);
    s.bg_owned=(s.bg_owned&15)|1;
    s.background=clamp(s.background,0,3);
    if (!(s.bg_owned&(1<<s.background))) s.background=0;
    s.language=clamp(s.language,0,1);
    s.coins=clamp(s.coins,0,999999);
    s.owned=(s.owned&255)|1;
    s.skin=clamp(s.skin,0,7);
    if (!(s.owned&(1<<s.skin))) s.skin=0;
    s.music=clamp(s.music,0,100);s.sfx=clamp(s.sfx,0,100);
    s.motion=!!s.motion;s.fullscreen=!!s.fullscreen;s.legacy=!!s.legacy;
    s.last_claim_day=clamp(s.last_claim_day,0,9999999);
    s.attendance_streak=clamp(s.attendance_streak,0,7);
    s.tutorial_seen_mask &=255;
    s.played=clamp(s.played,0,999999);s.wins=clamp(s.wins,0,s.played);
    for(int i=0;i<4;i++) s.best[i]=clamp(s.best[i],0,INT_MAX);
    a->save=s;
}
bool save_write(App *a) {
    if (a->test_mode)
        return true;
    char tmp[1100];
    snprintf(tmp, sizeof(tmp), "%s.tmp", a->save_path);
#ifdef _WIN32
    FILE *f = utf8_open(tmp, L"w");
#else
    FILE *f = fopen(tmp, "w");
#endif
    if (!f)
        return false;
    Save *s = &a->save;
    int n=fprintf(f,"369COZY_V9\n%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                  s->coins,s->owned,s->skin,s->music,s->sfx,s->motion,s->fullscreen,s->played,s->wins,
                  s->best[0],s->best[1],s->best[2],s->best[3],s->legacy,s->language,s->bg_owned,
                  s->background,s->last_claim_day,s->attendance_streak,s->tutorial_seen_mask);
    if (n>=0) n=fprintf(f,"%d\n",s->mistakes.count);
    for (int i=0;n>=0 && i<s->mistakes.count;i++) {
        MistakeNote *m=&s->mistakes.note[i];
        n=fprintf(f,"%d %d %d %d %d %d\n",m->kind,m->left,m->right,m->answer,m->wrong,m->reviewed);
    }
    int err = fclose(f);
    if (n < 0 || err)
        return false;
#ifdef _WIN32
    wchar_t from[1200], to[1200];
    if (!MultiByteToWideChar(CP_UTF8, 0, tmp, -1, from, 1200) ||
        !MultiByteToWideChar(CP_UTF8, 0, a->save_path, -1, to, 1200))
        return false;
    return MoveFileExW(from, to, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(tmp, a->save_path) == 0;
#endif
}
