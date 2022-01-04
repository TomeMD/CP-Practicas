#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int sum=0;
pthread_mutex_t m;

struct thr_arg {
  pthread_mutex_t *m;
};

void *h(void *ptr) {
  struct thr_arg *arg=ptr;

  pthread_mutex_lock(arg->m);
  sum = sum + 1;
  pthread_mutex_unlock(arg->m);

  return NULL;
}

void e6(int n) {
  struct thr_arg arg[n];
  pthread_t thr[n];

  pthread_mutex_init(&m, NULL);

  for(int i=0; i<n; i++) {
    arg[i].m = &m;

    pthread_create(&thr[i], NULL, h, &arg[i]);
  }

  for(int i=0; i<n; i++)
    pthread_join(thr[i], NULL);

  printf("%d\n", sum);
}


int main() {
	e6(10);
	return 0;
}
