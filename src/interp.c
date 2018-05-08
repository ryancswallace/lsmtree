#include "interp.h"

int main(void) {
	// initialize LSM tree
	lsmtree *tree = new_lsmtree();

	// read in LSM tree or create new one
	char mode = '0';
	while(!(mode == 'e' || mode == 'n')) {
		printf("\nNew [n] or existing [e] LSM tree: ");
		mode = fgetc(stdin);
		fgetc(stdin); // eat newline
	}

	if(mode == 'e') {
		char *tree_path = malloc(LINE_BUFF_SIZE);
		printf("Enter full path to existing LSM tree: ");
		fgets(tree_path, LINE_BUFF_SIZE, stdin);
		tree_path[strlen(tree_path) - 1] = 0;

		int load_failed = load_lsmtree(tree);
		if(load_failed) {
			printf("Failed to load LSM tree.\n");
			return 1;
		}
	}
	else {
		char *name = malloc(LINE_BUFF_SIZE);
		printf("Enter name for new LSM tree: ");
		fgets(name, LINE_BUFF_SIZE, stdin);
		name[strlen(name) - 1] = 0;

		int failed = empty_lsmtree(tree, name);
		if(failed) {
			return 1;
		}
	}

	// begin to process queries
	printf("\nType quit at any time to end session.\n\n");

	char *command = malloc(LINE_BUFF_SIZE);
	while(1) {
		printf(">>> ");
		fgets(command, LINE_BUFF_SIZE, stdin);
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
		char *pair = malloc(LINE_BUFF_SIZE);
		strncpy(pair, query+2, strlen(query));
		KEY_TYPE key = atoi(strtok(pair, " "));
		VAL_TYPE val = atoi(strtok(NULL, " "));

		// query
		put(tree, key, val, false);
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
		char *pair = malloc(LINE_BUFF_SIZE);
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
		char *filepath = malloc(LINE_BUFF_SIZE);
		strncpy(filepath, query+3, strlen(query));
		filepath[strlen(filepath) - 1] = 0;

		// query
		load(tree, filepath);

		free(filepath);
	}	
	else if(query_type == 'w') { // a full workload file
		// parse
		char *filepath = malloc(LINE_BUFF_SIZE);
		strncpy(filepath, query+3, strlen(query));
		filepath[strlen(filepath) - 1] = 0;

		// query
		int failed = exec_workload(tree, filepath);
		if(failed) {
			return 1;
		}

		free(filepath);
	}
	else {
	    printf("Invalid query.\n");
	}	

	return 0;
}

int exec_workload(lsmtree *tree, char *filepath) {
	// reads and executes workload file. reads chunks of lines to minimize 
	// both memory overhead and I/O. workload must end in trailing newline
	char *chunk = malloc(WORKLOAD_BUFF_SIZE);
	char *curr_query = NULL;
	char *next_query = NULL;
	long offset;
	long num_bytes_read = WORKLOAD_BUFF_SIZE;
	bool redo_line = false;

	FILE *f = fopen(filepath, "r");
	if (f) {
		while(num_bytes_read == WORKLOAD_BUFF_SIZE || redo_line) {
			redo_line = false;

			num_bytes_read = fread(chunk, 1, WORKLOAD_BUFF_SIZE, f);

			// loop through each line
			offset = ftell(f);
			curr_query = strtok(chunk, "\n");
			if (curr_query != NULL) {
				next_query = strtok(NULL, "\n");
			}
			while (next_query != NULL) {
				exec_query(tree, curr_query);

				curr_query = next_query;
				next_query = strtok(NULL, "\n");

				offset += strlen(curr_query) + 1;
			}
			if(num_bytes_read != WORKLOAD_BUFF_SIZE) {
				// reached end of file
				exec_query(tree, curr_query);

				break;
			}
			else {
				// not at end of file
				// back up file pointer to previous 
				fseek(f, offset, SEEK_SET);
				redo_line = true;
			}
		}
		
		fclose (f);
	}
	else {
		printf("Workload file not found.\n");
	}

	free(chunk);

	return 0;
}
