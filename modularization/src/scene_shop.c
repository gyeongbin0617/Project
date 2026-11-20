#include "scene_internal.h"
#include "shop_settings.h"
#include <stdio.h>

/* [내부 캡슐화] 개별 상점 슬롯 카드 렌더링 및 구매/장착 버튼 처리 */
static bool render_shop_slot(App *a, int slot, const SDL_Rect *card_rect) {
    ShopItemSpec spec;
    shop_build_item_spec(a, slot, &spec);

    bool owned = shop_is_item_owned(&a->save, &spec);
    bool selected = shop_is_item_equipped(&a->save, &spec);

    ui_panel(a, card_rect->x, card_rect->y, card_rect->w, card_rect->h, CREAM);

    if (spec.is_background) {
        const SDL_Rect preview = {card_rect->x + 16, 215, 252, 175};
        ui_background(a, spec.item_index, preview);
        ui_text(a, "Preview", card_rect->x + 142, 402, 16, MUTED, true);
    } else {
        ui_round(a, card_rect->x + 30, 204, 224, 224, 105,
                 spec.item_index % 2 ? LAVENDER : MINT);
        ui_avatar(a, spec.item_index, card_rect->x + 32, 207, 220, selected, true);
    }

    ui_text(a, spec.display_name, card_rect->x + 142, 442, 25, PLUM, true);

    char cost[64];
    snprintf(cost, sizeof(cost), owned ? L(a, "Owned") : L(a, "%d coins"), spec.price);
    ui_text(a, cost, card_rect->x + 142, 487, 18, MUTED, true);

    const char *label = selected ? "EQUIPPED" : owned ? "Apply" : "Buy & apply";
    const SDL_Rect action_btn = {card_rect->x + 22, 547, 240, 53};
    if (ui_button(a, label, action_btn.x, action_btn.y,
                  action_btn.w, action_btn.h, selected ? LAVENDER : MINT, !selected)) {
        shop_try_purchase_or_equip(a, &spec);
        return true;
    }
    return false;
}

void scene_shop(App *a) {
    if (!a) return;
    if (scene_back_header(a, "SHOP", LOBBY)) return;

    const SDL_Rect tab_char = {365, 115, 260, 48};
    const SDL_Rect tab_bg   = {655, 115, 260, 48};

    if (ui_button(a, "Characters", tab_char.x, tab_char.y, tab_char.w, tab_char.h,
                  a->shop_category == 0 ? CORAL : CREAM, true)) {
        a->shop_category = 0;
        return;
    }
    if (ui_button(a, "Backgrounds", tab_bg.x, tab_bg.y, tab_bg.w, tab_bg.h,
                  a->shop_category == 1 ? MINT : CREAM, true)) {
        a->shop_category = 1;
        return;
    }

    for (int slot = 0; slot < 4; slot++) {
        const SDL_Rect card_rect = {42 + slot * 304, 185, 284, 439};
        if (render_shop_slot(a, slot, &card_rect)) {
            return;
        }
    }

    if (a->shop_category == 0) {
        if (ui_button(a, "1 / 2", 295, 653, 320, 45, a->shop_tab == 0 ? CORAL : CREAM, true)) {
            a->shop_tab = 0;
        }
        if (ui_button(a, "2 / 2", 665, 653, 320, 45, a->shop_tab == 1 ? MINT : CREAM, true)) {
            a->shop_tab = 1;
        }
    } else {
        ui_text(a, "Your background appears throughout the game", 640, 656, 20, MUTED, true);
    }
}