#include "app.h"
#include "scene_internal.h"
#include "shop_settings.h"

void shop_build_item_spec(const App *a, int slot, ShopItemSpec *out_spec) {
    static const int k_skin_prices[] = {0, 100, 160, 220, 60, 80, 100, 120};
    static const int k_bg_prices[]   = {0, 80, 120, 160};
    static const char *k_bg_names[]  = {"Cozy room", "Spring picnic", "Sunset beach", "Starry night"};

    if (!a || !out_spec) return;

    bool is_bg = (a->shop_category == 1);
    int idx = is_bg ? slot : (slot + a->shop_tab * 4);

    out_spec->is_background = is_bg;
    out_spec->item_index = idx;
    out_spec->price = is_bg ? k_bg_prices[idx] : k_skin_prices[idx];
    out_spec->display_name = is_bg ? k_bg_names[idx] : scene_friend_name(idx);
}

bool shop_is_item_owned(const Save *s, const ShopItemSpec *spec) {
    if (!s || !spec) return false;
    int mask = spec->is_background ? s->bg_owned : s->owned;
    return (mask & (1 << spec->item_index)) != 0;
}

bool shop_is_item_equipped(const Save *s, const ShopItemSpec *spec) {
    if (!s || !spec) return false;
    int active = spec->is_background ? s->background : s->skin;
    return active == spec->item_index;
}

/* [내부 캡슐화] Save 구조체 포인터에 해금 및 장착 상태 반영 */
static void apply_item_unlock_and_equip(Save *s, const ShopItemSpec *spec) {
    if (spec->is_background) {
        s->bg_owned |= (1 << spec->item_index);
        s->background = spec->item_index;
    } else {
        s->owned |= (1 << spec->item_index);
        s->skin = spec->item_index;
    }
}

void shop_try_purchase_or_equip(App *a, const ShopItemSpec *spec) {
    if (!a || !spec) return;

    bool owned = shop_is_item_owned(&a->save, spec);
    if (!owned && a->save.coins < spec->price) {
        app_message(a, "Not enough coins");
        return;
    }

    Save snapshot_before = a->save;
    if (!owned) {
        a->save.coins -= spec->price;
    }
    apply_item_unlock_and_equip(&a->save, spec);

    if (!save_write(a)) {
        a->save = snapshot_before;
        app_message(a, "Could not save your purchase.");
    } else {
        app_message(a, "Equipped");
    }
}