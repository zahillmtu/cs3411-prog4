/**
 * Zachary Hill
 * CS3411
 * prog4 - Shell
 * 3/17/2016
 */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

#define GRN  "\001\x1B[32m\002" // for the prompt
#define NRM  "\001\x1B[0m\002" // to set the color back to normal

#define READ_END 0
#define WRITE_END 1

/**
 * Function to fork a process and run the execvp command. exec should be set
 * to the name of the executable and args should contain an array where arg[0]
 * is the name of the exec and arg[1 - 31] are the arguments passed
 */
void run(char *exec, char *args[]) {

    // fork the process
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return;
    }
    else if(pid == 0) {
        // The child process
        // Call execvp
        if (execvp(exec, args) < 0) {
            printf("An error occured running the program %s - Try again\n", exec);
            exit(EXIT_FAILURE);
        }
    }
    else {
        // wait for the child
        int status = 0;
        wait(&status);
        return;
    }
    return;
}

/**
 * Function to tokenize the string first by "|" and second by " "
 * line should be set to the string to be tokenized, returns an
 * array containing arrays of the commands
 */
void tokenize(char *cmds[64][64], char *line) {

    char *piped[64] = { NULL }; // make sure array is null terminated

    // first seperate by pipes
    char *toks = strtok(line, "|");
    if (toks != NULL) {
        int j = 0;
        // call again to get the next piece of the string
        while (toks != NULL) {
            piped[j] = toks;
            toks = strtok(NULL, "|");
            j = j + 1;
        }
    }

    // Then seperate by spaces
    int i = 0;
    int k = 0;

    while (piped[i] != NULL) {

        toks = strtok(piped[i], " ");
        while(toks != NULL) {
            cmds[i][k] = toks;
            toks = strtok(NULL, " ");
            k = k + 1;
        }
        k = 0;
        i = i + 1;
    }
    return;
}

/**
 * Function to pipe commands together
 */
void pipecmds(char *cmds[64][64]) {

    char *cmd1 = cmds[0][0];
    char *cmd2 = cmds[1][0];
    char *args1[64] = { NULL };
    char *args2[64] = { NULL };
    args1[0] = cmd1;
    args2[0] = cmd2;
    // separate the args
    int k = 1;
    while (cmds[0][k] != NULL) {
        args1[k] = cmds[0][k];
        k = k + 1;
    }
    int j = 1;
    while (cmds[1][j] != NULL) {
        args2[j] = cmds[1][j];
        j = j + 1;
    }
    // fork the processes and run the cmds
    int fd[2];
    pipe(fd);

    int child1 = fork();
    if (child1 == 0) {
        // Run the command and push output into pipe
        // Close end not used
        close(fd[READ_END]);

        // change write end from stdout to pipe
        dup2(fd[WRITE_END], STDOUT_FILENO);
        if (execvp(cmd1, args1) < 0) {
            printf("An error occured running the program %s - Try again\n", cmd1);
            close(fd[WRITE_END]); // close to ensure no errors
            exit(EXIT_FAILURE);
        }
    }

    int child2 = fork();
    if (child2 == 0) {
        // Run the command and push output into pipe
        // Close end not used
        close(fd[WRITE_END]);

        // change write end from stdout to pipe
        dup2(fd[READ_END], STDIN_FILENO);
        if (execvp(cmd2, args2) < 0) {
            printf("An error occured running the program %s - Try again\n", cmd2);
            close(fd[READ_END]);
            exit(EXIT_FAILURE);
        }
    }

    // Only parent should get here, clean up
    close(fd[READ_END]);
    close(fd[WRITE_END]);

    int st;
    int st2;
    waitpid(child1, &st, 0);
    waitpid(child2, &st2, 0);
}

/**
 * Function takes the args array and attempts to call chdir using arg[1]
 * arg[1] should be set to the desired directory and no other input should
 * be added after
 */
void callcd(char *args[]) {

    int ret;

    // Check to make sure it's just "cd" and the path
    if (args[2] != NULL) {
        printf("Error using cd - Try again\n");
        return;
    }
    // If there is no path do nothing
    if (args[1] == NULL) {
        return;
    }
    // attempt to call chdir, if it fails print a message
    ret = chdir(args[1]);
    if (ret == -1) {
        perror("chdir");
        return;
    }
    return;
}

/**
 * This is the main function that runs the control for the mtush shell
 * It will run in a loop collecting input and calculating results until
 * the user types "exit".
 */
int main(void) {

    //int retVal;

    while(1) {

        // get the current working directory
        char *cwd = getcwd(NULL, 0);
        if (cwd == NULL) {
            perror(cwd);
            exit(EXIT_FAILURE);
        }

        char *dest = (char*) malloc(sizeof(char)*1024);
        //sprintf(dest, GRN); // put in green
        sprintf(dest, "mtush %s> ", cwd); // put in value
        //strncat(dest,

        //retVal = sprintf(noColor, "mtush %s> ", cwd);
        //if (retVal < 0) {
        //    printf("Error writing to string\n");
        //    exit(EXIT_FAILURE);
        //}
        //char* prompt = GRN;
        //prompt = strcat(prompt, noColor);
        //prompt = strcat(prompt, NRM);

        // read in the user input
        char *line = readline(dest);
        if (!line) {
            printf("Error reading input, please try again\n");
            continue;
        }
        else { // Do stuff with the input
            // if the input is exit return
            printf("This is line: %s\n", line);
            // Check if malformed
            if (line[0] == '|' || line[strlen(line) - 1] == '|') {
                printf("Error - Malformed cmd\n");
                free(line);
                free(dest);
                free(cwd);
                continue;
            }

            if (strcmp(line, "exit") == 0) {
                free(line);
                free(dest);
                free(cwd);
                exit(EXIT_SUCCESS);
            }
            else {
                char *cmds[64][64] = {{ NULL }};
                memset(cmds, 0, sizeof cmds);
                char *exec;
                char *args[64] = { NULL };

                // Separate the line into usable chunks
                tokenize(cmds, line);

                // if there is no input just prompt again
                if (cmds[0][0] == NULL) {
                    free(line);
                    free(dest);
                    free(cwd);
                    continue;
                }
                else if (cmds[1][0] != NULL) { // There was a pipe

                    pipecmds(cmds);

                }
                else { // No pipe
                    exec = cmds[0][0];
                    args[0] = exec;
                    int k = 1;
                    while (cmds[0][k] != NULL) {
                        args[k] = cmds[0][k];
                        k = k + 1;
                    }
                    // if cd then call chdir()
                    if (strcmp(exec, "cd") == 0) {
                        callcd(args);
                        continue;
                    }
                    // run the exec command
                    run(exec, args);
                }


               // char *toks = strtok(line, " ");
               // // run the command given
               // if (toks != NULL) {
               //     // set the executable
               //     char *exec = toks;
               //     char *args[64] = { NULL }; // make sure array is null terminated
               //     args[0] = exec; // first value must be set to program name
               //     int j = 1;
               //     // call again to get the next piece of the string
               //     toks = strtok(NULL, " ");
               //     while (toks != NULL) {
               //         args[j] = toks;
               //         toks = strtok(NULL, " ");
               //         j = j + 1;
               //     }
                    // if the exec is cd, run chdir()
            }
                printf("You entered: %s\n", line);
                free(line);
                free(dest);
                free(cwd);
        }
    }
    exit(EXIT_FAILURE);
} // end of main

/**
 * References
 * strtok - http://stackoverflow.com/questions/9629473/c-extracting-words-from-string
 */
