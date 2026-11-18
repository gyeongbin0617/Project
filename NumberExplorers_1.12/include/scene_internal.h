#ifndef NUMBER_SCENE_INTERNAL_H
#define NUMBER_SCENE_INTERNAL_H
#include "app.h"
bool scene_back_header(App *a,const char *title,Scene back);
const char *scene_friend_name(int index);
void scene_title(App *a);
void scene_lobby(App *a);
void scene_single(App *a);
void scene_tutorial(App *a);
void scene_setup(App *a);
void scene_shop(App *a);
void scene_settings(App *a);
void scene_match(App *a);
void scene_result(App *a);
#endif
