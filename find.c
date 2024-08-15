#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void find(char *(*func)(const char *, const char *),
          DIR *dir,
          char *palabra,
          char *ruta_parcial);
void chequear_imprimir(char *(*func)(const char *, const char *),
                       char *nombre_actual,
                       char *palabra,
                       char *ruta_parcial);
void cerrar_todo(int fd, int fd2, DIR *subdir);

int
main(int argc, char *argv[])
{
	DIR *dir = opendir(".");
	if (dir == NULL) {
		perror("Error en opendir");
		exit(-1);
	}
	char ruta_parcial[PATH_MAX] = "";
	if (argc == 3) {  // Dos parametros
		find(strcasestr, dir, argv[2], ruta_parcial);
	} else {
		find(strstr, dir, argv[1], ruta_parcial);
	}
	closedir(dir);
	return 0;
}

void
find(char *(*func)(const char *, const char *),
     DIR *dir,
     char *palabra,
     char *ruta_parcial)
{
	struct dirent *actual;
	while ((actual = readdir(dir))) {
		if (actual->d_type == DT_DIR) {
			if (!func(actual->d_name, "..") &&
			    !func(actual->d_name, ".")) {
				chequear_imprimir(func,
				                  actual->d_name,
				                  palabra,
				                  ruta_parcial);
				int fd = dirfd(dir);
				int fd2 = openat(fd, actual->d_name, O_DIRECTORY);
				DIR *subdir = fdopendir(fd2);
				if (subdir) {
					char ruta_parcial_recursiva[PATH_MAX];
					find(func,
					     subdir,
					     palabra,
					     strcat(strcat(strcpy(ruta_parcial_recursiva,
					                          ruta_parcial),
					                   actual->d_name),
					            "/"));
					cerrar_todo(fd, fd2, subdir);
				}
			}
		} else if (actual->d_type == DT_REG) {
			chequear_imprimir(
			        func, actual->d_name, palabra, ruta_parcial);
		}
	}
}

void
chequear_imprimir(char *(*func)(const char *, const char *),
                  char *nombre_actual,
                  char *palabra,
                  char *ruta_parcial)
{
	if (func(nombre_actual, palabra)) {
		printf("%s%s\n", ruta_parcial, nombre_actual);
	}
}

void
cerrar_todo(int fd, int fd2, DIR *subdir)
{
	closedir(subdir);
	close(fd2);
	close(fd);
}