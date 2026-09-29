#ifndef PARTY_APP_H
#define PARTY_APP_H
#define SDL_MAIN_HANDLED
#include "game.h"
#include "i18n.h"
#include "learn.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#define L(a, s) tr((a)->save.language, (s))
#define WIDTH 1280
#define HEIGHT 720
typedef enum {
    TITLE,
    LOBBY,
    SINGLE,
    SETUP,
    MULTI,
    SHOP,
    SETTINGS,
    MATCH,
    RESULT,
    LEARN_SETUP,
    LEARN
} Scene;
typedef struct {
    int coins, owned, skin, music, sfx, motion, fullscreen, played, wins, best[4], legacy, language;
} Save;
typedef struct {
    SDL_Texture *texture;
    char text[512];
    int size, w, h;
} TextCache;
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *room, *friends, *new_friends;
    TTF_Font *fonts[322];
    TextCache cache[256];
    int cache_next;
    Learn lesson;
    int learn_kind, learn_level, learn_operation, learn_table;
    bool show_table, help_open;
    int help_page, help_focus;
    Save save;
    Game game;
    Scene scene, settings_back;
    Mode chosen;
    bool running, click, activate, moved, paused, quit_confirm, reset_confirm, recorded, test_mode;
    int mx, my, focus, button_count, previous_buttons, players, difficulty, input, shop_tab, reward;
    double clock, toast_end;
    char toast[128];
    char base[1024], save_path[1024];
    SDL_AudioDeviceID audio;
    double music_phase, sfx_phase;
    uint64_t audio_samples;
    float sfx_left, sfx_frequency;
} App;
extern const SDL_Color PLUM, CREAM, MUTED, CORAL, MINT, LAVENDER;
bool app_init(App *a, bool headless);
void app_destroy(App *a);
void app_event(App *a, SDL_Event *e);
void app_update(App *a, double dt);
void app_render(App *a);
void app_go(App *a, Scene s);
void app_start(App *a);
void app_finish(App *a);
void app_message(App *a, const char *s);
void app_game_key(App *a, SDL_Keycode key);
void save_load(App *a);
bool save_write(App *a);
void audio_init(App *a);
void audio_cue(App *a, int cue);

bool ui_header(App *a, const char *title);
void ui_text(App *a, const char *s, int x, int y, int size, SDL_Color c, bool center);
void ui_panel(App *a, int x, int y, int w, int h, SDL_Color c);
void ui_round(App *a, int x, int y, int w, int h, int r, SDL_Color c);
bool ui_button(App *a, const char *s, int x, int y, int w, int h, SDL_Color c, bool enabled);
void ui_avatar(App *a, int index, int x, int y, int size, bool active, bool alive);
void render_scene(App *a);
void help_toggle(App *a);
void help_render(App *a);
void learn_render(App *a);
void learn_begin(App *a);
void learn_choose(App *a, int choice);
void learn_advance(App *a);
#endif
