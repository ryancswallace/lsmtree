#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>
#include <stdbool.h>

// LSM tree parameters
typedef int32_t KEY_TYPE; 
typedef int32_t VAL_TYPE;
typedef bool DEL_TYPE;
#define MIN_KEY -2147483648
#define MAX_KEY 2147483647

#define RATIO 3

#define BUFF_PLUS_BF_CAPACITY 100000000 // in bytes
#define BUFF_CAPACITY 2 // in pairs

#define NUM_HASHES 3 // hash functions per bloom filter
#define RUN_MIN 1 // minimum run length for bloom filter
#define P_1 0.05 // target false positive rate for level 1

#define M_FENCES 1 // disk pages per fence pointer

// interpreter parameters
#define MAX_DIR_LEN 4096
#define LOAD_NUM_PAIRS 10000 // pairs read from file at a time for load

// user config parameters
#define PAGE_NUM_BYTES 4096
#define MAX_NUM_LEVELS 64
#define DATA_DIR "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/data/"
#endif

// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_big.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_sm.txt"
