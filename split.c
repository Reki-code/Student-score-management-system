#include "split.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
size_t split(char *buffer, char *argv[], size_t argv_size) {
  char *p, *start_of_word;
  int c;
  enum states { DULL, IN_WORD, IN_STRING } state = DULL;
  size_t argc = 0;

  for (p = buffer; argc < argv_size && *p != '\0'; p++) {
    c = (unsigned char)*p;
    switch (state) {
    case DULL:
      if (isspace(c)) {
        continue;
      }

      if (c == '"') {
        state = IN_STRING;
        start_of_word = p + 1;
        continue;
      }
      state = IN_WORD;
      start_of_word = p;
      continue;

    case IN_STRING:
      if (c == '"') {
        *p = 0;
        argv[argc++] = start_of_word;
        state = DULL;
      }
      continue;

    case IN_WORD:
      if (isspace(c)) {
        *p = 0;
        argv[argc++] = start_of_word;
        state = DULL;
      }
      continue;
    }
  }

  if (state != DULL && argc < argv_size)
    argv[argc++] = start_of_word;

  return argc;
}
void test_split(const char *s) {
  char buf[1024];
  size_t i, argc;
  char *argv[20];

  strcpy(buf, s);
  argc = split(buf, argv, 20);
  printf("input: '%s'\n", s);
  for (i = 0; i < argc; i++)
    printf("[%lu] '%s'\n", i, argv[i]);
}
/* int main(int ac, char *av[]) */
/* { */
/*     test_split("\"some text in quotes\" plus four simple words p\"lus
 * something strange\""); */
/*     return 0; */
/* } */
