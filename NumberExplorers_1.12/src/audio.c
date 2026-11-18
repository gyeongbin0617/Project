#include "app.h"
#include <math.h>
#include <string.h>
#define TAU 6.283185307179586

static void callback(void *data, Uint8 *stream, int len) {
    App *a = data;
    float *out = (float *)stream;
    int count = len / (int)sizeof(float);
    static const double notes[] = {261.63, 329.63, 392.00, 329.63, 293.66, 392.00, 440.00, 392.00,
                                   329.63, 261.63, 293.66, 329.63, 392.00, 329.63, 293.66, 0};
    for (int i = 0; i < count; i++) {
        double t = (double)a->audio_samples / 48000.0;
        int step = (int)(t / 0.38) % 16;
        double local = fmod(t, 0.38);
        double env = exp(-local * 9.0) * fmin(local * 100, 1.0);
        double freq = notes[step];
        a->music_phase += TAU * freq / 48000.0;
        if (a->music_phase > TAU)
            a->music_phase -= TAU;
        float sample = (float)(sin(a->music_phase) * env * 0.075 * (a->save.music / 100.0));
        if (a->sfx_left > 0) {
            a->sfx_phase += TAU * a->sfx_frequency / 48000.0;
            if (a->sfx_phase > TAU)
                a->sfx_phase -= TAU;
            sample += (float)(sin(a->sfx_phase) * fmin(a->sfx_left * 20, 1) * 0.18 *
                              (a->save.sfx / 100.0));
            a->sfx_left -= 1.0f / 48000.0f;
        }
        out[i] = sample;
        a->audio_samples++;
    }
}
void audio_init(App *a) {
    SDL_AudioSpec spec = {0};
    spec.freq = 48000;
    spec.format = AUDIO_F32SYS;
    spec.channels = 1;
    spec.samples = 1024;
    spec.callback = callback;
    spec.userdata = a;
    a->audio = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    if (a->audio)
        SDL_PauseAudioDevice(a->audio, 0);
}
void audio_cue(App *a, int cue) {
    if (!a->audio)
        return;
    SDL_LockAudioDevice(a->audio);
    a->sfx_frequency = cue == 1 ? 740 : cue == 2 ? 190 : cue == 3 ? 1000 : 460;
    a->sfx_left = cue == 2 ? 0.18f : 0.085f;
    SDL_UnlockAudioDevice(a->audio);
}
