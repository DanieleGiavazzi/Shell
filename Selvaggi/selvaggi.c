#include<stdio.h>
#include<semphafore.h>
#include<sys/types.h>
#include<pthread.h>

#define M 10 /**Quante porzioni contiene il pentolone*/
#define NGIRI 3 /**quante volte mangia un selvaggio*/
#define S 10 /**numero di selvaggi*

sem_t pieno;
sem_t vuoto;
pthread_mutex_t mutex; /**garantisce atomicita' quando si modificano le porzioni*/

void* Cuoco(int M){
  while(1){
    sem_wait(&vuoto); /**dorme in attesa di essere svegliato*/
    if(porzioni == 0){ /**se pentola vuota*/
      /**cucina*/
      pthread_mutex_lock(&mutex);
      /**inizio sezione critica*/
      porzioni = M;
      /**fine sezione critica*/
      pthread_mutex_unlock(&mutex);
    }
    if(porzioni == M){ /**se pentola piena*/
      /**sveglia chi lo ha svegliato*/
      sem_post(&pieno);
    }
  }
}

void *Selvaggio(int NGIRI){
  for(i=0;i < N; i++){ /**per il numero di selvaggi*/
    pthread_mutex_lock(&mutex); /**affamato*/
    /**inizio sezione critica*/
    if(porzioni == 0){
      sem_post(&vuoto); /**sveglio il cuoco*/
      sem_wait(&pieno); /**attendo che prepari*/
    }
    porzioni-- /**si appropria di una porzione*/
    /**fine sezione critica*/
    pthread_mutex_unlock(&mutex);
    /**mangia porzione*/
  }
  pthread_exit(NULL);
}


void main(){
  int retThread;
  int porzioni = 0; /**numero di porzioni disponibili*/
  sem_init(&pieno, 0, 0); /**il calderone e' vuoto*/
  sem_init(&vuoto, 0, 0); /**si possono produrre M porzioni*/
  pthread_mutex_init(&mutex, NULL);

}
