#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define TAM 10000

// --- Variáveis Globais (Compartilhadas) ---
int lista[TAM];
double media_global = 0;
double mediana_global = 0;
double desvio_global = 0;

// Função auxiliar para comparar inteiros (usada no qsort)
int comparar(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// --- Funções das Threads ---

void *calc_media(void *arg) {
    double soma = 0;
    for (int i = 0; i < TAM; i++) {
        soma += lista[i];
    }
    media_global = soma / TAM;
    pthread_exit(NULL);
}

void *calc_mediana(void *arg) {
    // IMPORTANTE: Para achar a mediana, precisamos ordenar.
    // Se ordenarmos o vetor global 'lista', podemos atrapalhar as outras threads
    // que estão lendo ele agora. Por isso, fazemos uma cópia local.
    int *copia_lista = malloc(TAM * sizeof(int));
    for(int i=0; i<TAM; i++) copia_lista[i] = lista[i];

    // Ordena a cópia
    qsort(copia_lista, TAM, sizeof(int), comparar);

    if (TAM % 2 == 0)
        mediana_global = (copia_lista[TAM/2 - 1] + copia_lista[TAM/2]) / 2.0;
    else
        mediana_global = copia_lista[TAM/2];
    
    free(copia_lista); // Libera memória da cópia
    pthread_exit(NULL);
}

void *calc_desvio(void *arg) {
    // O desvio padrão precisa da média. 
    // Opção A: Recalcular a média aqui.
    // Opção B: Esperar a thread da média acabar (mais complexo).
    // Vamos recalcular a média localmente para ser totalmente paralelo.
    double soma = 0;
    for (int i = 0; i < TAM; i++) soma += lista[i];
    double media_local = soma / TAM;

    double soma_variancia = 0;
    for (int i = 0; i < TAM; i++) {
        soma_variancia += pow(lista[i] - media_local, 2);
    }
    desvio_global = sqrt(soma_variancia / TAM);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    // 1. Gera lista aleatória [0, 100]
    for (int i = 0; i < TAM; i++) {
        lista[i] = rand() % 101;
    }

    pthread_t t1, t2, t3;
    clock_t inicio, fim;

    // --- MODO MULTI-THREAD ---
    printf(">>> Iniciando com 3 Threads...\n");
    inicio = clock(); // Marca tempo inicial

    // Criação das threads
    pthread_create(&t1, NULL, calc_media, NULL);
    pthread_create(&t2, NULL, calc_mediana, NULL);
    pthread_create(&t3, NULL, calc_desvio, NULL);

    // Espera as threads terminarem (Join)
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    fim = clock(); // Marca tempo final
    
    double tempo_threads = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("Média: %.2f\n", media_global);
    printf("Mediana: %.2f\n", mediana_global);
    printf("Desvio Padrão: %.2f\n", desvio_global);
    printf("Tempo com Threads: %f segundos\n\n", tempo_threads);

    // --- MODO SEQUENCIAL (SINGLE THREAD) PARA COMPARAÇÃO ---
    printf(">>> Iniciando modo Sequencial (1 thread)...\n");
    inicio = clock();
    
    // Executa as funções sequencialmente (simulando como se fossem chamadas normais)
    // Nota: Teria que adaptar as funções para não usar pthread_exit, 
    // mas aqui vamos apenas chamar a lógica novamente ou imaginar o tempo somado.
    // Para simplificar o exercício, vou apenas chamar as threads uma após a outra (join logo após create)
    
    pthread_create(&t1, NULL, calc_media, NULL);
    pthread_join(t1, NULL);
    
    pthread_create(&t2, NULL, calc_mediana, NULL);
    pthread_join(t2, NULL);
    
    pthread_create(&t3, NULL, calc_desvio, NULL);
    pthread_join(t3, NULL);

    fim = clock();
    double tempo_seq = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo Sequencial: %f segundos\n", tempo_seq);

    return 0;
}