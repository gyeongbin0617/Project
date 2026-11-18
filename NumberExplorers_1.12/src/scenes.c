#include "scene_internal.h"
void render_scene(App *a) {
    switch (a->scene) {
    case TITLE:
        scene_title(a);
        break;
    case LOBBY:
        scene_lobby(a);
        break;
    case SINGLE:
        scene_single(a);
        break;
    case TUTORIAL:
        scene_tutorial(a);
        break;
    case SETUP:
        scene_setup(a);
        break;
    case SHOP:
        scene_shop(a);
        break;
    case SETTINGS:
        scene_settings(a);
        break;
    case MATCH:
        scene_match(a);
        break;
    case RESULT:
        scene_result(a);
        break;
    case LEARN_SETUP:
    case LEARN:
        learn_render(a);
        break;
    case PUZZLE:
        puzzle_render(a);
        break;
    case REVIEW:
        review_render(a);
        break;
    case TRAIN:
        train_render(a);
        break;
    }
}

