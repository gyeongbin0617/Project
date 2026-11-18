#ifndef PARTY_APP_H
#define PARTY_APP_H
#define SDL_MAIN_HANDLED
#include "game.h"
#include "i18n.h"
#include "learn.h"
#include "mistakes.h"
#include "puzzles.h"
#include "train.h"
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
    TUTORIAL,
    SETUP,
    SHOP,
    SETTINGS,
    MATCH,
    RESULT,
    LEARN_SETUP,
    LEARN,
    PUZZLE,
    REVIEW,
    TRAIN
} Scene;
typedef struct {
    int coins, owned, skin, music, sfx, motion, fullscreen, played, wins, best[4], legacy, language, bg_owned, background;
    int last_claim_day, attendance_streak, tutorial_seen_mask;
    MistakeBook mistakes;
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
    Train train;
    Hanoi hanoi;
    Count31 count31;
    DotGame dots;
    int puzzle_kind, mode_page, tutorial_mode, train_level;
    int review_index, review_choice, review_last;
    bool review_feedback;
    int learn_kind, learn_level, learn_operation, learn_table;
    bool show_table, help_open;
    int help_page, help_focus;
    Save save;
    Game game;
    Scene scene, settings_back;
    Mode chosen;
    bool running, click, activate, moved, paused, quit_confirm, reset_confirm, recorded, test_mode;
    int mx, my, focus, button_count, previous_buttons, players, difficulty, input, shop_tab, shop_category, reward;
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
bool app_pay_entry(App *a, int mode);
int app_bot_skin(const App *a, int player);
int attendance_claim(Save *s, int today, int yesterday);
void app_reset_settings(Save *s);
void app_message(App *a, const char *s);
void app_game_key(App *a, SDL_Keycode key);
void app_note_feedback(App *a);
void review_render(App *a);
void review_answer(App *a, int choice);
void train_begin(App *a);
void train_answer(App *a, int choice);
void train_advance(App *a);
void train_render(App *a);
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
void ui_background(App *a, int theme, SDL_Rect area);
void help_toggle(App *a);
void help_render(App *a);
void learn_render(App *a);
void learn_begin(App *a);
void learn_choose(App *a, int choice);
void learn_advance(App *a);
void puzzle_begin(App *a, int kind);
void puzzle_render(App *a);
void puzzle_update(App *a);
bool puzzle_key(App *a, SDL_Keycode key);
void puzzle_help(App *a);
#endif
