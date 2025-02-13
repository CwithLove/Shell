#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nb_commande(struct cmdline l) {
    if (l->)
}

int cd() {

}

int internal(char **cmd) {
    if (strcmp(cmd[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd[0], "cd")) {
        //faire
        return 1;
    }
    return 0;
}

int execution(struct cmd l) {
    int nb_cmd = nb_commande();
    for (int i = 0; i < nb_cmd; i++) {
        if (internal(l->seq[i])) {
            //
        } else {
            Execve();
        }
    }

}

