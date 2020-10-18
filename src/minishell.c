#include "minishell.h"

void sigint_handler(int sig) {
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

int get_input(char* cmdInput) {
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

/*
NAME: minishell
DOES: take input from user, parse the input, anction based on it, and loop
*/
void minishell()
{
    while (1) {
        print_prompt();

        char* cmdInput = (char*)malloc(sizeof(char) * BUFFER_SIZE);
        if (get_input(cmdInput)) {
            printf("Input is too long for my Mini-shell, please try other commands.\n");
            continue;
        }
        printf("You typed: ");
        printf("%s", cmdInput);
        free(cmdInput);

    }
}

void main()
{
    signal(SIGINT, sigint_handler);

    init_shell();
    minishell();

    return;
}
