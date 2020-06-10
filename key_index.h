#ifndef _KEY_INDEX_H_
#define _KEY_INDEX_H_

typedef struct {
  int key;
  int index;
} key_index_t;

int key_index_compar(const void *a, const void *b);

void key_index_qsort(key_index_t *ki, int nmemb);

#endif
