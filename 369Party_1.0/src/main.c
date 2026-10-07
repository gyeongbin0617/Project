#include "app.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    App *a = calloc(1, sizeof(*a));
    if (!a)
        return 1;
    if (!app_init(a, false)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "369 Party", SDL_GetError(), NULL);
        app_destroy(a);
        free(a);
        return 1;
    }
    uint64_t last = SDL_GetPerformanceCounter();
    double frequency = (double)SDL_GetPerformanceFrequency();
    while (a->running) {
        uint64_t now = SDL_GetPerformanceCounter();
        double dt = (now - last) / frequency;
        last = now;
        if (dt > 0.1)
            dt = 0.1;
        SDL_Event e;
        while (SDL_PollEvent(&e))
            app_event(a, &e);
        app_update(a, dt);
        app_render(a);
        SDL_RenderPresent(a->renderer);
        SDL_Delay(8);
    }
    if (!save_write(a))
        fprintf(stderr, "Could not save progress to %s\n", a->save_path);
    app_destroy(a);
    free(a);
    return 0;
}
