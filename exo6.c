#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 100
#define MAX_ARGUMENTS 10
#define MAX_INPUT_SIZE 1024

char *executeCommand()
{
    const char *MSGINPUT = "enseash%% ";

    char *cmd = (char *)malloc(MAX_INPUT_SIZE * sizeof(char));
    if (cmd == NULL)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, MSGINPUT, strlen(MSGINPUT)) == -1)
    {
        perror("Error writing to the terminal");
        free(cmd);
        exit(EXIT_FAILURE);
    }
    ssize_t bytesRead = read(STDIN_FILENO, cmd, MAX_INPUT_SIZE);
    if (bytesRead == -1)
    {
        perror("Error reading from the terminal");
        free(cmd);
        exit(EXIT_FAILURE);
    }

    // Remove the newline character from the end of the input
    size_t len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n')
    {
        cmd[len - 1] = '\0';
    }

    return cmd;
}

// used to split the command ==> (cmd ,-option ....)
void parseArguments(char *input, char *args[])
{
    char commandCopy[MAX_INPUT_SIZE];
    strcpy(commandCopy, input);

    char *token = strtok(commandCopy, " ");
    int i = 0;

    while (token != NULL && i < MAX_ARGUMENTS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    args[i] = NULL; // Ensure the last element is NULL for execvp
}

int main()
{
    const char *ERRORMSG = "Erreur: You can't execute this command\n";
    char *command;
    struct timespec t_init_child, t_finish_child;

    while (1)
    {

        while (1)
        {
            char *command = executeCommand();
            clock_gettime(CLOCK_REALTIME, &t_init_child); // get time start child
            pid_t child_pid = fork();

            if (child_pid == -1)
            {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }

            // child process
            if (child_pid == 0)
            {
                printf("child_pid:%d\n", getpid());
                sleep(2);
                char *args[MAX_ARGUMENTS];
                parseArguments(command, args);

                if (execvp(args[0], args) == -1)
                {
                    perror(ERRORMSG);
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }

            // parent instruction
            else
            {
                int status;
                waitpid(child_pid, &status, 0); // wait for the specific child process

                clock_gettime(CLOCK_REALTIME, &t_finish_child);                                                                                       // get time finish child
                double finish = (t_finish_child.tv_sec - t_init_child.tv_sec) * 1000.0 + (t_finish_child.tv_nsec - t_init_child.tv_nsec) / 1000000.0; // Calculate the elapsed time in milliseconds

                char msgState[100] = "\0";

                // test if the process completed correctly or if there is an interruption and specify which type of interruption presented
                if (WIFEXITED(status))
                {
                    snprintf(msgState, sizeof(msgState), "enseash %% [exit : %d | %dms]\n", WEXITSTATUS(status), (int)finish);
                    write(STDOUT_FILENO, msgState, strlen(msgState));
                }
                else
                {
                    snprintf(msgState, sizeof(msgState), "enseash %% [sign : %d | %dms]\n", status, (int)finish);
                    write(STDOUT_FILENO, msgState, strlen(msgState));
                }

                // free memory
                free(command);
            }
        }

        return 0;
    }
}
