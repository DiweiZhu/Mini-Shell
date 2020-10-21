/*
This file includes builtin function cd.
It takes the parsed user input, and try to change to the directory.
*/

#include "minishell.h"

void builtin_cd(char** cmd) {
    int size = 0;
    while (*(cmd + size)) {
        size++;
    }

	if (size == 1) {
		if (chdir(getenv("HOME")) == -1) {
			printf("error cd to root\n");
		}
        return;
	}

	if (size > 2){
		printf("usage of cd: cd [dir]\n");
        return;
	}

    if (chdir(cmd[1]) == -1) {
        printf("error cd to %s\n", cmd[1]);
    }

    return;
}
