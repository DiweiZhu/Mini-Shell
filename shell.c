// Modify this file for your assignment
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

// CONSTANTS
// The buffer size is 82 for input with length 80, because we do not count the \n and there will always be a null \0 at the end.
int BUFFER_SIZE = 82;
int TOKEN_SIZE = 24;
char COMMAND_ROOT[] = "/bin/";

int NUMBER_OF_BUILTIN_FUNCS = 4;
char* BUILTIN_FUNCS[] = {"cd", "exit", "help", "alias"};
char* ENVP[] = {"PATH=/bin", NULL};

char** ALIAS_KEY;
char** ALIAS_VALUE;
int ALIAS_COUNT = 0;

void free_constants() {
	int i = 0;

        for(; i < ALIAS_COUNT; i ++) {
                free(ALIAS_KEY[i]);
                free(ALIAS_VALUE[i]);
        }
        free(ALIAS_KEY);
        free(ALIAS_VALUE);
	
	return;
}
void sigint_handler(int sig) {
	free_constants();
	write(1, "\nTerminating through signal handler\n", 36);
	exit(0);
}

void init_shell() {
	system("clear");
	printf("******WELCOME TO MY SHELL*******\n");

	return;
}

void print_prompt() {
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("Mini-shell:%s> ", cwd);
}

int get_input(char cmdInput[]) {
	fgets(cmdInput, BUFFER_SIZE, stdin);

	if (strchr(cmdInput, '\n') == NULL) {
		int ch;
		int extra_data_found = 0;
		while ((ch = fgetc(stdin)) != '\n' && ch != EOF) {
			extra_data_found += 1;
		}
		printf("Extra data found: %d\n", extra_data_found);
		return 1;
	}

	return 0;
}

int builtin_help(char* cmd[], int size) {
	printf("cd [dir]\n");
	printf("help\n");
	printf("exit\n");
	printf("alias [name=value ...]\n");
	return 0;
}

void tokens_to_cmd(char* cmd[], int size, char cmdInput[]) {
	int i = 0;
	for (;i < size; i ++) {
		strcat(cmdInput, cmd[i]);
		if (i != size - 1) {
			strcat(cmdInput, " ");
		}
	}
	
	return;
}

int builtin_alias(char* cmd[], int size) {
	// check if there is no parameter
	if (size == 1) {
		int i = 0;
		for (; i < ALIAS_COUNT; i ++) {
			printf("%s='%s'\n", ALIAS_KEY[i], ALIAS_VALUE[i]);
		}
		return 0;
	}

	// first of all we need to make the tokens one sentence
	char cmdInput[BUFFER_SIZE];
	strcpy(cmdInput, "");
	tokens_to_cmd(cmd, size, cmdInput);

	// then parse the sentence using '
	char* aliasCmd[24];
	char* tok;
	tok = strtok(cmdInput, "'");
	
	int sizeAlias = 0;
	while (tok != NULL) {
		aliasCmd[sizeAlias] = tok;
		sizeAlias ++;
		tok = strtok(NULL, "'");
	}

	// get key and value
	int i = 0;
	if (i + 2 > sizeAlias) {
		return 0;
	}

	char* key = (char*)malloc(sizeof(char*));
	char item[BUFFER_SIZE];
	strcpy(item, aliasCmd[0]);
	tok = strtok(item, " =");
	tok = strtok(NULL, " =");
	strcpy(key, tok);
	
	char* value = (char*)malloc(sizeof(char*));
	strcpy(value, aliasCmd[1]);

	ALIAS_KEY[ALIAS_COUNT] = key;
	ALIAS_VALUE[ALIAS_COUNT] = value;
	ALIAS_COUNT ++;

	for (i = 2; i < sizeAlias; i += 2) {
		if (i + 2 > sizeAlias) {
			break;
		}
		strcpy(item, aliasCmd[i]);
		tok = strtok(item, " =");
		ALIAS_KEY[ALIAS_COUNT] = (char*)malloc(sizeof(char*));
		strcpy(ALIAS_KEY[ALIAS_COUNT], tok);

		ALIAS_VALUE[ALIAS_COUNT] = (char*)malloc(sizeof(char*));
		strcpy(ALIAS_VALUE[ALIAS_COUNT], aliasCmd[i + 1]);
		ALIAS_COUNT ++;
	}

	return 0;
} 

int builtin_cd(char* cmd[], int size) {
	if (size == 1) {
		if (chdir(getenv("HOME")) == -1) {
			printf("error cd to root\n");
			return -1;
		}
	}
	else {
		if (chdir(cmd[1]) == -1) {
			printf("error cd to %s\n", cmd[1]);
			return -1;
		}
	}
	
	return 0;
}

int builtin_exit(char* cmd[], int size) {
	free_constants();
	printf("\nGood-Bye~\n");
	return 0;
}
	
// return 0 if the given parsed commands is one of the builtin functions
// return 1 if not
// return -1 if error happens, which means the command is a builtin function but with illegal parameters like cd .. abc
int try_builtin_func(char* cmd[], int size) {

	int builtin_func = 0;
	int i = 0;
	for (i = 0; i < NUMBER_OF_BUILTIN_FUNCS; i ++) {
		if (strcmp(cmd[0], BUILTIN_FUNCS[i]) == 0) {
			builtin_func = i + 1;
			break;
		}
	}

	switch (builtin_func) {
		case 0:
			return 1;
		case 1:
			return builtin_cd(cmd, size);
		case 2:
			return builtin_exit(cmd, size);
		case 3:
			return builtin_help(cmd, size);
		case 4:
			return builtin_alias(cmd, size);
		default:
			break;
	}

	return 0;
}

int parse_input(char cmdInput[], char* cmd[]) {
	char* tok;
	tok = strtok(cmdInput, " \n");
	
	int size = 0;
	while (tok != NULL) {
		cmd[size] = tok;
		size ++;
		tok = strtok(NULL, " \n");
	}
	cmd[size] = NULL;
	
	return size;
}

int is_piped(char* cmd[], int size) {
	int i = 0;
	for (i = 0; i < size; i ++) {
		if (strcmp(cmd[i], "|") == 0) {
			return 1;
		}
	}
	return 0;
}

void execCmd(char* cmd[], int size) {
	int i = 0;
	for (; i < ALIAS_COUNT; i ++) {
		if (strcmp(cmd[0], ALIAS_KEY[i]) == 0) {
			cmd[0] = ALIAS_VALUE[i];
			break;
		}
	}

	char* cmdActual[1024];
	int j = 0;
	char firstItem[1024];
	strcpy(firstItem, cmd[0]);
	char* tok = strtok(firstItem, " ");
	while (tok != NULL) {
		cmdActual[j] = tok;
		j ++;
		tok = strtok(NULL, " ");
	}

	for (i = 1; i < size; i ++, j ++) {
		cmdActual[j] = cmd[i];
	}

	char tmp[6] = "";
	strcpy(tmp, COMMAND_ROOT);
	strcat(tmp, cmdActual[0]);
	cmdActual[0] = tmp;
	
	execve(cmdActual[0], cmdActual, ENVP);
	if (errno == ENOENT) {
		printf("Command not found.\n");
	}
	return;
}

void run_single_command(char* cmd[], int size) {

	if (fork() == 0) {
		execCmd(cmd, size);
		printf("Child: failed executing the command.\n");
		exit(1);
	}
	else {
		wait(NULL);
	}

	return;
}

void run_piped_commands(char* cmd[], int size) {
	
	// split the cmd into two commands
	char* cmd1[24];
	char* cmd2[24];
	
	int i = 0;
	for (;i < size; i ++) {
		if (strcmp(cmd[i], "|") == 0) {
			break;
		}
		cmd1[i] = cmd[i];
	}
	
	int size_cmd1 = i;
	cmd1[size_cmd1] = NULL;

	i ++;
	for (; i < size; i ++) {
		cmd2[i - 1 - size_cmd1] = cmd[i];
	}
	int size_cmd2 = size - size_cmd1 - 1;
	cmd2[size_cmd2] = NULL;

	// start
	int pipefd[2];
	pid_t p1, p2;

	if (pipe(pipefd) < 0) {
		printf("Pipe could not be initialized\n");
		return;
	}

	int saved_stdout = STDOUT_FILENO;
	p1 = fork();
	if (p1 < 0) {
		printf("could no fork\n");
		return;
	}

	if (p1 == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);

		execCmd(cmd1, size_cmd1);

		printf("Child command 1 failed\n");
		exit(1);
		
	}
	
	p2 = fork();

	if (p2 < 0) {
		printf("could not fork for command 2\n");
		return;
	}
		
	if (p2 == 0) {
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);

		execCmd(cmd2, size_cmd2);
		
		printf("Child command 2 failed\n");
		exit(1);		
	}
	
	close(pipefd[0]);
	close(pipefd[1]);
	waitpid(p1, NULL, 0);
	waitpid(p2, NULL, 0);
	return;
}

int main(){
	signal(SIGINT, sigint_handler);
	
	ALIAS_KEY = (char**)malloc(sizeof(char*) * 1024);
	ALIAS_VALUE = (char**)malloc(sizeof(char*) * 1024);	
	init_shell();

	while (1) {
		print_prompt();

		char cmdInput[BUFFER_SIZE];
		if (get_input(cmdInput)) {
			printf("Input is too long for my Mini-shell, please try other commands.\n");
			continue;
		}

		// Parse the input by space
		char* cmd[24];
		int size = parse_input(cmdInput, cmd);
		
		if (size == 0) {
			continue;
		}

		// try to run the command as a builtin function, if seccueed, continue
		if (try_builtin_func(cmd, size) != 1) {
			continue;
		}

		// run the command based on it's single command or piped commands
		if (is_piped(cmd, size) == 0) {
			run_single_command(cmd, size);
		}
		else {
			run_piped_commands(cmd, size);
		}
	}

	return 0;
}
