#include "main.h"


// Function to add a job to the job list
void add_job(pid_t pid, const char *command) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].command, command, 255);
        jobs[job_count].active = 1;
        jobs[job_count].stopped = 0;
        jobs[job_count].job_id = job_count + 1; // Job ID starts from 1
        job_count++;
    } else {
        printf("Job list is full.\n");
    }
}

// Function to list jobs
void list_jobs() {
    printf("Jobs:\n");
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].active) {
            const char *status = jobs[i].stopped ? "Stopped" : "Running";
            printf("[%d] %s %s\n", jobs[i].job_id, status, jobs[i].command);
        }
    }
}


void bring_to_foreground_by_id(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id && jobs[i].active) {
            pid_t pid = jobs[i].pid;
            printf("Bringing job %d (PID: %d) to the foreground...\n", job_id, pid);
            foreground_pid = pid;
            
            if (jobs[i].stopped) {
                kill(pid, SIGCONT);
                jobs[i].stopped = 0;
            }
            
            // Wait for the job to complete
            int status;
            waitpid(pid, &status, WUNTRACED);
            
            if (WIFSTOPPED(status)) {
                // If stopped, add back to job list
                jobs[i].stopped = 1;
            } else {
                // If terminated, remove from job list
                remove_job(job_id);
            }
            foreground_pid = -1;
            return;
        }
    }
    printf("No such job with ID %d\n", job_id);
}


// Add this function to job_manager.c or another appropriate file

void remove_job(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].job_id == job_id) {
            // Remove job by shifting remaining jobs
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];
            }
            job_count--;  // Decrease job count
            return;
        }
    }
    printf("No such job with ID %d\n", job_id);
}


// Function to resume a job in the background by job ID
void resume_job_in_background(int job_id) {
    if (job_id > 0 && job_id <= job_count && jobs[job_id - 1].active && jobs[job_id - 1].stopped) {
        pid_t pid = jobs[job_id - 1].pid;
        printf("Resuming job %d (PID: %d) in the background...\n", job_id, pid);
        if (kill(pid, SIGCONT) == 0) {
            jobs[job_id - 1].stopped = 0; // Mark job as running
        } else {
            perror("Failed to send SIGCONT");
        }
    } else {
        printf("Invalid job ID or job not stopped.\n");
    }
}
