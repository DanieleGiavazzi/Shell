#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

sem_t pieno;
sem_t vuoto;
pthread_mutex_t mutexS, mutexC; /**garantiscono atomicita'*/
int porzioni, Ssleep, ngiri;

void* Cuoco(void* arg){
  int maxP = *((int*)arg);
  while(1){
    printf("Il CUOCO si addormenta\n");
    sem_wait(&vuoto); /**dorme in attesa di essere svegliato*/
    if(porzioni == 0){ /**se pentola vuota*/
      /**cucina*/
      pthread_mutex_lock(&mutexC);
      /**inizio sezione critica*/
      printf("Il cuoco cucina\n");
      porzioni = maxP;
      printf("Il pentolone e' pieno\n");
      /**fine sezione critica*/
      pthread_mutex_unlock(&mutexC);
    }
    if(porzioni == maxP){ /**se pentola piena*/
      /**sveglia tutti*/
      printf("Chiamo i selvaggi addormentati\n");
      while(Ssleep > 0){
        Ssleep--;
        sem_post(&pieno);
      }
    }
  }
}

void *Selvaggio(void* arg){
  int iS = *((int*)arg); /**indice selvaggi*/
  int i;
  for(i = 0; i < ngiri; i++){ /**per il numero di selvaggi*/
    pthread_mutex_lock(&mutexS); /**affamato*/
    /**inizio sezione critica*/
    printf("Il selvaggio N %d ha fame\n", iS);
    if(porzioni == 0){
      printf("Il pentolone e' vuoto, sveglio il cuoco\n");
      sem_post(&vuoto); /**sveglio il cuoco*/
      printf("Attendo che il cuoco prepari\n");
      Ssleep++;
      pthread_mutex_unlock(&mutexS);
      printf("Il selvaggio N %d si addormenta\n", iS); /**debug*/
      sem_wait(&pieno); /**attendo che prepari*/
      printf("Il selvaggio N %d e' stato svegliato\n", iS); /**debug*/
      pthread_mutex_lock(&mutexS);
    }
    pthread_mutex_lock(&mutexC);
    printf("Prendo una porzione\n");
    printf("IL selvaggio N %d mangia per la %d volta\n", iS, i+1);
    porzioni--; /**si appropria di una porzione*/
    pthread_mutex_unlock(&mutexC);
    /**inizio sezione critica*/
    printf("porzioni = %d\n", porzioni); /**debug*/
    /**fine sezione critica*/
    pthread_mutex_unlock(&mutexS);
  }
  pthread_exit(NULL);
}


void main(int argc, char* argv[]){
  int maxP, nselvaggi;
  int ret, i;
  if (argc!=4){
    printf("Numero di argomenti non corretto\n");
    printf("Inserie: numero selvaggi, dimensione pentolone, n di volte in cui un selvaggio ha fame\n");
    exit(-1);
  }
  sem_init(&pieno, 0, 0);
  sem_init(&vuoto, 0, 0);
  pthread_mutex_init(&mutexS, NULL);
  pthread_mutex_init(&mutexC, NULL);
  nselvaggi = atoi(argv[1]);
  maxP = atoi(argv[2]);
  ngiri = atoi(argv[3]);
  pthread_t tcuoco;
  pthread_t tselvaggio[nselvaggi - 1];
  Ssleep = 0; /**conta i selvaggi addormentati in attesa del cibo*/
  porzioni = maxP;
  printf("Creo il cuoco\n");/**debug*/
  ret = pthread_create(&tcuoco, NULL, Cuoco, &maxP);
  if(ret != 0){
    printf("ERRORE creazione cuoco\n");
    exit(-1);
  }
  printf("Inizio a creare i selvaggi\n");/**debug*/
  for(i = 0; i < nselvaggi; i++){
    ret = pthread_create(&tselvaggio[i], NULL, Selvaggio, &i);
    printf("SELVAGGIO N %d PENSA --\n", i);
    if(ret != 0){
      printf("ERRORE creazione selvaggio\n");
      exit(-1);
    }
  }
  for(i = 0; i < nselvaggi; i++){
    ret = pthread_join(tselvaggio[i], NULL);
    if(ret != 0){
      printf("ERRORE\n");
      exit(-1);
    }
  }
  printf("Tutti i selvaggi hanno mangiato\n");
}
