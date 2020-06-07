#ifndef _RECORD_OPS_
#define _RECORD_OPS_
#include "cJSON.h"
#include <stdbool.h>

cJSON *new_score_item(const char *class_name, const char *score_num);
cJSON *new_student_with_score(const char *id_num, const char *class_name, const char *score_num);
bool record_add_student(cJSON *record, const char *id_num, const char *class_name, const char *score_num);
bool student_add_score(cJSON *student, const char *class_name, const char *score_num);
bool score_add_class(cJSON *score, const char *class_name, const char *score_num);

#endif
