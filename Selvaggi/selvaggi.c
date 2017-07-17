#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

#define M 10 /**Quante porzioni contiene il pentolone*/
#define NGIRI 3 /**quante volte mangia un selvaggio*/
#define S 10 /**numero di selvaggi*/

sem_t pieno, vuoto;
pthread_mutex_t mutexS, mutexC; /**garantisce atomicita' quando si modifica porzioni*/
int porzioni, Ssleep;

void* Cuoco(void* arg){
  int maxP = *((int*)arg);
  while(1){
    printf("Pentolone pieno, il CUOCO si addormenta\n");
    sem_wait(&vuoto); /**dorme in attesa di essere svegliato*/
    pthread_mutex_lock(&mutexC);
    printf("IL CUOCO controlla le porzioni\n");
    /**inizio sezione critica*/
    if(porzioni == 0){ /**se pentola vuota*/
      /**cucina*/
      printf("Il cuoco cucina\n");
      porzioni = maxP;
      printf("Il pentolone e' pieno\n");
    }
    if(porzioni == maxP){ /**se pentola piena*/
      pthread_mutex_unlock(&mutexC);
      /**fine sezione critica*/
      /**sveglia chi lo ha svegliato*/
      printf("Chiamo i selvaggi\n");
      while(Ssleep > 0){ /**sveglio tutti i selvaggi*/
        Ssleep--;
        sem_post(&pieno);
      }
    }
  }
}

void *Selvaggio(void* arg){
  int iS = *((int*)arg); /**indice selvaggi*/
  int i;
  for(i = 0; i < NGIRI; i++){ /**per il numero di selvaggi*/
    printf("Il selvaggio N %d ha fame\n", iS);
    pthread_mutex_lock(&mutexS);
    /**inizio sezione critica*/
    if(porzioni == 0){
      printf("Il pentolone e' vuoto, sveglio il cuoco\n");
      sem_post(&vuoto); /**sveglio il cuoco*/
      printf("Attendo che il cuoco prepari\n");
      Ssleep++;
      sem_wait(&pieno); /**attendo che prepari*/
    }
    pthread_mutex_lock(&mutexC);
    printf("Prendo una porzione\n");
    printf("IL selvaggio N %d mangia per la %d volta\n", iS, i+1);
    porzioni--; /**si appropria di una porzione*/
    printf("porzioni = %d\n", porzioni); /**debug*/
    pthread_mutex_unlock(&mutexC);
    /**fine sezione critica*/
    pthread_mutex_unlock(&mutexS);
    /**mangia porzione*/
  }
  pthread_exit(NULL);
}


void main(){
  int maxP = M;
  int nselvaggi = S;
  int retThread, i;
  int Ssleep = 0;
  sem_init(&pieno, 0, 0);
  sem_init(&vuoto, 0, 0);
  pthread_mutex_init(&mutexS, NULL);
  pthread_mutex_init(&mutexC, NULL);
  pthread_t tcuoco;
  pthread_t tselvaggio;
  porzioni = maxP;
  retThread = pthread_create(&tcuoco, NULL, Cuoco, &maxP);
  if(retThread != 0){
    printf("ERRORE creazione cuoco\n");
    exit(-1);
  }
  for(i = 0; i < S; i++){
    retThread = pthread_create(&tselvaggio, NULL, Selvaggio, &i);
    printf("SELVAGGIO N %d PENSA --\n", i);
    if(retThread != 0){
      printf("ERRORE creazione selvaggio\n");
      exit(-1);
    }
  }
}
