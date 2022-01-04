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

void e8 () {
  int pid;
  int *x=NULL;

  pid = fork();

  if(pid==0) {
    x = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    *x= 1;

    exit(0);

  } else {
    waitpid(pid, NULL, 0);
    printf("%d\n", *x);
  }
}

int main() {
	e8();
	return 0;
}
