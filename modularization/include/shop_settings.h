#ifndef SHOP_SETTINGS_H
#define SHOP_SETTINGS_H

#include <stdbool.h>
#include <SDL.h>
#include "app_types.h"

/* 상점 슬롯 아이템 명세 구조체 (포인터 전달용) */
typedef struct {
    bool is_background;
    int item_index;
    int price;
    const char *display_name;
} ShopItemSpec;

/* 상점 로직 인터페이스 (shop_logic.c 구현) */
void shop_build_item_spec(const App *a, int slot, ShopItemSpec *out_spec);
bool shop_is_item_owned(const Save *s, const ShopItemSpec *spec);
bool shop_is_item_equipped(const Save *s, const ShopItemSpec *spec);
void shop_try_purchase_or_equip(App *a, const ShopItemSpec *spec);

/* 설정 로직 인터페이스 (settings_logic.c 구현) */
void settings_toggle_fullscreen(SDL_Window *window, Save *s, App *a);
void settings_adjust_audio(SDL_AudioDeviceID audio, int *target_volume, int delta, App *a);
void scene_settings(App *a);

/* 세이브 보조 인터페이스 (save.c 구현) */
void save_init_defaults(Save *s);
void save_sanitize(Save *s);
bool save_write_file(const char *path, const Save *s);

#endif /* SHOP_SETTINGS_H */