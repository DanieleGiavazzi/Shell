#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_mutex_t pentolone, selvaggi;
sem_t cuocoInizio, cuocoFine;
int porzioni, appetito;

void *fCuoco (void *arg){
  int* max= (int*)arg;
  int j;
  while(1){
    sem_wait(&cuocoInizio);//attende che un selvaggio svegli il cuoco
    if(porzioni == 0){
      printf("|\t\t\t|\t\t\t|\t\t\t|Il cuoco cucina\t|\n");
      printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
      pthread_mutex_lock(&pentolone);
      porzioni = *max;// ripristino il valore massimo delle porzioni
      printf("|\t\t\t|\t\t\t|\t\t\t|Il cuoco ha cucinato\t|\n");
      printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
      pthread_mutex_unlock(&pentolone);
      sem_post(&cuocoFine);//sveglia il selvaggio che ha svegliato il cuoco
    }
  }
}

void *fSelvaggio (void *arg){
  int k, id = *((char*)arg);
  for (k=0; k<appetito; k++){
    sleep((int)(random()%10)+1);// attende da 1 a 10 prima di avere fame
    printf("|S%d ha fame perl la V%d\t|\t\t\t|\t\t\t|\t\t\t|\n", id, k+1);
    printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
    pthread_mutex_lock(&selvaggi);
    if (porzioni == 0){
      sem_post(&cuocoInizio);//sveglia cuoco
      sem_wait(&cuocoFine);//aspetta che il cuoco prepari le porzioni
    }
    pthread_mutex_lock (&pentolone);
    porzioni--;
    pthread_mutex_unlock (&pentolone);
    pthread_mutex_unlock(&selvaggi);
    printf("|\t\t\t|S%d mangia per la V%d\t|\t\t\t|\t\t\t|\n", id, k+1);
    printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
    sleep((int)(random()%10)+1);// mangia per un numero di secondi variabile tra 1 e 10
    printf("|\t\t\t|\t\t\t|S%d ha finito per la V%d\t|\t\t\t|\n", id, k+1);
    printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
  }
  printf("|\t\t\tS%d\tE'\tSAZIO\tNON\tMANGERA'\tPIU'\t\t\t|\n", id);
  printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
  pthread_exit(NULL);
}

void main(int argc, char* argv[]){
  int n_selvaggi, valRitThread;
  int i;
  if (argc!=4){
    fprintf(stderr,"Numero di argomenti non corretto\n");
    fprintf(stderr,"Primo parametro il numero di selvaggi [MAX 256]\n");
    fprintf(stderr,"Secondo parametro il numero di porzioni\n");
    fprintf(stderr,"Terzo parametro quante volte mangiano i selvaggi\n");
    exit(1);
  }
  sem_init(&cuocoInizio, 0, 0);//inizializzo il semaforo per sveglaire il cuoco a 0
  sem_init(&cuocoFine, 0, 0);//inizializzo il semaforo per comunicare che il cuoco ha finito a 0
  pthread_mutex_init(&selvaggi, NULL);//inizializzo il mutex per i selvaggi
  pthread_mutex_init(&pentolone, NULL);//inizializzo il mutex per il pentolone
  appetito=atoi(argv[3]);//setto il numero di volte che devono mangiare i selvaggi
  porzioni = atoi(argv[2]);//setto il parametro numreo di porzioni
  n_selvaggi=atoi(argv[1]);//Setto il parametro numero selvaggio
  int s_selvaggi [n_selvaggi-1];
  pthread_t tSelvaggio[n_selvaggi-1]; //gestisco un array di thread pari al numero di selvaggi
  pthread_t tCuoco;
  int pMax = porzioni;//imposto il valore massimo delle porzioni
  valRitThread=pthread_create (&tCuoco, NULL, fCuoco, &pMax);
  if (valRitThread!=0){//controllo il risultato dellacreazione del thread cuoco
    fprintf(stderr, "Errore creazione cuoco\n");//se diverso da 0 fornisco un messaggio ed esco
    exit(-1);
  }
  printf("-------------------------------------------------------------------------------------------------\n");
  printf("|SELVAGGIO CHE HA FAME\t|SELVAGGIO CHE MANGIA\t|SELVAGGIO NON AFFAMATO\t|\tCUOCO\t\t|\n");
  printf("|-----------------------|-----------------------|-----------------------|-----------------------|\n");
  for(i=0; i<n_selvaggi; i++){
    s_selvaggi[i]=i;
    valRitThread=pthread_create(&tSelvaggio[i], NULL, fSelvaggio, &s_selvaggi[i]);//genero i thread selvaggi
    if (valRitThread!=0){//controllo il risultato della attesa del thread
      fprintf(stderr, "errore con la creazione del thread selvaggio %d\n", i+1);//se diverso da 0 fornisco un messaggio ed esco
      exit(-1);
    }
  }
  for(i=0; i<n_selvaggi; i++){
    valRitThread=pthread_join(tSelvaggio[i], NULL); //attendo la fine dei selvaggi
    if (valRitThread!=0){//controllo il risultato della attesa del thread
      fprintf(stderr, "errore con i thread\n");//se diverso da 0 fornisco un messaggio ed esco
      exit(-1);
    }
  }
  printf("\t##########\tTUTTI I SELVAGGI SONO SAZI\t##########\n");
}
