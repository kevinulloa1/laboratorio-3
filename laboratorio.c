// Autores: Fermin Delgado, Kevin Ulloa
//Repositorio Github: git@github.com:kevinulloa1/laboratorio-3.git
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // Para fork(), pipes, etc.
#include <sys/wait.h>    // Para wait() (esperar a los hijos)
#include <semaphore.h>   // Para manejar los semáforos
#include <sys/mman.h>    // Para mmap() (memoria compartida)
#include <fcntl.h>       // Para constantes de apertura de archivos
#include <string.h>

// Definimos que vamos a compartir entre los procesos
typedef struct {
    double saldo;
    sem_t semaforo;     // sem_t = Semaphore Type
} DatosCompartidos;

void credito(char *archivo_montos, int p[], DatosCompartidos *memoria) {
    // 1. Abrimos el archivo en modo lectura ("r")
    FILE *archivo = fopen(archivo_montos, "r");
    if (archivo == NULL) {
        perror("Hijo Crédito: Error al abrir el archivo");
        exit(1);
    }

    double monto;
    
    // 2. Leemos el archivo línea por línea hasta que no haya más números
    while (fscanf(archivo, "%lf", &monto) == 1) {
        
        // --- SECCIÓN CRÍTICA (Uso del semáforo) ---
        // sem_wait es como pedir permiso. Si otro lo está usando, se queda esperando acá.
        sem_wait(&memoria->semaforo); 
        
        memoria->saldo += monto; // Sumamos a la pizarra compartida
        
        // sem_post es soltar el bastón para que otro pueda entrar
        sem_post(&memoria->semaforo);
        // ------------------------------------------

        // 3. Le mandamos el número al padre por el walkie-talkie (pipe de escritura)
        write(p[1], &monto, sizeof(double));
    }

    // 4. Limpieza del hijo
    fclose(archivo);
    close(p[1]); // Cerramos la boca del walkie-talkie. Esto le avisa al padre que terminamos (read devuelve 0).
}
void debito(char *archivo_montos, int p[], DatosCompartidos *memoria) {
    // 1. Abrimos el archivo en modo lectura ("r")
    FILE *archivo = fopen(archivo_montos, "r");
    if (archivo == NULL) {
        perror("Hijo Débito: Error al abrir el archivo");
        exit(1);
    }

    double monto;
    
    // 2. Leemos el archivo línea por línea
    while (fscanf(archivo, "%lf", &monto) == 1) {
        
        // --- SECCIÓN CRÍTICA ---
        sem_wait(&memoria->semaforo); 
        
        memoria->saldo -= monto; // Restamos a la pizarra compartida
        
        sem_post(&memoria->semaforo);
        // -----------------------

        // 3. Le mandamos el número al padre
        write(p[1], &monto, sizeof(double));
    }

    // 4. Limpieza del hijo
    fclose(archivo);
    close(p[1]); // Le avisamos al padre que ya no vamos a mandar más nada.
}
int main();{
  int pipe_credito[2];        
    int pipe_debito[2];         
    pid_t pid_credito, pid_debito;  

    printf("Padre: Preparando el entorno...\n");
   DatosCompartidos *memoria = mmap(NULL, sizeof(DatosCompartidos),        
                                     PROT_READ | PROT_WRITE,                
                                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);    
    if (memoria == MAP_FAILED) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    memoria->saldo = 0.0; 
    
    if (sem_init(&memoria->semaforo, 1, 1) == -1) {     
        perror("Error al inicializar el semáforo");
        exit(1);
    }

    if (pipe(pipe_credito) == -1 || pipe(pipe_debito) == -1) {      
        perror("Error al crear los pipes");
        exit(1);
    }

    printf("Padre: Memoria, semáforo y pipes listos.\n");
    pid_credito = fork();       

    if (pid_credito < 0) {
        perror("Error al crear el hijo credito");
        exit(1);
    } else if (pid_credito == 0) {
        close(pipe_credito[0]); 
        close(pipe_debito[0]);  
        close(pipe_debito[1]);

        credito("credito.txt", pipe_credito, memoria);
        
        exit(0); 
    }

    pid_debito = fork();

    if (pid_debito < 0) {
        perror("Error al crear el hijo debito");
        exit(1);
    } else if (pid_debito == 0) {
        close(pipe_debito[0]);  
        close(pipe_credito[0]); 
        close(pipe_credito[1]);

        debito("debito.txt", pipe_debito, memoria);
        
        exit(0); 
    }


    printf("Padre: Hijos creados correctamente. Esperando los reportes...\n");
    return 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    }
