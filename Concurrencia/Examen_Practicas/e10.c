#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ftw.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <signal.h>

void e10(int n) {
  int *sum = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  int pid[n];

  *sum=0;

  for(int i=0; i<n; i++) {
    if((pid[i]=fork()) == 0) {
      *sum = *sum + 1;
      exit(0);
    }
  }


  printf("%d\n", *sum);
}

int main() {
	e10(2);
	return 0;
}
