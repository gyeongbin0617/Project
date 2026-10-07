#include "app.h"
#include <limits.h>
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
    a->save = (Save){.coins = 100,
                     .owned = 1,
                     .skin = 0,
                     .music = 50,
                     .sfx = 70,
                     .motion = 1,
                     .legacy = 1,
                     .language = 1};
#ifdef _WIN32
    FILE *f = utf8_open(a->save_path, L"r");
#else
    FILE *f = fopen(a->save_path, "r");
#endif
    if (!f)
        return;
    char magic[32];
    Save s = {0};
    int n = fscanf(f, "%31s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", magic, &s.coins,
                   &s.owned, &s.skin, &s.music, &s.sfx, &s.motion, &s.fullscreen, &s.played,
                   &s.wins, &s.best[0], &s.best[1], &s.best[2], &s.best[3], &s.legacy, &s.language);
    fclose(f);
    if (!((n == 15 && !strcmp(magic, "369COZY_V1")) || (n == 16 && !strcmp(magic, "369COZY_V2"))))
        return;
    s.language = clamp(s.language, 0, 1);
    s.coins = clamp(s.coins, 0, 999999);
    s.owned = (s.owned & 255) | 1;
    s.skin = clamp(s.skin, 0, 7);
    if (!(s.owned & (1 << s.skin)))
        s.skin = 0;
    s.music = clamp(s.music, 0, 100);
    s.sfx = clamp(s.sfx, 0, 100);
    s.motion = !!s.motion;
    s.fullscreen = !!s.fullscreen;
    s.legacy = !!s.legacy;
    s.played = clamp(s.played, 0, 999999);
    s.wins = clamp(s.wins, 0, s.played);
    for (int i = 0; i < 4; i++)
        s.best[i] = clamp(s.best[i], 0, INT_MAX);
    a->save = s;
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
    int n =
        fprintf(f, "369COZY_V2\n%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", s->coins, s->owned,
                s->skin, s->music, s->sfx, s->motion, s->fullscreen, s->played, s->wins, s->best[0],
                s->best[1], s->best[2], s->best[3], s->legacy, s->language);
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
