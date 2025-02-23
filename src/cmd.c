#include "cmd.h"

extern jobs_t *jobs;
extern int current_job;

int nb_cmd(struct cmdline *l) {
    int i = 0;
    while (l->seq[i] != NULL) {
        i++;
    }
    return i;
}

int cd(char **cmd) {
    char *path = malloc(sizeof(char) * MAXPATH + 1);
    if (path == NULL) {
        perror("malloc");
    }

    if (cmd[1] == NULL) {
        char *tem = getenv("HOME");
        if (tem != NULL) {
            strncpy(path, tem, MAXPATH);
        }

    } else if (cmd[1][0] == '~') {
        if (cmd[1][1] == '/' || cmd[1][1] == '\0') {
            char *tem = getenv("HOME");
            if (tem != NULL) {
                snprintf(path, MAXPATH, "%s%s", tem, cmd[1] + 1);
            }
        } else {
            perror("cd");
        }
    } else {
        strncpy(path, cmd[1], MAXPATH);
    }
    
    if (chdir(path) != 0) {
        perror("cd");
        return 1;
    }

    char cwd[MAXPATH];
    getcwd(cwd, MAXPATH);
    setenv("PWD", cwd, 1);
    free(path);
    return 1;
}

int export(char **cmd) {
    if (cmd[1] == NULL) {
        fprintf(stderr, "Error: export VAR=value\n");
        return 1;
    }

    char *arg = cmd[1];
    char *eq_ptr = strchr(arg, '=');

    if (eq_ptr == NULL) {
        char *current_val = getenv(arg);
        if (current_val == NULL) {
            if (setenv(arg, "", 1) < 0) {
                perror("setenv");
                return 1;
            }
        } else {
            if (setenv(arg, current_val, 1) < 0) {
                perror("setenv");
                return 1;
            }
        }
    } else {
        *eq_ptr = '\0';    
        char *var_name = arg;
        char *var_value = eq_ptr + 1;
        if (setenv(var_name, var_value, 1) < 0) {
            perror("setenv");
            return 1;
        }
    }

    return 1;
}

int internal(char **cmd) {
    if (cmd[0][0] == '#') {
        return 1;
    }
    if (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit")) {
        // list_jobs_free(jobs);
        int status;
        if (cmd[1] != NULL) {
            status = atoi(cmd[1]);
        } else {
            status = 0;
        }
        exit(status);
    } else if (!strcmp(cmd[0], "cd")) { 
        return cd(cmd);
    } else if (!strcmp(cmd[0], "export")) {
        return export(cmd);
    } else if (!strcmp(cmd[0], "jobs")) {
        list_jobs_print(jobs);
        return 1;
    } else if (!strcmp(cmd[0], "fg") || !strcmp(cmd[0], "bg") || !strcmp(cmd[0], "stop")) {
        int num; 

        if (cmd[1] == NULL) {
            num = jobs->count;
        } else {
            if (cmd[1][0] == '%') {
                num = atoi(cmd[1] + 1);
            } else {
                num = atoi(cmd[1]);
            }
        }

        switch (cmd[0][0])
        {
        case 'f':
            fg_job(num);
            return 1;
        
        case 'b':
            bg_job(num);
            return 1;

        case 's':
            stop_job(num);
            return 1;
        }
    }
    
    return 0;
}

int **create_pipes(int n_cmd) {
    int **pipes = (int **)malloc(sizeof(int *) * (n_cmd - 1));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_cmd - 1; i++) {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (pipes[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    return pipes;
}

void connect_pipes(int **pipes, int rang_cmd, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++) {
        if (i == rang_cmd) {
            close(pipes[i][0]);
            if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                perror("dup2 in connect");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][1]);
        }

        else if (i == rang_cmd - 1) {
            close(pipes[i][1]);
            if (dup2(pipes[i][0], STDIN_FILENO) == -1) {
                perror("dup2 out pipe");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][0]);
        } else {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }  
    } 
}

void free_pipes(int **pipes, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
}

void connect_in_out(struct cmdline* l, int rang_cmd, int n_cmd) 
{
    int fd;  
    if (rang_cmd == 0 && l->in != NULL) {
        fd = open(l->in, O_RDONLY);
        if (fd == -1) {
            perror(l->in);
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2 in");
            exit(EXIT_FAILURE);
        }
        close(fd);
    } if (rang_cmd == n_cmd - 1 && l->out != NULL) {
        fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror(l->out);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 out");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
}

char *get_each_cmd(char **seq) {
    char *cmd;
    int total_len = 0;
    int i = 0;
    while (seq[i] != NULL) {
        total_len += strlen(seq[i]) + 1;
        i++;
    }
    cmd = malloc(sizeof(char) * total_len);
    if (cmd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd[0] = '\0';
    for (int j = 0; seq[j] != NULL; j++) {
        strcat(cmd, seq[j]);
        if (seq[j + 1] != NULL) {
            strcat(cmd, " ");
        }
    }
    return cmd;
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    linked_list_t *pids = linked_list_init();
    int **pipes;
    pid_t pid;
    gid_t gpid = -1;
    
    if (n_cmd > 0) {
        pipes = create_pipes(n_cmd);
    }
    
    int n_internal = 0;
    for (int cmd = 0; cmd < n_cmd; cmd++) {
        if (internal(l->seq[cmd])) {
            n_internal++;
            continue;
        } 
    
        if (jobs->count >= MAXJOBS) {
            fprintf(stderr, "Error: No more jobs\n");
            errno = EAGAIN;
            return;
        }

        char *cmd_str = get_each_cmd(l->seq[cmd]);
        wordexp_t p;
        int ret;

        ret = wordexp(cmd_str, &p, 0);
        if (ret != 0) {
            perror("wordexp");
            return;
        }

        pid = fork();
        if (pid == -1) {
            perror("Fork");

        } else if (pid > 0) { // Parent
            
            if (gpid == -1) {
                gpid = pid;
            }
            setpgid(pid, gpid);
            linked_list_add(pids, pid);

        } else { // Fils
            connect_in_out(l,cmd,n_cmd);
            connect_pipes(pipes, cmd,n_cmd);
            free_pipes(pipes, n_cmd);
            
            if (execvp(p.we_wordv[0], p.we_wordv) == -1) {
                if (l->bg) {
                    l->bg = 0;
                }
                fprintf(stderr, "%s : Command not found\n", l->seq[cmd][0]);
                exit(3);
            }
        }
        wordfree(&p);
    }
    
    connect_pipes(pipes, -1, n_cmd);
    free_pipes(pipes, n_cmd);

    if (n_internal == n_cmd) {
        return;
    }

    // create job
    int job_num = jobs_add(pids, gpid, l->seq);
    if (!l->bg) {
        current_job = job_num;
    } else {
        printf("[%d] %d\n", job_num, pids->head->pid);
    }
}
