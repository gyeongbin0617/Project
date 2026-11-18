#include "scene_internal.h"
static const char *friends[]={"BUNNY","MISO","BIBI","BOMO","RUBY","PANDA","FROGGY","KOALA"};
const char *scene_friend_name(int index) {return friends[index];}
bool scene_back_header(App *a,const char *title,Scene back) {
    if (!ui_header(a,title)) return false;
    app_go(a,back);return true;
}
