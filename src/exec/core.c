#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "service.h"
#include "network.h"
#include "queue.h"

FILE *__FILE_POINTER; //Logger
int current_status = 0; //if detected =>1 else 0
int thread_cont;
pthread_cond_t cond; //conditions to wake thread
pthread_mutex_t mutex;
queue_t irq_queue; //irq handler queue

static void save_pid(void);
static void delete_pid(void);

int main(int argc, char *argv[])
{

pthread_t  network, camera, detection, irq;
int ret[4], __pipe[2];

save_pid();
queue_create(&irq_queue);
pthread_mutex_init(&mutex, NULL);
pthread_cond_init(&cond,NULL);

start_thread_cont(&thread_cont);

if(pipe(__pipe) == -1)
	return -1;

        if(pthread_create(&irq,NULL,irq_service,(void*)NULL))
		return -2;
	if(pthread_create(&camera,NULL,camera_service,(void*)&__pipe[0]))	//at fails it returns > 0
		return -3;
	if(pthread_create(&detection,NULL,detect_service,(void*)&__pipe[1]))
		return -4;
	if(pthread_create(&network,NULL,network_service,(void*)argv[1]))
		return -5;
	


pthread_join(camera,(void**)&ret[1]);
pthread_join(detection,(void**)&ret[2]);
pthread_join(irq,(void**)&ret[3]);
pthread_join(network,(void**)&ret[0]);
delete_pid();
}


static void save_pid(void){
    
    FILE* fp = fopen("./Temp/pid","wt");
    if(fp !=NULL)
        fprintf(fp,"%d",getpid());
    fclose(fp);
    
}

static void delete_pid(void){
    unlink("./Temp/pid");
    
}