#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lsmtree.h"

#define MAX_CHARS_IN 2048

// LSM tree parameters
#define CAPACITY 100

int exec_workload(lsmtree *tree, char *workload);
int exec_query(lsmtree *tree, char* query);