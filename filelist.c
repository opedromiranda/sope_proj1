#include "filelist.h"

void inic(FILA* fila) {
	*fila = NULL;
}

void inserir(FILA* fila, char* name, int timestamp) {

	if (*fila == NULL ) {
		*fila = (FILA) malloc(sizeof(FICHEIRO));

		if (*fila == NULL ){
			return;
		}
		(*fila)->name = malloc(sizeof(FILENAME_MAX));
		strcpy( (*fila)->name, name);
		(*fila)->timestamp = timestamp;
		(**fila).next = NULL;

	} else {
		inserir(&(**fila).next, name, timestamp);
	}

}

FICHEIRO* get_file_with_name(FILA* fila, char *name) {
	if (*fila == NULL )
		return NULL;
	else {
		if ((strcmp((*fila)->name, name)) == 0)
			return (*fila);
		else{
			return get_file_with_name(& (**fila).next, name);
		}

	}
}

void listar(FILA fila) {
	if (fila == NULL )
		return;
	printf("name: %s, timestamp: %d\n", fila->name, fila->timestamp);
	listar(fila->next);
}

void save_to_file(FILA fila, FILE* fich){
	if(fila == NULL){
		fclose(fich);
		return;
	}

	fprintf(fich,"%s\n%d\n",fila->name, fila->timestamp);
	save_to_file(fila->next, fich);
}

void delete(FILA* fila){
	FICHEIRO* tmp = *fila;
	if(*fila == NULL)
		return;
	*fila = (*fila)->next;
	free(tmp);
}

