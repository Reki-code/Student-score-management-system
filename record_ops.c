#include "record_ops.h"
#include <stdlib.h>

cJSON *new_student_with_score(const char *id_num, const char *class_name,
                              const char *score_num) {
  cJSON *new_student = cJSON_CreateObject();
  cJSON *id = cJSON_CreateString(id_num);
  cJSON_AddItemToObject(new_student, "id", id);
  cJSON *score = cJSON_CreateObject();
  int score_int = strtol(score_num, NULL, 10);
  cJSON_AddItemToObject(score, class_name, cJSON_CreateNumber(score_int));
  cJSON_AddItemToObject(new_student, "score", score);
  return new_student;
}

bool record_add_student(cJSON *record, const char *id_num,
                        const char *class_name, const char *score_num) {
  cJSON *student = new_student_with_score(id_num, class_name, score_num);
  return cJSON_AddItemToArray(record, student);
}

bool student_add_score(cJSON *student, const char *class_name,
                       const char *score_num) {
  cJSON *score = cJSON_GetObjectItem(student, "score");
  int score_int = strtol(score_num, NULL, 10);
  return cJSON_AddItemToObject(score, class_name,
                               cJSON_CreateNumber(score_int));
}

bool score_add_class(cJSON *score, const char *class_name,
                     const char *score_num) {
  int score_num_int = strtol(score_num, NULL, 10);
  cJSON *score_ = cJSON_CreateNumber(score_num_int);
  return cJSON_AddItemToObject(score, class_name, score_);
}
