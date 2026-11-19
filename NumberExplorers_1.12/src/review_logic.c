#include "app.h"
#include "review_internal.h"

/* Check a selected answer and update review progress. */

void review_answer(App *a, int choice) {
    if (a->scene!=REVIEW || a->review_feedback || choice<0 || choice>=3 || a->review_index<0) return;
    MistakeNote *n=&a->save.mistakes.note[a->review_index];
    int options[3];review_choices(n,options);
    int correct=options[choice]==n->answer;
    mistake_review(&a->save.mistakes,a->review_index,correct);
    a->review_feedback=correct;
    a->review_last=choice;
    if (!save_write(a)) app_message(a,"Could not save progress. Check folder access.");
    if (!correct) app_message(a,"Try again");
    audio_cue(a,correct?3:2);
}
