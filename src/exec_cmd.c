#include "minishell.h"

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
    /*
	for (; i < ALIAS_COUNT; i ++) {
		if (strcmp(cmd[0], ALIAS_KEY[i]) == 0) {
			cmd[0] = ALIAS_VALUE[i];
			break;
		}
	}
    */

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
    char COMMAND_ROOT[] = "/bin/";
	strcpy(tmp, COMMAND_ROOT);
	strcat(tmp, cmdActual[0]);
	cmdActual[0] = tmp;

    char* ENVP[] = {"PATH=/bin", NULL};
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

void exec_cmd(char **cmd) {
    int size = 0;
    int i = 0;
    while (cmd[i]) {
        size++;
        i++;
    }

    if (is_piped(cmd, size) == 0) {
        run_single_command(cmd, size);
    }
    else {
        run_piped_commands(cmd, size);
    }
}
