#include "scene_internal.h"
#include "shop_settings.h"
#include <stdio.h>

void settings_toggle_fullscreen(SDL_Window *window, Save *s, App *a) {
    if (!s) return;
    s->fullscreen = !s->fullscreen;
    SDL_SetWindowFullscreen(window, s->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    save_write(a);
}

void settings_adjust_audio(SDL_AudioDeviceID audio, int *target_volume, int delta, App *a) {
    if (!target_volume) return;

    if (audio) SDL_LockAudioDevice(audio);
    int next = *target_volume + delta;
    if (next < 0) next = 0;
    if (next > 100) next = 100;
    *target_volume = next;
    if (audio) SDL_UnlockAudioDevice(audio);

    save_write(a);
}

/* [내부 캡슐화] 설정 초기화 확인 팝업 화면 처리 */
static void render_reset_confirm_dialog(App *a) {
    ui_text(a, "Reset settings?", 640, 268, 34, PLUM, true);

    if (ui_button(a, "CANCEL", 340, 380, 600, 64, MINT, true)) {
        a->reset_confirm = false;
    }
    if (ui_button(a, "RESET", 340, 468, 600, 58, CORAL, true)) {
        if (a->audio) SDL_LockAudioDevice(a->audio);
        Save before = a->save;
        app_reset_settings(&a->save);
        if (a->audio) SDL_UnlockAudioDevice(a->audio);

        SDL_SetWindowFullscreen(a->window, 0);
        if (!save_write(a)) {
            a->save = before;
            SDL_SetWindowFullscreen(a->window, a->save.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
            app_message(a, "Could not save progress. Check folder access.");
            return;
        }
        a->reset_confirm = false;
        app_message(a, "Reset complete");
    }
}

/* [내부 캡슐화] 배경음(BGM) 및 효과음(SFX) 음량 슬롯 렌더링 */
static void render_volume_row(App *a, const char *label, int *volume_ptr, int row_y) {
    ui_text(a, label, 260, row_y + 11, 23, PLUM, false);

    if (ui_button(a, "-", 731, row_y, 56, 47, CREAM, true)) {
        settings_adjust_audio(a->audio, volume_ptr, -10, a);
    }

    char value[30];
    snprintf(value, sizeof(value), "%d%%", *volume_ptr);
    ui_text(a, value, 863, row_y + 10, 23, PLUM, true);

    if (ui_button(a, "+", 945, row_y, 56, 47, CREAM, true)) {
        settings_adjust_audio(a->audio, volume_ptr, 10, a);
    }
}

void scene_settings(App *a) {
    if (!a) return;
    if (scene_back_header(a, "SETTINGS", a->settings_back)) return;

    const SDL_Rect panel = {220, 126, 840, 529};
    ui_panel(a, panel.x, panel.y, panel.w, panel.h, CREAM);

    if (a->reset_confirm) {
        render_reset_confirm_dialog(a);
        return;
    }

    render_volume_row(a, "Music", &a->save.music, 158);
    render_volume_row(a, "Sound effects", &a->save.sfx, 231);

    ui_text(a, "Fullscreen", 260, 318, 23, PLUM, false);
    if (ui_button(a, a->save.fullscreen ? "ON  /  F11" : "OFF  /  F11",
                  731, 305, 270, 48, LAVENDER, true)) {
        settings_toggle_fullscreen(a->window, &a->save, a);
    }

    ui_text(a, "Motion", 260, 391, 23, PLUM, false);
    if (ui_button(a, a->save.motion ? "ON" : "REDUCED",
                  731, 378, 270, 48, MINT, true)) {
        a->save.motion = !a->save.motion;
        save_write(a);
    }

    ui_text(a, "Language / 언어", 260, 464, 23, PLUM, false);
    if (ui_button(a, a->save.language ? "한국어 >" : "English >",
                  731, 451, 270, 48, LAVENDER, true)) {
        a->save.language = !a->save.language;
        if (!save_write(a)) {
            app_message(a, "Could not save progress. Check folder access.");
        }
    }

    if (ui_button(a, "RESET SETTINGS", 260, 570, 334, 54, CREAM, true)) {
        a->reset_confirm = true;
        a->focus = 0;
    }
    if (ui_button(a, "DONE", 670, 570, 331, 54, CORAL, true)) {
        save_write(a);
        app_go(a, a->settings_back);
    }
}