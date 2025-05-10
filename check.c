#include "main.h"

char *internal_cmd[] = {"cd", "pwd","jobs", "fg", "bg","exit", NULL};
int idx;
int check_command_type(char *input) {
    char temp[10]; 
    get_command(input, temp);  // Extract the command part into temp
    // Find internal or external command
    for (int i = 0; internal_cmd[i] != NULL; i++) {
        if (strcmp(temp, internal_cmd[i]) == 0) {
      idx = i;
      return INTERNAL;
    }
}
return EXTERNAL;
}
