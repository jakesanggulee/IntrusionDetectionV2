
#include <pthread.h>
#include <stdlib.h>
#include "service.h"

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern int thread_cont;

void* camera_service(void* arg)
{
//int* fd = (int*)arg;
THREAD_LOCK;
THREAD_UNLOCK;


while(thread_cont)
    {
    system("raspistill -o ./Temp/image1.bmp -e bmp -h 102 -w 102 -n -t 1");
    system("raspistill -o ./Temp/image2.bmp -e bmp -h 102 -w 102 -n -t 1");
    }    
}
