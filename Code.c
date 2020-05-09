#include<sys/types.h> 
#include<sys/sem.h> 
#include<sys/ipc.h> 
#include<unistd.h> 
#include<pthread.h> 
#include<stdio.h> 
#include<stdlib.h> 
typedef pthread_mutex_t Mutex; 
typedef pthread_cond_t Cond; 
typedef struct
{ 
int value,wakeup; Mutex *mutex; Cond *cond; 
}Semaphore; 
int oxygen=0,hydrogen=0; 
Semaphore *mutex,*barrier,*oxyQueue,*hydroQueue; 
pthread_t oxyThread,hydroThread1,hydroThread2; 
Mutex* make_mutex(void); 
Mutex* make_mutex(void) 
{ 
int n; 
Mutex* mutex=(Mutex*)malloc(sizeof(Mutex)); 
n=pthread_mutex_init(mutex,NULL); 
return(mutex); 
} 
void mutex_lock(Mutex *mutex) 
{ 
int n=pthread_mutex_lock(mutex); 
} 
void mutex_unlock(Mutex *mutex) 
{ 
int n=pthread_mutex_unlock(mutex); 
} 
Cond* make_cond(void) 
{ 
int n; 
Cond* cond=(Cond*)malloc(sizeof(Cond)); 
n=pthread_cond_init(cond,NULL); 
return(cond); 
} 
void cond_wait(Cond* cond,Mutex* mutex) 
{ 
int n=pthread_cond_wait(cond,mutex); 
} 
void cond_signal(Cond* cond) 
{ 
int n=pthread_cond_signal(cond); 
} 
Semaphore* make_semaphore(int value) 
{ 
Semaphore* semaphore=(Semaphore*)malloc(sizeof(Semaphore)); 
semaphore->value=value; 
semaphore->wakeup=0; 
semaphore->mutex=make_mutex(); 
semaphore->cond=make_cond(); 
return(semaphore); 
} 
void sem_wait(Semaphore *semaphore) 
{ 
mutex_lock(semaphore->mutex); 
semaphore->value--; 
if(semaphore->value < 0) 
{ 
do
	{ 
	cond_wait(semaphore->cond,semaphore->mutex); 
	}
while (semaphore->wakeup < 1); 
semaphore->wakeup--; 
} 
mutex_unlock(semaphore->mutex); 
} 
void sem_signal(Semaphore* semaphore) 
{ 
mutex_lock(semaphore->mutex); 
semaphore->value++; 
if(semaphore->value<=0) 
{ 
semaphore->wakeup++; 
cond_signal(semaphore->cond); 
} 
mutex_unlock(semaphore->mutex); 
} 

int bond(void) 
{ 
static int i=0; 
i++; 
if((i%3)==0)
printf("** Molecule no. %d created**\n\n",i/3); 
sleep(2); 
return(0); 
} 
void* oxyFn(void* arg) 
{
while(1)
{ 
sem_wait(mutex); 
oxygen+=1; 
if(hydrogen>=2) 
{ 
sem_signal(hydroQueue); 
sem_signal(hydroQueue); 
hydrogen-=2; 
sem_signal(oxyQueue); 
oxygen-=1; 
} 
else 
{ 
sem_signal(mutex);
} 
sem_wait(oxyQueue); 
printf("Oxygen Bond\n"); 
bond(); 
sem_signal(mutex); 
} 
} 
void* hydroFn(void* arg) 
{
while(1)
{ 
sem_wait(mutex); 
hydrogen+=1; 
if(hydrogen>=2 && oxygen>=1) 
{ 
sem_signal(hydroQueue); 
sem_signal(hydroQueue); 
hydrogen-=2; 
sem_signal(oxyQueue); 
oxygen-=1; 
} 
else 
{ 
sem_signal(mutex);
} 
sem_wait(hydroQueue); 
printf("Hydrogen Bond\n"); 
bond(); 
} 
} 
main() 
{ 
void *arg; 
while(1)
{
mutex=make_semaphore(1); 
barrier=make_semaphore(0); 
oxyQueue=make_semaphore(0); 
hydroQueue=make_semaphore(0); 
pthread_create(&oxyThread,0,oxyFn,arg); 
pthread_create(&hydroThread1,0,hydroFn,arg); 
pthread_create(&hydroThread2,0,hydroFn,arg); 
}
}
