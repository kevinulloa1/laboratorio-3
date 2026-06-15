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
int main();{
    
    return 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    }
