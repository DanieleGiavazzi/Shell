#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

#define M 10 /**Quante porzioni contiene il pentolone*/
#define NGIRI 3 /**quante volte mangia un selvaggio*/
#define S 10 /**numero di selvaggi*/

sem_t pieno;
sem_t vuoto;
pthread_mutex_t mutex; /**garantisce atomicita' quando si modificano le porzioni*/
int porzioni;

void* Cuoco(void* arg){
  int maxP = *(int)arg;
  while(1){
    sem_wait(&vuoto); /**dorme in attesa di essere svegliato*/
    if(porzioni == 0){ /**se pentola vuota*/
      /**cucina*/
      pthread_mutex_lock(&mutex);
      /**inizio sezione critica*/
      printf("Il cuoco cucina\n");
      porzioni = maxP;
      printf("Il pentolone e' pieno\n");
      /**fine sezione critica*/
      pthread_mutex_unlock(&mutex);
    }
    if(porzioni == maxP){ /**se pentola piena*/
      /**sveglia chi lo ha svegliato*/
      printf("Chiamo un selvaggio\n");
      sem_post(&pieno);
    }
  }
}

void *Selvaggio(void* arg){
  int nSelvaggi = *(int)arg;
  int i;
  for(i = 0; i < nSelvaggi; i++){ /**per il numero di selvaggi*/
    pthread_mutex_lock(&mutex); /**affamato*/
    /**inizio sezione critica*/
    printf("Un selvaggio ha fame\n");
    if(porzioni == 0){
      printf("Il pentolone e' vuoto, sveglio il cuoco\n");
      sem_post(&vuoto); /**sveglio il cuoco*/
      printf("Attendo che il cuoco prepari\n");
      sem_wait(&pieno); /**attendo che prepari*/
    }
    printf("Prendo una porzione\n");
    porzioni--; /**si appropria di una porzione*/
    /**fine sezione critica*/
    pthread_mutex_unlock(&mutex);
    /**mangia porzione*/
  }
  pthread_exit(NULL);
}


void main(){
  int maxP = M;
  int nselvaggi = S;
  int retThread, i;
  sem_init(&pieno, 0, 0);
  sem_init(&vuoto, 0, 0);
  pthread_mutex_init(&mutex, NULL);
  pthread_t tcuoco;
  pthread_t tselvaggio;
  porzioni = 0;
  retThread = pthread_create(&tcuoco, NULL, Cuoco, &maxP);
  if(retThread != 0){
    printf("ERRORE creazione cuoco\n");
    exit(1);
  }
  for(i = 0; i < S; i++){
    retThread = pthread_create(&tselvaggio, NULL, Selvaggio, &nselvaggi);
    if(retThread != 0){
      printf("ERRORE creazione selvaggio\n");
      exit(1);
    }
  }
}
