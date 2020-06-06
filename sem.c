#include "sem.h"

void Sem_init(sem_t *sem, int pshared, unsigned int value) {
  if (sem_init(sem, pshared, value) < 0) {
    perror("sem init");
  }
}

void P(sem_t *sem) {
  if (sem_wait(sem) < 0) {
    perror("P sem wait");
  }
}

void V(sem_t *sem) {
  if (sem_post(sem) < 0) {
    perror("V sem post");
  }
}
