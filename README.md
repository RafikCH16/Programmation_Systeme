# TP de Synthèse _ Ensea in the Shell

## Objectifs : Développer un micro-shell affichant les codes de sortie et les temps d’exécution des programmes lancés.

Dans le cadre de ce projet, nous avons travaillé sur la conception d'un micro-shell appelé "enseash". L'objectif principal de ce micro-shell est de permettre l'exécution de commandes tout en fournissant des informations détaillées sur leur déroulement. Il offre une interface simplifiée pour lancer des commandes et affiche immédiatement des informations telles que les codes de sortie et les temps d'exécution.

## Exercice 1 (Micro Shell de Base):

L'objectif de cet exercice était de créer la base du micro-shell en affichant un message d'accueil suivi d'un prompt simple. La fonction `displayPrompt` est responsable de cette tâche, informant l'utilisateur sur l'utilisation du shell et la manière de le quitter. Le prompt est représenté par "enseash %%". 
La fonction utilise les appels système `write` pour écrire le message d'accueil dans la sortie standard (STDOUT\_FILENO).

## Exercice 2 (Exécution de Commandes):

Dans cette étape, nous avons ajouté la capacité d'exécuter des commandes saisies par l'utilisateur dans le micro-shell. La fonction `executeCommand` lit la commande de l'utilisateur, affiche le prompt "enseash%%", crée un nouvel enfant avec `fork()`, attend que l'enfant se termine avec `wait`, puis réinitialise le prompt pour la prochaine itération.

## Exercice 3 (Gestion de la sortie du shell avec la commande “exit” ou un <ctrl>+d):

Nous avons introduit la fonctionnalité permettant de quitter le micro-shell en saisissant la commande "exit" ou en atteignant la fin du fichier d'entrée (EOF) avec la combinaison de touches Ctrl+D. La boucle principale vérifie si la commande est "exit" ou si sa longueur est nulle, affiche "Bye bye..." et quitte le programme.

## Exercice 4 (Affichage du code de retour de la commande précédente dans le prompt):

L'objectif ici était d'afficher le code de retour (ou le signal) de la commande précédente dans le prompt. Une variable `status` stocke le statut de la commande exécutée par le processus fils, et les macros `WIFEXITED` et `WEXITSTATUS` sont utilisées pour obtenir le code de retour après l'exécution.

## Exercice 5 (Mesure du temps d’exécution de la commande en utilisant l’appel clock\_gettime):

Nous avons introduit la mesure du temps d'exécution de la commande en utilisant l'appel système `clock\_gettime`. Deux structures `struct timespec` sont utilisées pour stocker le temps de début et de fin d'exécution du processus enfant. La différence entre ces temps est calculée pour obtenir le temps total d'exécution en millisecondes.

## Exercice 6 (Exécution d’une commande complexe avec arguments):

Dans cet exercice, le micro-shell a été étendu pour prendre en charge l'exécution de commandes complexes avec des arguments. La fonction `parseArguments` divise la commande complète en tokens (commande et arguments) et les stocke dans un tableau. La boucle principale est modifiée pour lire et exécuter les commandes avec des arguments, utilisant `execvp` dans le processus enfant.

## Exercice 7 (Gestion des redirections avec ‘<’ et ‘>’):

La gestion des redirections de fichiers avec les symboles ‘<’ et ‘>’ a été ajoutée. Les fonctions `open`, `dup2`, et `close` sont utilisées pour rediriger respectivement l'entrée standard (stdin) et la sortie standard (stdout) d'une commande. Un fichier texte est créé pour contenir la commande entrée.
