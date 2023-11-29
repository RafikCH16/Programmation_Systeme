#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

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
    const char *ERRORMSG = "Erreur lors de l'exécution de la commande\n";

    while (1)
    {
        char *command = executeCommand();

        // Quitter la boucle si la commande est "exit" ou si Ctrl+D (EOF) est atteint
        if (strcmp(command, "exit") == 0 || strlen(command) == 0)
        {
            write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
            free(command);
            break;
        }

        pid_t child_pid = fork();

        if (child_pid == -1)
        {
            perror("Fork failed");
            free(command);
            exit(EXIT_FAILURE);
        }
        if (child_pid == 0)
        {
            // Processus fils
            if (execlp(command, command, NULL) == -1)
            {
                perror(ERRORMSG);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // Processus parent
            int status;
            waitpid(child_pid, &status, 0);
            command="\0";
            // Libérer la mémoire allouée
        }
    }

    return 0;
}