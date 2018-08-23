#ifndef _SERVICE_H_
#define _SERVICE_H_

void*	camera_service(void*);
void*   detect_service(void*);
void*	irq_service(void*);
void   write_net_string(int fd, char* str);
void    write_net_image();

static inline void start_thread_cont(int* cont){
    *cont = 1;
}

static inline void stop_thread_cont(int* cont){
    *cont = 0;
}

#define  delete_enter_key(str)          for(int i=0; i < MAX_STRING ; i++ )             \
                                                if(str[i] =='\n' || str[i] =='\r')      \
                                                {                                       \
                                                        str[i]=0;                       \
                                                        break;                          \
                                                }


#define THREAD_LOCK	pthread_mutex_lock(&mutex)
#define THREAD_UNLOCK	pthread_mutex_unlock(&mutex)
#define THREAD_WAIT	pthread_cond_wait(&cond,&mutex);
#define THREAD_SIG	pthread_cond_signal(&cond);

#endif

