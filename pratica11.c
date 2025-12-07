#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Sou o P1 (Avô). Vou criar o F1...\n");

    pid_t pid_f1 = fork(); // P1 se divide. Agora temos P1 e F1.

    if (pid_f1 == 0) {
        // --- AQUI DENTRO ESTAMOS NO F1 ---
        printf("  Sou o F1 (Filho). Vou criar o N1...\n");

        pid_t pid_n1 = fork(); // F1 se divide. Agora temos N1.

        if (pid_n1 == 0) {
            // --- AQUI DENTRO ESTAMOS NO N1 ---
            printf("    Sou o N1 (Neto). Vou rodar o 'ls'!\n");
            
            // O N1 vai deixar de ser código C e virar o comando 'ls'
            execl("/bin/ls", "ls", NULL);
            
            // Essa linha só roda se o execl der erro
            perror("Erro ao executar ls"); 
            exit(1); 
        }

        pid_t pid_n2 = fork(); // F1 se divide. Agora temos N1 e N2

        if (pid_n2 == 0) {
            // --- AQUI DENTRO ESTAMOS NO N2 ---
            printf("    Sou o N2 (Neto). Vou rodar o 'whoami'!\n");
            
            // O N2 vai deixar de ser código C e virar o comando 'whoami'
            execl("/usr/bin/whoami", "whoami", NULL);
            
            // Essa linha só roda se o execl der erro
            perror("Erro ao executar ls"); 
            exit(1); 
        }

        // --- VOLTAMOS AO CÓDIGO DO F1 ---
        // O F1 precisa esperar o N1 e N2 morrerem.
        wait(NULL); 
        printf("  Sou o F1. Meus filho N1 e N2 terminaram. Agora eu também vou sair.\n");
        exit(0); // F1 encerra
    }

    pid_t pid_f2 = fork(); // P1 se divide. Agora temos P1, F1 e F2.

    if (pid_f2 == 0) {
        // --- AQUI DENTRO ESTAMOS NO F2 ---
        printf("  Sou o F2 (Filho). Vou criar o N2...\n");

        pid_t pid_n3 = fork(); // F2 se divide.

        if(pid_n3 == 0) {
            // --- AQUI DENTRO ESTAMOS NO N3 ---
            printf("    Sou o N3 (Neto). Vou rodar o 'date'!\n");

            // O N3 vai deixar de ser código C e virar o comando 'date'
            execl("/bin/date", "date", NULL);
            
            // Essa linha só roda se o execl der erro
            perror("Erro ao executar date"); 
            exit(1); 
        }

        pid_t pid_n4 = fork(); // F2 se divide.

        if(pid_n4 == 0) {
            // --- AQUI DENTRO ESTAMOS NO N4 ---
            printf("    Sou o N4 (Neto). Vou rodar o 'pwd'!\n");

            // O N4 vai deixar de ser código C e virar o comando 'pwd'
            execl("/bin/pwd", "pwd", NULL);
            
            // Essa linha só roda se o execl der erro
            perror("Erro ao executar pwd"); 
            exit(1); 
        }

        // --- VOLTAMOS AO CÓDIGO DO F2 ---
        wait(NULL);
        printf("  Sou o F2. Meus filho N3 e N4 terminaram. Agora eu também vou sair.\n");
        exit(0); // F2 encerra
    }

    // --- VOLTAMOS AO CÓDIGO DO P1 ---
    // P1 espera o F1 e F2 terminarem
    wait(NULL);
    printf("Sou o P1. Meus filhos F1 e F2 terminaram. Acabou tudo.\n");


    return 0;
}