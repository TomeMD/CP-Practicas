#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int n = 5;
int res[5]; // All initialized to 0

 

void *f(void *ptr) {
   int *pos = ptr;

   res[10]=1;
   return NULL;

}

 

void e8(int n) {
  pthread_t thr[n];

  int i;

  for(i=0; i<n; i++) {
    pthread_create(&thr[i], NULL, f, &n);
  }

  for(int j=0; j<n; j++) {
    pthread_join(thr[j], NULL);
    printf("%d\n", res[j]);
  }
}

int main() {
	e8(n);
	return 0;
}
