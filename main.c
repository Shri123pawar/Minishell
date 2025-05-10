/*Name -: shridhar pawar
Batch no-:24017*/
#include "main.h"

int exit_val;
int job_count = 0;  // Initialize job_count to 0
Job jobs[MAX_JOBS]; // Define the jobs array
pid_t foreground_pid = -1;  // Initialize foreground_pid to -1

int get_command(char * input, char * temp) {
  int i;
  for (i = 0; input[i] != ' ' && input[i] != '\0'; i++) {
    temp[i] = input[i];
  }
  temp[i] = '\0';
  return i;
}
int special_variable(char * input) {
  // input = echo $$, $?, $SHELL, String
  char temp[10];
  int i = get_command(input, temp);
  if (strcmp(temp, "echo") == 0) {
    if (strcmp(input + i + 1, "$$") == 0) {
      printf("%d\n", getpid());
    } else if (strcmp(input + i + 1, "$?") == 0) {
      printf("%d\n", exit_val);
    } else if (strcmp(input + i + 1, "$SHELL") == 0) {
      printf("%s/minnishell\n",
        get_current_dir_name()); // current working directory
    } else {
      printf("%s", input + i + 1);
    }
    return SUCCESS;
  }
  return FAILURE;
}
void signal_handler(int signum) {
    if (foreground_pid > 0) {
        if (signum == SIGINT) { // Handle Ctrl+C
            printf("\nInterrupting process (PID: %d)...\n", foreground_pid);
            kill(foreground_pid, SIGKILL);
        } else if (signum == SIGTSTP) { // Handle Ctrl+Z
            printf("\nStopping process (PID: %d)...\n", foreground_pid);
            kill(foreground_pid, SIGTSTP);
            
            // Add the process to the jobs list and mark it as stopped
            add_job(foreground_pid, "Stopped process");
            jobs[job_count - 1].stopped = 1;
        }
        foreground_pid = -1;
    }
}


int main() {
  char input[50], prompt[50] = "minshell$";
// Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
  while (1) {
    printf(ANSI_COLOUR_YELLOW"[%s]$ "ANSI_COLOUR_CYAN ,prompt);
    fgets(input, 50, stdin); // input = PS1=NEW_PROMPT\n
    // Remove the newline character from the input
    input[strlen(input) - 1] = '\0';

    if (strncmp(input, "PS1=", 4) == 0) {
      strcpy(prompt, input + 4);
    } else if (special_variable(input) == SUCCESS) {
      continue;
    } else if (check_command_type(input) == EXTERNAL) {
      execute_external_command(input);
    } else if (check_command_type(input) == INTERNAL) {
      execute_internal_command(input);
    }
  }
}
