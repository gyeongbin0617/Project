#include "scene_internal.h"
#include <stdio.h>
void scene_shop(App *a) {
    if (scene_back_header(a,"SHOP",LOBBY)) return;
    if (ui_button(a,"Characters",365,115,260,48,a->shop_category==0?CORAL:CREAM,true)) {a->shop_category=0;return;}
    if (ui_button(a,"Backgrounds",655,115,260,48,a->shop_category==1?MINT:CREAM,true)) {a->shop_category=1;return;}
    static const int skin_prices[]={0,100,160,220,60,80,100,120};
    static const int bg_prices[]={0,80,120,160};
    static const char *bg_names[]={"Cozy room","Spring picnic","Sunset beach","Starry night"};
    for (int slot=0;slot<4;slot++) {
        bool bg=a->shop_category==1;
        int i=bg?slot:slot+a->shop_tab*4, x=42+slot*304;
        int price=bg?bg_prices[i]:skin_prices[i];
        bool owned=((bg?a->save.bg_owned:a->save.owned)&(1<<i))!=0;
        bool selected=(bg?a->save.background:a->save.skin)==i;
        ui_panel(a,x,185,284,439,CREAM);
        if (bg) {
            ui_background(a,i,(SDL_Rect){x+16,215,252,175});
            ui_text(a,"Preview",x+142,402,16,MUTED,true);
        } else {
            ui_round(a,x+30,204,224,224,105,i%2?LAVENDER:MINT);
            ui_avatar(a,i,x+32,207,220,selected,true);
        }
        ui_text(a,bg?bg_names[i]:scene_friend_name(i),x+142,442,25,PLUM,true);
        char cost[64];snprintf(cost,sizeof(cost),owned?L(a,"Owned"):L(a,"%d coins"),price);
        ui_text(a,cost,x+142,487,18,MUTED,true);
        const char *label=selected?"EQUIPPED":owned?"Apply":"Buy & apply";
        if (ui_button(a,label,x+22,547,240,53,selected?LAVENDER:MINT,!selected)) {
            if (!owned && a->save.coins<price) {app_message(a,"Not enough coins");return;}
            Save before=a->save;
            if (!owned) a->save.coins-=price;
            if (bg) {a->save.bg_owned|=1<<i;a->save.background=i;}
            else {a->save.owned|=1<<i;a->save.skin=i;}
            if (!save_write(a)) {a->save=before;app_message(a,"Could not save your purchase.");}
            else app_message(a,"Equipped");
            return;
        }
    }
    if (a->shop_category==0) {
        if (ui_button(a,"1 / 2",295,653,320,45,a->shop_tab==0?CORAL:CREAM,true)) a->shop_tab=0;
        if (ui_button(a,"2 / 2",665,653,320,45,a->shop_tab==1?MINT:CREAM,true)) a->shop_tab=1;
    } else ui_text(a,"Your background appears throughout the game",640,656,20,MUTED,true);
}
void scene_settings(App *a) {
    if (scene_back_header(a, "SETTINGS", a->settings_back))
        return;
    ui_panel(a, 220, 126, 840, 529, CREAM);
    if (a->reset_confirm) {
        ui_text(a, "Reset settings?", 640, 268, 34, PLUM, true);
        if (ui_button(a, "CANCEL", 340, 380, 600, 64, MINT, true))
            a->reset_confirm = false;
        if (ui_button(a, "RESET", 340, 468, 600, 58, CORAL, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            Save before=a->save;
            app_reset_settings(&a->save);
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            SDL_SetWindowFullscreen(a->window, 0);
            if(!save_write(a)) {a->save=before;SDL_SetWindowFullscreen(a->window,a->save.fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0);app_message(a,"Could not save progress. Check folder access.");return;}
            a->reset_confirm = false;
            app_message(a, "Reset complete");
        }
        return;
    }
    for (int i = 0; i < 2; i++) {
        int y = 158 + i * 73;
        ui_text(a, i == 0 ? "Music" : "Sound effects", 260, y + 11, 23, PLUM, false);
        int *v = i == 0 ? &a->save.music : &a->save.sfx;
        if (ui_button(a, "-", 731, y, 56, 47, CREAM, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            *v = *v >= 10 ? *v - 10 : 0;
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            save_write(a);
        }
        char value[30];
        snprintf(value, sizeof(value), "%d%%", *v);
        ui_text(a, value, 863, y + 10, 23, PLUM, true);
        if (ui_button(a, "+", 945, y, 56, 47, CREAM, true)) {
            if (a->audio)
                SDL_LockAudioDevice(a->audio);
            *v = *v <= 90 ? *v + 10 : 100;
            if (a->audio)
                SDL_UnlockAudioDevice(a->audio);
            save_write(a);
        }
    }
    ui_text(a, "Fullscreen", 260, 318, 23, PLUM, false);
    if (ui_button(a, a->save.fullscreen ? "ON  /  F11" : "OFF  /  F11", 731, 305, 270, 48, LAVENDER,
                  true)) {
        a->save.fullscreen = !a->save.fullscreen;
        SDL_SetWindowFullscreen(a->window, a->save.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        save_write(a);
    }
    ui_text(a, "Motion", 260, 391, 23, PLUM, false);
    if (ui_button(a, a->save.motion ? "ON" : "REDUCED", 731, 378, 270, 48, MINT, true)) {
        a->save.motion = !a->save.motion;
        save_write(a);
    }
    ui_text(a, "Language / 언어", 260, 464, 23, PLUM, false);
    if (ui_button(a, a->save.language ? "한국어 >" : "English >", 731, 451, 270, 48, LAVENDER,
                  true)) {
        a->save.language = !a->save.language;
        if (!save_write(a))
            app_message(a, "Could not save progress. Check folder access.");
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
