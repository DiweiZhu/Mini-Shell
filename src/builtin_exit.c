#include "minishell.h"

void builtin_exit()
{
    printf("exit through builtin_exit.\n");
    exit(0);
}
