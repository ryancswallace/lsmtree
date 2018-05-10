#ifndef PARAMS_H
#define PARAMS_H

#include <stdint.h>

// LSM tree parameters
typedef int32_t KEY_TYPE; 
typedef int32_t VAL_TYPE;
#define MAX_KEY 2147483648

#define BUFF_CAPACITY 2
#define RATIO 3

#define LOAD_NUM_PAIRS 10000 // pairs read from file at a time for load
#define MAX_NUM_LEVELS 64

// interpreter parameters
#define MAX_DIR_LEN 4096

// user parameters
#define DATA_DIR "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/data/"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_big.txt"
// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload_sm.txt"

#endif