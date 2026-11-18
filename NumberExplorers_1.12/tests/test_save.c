#include "app.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void){
 App *a=calloc(1,sizeof(*a));assert(a);snprintf(a->save_path,sizeof(a->save_path),"../qa_numberplay/save_roundtrip.txt");
 FILE*f=fopen(a->save_path,"w");assert(f);fputs("369COZY_V2\n345 31 4 50 70 1 0 10 2 9 8 7 6 1 1\n",f);fclose(f);
 save_load(a);assert(a->save.coins==345&&a->save.skin==4&&a->save.bg_owned==1&&a->save.background==0);
 a->save.bg_owned=9;a->save.background=3;assert(save_write(a));memset(&a->save,0,sizeof(a->save));save_load(a);
 assert(a->save.bg_owned==9&&a->save.background==3&&a->save.coins==345&&a->save.skin==4);
 a->save.bg_owned=1;a->save.background=3;assert(save_write(a));save_load(a);assert(a->save.background==0);
 remove(a->save_path);free(a);puts("PASS: old saves, background persistence, unowned background fallback");return 0;
}
