#include <stdio.h>
#include <unistd.h>
#include "functions.h"

// To-Do: commands all require a fork b/c exec exits completely upon successs

void lsCommand(void)
{
    execl("/bin/ls", "ls", 0, NULL);

    return;
}

void pwdCommand(void)
{
    execl("/bin/pwd", "pwd", 0, NULL);

    return;
}