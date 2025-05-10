#include "main.h"
extern int idx;  // Declare idx as external
int execute_internal_command(char *input) {
    char count = 0;
    switch (idx) {
        case 0:  // cd
            if (strncmp(input, "cd", 2) == 0) {
                for (int i = 2; input[i] == ' '; i++) {
                    count++;
                }

                if (chdir(&input[2 + count]) != 0) {
                    // Error changing directory
                    perror("cd");
                }
                return 0;
            }
            break;

        case 1:  // pwd
            printf("%s\n", get_current_dir_name());
            break;

        case 2:  // jobs
            list_jobs();  // Function to list all background jobs
            break;

     case 3:  // fg
{
    // Extract the job ID or PID from the input
    int job_id = 0;
    char *arg = input + 2; // Skip "fg"
    while (*arg == ' ') arg++; // Skip spaces

    if (*arg >= '0' && *arg <= '9') {
        job_id = atoi(arg); // Convert the argument to an integer (job ID or PID)
    }

    if (job_id > 0) {
        bring_to_foreground_by_id(job_id);
    } else {
        // If no job ID is provided, bring the last background job to the foreground
        if (job_count > 0) {
            bring_to_foreground_by_id(job_count);
        } else {
            printf("fg: no current jobs\n");
        }
    }
} break;


      case 4:  // bg
{
    pid_t job_pid = extract_pid(input);
    if (job_pid == -1) {
        // Handle invalid PID
        printf("bg: Invalid PID\n");
    } else {
        printf("Attempting to resume job with PID %d\n", job_pid);

        if (kill(job_pid, SIGCONT) == -1) {
            perror("bg: kill");
        } else {
            printf("Signal SIGCONT sent to job %d\n", job_pid);

            // Update job status to reflect that it's running
            for (int i = 0; i < job_count; i++) {
                if (jobs[i].pid == job_pid) {
                    jobs[i].stopped = 0;
                    break;
                }
            }
        }
    }
} break;

        case 5:  // exit
            exit(0);
            break;
    }
    return 0;
}
