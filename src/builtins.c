/*
Created by Diwei in 10/19/2020.
This file contains functions dealing with all builtins.
*/

#include "minishell.h"

void list_append(list_t *list, void *value)
{
    list_t *cur_node = (list_t*)malloc(sizeof(list_t));
    cur_node->value = value;
    cur_node->next = NULL;
    if (list == NULL) {
        list = cur_node;
        return;
    }

    while (list->next != 0) {
        list = list->next;
    }
    list->next = cur_node;

    return;
}

void add_builtin(list_t *builtins, char* name, void (*f)(), char* description)
{
    builtin_t* cur = (builtin_t*)malloc(sizeof(builtin_t));
    cur->name = (char*)malloc(sizeof(char) * strlen(name));
    strcpy(cur->name, name);
    cur->func = f;
    cur->description = (char*)malloc(sizeof(char) * strlen(description));
    strcpy(cur->description, description);

    list_append(builtins, cur);

    return;
}

void init_builtins(list_t *builtins)
{
    add_builtin(builtins, "exit", &builtin_exit, "This will free all memory and exit the shell.\n");
    return;
}

// return 0 if successfully execute a builtin, 1 otherwise.
int exec_builtin(list_t *builtins, char* name)
{
    while (builtins != NULL) {
        if (builtins->value == NULL) {
            builtins = builtins->next;
            continue;
        }

        builtin_t *builtin = builtins->value;
        if (builtin->name == NULL || builtin->func == NULL) {
            builtins = builtins->next;
            continue;
        }

        if (strcmp(builtin->name, name) == 0) {
            builtin->func();
            return 0;
        }

        builtins = builtins->next;
    }

    return 1;
}

void free_builtins(list_t *builtins)
{
    while (builtins != NULL) {
        list_t *cur = builtins;
        builtins = builtins->next;

        builtin_t *builtin = cur->value;

        if (builtin != NULL) {
            free(builtin->name);
            free(builtin->description);
            free(builtin);
        }
        free(cur);
    }
}
