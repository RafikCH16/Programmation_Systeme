#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define SIZE 100

char *executeCommand()
{
    const char *MSGINPUT = "enseash%% ";
    char *cmd = (char *)malloc(SIZE * sizeof(char));
    if (cmd == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, MSGINPUT, strlen(MSGINPUT)) == -1)
    {
        perror("Error writing to the terminal");
        free(cmd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = read(STDIN_FILENO, cmd, SIZE);
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

int main()
{
    const char *ERRORMSG = "Erreur: You can't execute this command\n";
    while (1)
    {
        char *command = executeCommand();
        //we create every time our child and execute the command entred
        if (fork() == 0)
        {
            if (execlp(command, command, NULL) == -1)
            {
                // If execlp fails
                perror(ERRORMSG);
                exit(EXIT_FAILURE);

            }
            exit(EXIT_SUCCESS);
        }
        else
        {
            wait(NULL);
            // Free the allocated memory
        }
    }
    

    return 0;
}
