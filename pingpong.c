#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int
main(void)
{
	int fds_ida[2];
	int fds_vuelta[2];
	if (pipe(fds_ida) == -1 || pipe(fds_vuelta) == -1) {
		perror("Error en pipe");
		exit(-1);
	}

	printf("Hola, soy PID <%d>:\n", getpid());
	printf("  - primer pipe me devuelve: [%d, %d]\n", fds_ida[0], fds_ida[1]);
	printf("  - segundo pipe me devuelve: [%d, %d]\n",
	       fds_vuelta[0],
	       fds_vuelta[1]);

	int i = fork();
	if (i < 0) {
		perror("Error en fork");
		exit(-1);
	}

	srandom(time(NULL));

	if (i == 0) {  // Proceso HIJO.
		close(fds_vuelta[0]);
		close(fds_ida[1]);
		printf("\nDonde fork me devuelve %d:\n", i);
		printf("  - getpid me devuelve: <%d>\n", getpid());
		printf("  - getppid me devuelve: <%d>\n", getppid());
		int recibido = 0;
		if (read(fds_ida[0], &recibido, sizeof(recibido)) < 0) {
			perror("Error en read");
			exit(-1);
		}
		printf("  - recibo valor <%d> vía fd=%d\n", recibido, fds_ida[0]);
		close(fds_ida[0]);
		printf("  - reenvío valor en fd=%d y termino\n", fds_vuelta[1]);
		if (write(fds_vuelta[1], &recibido, sizeof(recibido)) < 0) {
			perror("Error en read");
			exit(-1);
		}
		close(fds_vuelta[1]);
	} else {  // Proceso PADRE.
		close(fds_ida[0]);
		close(fds_vuelta[1]);
		int valor = random();
		printf("\nDonde fork me devuelve <%d>:\n", i);
		printf("  - getpid me devuelve: <%d>\n", getpid());
		printf("  - getppid me devuelve: <%d>\n", getppid());
		printf("  - random me devuelve: <%d>\n", valor);
		if (write(fds_ida[1], &valor, sizeof(valor)) < 0) {
			perror("Error en read");
			exit(-1);
		}
		printf("  - envío valor <%d> a través de fd=%d\n",
		       valor,
		       fds_ida[1]);
		close(fds_ida[1]);
		int recibido = 0;
		if (read(fds_vuelta[0], &recibido, sizeof(recibido)) < 0) {
			perror("Error en read");
			exit(-1);
		}
		printf("\nHola, de nuevo PID %d:\n", getpid());
		printf("  - recibí valor %d vía fd=%d\n", recibido, fds_vuelta[0]);
		close(fds_vuelta[0]);
		wait(NULL);
	}
	exit(0);
}