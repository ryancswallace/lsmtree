#include "execute.h"

int main(void) {
	// initialize LSM tree
	lsmtree *tree = init_lsmtree();

	// read in LSM tree or create new one
	char mode = '0';
	while(!(mode == 'e' || mode == 'n')) {
		printf("\nNew [n] or existing [e] LSM tree: ");
		mode = fgetc(stdin);
		fgetc(stdin); // eat newline
	}

	if(mode == 'e') {
		char *tree_path = malloc(MAX_CHARS_IN);
		printf("Enter full path to existing LSM tree: ");
		fgets(tree_path, MAX_CHARS_IN, stdin);
		tree_path[strlen(tree_path) - 1] = 0;

		int load_failed = load_lsmtree(tree);
		if(load_failed) {
			printf("Failed to load LSM tree.\n");
			return 1;
		}
	}
	else {
		empty_lsmtree(tree, CAPACITY);
	}

	// begin to process queries
	printf("\nType quit at any time to end session.\n\n");

	char *command = malloc(MAX_CHARS_IN);
	while(1) {
		printf(">>> ");
		fgets(command, MAX_CHARS_IN, stdin);
		command[strlen(command) - 1] = 0;

		if (strcmp(command, "quit") == 0) {
			// end session
			serialize_lsmtree(tree);
			break;
		}
		else {
			if(exec_query(tree, command)) {
				printf("\n\nQuery %s failed.\n", command);
				return 1;
			}
		}
	}
	free(command);
	free_lsmtree(tree);

	printf("\nSession ended.\n");
}

int exec_query(lsmtree *tree, char* query) {
	// parses and executes query on the LSM tree

	char query_type = query[0];

	if(query_type == 'p') {
		// parse
		char *pair = malloc(MAX_CHARS_IN);
		strncpy(pair, query+2, strlen(query));
		KEY_TYPE key = atoi(strtok(pair, " "));
		VAL_TYPE val = atoi(strtok(NULL, " "));

		// query
		put(tree, key, val);
	}	
	else if(query_type == 'g') {
		// parse
		KEY_TYPE key = atoi(query+2);

		// query
		VAL_TYPE val = get(tree, key);
		printf("%d\n", val);
	}
	else if(query_type == 'r') {
		// parse
		char *pair = malloc(MAX_CHARS_IN);
		strncpy(pair, query+2, strlen(query));
		KEY_TYPE key_start = atoi(strtok(pair, " "));
		KEY_TYPE key_stop = atoi(strtok(NULL, " "));

		// query
		KEY_TYPE *pairs = range(tree, key_start, key_stop);

		// TODO: print pairs
	}	
	else if(query_type == 'd') {
		// parse
		KEY_TYPE key = atoi(query+2);

		// query
		delete(tree, key);
	}
	else if(query_type == 's') {
		// query
		print_stats(tree);
	}
	else if(query_type == 'l') {
		// parse
		char *filepath = malloc(MAX_CHARS_IN);
		strncpy(filepath, query+3, strlen(query));
		filepath[strlen(filepath) - 1] = 0;

		// query
		load(tree, filepath);

		free(filepath);
	}	
	else if(query_type == 'w') { // a full workload file
		// parse
		char *filepath = malloc(MAX_CHARS_IN);
		strncpy(filepath, query+3, strlen(query));
		filepath[strlen(filepath) - 1] = 0;

		// query
		exec_workload(tree, filepath);

		free(filepath);
	}
	else {
	    printf("Invalid query.\n");
	}	

	return 0;
}

int exec_workload(lsmtree *tree, char *filepath) {
	// reads and executes workload file 
	// "/Users/RyanWallace/Desktop/All/School/Harvard/S S2/CS 265/project/generator/generator/workload.txt"
	char *workload = NULL;
	long len;
	FILE *f = fopen(filepath, "r");

	if (f) {
		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fseek(f, 0, SEEK_SET);
		workload = malloc(len);
		if (workload) {
			fread(workload, 1, len, f);
		}
		fclose (f);
	}
	else {
		printf("Workload file not found.\n");
	}

	char *query;
	query = strtok(workload, "\n");

	// loop through each line
	while (query != NULL) {
		exec_query(tree, query);

		query = strtok(NULL, "\n");
	}

	// clean up
	free(workload);

	return 0;
}
