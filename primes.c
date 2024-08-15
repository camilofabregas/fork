#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void filtro(int *fds_ant);

int
main(int argc, char *argv[])
{
	(void) argc;
	int rango = atoi(argv[1]);
	if (rango < 2 || argc != 2) {
		printf("Error en input!\n");
		exit(-1);
	}

	int fds_prox[2];
	if (pipe(fds_prox) == -1) {
		perror("Error en pipe!");
		exit(-1);
	}

	int pid = fork();
	if (pid < 0) {
		perror("Error en fork");
		exit(-1);
	}

	if (pid == 0) {
		close(fds_prox[1]);
		filtro(fds_prox);
		close(fds_prox[0]);
	} else {
		close(fds_prox[0]);
		for (int num = 2; num <= rango; num++) {
			if (write(fds_prox[1], &num, sizeof(num)) < 0) {
				perror("Error en read");
				exit(-1);
			}
		}
		close(fds_prox[1]);
		wait(NULL);
	}
	exit(0);
}

void
filtro(int *fds_ant)
{
	close(fds_ant[1]);
	int num_primo;
	int res_pipe;
	if ((res_pipe = read(fds_ant[0], &num_primo, sizeof(num_primo))) < 0) {
		perror("Error en read");
		exit(-1);
	} else if (res_pipe == 0) {  // EOF, termine.
		close(fds_ant[0]);
		exit(0);
	}
	printf("primo %d\n", num_primo);

	int fds_prox[2];
	if (pipe(fds_prox) == -1) {
		perror("Error en pipe!");
		exit(-1);
	}

	int pid_prox = fork();
	if (pid_prox < 0) {
		perror("Error en fork");
		exit(-1);
	}

	int num_recibido;
	if (pid_prox == 0) {
		close(fds_ant[0]);
		close(fds_prox[1]);
		filtro(fds_prox);
		close(fds_prox[0]);
	} else {
		close(fds_prox[0]);
		while ((res_pipe = read(fds_ant[0],
		                        &num_recibido,
		                        sizeof(num_recibido))) != 0) {
			if (res_pipe < 0) {
				perror("Error en read");
				exit(-1);
			}
			if (num_recibido % num_primo != 0) {
				// No es multiplo, lo mando al proceso derecho
				if (write(fds_prox[1],
				          &num_recibido,
				          sizeof(num_recibido)) < 0) {
					perror("Error en read");
					exit(-1);
				}
			}
		}
		close(fds_ant[0]);
		close(fds_prox[1]);
		wait(NULL);
	}
}