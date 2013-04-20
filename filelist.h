/**
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct file_list{
	char * name;
	int timestamp;
	struct file_list *next;
} FICHEIRO;

typedef FICHEIRO* FILA;

void inic(FILA* fila);

void inserir(FILA* fila, char* name, int timestamp);

void listar(FILA fila);

int get_timestamp(FILA* fila, char *name);

void save_to_file(FILA fila, FILE* fich);

void delete(FILA* fila);
