#ifndef INTERP_H
#define INTERP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lsmtree.h"

#define MAX_CHARS_IN 2048

int exec_workload(lsmtree *tree, char *workload);
int exec_query(lsmtree *tree, char* query);

#endif
