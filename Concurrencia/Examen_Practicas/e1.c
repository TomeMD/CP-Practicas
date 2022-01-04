#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct thr_arg {
  int x;
  float y;
};

void *f(void *ptr) {
  struct thr_arg *arg=ptr;

  printf("%d\n", arg->x);
  return NULL;
}

void e1() {
  struct thr_arg arg;
  pthread_t thr;

  pthread_create(&thr, NULL, f, &arg);

  arg.x=0; arg.y=1.0;

  pthread_join(thr, NULL);
}

int main() {
	e1();
	return 0;
}
