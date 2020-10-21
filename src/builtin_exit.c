#include "minishell.h"

extern volatile sig_atomic_t flag;

void builtin_exit(char** cmd)
{
    printf("exit through builtin_exit.\n");
    flag = 0;
    return;
}
