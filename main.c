#include "stdio.h"
#include<stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <time.h>
// DIR library
#include <dirent.h>

#include <fcntl.h>

#include <assert.h>

#include "filelist.h"

#define SIZE 75

#define BACKUP_INFO "__bckpinfo__"
#define BACKUP_FOLDER "~BACKUP"

bool create_dir(char *path, int mode) {

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

int load_backup_info(char *backup_folder, FILA* fila) {
	int c = 0;
	FILE *fbackup;

	// opens BACKUP_INFO file. If it doesn't exist, creates new one;
	fbackup = fopen(BACKUP_INFO, "r+");

	if (fbackup == NULL )
		return 1;

	char ch;
	char* filename;
	filename = malloc(FILENAME_MAX);

	int timestamp = 0;
	filename[0] = 'c';

	/**
	 * each file backed up has 2 lines in BACKUP_INFO, name and timestamp.
	 * while reading the file, we are either reading it's name or timestamp per line.
	 */
	bool is_reading_filename = true;

	int pos = 0;
	while ((ch = fgetc(fbackup)) != EOF) {
		if (ch == '\n') {
			c++;
			if (is_reading_filename) {
				is_reading_filename = false;
				pos = 0;
			} else {
				is_reading_filename = true;
				printf("\nfilename: %s, timestamp: %d", filename, timestamp);
				inserir(fila, filename, timestamp);
				timestamp = 0;
				memset(filename, '\0', SIZE);
			}
		} else {
			if (is_reading_filename) {
				filename[pos] = ch;
				pos++;
			} else {
				timestamp *= 10;
				timestamp += atoi(&ch);
			}
		}

		putchar(ch);

	}
	return 0;
}

void save_backup_info(char *backup_folder, FILA fila) {
	FILE* fbackup;
	char * backup_file = malloc(FILENAME_MAX);
	strcat(backup_file, backup_folder);
	strcat(backup_file, "/");
	strcat(backup_file, BACKUP_INFO);
	printf("backup File: %s", backup_file);

	fbackup = fopen(backup_file, "w+");

	save_to_file(fila, fbackup);
}

int main(int argc, char *argv[]) {
	FILA fila;
	inic(&fila);

	/*
inserir(&fila, "fich1" , 125);
inserir(&fila, "fich2" , 123);
inserir(&fila, "fich3" , 123);

save_backup_info("~BACKUP",fila);
exit(1);
	 */
	/*FICHEIRO* fich1;
//fich1 = get_file_with_name(&fila,"fichx");
if(fich1 != NULL)
printf("timestamp de fich1: %d\n" , (fich1)->timestamp);
//fich1->name = "TROLOLO";

fich1 = NULL;
//fich1 = get_file_with_name(&fila,"fich1");
//if(fich1 != NULL)
//printf("timestamp de fich1: %d\n" , (fich1)->timestamp);
	 */

	//load_backup_info(".", &fila);
	//listar(fila);

	// check if number of arguments is right
	if (argc != 4) {
		printf(
				"Usage: %s <directory to backup> <directory to destination> <time of interval of copies>\n",
				argv[0]);
		exit(1);
	}

	int dt = atoi(argv[3]);
	if (dt == 0) {
		exit(2);
	}

	DIR *dip;
	struct dirent *dit;
	struct stat statbuf;

	// Open source directory
	if ((dip = opendir(argv[1])) == NULL )
		return errno;

	char* currFile;
	char* backupFile;

	char *backupInfoFile;
	backupInfoFile = malloc(FILENAME_MAX);

	char *info;
	info = malloc(FILENAME_MAX);

	char *infoTemp;
	infoTemp = malloc(FILENAME_MAX);

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

			inserir(&fila, dit->d_name, statbuf.st_mtime);
			printf("FICHEIRO: dit->d_name = %s\n", dit->d_name);
			printf("MODIFICADO EM: statbuf.st_mtime = %d\n", statbuf.st_mtime);
			copy_file(currFile, backupFile);
		}

		memset(currFile, '\0', SIZE);
		memset(backupFile, '\0', SIZE);
	}
	printf("\n\n\n\n");
	listar(fila);
	save_backup_info("~BACKUP", fila);


	char* backupIncremental;
	backupIncremental = malloc(FILENAME_MAX);

	char *folderBack;
	folderBack = malloc(FILENAME_MAX);
	memset(folderBack, '\0', SIZE);


	sleep(dt);


	bool alterado = false;
	sprintf(folderBack, "root");

	char *temp;
	temp = malloc(FILENAME_MAX);


	strcat(temp, backup_dir);
	strcat(temp, "/");
	strcat(temp, "bckpfolders");

	int bckInfoFile = open(temp, O_WRONLY | O_CREAT, 0755);
	assert(bckInfoFile >= 0);




	// INCRMENTAL BACKUP
	while (1) {

		printf("\n\nPasta anterior: %s \n\n",folderBack);
		sprintf(infoTemp, "%s\n", folderBack);



		time_t t = time(0);
		struct tm lt = *localtime(&t);

		sprintf(backupIncremental, "%s/~BACKUP/%d_%d_%d_%d_%d_%d", argv[2],
				lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour,
				lt.tm_min, lt.tm_sec);

		create_dir(backupIncremental, 0755);

		memset(folderBack, '\0', SIZE);


		sprintf(folderBack, "%d_%d_%d_%d_%d_%d",
						lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour,
						lt.tm_min, lt.tm_sec);


		printf("\n\nPasta anterior: %s \n\n",folderBack);

		strcat(backupInfoFile, backupIncremental);
		strcat(backupInfoFile, "/__bckpinfo__");

		int backupInfo_fd = open(backupInfoFile, O_WRONLY | O_CREAT, 0755);
		assert(backupInfo_fd >= 0);

		printf("Incremental folder : %s \n", backupIncremental);

		dip = opendir(argv[1]);
		while ((dit = readdir(dip)) != NULL ) {

			if (strcmp(dit->d_name, ".") == 0 || strcmp(dit->d_name, "..") == 0)
				continue;

			// create path to current file
			strcat(currFile, argv[1]);
			strcat(currFile, "/");
			strcat(currFile, dit->d_name);

			strcat(backupFile, backupIncremental);
			strcat(backupFile, "/");
			strcat(backupFile, dit->d_name);

			if (stat(currFile, &statbuf) == -1) {
				perror("stat");
				return errno;
			}

			if (S_ISREG(statbuf.st_mode) == true) {
				bool makeNewBackup = true;
				FICHEIRO* fich1;
				fich1 = get_file_with_name(&fila, dit->d_name);

				if (fich1 != NULL ) {
					if (fich1->timestamp < statbuf.st_mtime) {
						makeNewBackup = true;
						alterado = true;
						fich1->timestamp = statbuf.st_mtime;
					} else
						makeNewBackup = false;
				} else {
					inserir(&fila, dit->d_name, statbuf.st_mtime);
					makeNewBackup = true;
				}

				if (makeNewBackup) {

					printf("Part 2, FICHEIRO: dit->d_name = %s\n", dit->d_name);
					printf("Part 2, MODIFICADO EM: statbuf.st_mtime = %d\n",
							statbuf.st_mtime);
					copy_file(currFile, backupFile);
					struct tm *infoTime = localtime(&statbuf.st_mtime);
					sprintf(info, "%s %d %d %d %d %d %d\n", dit->d_name,
							lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
							lt.tm_hour, lt.tm_min, lt.tm_sec);

				}

			}


			strcat(infoTemp, info);


			memset(currFile, '\0', SIZE);
			memset(backupFile, '\0', SIZE);
		}

		char *bckInfoFileChar;
		bckInfoFileChar = malloc(FILENAME_MAX);

		if(alterado){
			sprintf(bckInfoFileChar, "%s\n", folderBack);
			write(bckInfoFile, bckInfoFileChar , strlen(bckInfoFileChar));
		}

		memset(bckInfoFileChar, '\0', SIZE);
		save_backup_info(BACKUP_FOLDER, fila);

		write(backupInfo_fd, infoTemp, strlen(infoTemp));

		memset(infoTemp, '\0', SIZE);
		memset(backupIncremental, '\0', SIZE);
		memset(backupInfoFile, '\0', SIZE);

		sleep(dt);
	}

	return 0;
}
