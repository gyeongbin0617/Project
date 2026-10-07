#include "app.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
const SDL_Color PLUM = {68, 48, 66, 255}, CREAM = {255, 252, 244, 255}, MUTED = {109, 96, 106, 255},
                CORAL = {239, 159, 137, 255}, MINT = {180, 213, 190, 255},
                LAVENDER = {214, 198, 229, 255};
static void color(App *a, SDL_Color c) { SDL_SetRenderDrawColor(a->renderer, c.r, c.g, c.b, c.a); }
void ui_round(App *a, int x, int y, int w, int h, int r, SDL_Color c) {
    if (w <= 0 || h <= 0)
        return;
    if (r > w / 2)
        r = w / 2;
    if (r > h / 2)
        r = h / 2;
    color(a, c);
    SDL_Rect mid = {x + r, y, w - 2 * r, h};
    SDL_RenderFillRect(a->renderer, &mid);
    for (int j = 0; j < h; j++) {
        int d = j < r ? r - j - 1 : j >= h - r ? j - (h - r) : 0;
        int inset = (j < r || j >= h - r) ? r - (int)sqrt((double)r * r - d * d) : 0;
        SDL_RenderDrawLine(a->renderer, x + inset, y + j, x + r - 1, y + j);
        SDL_RenderDrawLine(a->renderer, x + w - r, y + j, x + w - inset - 1, y + j);
    }
}
void ui_panel(App *a, int x, int y, int w, int h, SDL_Color c) {
    ui_round(a, x + 1, y + 7, w, h, 24, (SDL_Color){68, 43, 66, 24});
    ui_round(a, x, y, w, h, 24, (SDL_Color){214, 201, 195, 255});
    ui_round(a, x + 2, y + 2, w - 4, h - 4, 22, c);
}
void ui_text(App *a, const char *s, int x, int y, int size, SDL_Color c, bool center) {
    if (!s || !*s)
        return;
    s = L(a, s);
    if (size < 10)
        size = 10;
    if (size > 160)
        size = 160;
    int korean = a->save.language || !strcmp(s, "Language / 언어");
    int fontkey = size + 161 * korean;
    TextCache *entry = NULL;
    for (int i = 0; i < 256; i++)
        if (a->cache[i].texture && a->cache[i].size == fontkey && !strcmp(a->cache[i].text, s)) {
            entry = &a->cache[i];
            break;
        }
    if (!entry) {
        if (!a->fonts[fontkey]) {
            char path[1200];
            snprintf(path, sizeof(path), "%sAssets/%s.ttf", a->base,
                     korean       ? "Korean"
                     : size >= 24 ? "Heading"
                                  : "Body");
            a->fonts[fontkey] = TTF_OpenFont(path, size);
            if (!a->fonts[fontkey])
                return;
        }
        SDL_Surface *surface =
            TTF_RenderUTF8_Blended(a->fonts[fontkey], s, (SDL_Color){255, 255, 255, 255});
        if (!surface)
            return;
        entry = &a->cache[a->cache_next++ % 256];
        if (entry->texture)
            SDL_DestroyTexture(entry->texture);
        entry->texture = SDL_CreateTextureFromSurface(a->renderer, surface);
        entry->size = fontkey;
        entry->w = surface->w;
        entry->h = surface->h;
        snprintf(entry->text, sizeof(entry->text), "%s", s);
        SDL_FreeSurface(surface);
    }
    if (!entry->texture)
        return;
    SDL_SetTextureColorMod(entry->texture, c.r, c.g, c.b);
    SDL_SetTextureAlphaMod(entry->texture, c.a);
    SDL_Rect dst = {center ? x - entry->w / 2 : x, y, entry->w, entry->h};
    SDL_RenderCopy(a->renderer, entry->texture, NULL, &dst);
}
bool ui_button(App *a, const char *s, int x, int y, int w, int h, SDL_Color c, bool enabled) {
    int id = a->button_count++;
    bool hovered = a->mx >= x && a->mx < x + w && a->my >= y && a->my < y + h;
    if (hovered && a->moved)
        a->focus = id;
    bool focus = a->focus == id,
         press = enabled && ((hovered && a->click) || (focus && a->activate));
    if (!enabled)
        c = (SDL_Color){226, 218, 213, 255};
    if (enabled && (hovered || focus)) {
        c.r = (Uint8)fmin(c.r + 9, 255);
        c.g = (Uint8)fmin(c.g + 9, 255);
        c.b = (Uint8)fmin(c.b + 9, 255);
    }
    ui_round(a, x, y + 4, w, h, 17, (SDL_Color){66, 39, 66, 32});
    ui_round(a, x, y, w, h, 17,
             enabled && (hovered || focus) ? PLUM : (SDL_Color){195, 178, 181, 255});
    ui_round(a, x + 2, y + 2, w - 4, h - 4, 15, c);
    if (focus && enabled) {
        SDL_Color ring = PLUM;
        ui_round(a, x + 8, y + h - 7, w - 16, 3, 1, ring);
    }
    int size = h >= 65 ? 24 : 20;
    if (w < 120)
        size = 16;
    if (h >= 70 && s[0] && strspn(s, "0123456789") == strlen(s))
        size = 40;
    ui_text(a, s, x + w / 2, y + (h - (int)(size * 1.25)) / 2, size, enabled ? PLUM : MUTED, true);
    if (press) {
        a->click = false;
        a->activate = false;
        audio_cue(a, 0);
    }
    return press;
}
void ui_avatar(App *a, int index, int x, int y, int size, bool active, bool alive) {
    SDL_Texture *texture = index >= 4 ? a->new_friends : a->friends;
    static const SDL_Rect portraits[] = {
        {144, 2, 421, 637}, {659, 79, 459, 558},  {133, 649, 437, 594}, {649, 659, 485, 585},
        {119, 7, 485, 674}, {658, 111, 527, 571}, {144, 715, 458, 521}, {610, 688, 633, 548}};
    static const int face_width[] = {360, 410, 395, 410, 430, 460, 430, 420};
    SDL_Rect src = portraits[index];
    int bob = active && a->save.motion ? (int)(sin(a->clock * 5) * 4) : 0;
    if (active)
        ui_round(a, x + size / 8, y + size * 3 / 4, size * 3 / 4, size / 5, size / 10,
                 (SDL_Color){239, 145, 117, 150});
    SDL_SetTextureAlphaMod(texture, alive ? 255 : 95);
    double tilt = 0;
    if (active && a->save.motion && a->scene == MATCH) {
        if (a->game.phase == FEEDBACK) {
            if (a->game.success)
                bob -= (int)(fabs(sin(a->clock * 11)) * 9);
            else
                tilt = sin(a->clock * 30) * 4;
        } else if (a->game.current == 0 && a->game.last_clap >= 0 &&
                   a->game.now - a->game.last_clap < 0.2)
            bob -= 7;
    }
    double scale = 0.63 * size / face_width[index];
    double max_scale = 1.10 * size / src.h;
    if (scale > max_scale)
        scale = max_scale;
    int w = (int)(src.w * scale), h = (int)(src.h * scale);
    SDL_Rect dst = {x + (size - w) / 2, y + bob + (int)(0.98 * size) - h, w, h};
    SDL_RenderCopyEx(a->renderer, texture, &src, &dst, tilt, NULL, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(texture, 255);
}

bool ui_header(App *a, const char *title) {
    bool back = ui_button(a, "< BACK", 40, 30, 112, 48, CREAM, true);
    ui_text(a, title, 184, 30, 30, PLUM, false);
    char coins[48];
    snprintf(coins, sizeof(coins), L(a, "%d coins"), a->save.coins);
    ui_round(a, 1060, 30, 176, 48, 20, CREAM);
    ui_text(a, coins, 1148, 40, 18, PLUM, true);
    return back;
}
