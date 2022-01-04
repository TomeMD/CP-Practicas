#ifndef __OPTIONS_H__
#define __OPTIONS_H__

struct options {
    int num_threads;
    int size;
    int queue_size;
};

int read_options(int argc, char **argv, struct options *opt);


#endif
