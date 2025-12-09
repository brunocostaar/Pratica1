//Esse programa gera os 10mil números aleatórios que
//serão escritos no arquivo randnum.txt e lidos e utilizados
//pelo programa pratica12.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM 10000 //10 mil

int main(){
	//Abre o arquivo.
	FILE* pFile = fopen("randnum.txt", "w");
	
	//Checa se conseguiu abrir o arquivo.
	if(pFile == NULL){
		printf("Falha ao abrir o arquivo!\n");
		return 1;
	}
	
	//Setta a seed de aleatoriedade.
	srand(time(NULL));
	
	//Escreve 10 mil numeros aleatorios no arquivo
	for(int i = 0; i < TAM; i++){
		int rand_num = rand() % 101;	//Numeros aleatorios de 0 - 100.
		
		fprintf(pFile, "%d ", rand_num);
	}
	
	//Fecha o arquivo.
	fclose(pFile);
	
	return 0;
}