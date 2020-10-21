#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

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

// builtins
void builtin_exit();
void builtin_cd();


#endif
