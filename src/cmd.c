#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int internal(char **cmd) {
    if (strcmp(cmd[0], "exit") == 0) {
        exit(0);
    }
    return 0;
}