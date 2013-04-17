#include "stdio.h"
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
// DIR library
#include <dirent.h>

#include <fcntl.h>

#include <assert.h>
#define SIZE  75

bool create_dir(char *path, int mode) {
	//strcat(path,"/BACKUP");
	if (mkdir(path, mode) != 0 && errno == EEXIST)
		return false;
	else
		return true;
}

void copy_file(char *src, char *dest) {
	int src_fd = open(src, O_RDONLY);
	assert(src_fd >= 0);
	int dest_fd = open(dest, O_WRONLY | O_CREAT, 0755);
	assert(dest_fd >= 0);
	char buf[8192];

	ssize_t result = read(src_fd, &buf[0], sizeof(buf));
	while (result) {
		assert(result > 0);
		assert(write(dest_fd, &buf[0], result) == result);
		result = read(src_fd, &buf[0], sizeof(buf));
	}

}
int main(int argc, char *argv[]) {

	// check if number of arguments is right
	if (argc != 4) {
		printf(
				"Usage: %s <directory to backup> <directory to destination> <time of interval of copies>\n",
				argv[0]);
		exit(1);
	}


	int dt = atoi(argv[3]);
	if(dt == 0){
		exit(2);
	}

	DIR *dip;
	struct dirent *dit;
	struct stat statbuf;

	// Open Source Directory
	if ((dip = opendir(argv[1])) == NULL )
		return errno;

	char* currFile;
	char* backupFile;

	currFile = malloc(FILENAME_MAX);
	backupFile = malloc(FILENAME_MAX);

	char *backup_dir;
	backup_dir = malloc(FILENAME_MAX);
	strcat(backup_dir, argv[2]);
	strcat(backup_dir, "/~BACKUP");
	create_dir(backup_dir, 0755);

	while ((dit = readdir(dip)) != NULL ) {

		if (strcmp(dit->d_name, ".") == 0 || strcmp(dit->d_name, "..") == 0)
			continue;

		// create path to current file
		strcat(currFile, argv[1]);
		strcat(currFile, "/");
		strcat(currFile, dit->d_name);

		strcat(backupFile, backup_dir);
		strcat(backupFile, "/");
		strcat(backupFile, dit->d_name);

		if (stat(currFile, &statbuf) == -1) {
			perror("stat");
			return errno;
		}

		if (S_ISREG(statbuf.st_mode) == true) {
			printf("FICHEIRO: dit->d_name = %s\n", dit->d_name);
			printf("MODIFICADO EM: statbuf.st_mtime = %d\n", statbuf.st_mtime);

			copy_file(currFile, backupFile);
		}

		memset(currFile, '\0', SIZE);
		memset(backupFile, '\0', SIZE);
	}

	while (1) {

	}

	return 0;
}

