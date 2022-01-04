#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "options.h"
#include "queue.h"

struct thread_info {
	pthread_t       thread_id;        // id returned by pthread_create()
	int             thread_num;       // application defined thread #
};

struct args {
	int 		thread_num;       // application defined thread #
	int		iterations;
	queue queue;
};

void* productor(void* ptr) {
	struct args* args = ptr;
	int *i = malloc(args->iterations*sizeof(int));
	if (i == NULL) {
		printf("Error: Not enough memory");
		exit(1);
	}
	
	while (args->iterations--) {
		i[args->iterations] = rand()%rand();
		printf("Productor %d (iteración %d) insertando\n", args->thread_num, 9 - args->iterations);
		q_insert(args->queue, &i[args->iterations]);
		printf("Productor %d (iteración %d) elemento insertado\n\n", args->thread_num, 9 - args->iterations);
	}
	
	free(i);
	return NULL;
}

void* consumidor(void* ptr) {
	struct args* args = ptr;
	
	while (args->iterations--) {
		printf("Consumidor %d (iteración %d) eliminando\n", args->thread_num, 9 - args->iterations);
		q_remove(args->queue);
		printf("Consumidor %d (iteración %d) elemento eliminado\n\n", args->thread_num, 9 - args->iterations);
	}
	return NULL;
}


void start_threads(struct options opt)
{
	int i;
	struct thread_info *threads;
	struct args *args;
	queue queue = q_create(opt.queue_size);
	void *thread_function;

	srand(time(NULL));

	printf("creating %d threads\n", opt.num_threads);
	threads = malloc(sizeof(struct thread_info) * opt.num_threads);
	args = malloc(sizeof(struct args) * opt.num_threads);

	if (threads == NULL || args==NULL) {
		printf("Not enough memory\n");
		exit(1);
	}		

	// Create num_thread threads running swap() 
	for (i = 0; i < opt.num_threads; i++) {
		threads[i].thread_num = i;
		
		args[i].thread_num = i;
		args[i].queue    =  queue;
		args[i].iterations = 10;
		
		
		if (!(i%2))
			thread_function = productor;
		else
			thread_function = consumidor;

		if ( 0 != pthread_create(&threads[i].thread_id, NULL,
					 thread_function, &args[i])) {
			printf("Could not create thread #%d", i);
			exit(1);
		}
	}
	
	// Wait for the threads to finish
	for (i = 0; i < opt.num_threads; i++) {
		pthread_join(threads[i].thread_id, NULL);
	}
	
	
	free(args);
	free(threads);
	q_destroy(queue);
	pthread_exit(NULL);
}

int main (int argc, char **argv)
{
	struct options opt;
	
	// Default values for the options
	opt.num_threads = 10;
	opt.queue_size = 10;

	
	read_options(argc, argv, &opt);

	start_threads(opt);

	exit (0);
}

