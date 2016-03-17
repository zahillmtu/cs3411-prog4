#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>

#define GRN  "\001\x1B[32m\002"
#define NRM  "\001\x1B[0m\002"

// new file for shell creation


int main(void) {

    while(1) {

        char *line = readline(GRN "mtush CWD> " NRM);
        if (!line) {
            printf("Error reading input, please try againi\n");
            continue;
        }
        else {
            if (strcmp(line, "exit") == 0) {
                free(line);
                exit(EXIT_SUCCESS);
            }
            else {
                printf("You entered: %s\n", line);
                free(line);
            }
        }
    }
    exit(EXIT_FAILURE);
}
