#include "main.h"

pid_t extract_pid(char *input) {
    char *arg = input + 3; // Skip "bg" or "fg"
    while (*arg == ' ' || *arg == '%') arg++; // Skip spaces and '%'

    if (*arg == '\0') {
        return -1; // No PID found
    }
    return atoi(arg); // Convert argument to PID
}
