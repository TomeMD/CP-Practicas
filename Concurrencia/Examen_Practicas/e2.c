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
	sleep(3);
  printf("%f\n", arg->y);
  return NULL;
}

void e2() {
  struct thr_arg *arg=malloc(sizeof(struct thr_arg));
  pthread_t thr;

  arg->x=20; arg->y=1.0;

  pthread_create(&thr, NULL, f, arg);

  free(arg);
     printf("Hola\n");
  sleep(10);

}

int main() {
	e2();
	return 0;
}
