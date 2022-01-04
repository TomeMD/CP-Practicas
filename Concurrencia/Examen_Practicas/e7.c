#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct thr_arg {
  int *x;
};

 

void *f(void *ptr) {
  struct thr_arg *arg = ptr;

  *arg->x = 20;

  return arg->x;

}

 

void e7() {
  pthread_t thr;
  struct thr_arg arg;
  int i=0;
  int *res;

  arg.x=&i;

  pthread_create(&thr, NULL, f, &arg);
  pthread_join(thr, (void**)&res);

  printf("%d\n", *res);

}

int main() {
	e7();
	return 0;
}
