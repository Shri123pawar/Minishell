#include "main.h"

int execute_external_command(char *input) {
    char *commands[MAX_COMMANDS];
    char *token;
    int num_cmds = 0;
    int background = 0; // To check if the command should be run in background

    // Check if the command should run in background
    if (input[strlen(input) - 1] == '&') {
        background = 1;
        input[strlen(input) - 1] = '\0'; // Remove the '&'
    }

    // Split the input by pipes ('|')
    token = strtok(input, "|");
    while (token != NULL && num_cmds < MAX_COMMANDS) {
        commands[num_cmds++] = token;
        token = strtok(NULL, "|");
    }
    commands[num_cmds] = NULL;  // Null-terminate the array

    int pipefds[2 * (num_cmds - 1)];  // Array for pipe file descriptors

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            return -1;
        }
    }

    pid_t pid;
    for (int i = 0; i < num_cmds; i++) {
        pid = fork();
        if (pid == 0) {  // Child process
            // Redirect stdin from the previous pipe, if not the first command
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Redirect stdout to the next pipe, if not the last command
            if (i < num_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (num_cmds - 1); j++) {
                close(pipefds[j]);
            }

            // Parse the command and execute it
            char *cmd_args[100];
            char *cmd_token = strtok(commands[i], " ");
            int j = 0;
            while (cmd_token != NULL) {
                cmd_args[j++] = cmd_token;
                cmd_token = strtok(NULL, " ");
            }
            cmd_args[j] = NULL;

            if (execvp(cmd_args[0], cmd_args) == -1) {
                perror("Command not found");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {  // Fork failed
            perror("fork");
            return -1;
        }

        // If it's the first command and not in background, set the foreground PID
        if (i == 0 && !background) {
            foreground_pid = pid;
        }

        if (background) {
            add_job(pid, input);  // Store the command and PID in the job list
        }

    }

    // Parent process: close all pipes
    for (int i = 0; i < 2 * (num_cmds - 1); i++) {
        close(pipefds[i]);
    }

    // If the command is not running in background, wait for the child process to finish
    if (!background) {
        for (int i = 0; i < num_cmds; i++) {
            int status;
            pid_t child_pid = wait(&status);  // Wait for each child to finish

            if (WIFEXITED(status)) {
                printf("The child %d terminated normally with code %d\n", child_pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("The child %d terminated by signal %d\n", child_pid, WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("The child %d was stopped by signal %d\n", child_pid, WSTOPSIG(status));
            }

            // Reset foreground_pid after the process finishes
            if (child_pid == foreground_pid) {
                foreground_pid = -1;
            }
        }
    } else {
        // In background mode, print a message with job information
        printf("[%d] %d\n", job_count, pid);
    }

    return 0;
}
