#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct thr_arg {
  int x;
};

 

void *g(void *ptr) {
  struct thr_arg *arg=ptr;

  arg->x = 20;

  return NULL;
}

 

void e4() {
  pthread_t thr;
  struct thr_arg arg;

  pthread_create(&thr, NULL, g, &arg);
  pthread_join(thr, NULL);

  printf("%d\n", arg.x);
}

int main() {
	e4();
	return 0;
}
