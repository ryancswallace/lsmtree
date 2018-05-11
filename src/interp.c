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

	// interactive or text file mode
	char interactive = '0';
	while(!(interactive == 'i' || interactive == 'f')) {
		printf("Interactive [i] or write to file [f] mode: ");
		interactive = fgetc(stdin);
		fgetc(stdin); // eat newline
	}

	char dir_name[MAX_DIR_LEN] = DATA_DIR;
	if(interactive == 'f') {
		// redirect stdout to file
		strcat(dir_name, tree->name);
		strcat(dir_name, "/log.txt");

		freopen(dir_name, "a+", stdout);
	}

	// begin to process queries
	printf("\nType quit at any time to end session.\n\n");

	char *command = malloc(LINE_BUFF_SIZE);
	while(1) {
		if(interactive == 'i') {
			printf(">>> ");
		} 
		
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

	printf("\nSession ended.\n");
}

int exec_query(lsmtree *tree, char* q) {
	// parses and executes query on the LSM tree
	// don't modify query argument
	char query[LINE_BUFF_SIZE];
	strcpy(query, q);

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
		VAL_TYPE *val = get(tree, key);
		if (val) {
			// if val found, not NULL
			printf("%d", *val);
			free(val);
		}
		printf("\n");
	}
	else if(query_type == 'r') {
		// parse
		char *pair = malloc(LINE_BUFF_SIZE);
		strncpy(pair, query+2, strlen(query));
		KEY_TYPE key_start = atoi(strtok(pair, " "));
		KEY_TYPE key_stop = atoi(strtok(NULL, " "));

		// query
		buffer *buff = range(tree, key_start, key_stop);

		// print pairs
		for (int i = 0; i < *buff->size; i++) {
			if (!buff->dels[i]) {
				printf("%d:%d", buff->keys[i], buff->vals[i]);
			}
		}
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
	char *saveptr;
	query = strtok_r(workload, "\n", &saveptr);

	// loop through each line
	while (query != NULL) {
		exec_query(tree, query);
		query = strtok_r(NULL, "\n", &saveptr);
	}

	// clean up
	free(workload);

	return 0;
}
