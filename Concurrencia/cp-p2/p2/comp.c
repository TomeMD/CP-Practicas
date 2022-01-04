#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "compress.h"
#include "chunk_archive.h"
#include "queue.h"
#include "options.h"

#define CHUNK_SIZE (1024*1024)
#define QUEUE_SIZE 20

#define COMPRESS 1
#define DECOMPRESS 0

struct args {
	queue in;
	queue out;
	chunk (*process) (chunk);
};

struct argsLector {
	queue in;
	int chunks;
	int fd;
	int size;
};

struct argsEscritor {
	queue out;
	int chunks;
	archive ar;
};

struct argsLectorD {
	queue in;
	archive ar;
};

struct argsEscritorD {
	queue out;
	int fd;
	archive ar;
};

// read input file and send chunks to the in queue
void * lector(void * ptr) {
	struct argsLector* argsLector = ptr;
	chunk ch;
	int i, offset;
	
    for(i=0; i<argsLector->chunks; i++) {
        ch = alloc_chunk(argsLector->size);

        offset=lseek(argsLector->fd, 0, SEEK_CUR);

        ch->size   = read(argsLector->fd, ch->data, argsLector->size);
        ch->num    = i;
        ch->offset = offset;

        q_insert(argsLector->in, ch);
    }
    return NULL;
}

// send chunks to the output archive file
void * escritor(void * ptr) {
	struct argsEscritor* argsEscritor = ptr;
	chunk ch;
	int i;
	
    for(i=0; i<argsEscritor->chunks; i++) {
        ch = q_remove(argsEscritor->out);

        add_chunk(argsEscritor->ar, ch);
        free_chunk(ch);
    }
    return NULL;
}

// read chunks with compressed data
void * lectorD(void * ptr) {
	struct argsLectorD* argsLectorD = ptr;
	chunk ch;
	int i;
	
    for(i=0; i<chunks(argsLectorD->ar); i++) {
        ch = get_chunk(argsLectorD->ar, i);
        q_insert(argsLectorD->in, ch);
    }
    return NULL;
}

// write chunks from output to decompressed file
void * escritorD(void * ptr) {
	struct argsEscritorD* argsEscritorD = ptr;
	chunk ch;
	int i;
	
    for(i=0; i<chunks(argsEscritorD->ar); i++) {
        ch=q_remove(argsEscritorD->out);
        lseek(argsEscritorD->fd, ch->offset, SEEK_SET);
        write(argsEscritorD->fd, ch->data, ch->size);
        free_chunk(ch);
    }
    return NULL;
}

// take chunks from queue in, run them through process (compress or decompress), send them to queue out
void worker(queue in, queue out, chunk (*process)(chunk)) {
    chunk ch, res;
    while(q_elements(in)>0) {
        ch = q_remove(in);

        res = process(ch);
        free_chunk(ch);

        q_insert(out, res);
    }
}


void * work(void * ptr) {
	struct args* args = ptr;
	worker(args->in, args->out, args->process);
	return NULL;
}

// Compress file taking chunks of opt.size from the input file,
// inserting them into the in queue, running them using a worker,
// and sending the output from the out queue into the archive file
void comp(struct options opt) {
    int fd, chunks, i;
    struct stat st;
    char comp_file[256];
    archive ar;
    queue in, out;
    pthread_t *threads;
    pthread_t threadLector, threadEscritor;
    struct args args;
    struct argsLector argsLector;
    struct argsEscritor argsEscritor;

    if((fd=open(opt.file, O_RDONLY))==-1) {
        printf("Cannot open %s\n", opt.file);
        exit(0);
    }

    fstat(fd, &st);
    chunks = st.st_size/opt.size+(st.st_size % opt.size ? 1:0);

    if(opt.out_file) {
        strncpy(comp_file,opt.out_file,255);
    } else {
        strncpy(comp_file, opt.file, 255);
        strncat(comp_file, ".ch", 255);
    }

    ar = create_archive_file(comp_file);

    in  = q_create(opt.queue_size);
    out = q_create(opt.queue_size);

	argsLector.in = in;
	argsLector.chunks = chunks;
	argsLector.fd = fd;
	argsLector.size = opt.size;
    
    if ( 0 != pthread_create(&threadLector, NULL, lector, &argsLector)) {
		printf("Could not create reader thread");
		exit(1);
	}
	
	
    threads = malloc(opt.num_threads*sizeof(pthread_t));
	
	if (threads == NULL) {
		printf("Not enough memory\n");
		exit(1);
	}
	
	
	args.in = in;
	args.out = out;
	args.process = zcompress;
	
	for (i = 0; i < opt.num_threads; i++) {
		if ( 0 != pthread_create(&threads[i], NULL, work, &args)) {
			printf("Could not create thread #%d", i);
			exit(1);
		}
	}
	
	argsEscritor.out = out;
	argsEscritor.chunks = chunks;
	argsEscritor.ar = ar;
    
    if ( 0 != pthread_create(&threadEscritor, NULL, escritor, &argsEscritor)) {
			printf("Could not create writer thread");
			exit(1);
	}
	
	// Wait for the reader thread
	pthread_join(threadLector, NULL);
	
	// Wait for the threads to finish
	for (i = 0; i < opt.num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// Wait for the writer thread
	pthread_join(threadEscritor, NULL);

    

    close_archive_file(ar);
    close(fd);
    free(threads);
    q_destroy(in);
    q_destroy(out);
}


// Decompress file taking chunks of opt.size from the input file,
// inserting them into the in queue, running them using a worker,
// and sending the output from the out queue into the decompressed file
void decomp(struct options opt) {
    int fd, i;
    //struct stat st;
    char uncomp_file[256];
    archive ar;
    queue in, out;
    pthread_t threadLectorD, threadEscritorD;
    pthread_t *threads;
    struct args args;
    struct argsLectorD argsLectorD;
    struct argsEscritorD argsEscritorD;

    if((ar=open_archive_file(opt.file))==NULL) {
        printf("Cannot open archive file\n");
        exit(0);
    };

    if(opt.out_file) {
        strncpy(uncomp_file, opt.out_file, 255);
    } else {
        strncpy(uncomp_file, opt.file, strlen(opt.file) -3);
        uncomp_file[strlen(opt.file)-3] = '\0';
    }

    if((fd=open(uncomp_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))== -1) {
        printf("Cannot create %s: %s\n", uncomp_file, strerror(errno));
        exit(0);
    }

    in  = q_create(opt.queue_size);
    out = q_create(opt.queue_size);

	argsLectorD.in = in;
	argsLectorD.ar = ar;
    
    if ( 0 != pthread_create(&threadLectorD, NULL, lectorD, &argsLectorD)) {
			printf("Could not create reader thread");
			exit(1);
	}
    
    threads = malloc(opt.num_threads*sizeof(pthread_t));
	
	if (threads == NULL) {
		printf("Not enough memory\n");
		exit(1);
	}
	
	args.in = in;
	args.out = out;
	args.process = zdecompress;
	
	for (i = 0; i < opt.num_threads; i++) {
		if ( 0 != pthread_create(&threads[i], NULL, work, &args)) {
			printf("Could not create thread #%d", i);
			exit(1);
		}
	}
	
	argsEscritorD.out = out;
	argsEscritorD.fd = fd;
	argsEscritorD.ar = ar;
    
    if ( 0 != pthread_create(&threadEscritorD, NULL, escritorD, &argsEscritorD)) {
			printf("Could not create writer thread");
			exit(1);
	}
	
	// Wait for the reader thread
	pthread_join(threadLectorD, NULL);
	
	// Wait for the threads to finish
	for (i = 0; i < opt.num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// Wait for the writer thread
	pthread_join(threadEscritorD, NULL);

    close_archive_file(ar);
    close(fd);
    free(threads);
    q_destroy(in);
    q_destroy(out);
}

int main(int argc, char *argv[]) {
    struct options opt;

    opt.compress    = COMPRESS;
    opt.num_threads = 3;
    opt.size        = CHUNK_SIZE;
    opt.queue_size  = QUEUE_SIZE;
    opt.out_file    = NULL;

    read_options(argc, argv, &opt);

    if(opt.compress == COMPRESS) comp(opt);
    else decomp(opt);
}
