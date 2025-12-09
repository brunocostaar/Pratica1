#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

#define TAM 10000 //10 mil
#define FILHOS 3
#define THREADS 3

void selection_sort(int* v);

//Funçoes para os processos.
float calcula_mediav(int* v);
float calcula_medianav(int* v);
float calcula_desvio_padraov(int* v);

//Funçoes para as threads.
void* calcula_mediavth(void* v);
void* calcula_medianavth(void* v);
void* calcula_desvio_padraovth(void* v);
void* calcula_tudovth(void* v);

int main(){
	//Abre o arquivo com os 10 mil numeros aleatorios.
	FILE* pFile = fopen("randnum.txt", "r");
	
	//Checa se conseguiu abrir o arquivo.
	if(pFile == NULL){
		printf("Falha ao abrir o arquivo!\n");
		return 1;
	}
	
	//Vetor que vai guardar esses numeros.
	int v[TAM];
	
	//Le os valores do arquivo pra dentro dos indices do vetor.
	for(int i = 0; i < TAM; i++){
		fscanf(pFile ,"%d", &v[i]);
	}
	
	//Fecha o arquivo.
	fclose(pFile);
	
	//Ordena o Array.
	selection_sort(v);
	
	//ID do processo pai
	pid_t pai = getpid();
	
	//Iniciar método com 3 processos.
	printf("Sou o processo pai de PID %d e criarei 3 processos " 
	       "filhos para calcular a media, mediana e desvio padrao do vetor\n", pai);
	
	//Inicia o cronometro.
	clock_t inicio, fim;
	inicio = clock();
	
	//Cria o processo filho 1, que calculara a media aritmetica.
	pid_t f1 = fork();
	
	if(f1 == 0){
		float media = calcula_mediav(v);
		printf("\nSou f1 meu PID eh: %d e a media dos numeros eh: %.2f\n", getpid(), media);
		exit(0);
	}
	
	//Cria o processo filho 2, que calculara a mediana.
	pid_t f2 = fork();
	
	if(f2 == 0){
		float mediana = calcula_medianav(v);
		printf("\nSou f2 meu PID eh: %d e a mediana do vetor eh: %.1f\n", getpid(), mediana);
		exit(0);
	}
	
	//Cria o processo filho 3, que calculara o desvio padrao.
	pid_t f3 = fork();
	
	if(f3 == 0){
		float desvio_padrao = calcula_desvio_padraov(v);
		printf("\nSou f3 meu PID eh: %d e o desvio_padrao do vetor eh: %.2f\n", getpid(), desvio_padrao);
		exit(0);
	}
	
	//Espera por cada filho encerrar seu processo.
	for(int i = 0; i < FILHOS; i++)
		wait(NULL);
	
	//Encerra o cronometro.
	fim = clock();
	
	//Calcula o tempo gasto.
	double tempo1 = ((double)(fim - inicio))/CLOCKS_PER_SEC;
	
	printf("\nMeus filhos ja encerraram!\nTempo gasto: %f seg\n", tempo1);
	
	//Método 2 - 1 Processo
	printf("\nIniciando teste com apenas 1 processo!\n");
	inicio = clock();
	
	float media = calcula_mediav(v);
	printf("\nA media eh: %.2f\n", media);
	
	float mediana = calcula_medianav(v);
	printf("\nA mediana eh: %.2f\n", mediana);
	
	float desvio_padrao = calcula_desvio_padraov(v);
	printf("\nO desvio padrao eh: %.2f\n", desvio_padrao);
	
	fim = clock();
	double tempo2 = ((double)(fim - inicio))/CLOCKS_PER_SEC;
	
	printf("\nTempo gasto: %f seg\n", tempo2);
	printf("\nO metodo com apenas 1 Processo eh cerca de %.2f vezes mais rapido!\n", tempo1/tempo2);
	
	//Array que guarda as três threads a serem criadas.
	pthread_t th[THREADS];
	
	//Método 3 - 3 Threads
	printf("\nIniciando teste com 3 threads!\n");
	inicio = clock();
	
	//Valores de retorno de cada thread.
	float* ret0;
	float* ret1;
	float* ret2;
	
	//Cria e executa as 3 threads e suas rotinas.
	pthread_create(&th[0], NULL, (void*) &calcula_mediavth, (void*) v);
	pthread_create(&th[1], NULL, (void*) &calcula_medianavth, (void*) v);
	pthread_create(&th[2], NULL, (void*) &calcula_desvio_padraovth, (void*) v);
	
	//Espera as 3 acabarem;
	pthread_join(th[0], (void**) &ret0);
	pthread_join(th[1], (void**) &ret1);
	pthread_join(th[2], (void**) &ret2);
	
	//Printa os valores retornados por cada thread.
	printf("\nA media eh: %.2f\n", *ret0);
	printf("\nA mediana eh: %.2f\n", *ret1);
	printf("\nO desvio padrao eh: %.2f\n", *ret2);
	
	fim = clock();
	double tempo3 = ((double)(fim - inicio))/CLOCKS_PER_SEC;
	
	printf("\nTempo gasto: %f seg\n", tempo3);
	
	//Libera a memória alocada dentro das rotinas das threads.
	free(ret0);
	free(ret1);
	free(ret2);
	
	//Thread do ultimo teste
	pthread_t t1;
	
	//Método 4 - 1 Thread.
	printf("\nIniciando teste com apenas uma thread!\n");
	inicio = clock();
	
	pthread_create(&t1, NULL, (void*) &calcula_tudovth, (void*) v);
	pthread_join(t1, NULL);
	
	fim = clock();
	double tempo4 = ((double)(fim - inicio))/CLOCKS_PER_SEC;
	
	printf("\nTempo gasto: %f seg\n", tempo4);
	printf("\nO metodo com apenas 1 Thread eh cerca de %.2f vezes mais rapido!\n", tempo3/tempo4);
	
	return 0;
}

//Calcula media aritmetica.
float calcula_mediav(int* v){
	float media = 0.0;
	
	for(int i = 0; i < TAM; i++){
		media += v[i];
	}
	media = media / TAM;
	
	return media;
}

//Calcula mediana.
float calcula_medianav(int* v){
	float mediana = 0.0;
	
	if(TAM % 2 == 0){
		mediana = (float)(v[TAM/2 -1] + v[TAM/2])/2;	//Media dos dois elementos centrais.
	}
	else{
		mediana = v[TAM/2]; 							//Elemento central unico.
	}
	
	return mediana;
}

//Calcula desvio padrao.
float calcula_desvio_padraov(int* v){
	float desvio_padrao = 0.0;
	float media = calcula_mediav(v);
	
	for(int i = 0; i < TAM; i++){
		float dispersao = v[i] - media;
		desvio_padrao += pow(dispersao, 2);		//Soma das dipersoes ao quadrado,
	}
	desvio_padrao = desvio_padrao / (TAM - 1);	//Sobre n - 1,
	desvio_padrao = sqrt(desvio_padrao);		//Tira a raiz.
	
	return desvio_padrao;
}

//Função com argumentos e retorno void a serem executadas pelas threads.
void* calcula_mediavth(void* v){
	float* media = (float*)malloc(sizeof(float));
	
	if(media == NULL){
		perror("Falha ao alocar memoria!\n");
		exit(1);
	}
	
	*media = calcula_mediav(((int*)v));
	
	return (void*) media;
}

void* calcula_medianavth(void* v){
	float* mediana = (float*)malloc(sizeof(float));
	
	if(mediana == NULL){
		perror("Falha ao alocar memoria!\n");
		exit(1);
	}
	
	*mediana = calcula_medianav(((int*)v));
	
	return (void*) mediana;
}

void* calcula_desvio_padraovth(void* v){
	float* desvio_padrao = (float*)malloc(sizeof(float));
	
	if(desvio_padrao == NULL){
		perror("Falha ao alocar memoria!\n");
		exit(1);
	}
	
	*desvio_padrao = calcula_desvio_padraov(((int*)v));
	
	return (void*) desvio_padrao;
}

void* calcula_tudovth(void* v){
	float* media;
	float* mediana;
	float* desvio_padrao;
	
	media 		  = (float*)calcula_mediavth(v);
	mediana       = (float*)calcula_medianavth(v);
	desvio_padrao = (float*)calcula_desvio_padraovth(v);
	
	printf("\nA media eh: %.2f\n", *media);
	printf("\nA mediana eh: %.2f\n", *mediana);
	printf("\nO desvio padrao eh: %.2f\n", *desvio_padrao);
	
	free(media);
	free(mediana);
	free(desvio_padrao);
}

void selection_sort(int* v){
	int index_menor;
	for(int i = 0; i < TAM - 1; i++){
		index_menor = i;
		
		for(int j = i + 1; j < TAM; j++){
			
			if(v[j] < v[index_menor]){
				index_menor = j;
			}
		}
		int temp = v[i];
		v[i] = v[index_menor];
		v[index_menor] = temp;
	}
}
