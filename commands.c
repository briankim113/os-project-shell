#include <stdio.h>
#include <unistd.h>
#include "functions.h"


void lsCommand(void)
{
    execl("/bin/ls", "ls", 0, NULL);

    return;
}