#include "key_index.h"
#include <stdio.h>
#include <stdlib.h>

static void print_key_index(void *ki) {
  key_index_t *kii = ki;
  printf("%d: %d\n", kii->key, kii->index);
}

int key_index_compar(const void *a, const void *b) {
  key_index_t *kia = (key_index_t *)a;
  key_index_t *kib = (key_index_t *)b;
  return -(kia->key - kib->key);
}

void key_index_qsort(key_index_t *ki, int nmemb) {
  qsort(ki, nmemb, sizeof(key_index_t *), key_index_compar);
}
