//I declare that this piece of work which is the basis for recognition of achieving learning outcomes in the OPS1 course was completed on my own.[Patryk Prusak] [305794]

#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#define SIZE 10
void read_arguments(int argc, char**argv, int *n, int*m, int* thread_count);
void* thread_work(void*arg);
void sig_handler(int sig);
void sethandler( void (*f)(int), int sigNo);
void print_table(char **arr, int n, int m);


volatile sig_atomic_t last_signal = 0;
typedef struct data
{
    char **arr;
    int n;
    int m;
    int seed;
    sigset_t  mask;
    pthread_mutex_t *mxarray;


} data_t;
int main(int argc, char**argv)
{
    int thread_count;
    int n,m;
    srand(time(NULL));
    read_arguments(argc,argv,&n,&m,&thread_count);

    data_t *data=malloc(sizeof(data_t));
    if(data==NULL)ERR("data malloc");


    char** table;

    table = malloc(n * sizeof(*table));
    if(table==NULL)ERR("table malloc");
    for (int i = 0; i < n; i++)
    {
        table[i] = malloc(m * sizeof(table[0]));
        if(table[i]==NULL)ERR("table malloc");
    }
    int lett=rand()%(122-97+1)+97;
    char letter=lett;
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            lett=rand()%(122-97+1)+97;
            letter=lett;
            table[i][j]=letter;
        }

    }
    data->mxarray=malloc(n*sizeof(pthread_mutex_t));
    if(data->mxarray==NULL)ERR("mutex malloc");
    for(int i=0; i<n; i++)
    {
        pthread_mutex_init(&data->mxarray[i],NULL);

    }
    data->arr=table;
    data->m=m;
    data->n=n;
    data->seed=rand();


    sigset_t mask, oldmask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);
    if(pthread_sigmask(SIG_BLOCK, &mask, &oldmask))
        ERR("SIGMASL");
    data->mask=mask;

    pthread_t *tid=malloc(thread_count*sizeof(pthread_t));
    if(tid==NULL)
        ERR("tid malloc");
    for(int i=0; i<thread_count; i++)
    {
        if(pthread_create(&tid[i],NULL,thread_work,data))ERR("Thread creation");
    }
    char command[SIZE];
    while(1)
    {
        fgets(command,SIZE,stdin);
        struct timespec t= {0,500000};
        if(!strncmp("print",command,5))
        {
            print_table(table,n,m);
        }
        else if(!strncmp("replace",command,7))
        {

            int number = command[8]-'0';
            for(int j=0; j<number; j++)
            {
                nanosleep(&t,NULL);
                kill(getpid(),SIGUSR1);
            }

        }
        else if(!strncmp("exit",command,4))
        {
            for(int j=0; j<thread_count; j++)
                pthread_kill(tid[j],SIGINT);
            break;




        }
    }
    for(int i=0; i<thread_count; i++)
    {
        if(pthread_join(tid[i],NULL))ERR("Thread joining");
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}
void read_arguments(int argc, char**argv, int *n, int*m, int* thread_count)
{

    if(argc!=4)
        ERR("Wrong number of arguments, 1st - number of rows , 2nd - number of columns, 3rd - number of threads");
    *thread_count=atoi(argv[3]);
    *n=atoi(argv[1]);
    *m=atoi(argv[2]);
    if(*n<1||*n>100)
        ERR("Wrong range of n [1,100]");
    if(*m<1||*m>100)
        ERR("Wrong range of m [1,100]");
    if(*thread_count<3||*thread_count>100)
        ERR("Wrong range of m [3,100]");
}
void* thread_work(void*arg)
{
    data_t *data=arg;
    int signo;

    while(1)
    {
        while(signo!=SIGUSR1&&signo!=SIGINT)
        {

            sigwait(&data->mask,&signo);

        }

        if(signo==SIGUSR1)
        {
            int r1=rand_r(&data->seed)%data->n;
            int r2=rand_r(&data->seed)%data->m;
            pthread_mutex_lock(&data->mxarray[r1]);
            int lett=rand_r(&data->seed)%(122-97+1)+97;
            char letter=lett;
            char old=data->arr[r1][r2];
            data->arr[r1][r2]=letter;

            printf("[TID %d] Received SIGUSR1: Replaceing '%c' to '%c' at index [%d,%d]\n",pthread_self(),old,letter,r1,r2);
            signo=0;
            pthread_mutex_unlock(&data->mxarray[r1]);
        }
        if(signo==SIGINT)
        {
            printf("[TID %d] Received SIGINT: Exiting\n",pthread_self());
            signo=0;
            return NULL;

        }
    }


}

void print_table(char **arr, int n, int m)
{

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            printf("%c",arr[i][j]);
        }
        printf("\n");
    }

}
void sig_handler(int sig)
{
    last_signal=sig;
}
void sethandler( void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1==sigaction(sigNo, &act, NULL)) ERR("sigaction");
}
