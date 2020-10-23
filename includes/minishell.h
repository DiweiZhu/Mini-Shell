#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFER_SIZE 82
#define TOKEN_SIZE 24

// data structures
typedef struct list {
    void *value;
    struct list *next;
} list_t;

typedef struct builtin {
    char *name;
    void (*func)();
    char *description;
} builtin_t;

// minishell.c

// builtin.c
void init_builtins(list_t *builtins);
int exec_builtin(list_t *builtins, char* name, char **cmd);
void free_builtins(list_t *builtins);

// exec_cmd.c
void exec_cmd(char **cmd);

// builtins
void builtin_exit(char **cmd);
void builtin_cd(char **cmd);




#endif
