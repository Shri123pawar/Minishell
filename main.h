#ifndef MAIN_H
#define MAIN_H

#define _GNU_SOURCE

#define SUCCESS 0
#define FAILURE -1
#define EXTERNAL 2
#define INTERNAL 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>   // For signal-related functions and constants
#include <sys/wait.h> // For waitpid
#include <ctype.h>

extern pid_t foreground_pid; // Global variable for the foreground process

#define ANSI_COLOUR_YELLOW  "\x1b[33m"
#define ANSI_COLOUR_BLUE    "\x1b[34m"
#define ANSI_COLOUR_MAGENTA "\x1b[35m"
#define ANSI_COLOUR_CYAN    "\x1b[36m"

#define MAX_JOBS 10
#define MAX_COMMANDS 100

typedef struct {
    pid_t pid;
    char command[256];
    int active;
    int stopped;
    int job_id; // Job ID to track jobs
} Job;


extern int job_count;
extern Job jobs[MAX_JOBS];
extern pid_t foreground_pid;

// Job management functions
void list_jobs();
void add_job(pid_t pid, const char *command);
void remove_job(int job_id);

void bring_to_foreground_by_id(int job_id);
void bring_to_foreground_by_pid(pid_t pid);
void resume_job_in_background(int job_id);
Job* find_job(pid_t pid);

// Command execution functions
int execute_external_command(char *input);
int execute_internal_command(char *input);
int check_command_type(char *input);
int get_command(char *input, char *output);
pid_t extract_pid(char *input);

#endif // MAIN_H
