#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGS 4
#endif

void xargs(char **args);
void limpiar_vector(char **args);

int
main(int argc, char *argv[])
{
	(void) argc;
	// Tiene largo +2 para el comando y el NULL del final.
	char *args[NARGS + 2] = {
		argv[1]
	};  // Inicializo con el comando en 1er lugar.

	char *line = NULL;
	size_t size;
	int i = 1;
	while (getline(&line, &size, stdin) != -1) {
		if (i == NARGS + 1) {
			args[i] = NULL;
			xargs(args);
			i = 1;
			limpiar_vector(args);
		}
		line[strcspn(line, "\n")] = 0;
		args[i] = strdup(line);
		i += 1;
	}
	args[i] = NULL;
	xargs(args);
	limpiar_vector(args);
	free(line);
	return 0;
}

void
xargs(char **args)
{
	int i = fork();
	if (i < 0) {
		perror("Error en fork");
		exit(-1);
	}
	if (i == 0) {  // Proceso HIJO.
		execvp(args[0], args);
	} else {  // Proceso PADRE.
		wait(NULL);
	}
}

void
limpiar_vector(char **args)
{
	int i = 1;
	while (args[i]) {
		free(args[i]);
		i += 1;
	}
}