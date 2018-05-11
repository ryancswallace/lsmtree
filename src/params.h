#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

// LSM tree parameters
typedef int32_t KEY_TYPE; 
typedef int32_t VAL_TYPE;
#define MIN_KEY -2147483648
#define MAX_KEY 2147483647

#define BUFF_CAPACITY 2
#define RATIO 3

#define M_FENCES 1 // pages per fence pointer
#define M_BLOOM 3 // hash functions per bloom filter
#define P_1 0.05 // target false positive rate for level 1
#define RUN_MIN 1 // minimum run length for bloom filter

#define MAX_NUM_LEVELS 64

// interpreter parameters
#define MAX_DIR_LEN 4096
#define LOAD_NUM_PAIRS 10000 // pairs read from file at a time for load

// user config parameters
#define PAGE_NUM_BYTES 4096
#define DATA_DIR "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/data/"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_big.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_sm.txt"

#endif
