/* Programa realitzat per Christina Kaur Krishan */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int fda[2], fdb[2]; 
int fd_resultat, fd_error;
int pid1, pid2, pid3;
char cami[100];

void redirigeix_canals() {
	// Redirigeix els canals de estandard output i error a dos fitxers, resultat.txt i error.txt respectivament
	fd_resultat = open("./resultat.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	close(1);
	dup(fd_resultat);
	close(fd_resultat);

	fd_error = open("./error.txt",  O_WRONLY | O_CREAT | O_TRUNC, 0600);
	close(2);
	dup(fd_error);
	close(fd_error);
}

int main() {
	sprintf(cami, "%s/.bash_history", getenv("HOME")); // 
	
	redirigeix_canals();
	
	if ((pipe(fda) < 0) || (pipe(fdb) < 0)) {
		perror("Una de les dos pipes no s'ha pogut crear correctament\n");
		exit(1);
	}
	
	// fork per fer cat
	pid1 = fork();
	if (pid1 == 0) { // procés fill
		close(1);
		dup(fda[1]); 
		close(fda[1]);
		close(fda[0]); // no és necessari 
		execl ("/bin/cat", "cat", cami, NULL);
		exit(1);
    }
	else if (pid1 < 0) { // hi ha hagut un error
      	perror("Hi ha hagut un error al crear el procés fill per fer cat\n");
      	exit(1);
	}
    else { // procés pare
        // fork per fer grep
		pid2 = fork();
		if (pid2 == 0) { // procés fill
			close(0);
			dup(fda[0]);
			close(fdb[0]);
			close(1);
			dup(fdb[1]);
			close(fda[1]);
			close(fda[1]); // no és necessari 
			execl("/usr/bin/grep", "grep", "^man", NULL);
			exit(1);
		}
		else if (pid2 < 0) { // hi ha hagut un error
			perror("Hi ha hagut un error al crear el procés fill per fer grep\n");
			exit(1);
		}
		else { // procés pare
			// fork per fer wc
			pid3 = fork();
			if (pid3 == 0) { // procés fill
				close(0);
				dup(fdb[0]);
				close(fda[0]);
				close(fda[1]);
				close(fdb[1]);
				execl("/usr/bin/wc", "wc", "-l", NULL);
				exit(1);
			}
			else if (pid3 < 0) { // hi ha hagut un error
				perror("Hi ha hagut un error al crear el procés fill per fer wc\n");
				exit(1);
			}
		}
	}
	
	close(fda[0]);
    close(fda[1]);
    close(fdb[0]);
    close(fdb[1]); 
	return 0;
}