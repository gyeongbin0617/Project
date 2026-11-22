#include "app.h"

/* Release application and SDL resources. */

void app_destroy(App *a) {
    if (a->audio)
        SDL_CloseAudioDevice(a->audio);
    for (int i = 0; i < 256; i++)
        if (a->cache[i].texture)
            SDL_DestroyTexture(a->cache[i].texture);
    for (int i = 0; i < 322; i++)
        if (a->fonts[i])
            TTF_CloseFont(a->fonts[i]);
    if (a->room)
        SDL_DestroyTexture(a->room);
    if (a->friends)
        SDL_DestroyTexture(a->friends);
    if (a->new_friends)
        SDL_DestroyTexture(a->new_friends);
    if (a->renderer)
        SDL_DestroyRenderer(a->renderer);
    if (a->window)
        SDL_DestroyWindow(a->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
