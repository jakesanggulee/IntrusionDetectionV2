#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "service.h"
#include "queue.h"
#include "sanggu.h"

extern queue_t irq_queue;
extern pthread_mutex_t	mutex;
extern pthread_cond_t	cond;
extern int thread_cont;

static int init_network(int*,int*,struct sockaddr_in*, struct sockaddr_in*);
static int wait_for_clnt(int *, int *, int *,struct sockaddr_in*,struct sockaddr_in*,int*);
static int parse_net_string(int *clnt_fd);


void network_service(void* arg)
{

int serv_fd, clnt_fd, len, serv_port = atoi((char*)arg);
struct sockaddr_in serv_addr, clnt_addr;
queue_element irq_net={IRQ_NET,-1,0}; //PID, no_req initialized

    if( init_network(&serv_fd,&serv_port,&serv_addr,&clnt_addr) ==-1 ){
                irq_net.req = IRQ_NET_INIT_ERR;
        	queue_enqueue(&irq_queue,irq_net);
    }
    else
    {
            while(1)
            {  
                if(    wait_for_clnt(&serv_fd,&clnt_fd,&serv_port,&serv_addr,&clnt_addr,&len)    )
                {
                    irq_net.req = parse_net_string(&clnt_fd);
                    irq_net.flg = clnt_fd;
                    queue_enqueue(&irq_queue,irq_net); //irq_service will handle the interruption..
                    
                    if(irq_net.req == IRQ_NET_SHUTDOWN)
                        return;

                }
                else{
                    irq_net.req = IRQ_NET_WAIT_ERR;
                    queue_enqueue(&irq_queue,irq_net);
                }
           
            }
            
    }
}

static int init_network(int *serv_fd, int *serv_port,struct sockaddr_in *serv_addr,struct sockaddr_in *clnt_addr)
{
	if( (*serv_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1   )
		return -1;
        
	memset(serv_addr, 0, sizeof(*serv_addr));
	memset(clnt_addr, 0, sizeof(*clnt_addr));
        
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr->sin_port = htons(*serv_port);

 	if (   bind(*serv_fd, (struct sockaddr*)serv_addr, sizeof(*serv_addr)) < 0   )
            return -1;
	if (   listen(*serv_fd, 5)<0    )
            return -1;
 
}


static int wait_for_clnt(int *serv_fd, int *clnt_fd ,int *serv_port,struct sockaddr_in *serv_addr,struct sockaddr_in *clnt_addr,int * len)
{
        *len = sizeof(*clnt_addr);
	if( (*clnt_fd = accept(*serv_fd, (struct sockaddr*)clnt_addr, len)) == -1   )
		return 0;
        else
		return 1;
}

static  int parse_net_string(int *clnt_fd)
{
        char  recevbuff[MAX_STRING];
        
//FIXME :IF READ ONLY FEW CHARACTERS???........................................................................
	int n = read(*clnt_fd,recevbuff,MAX_STRING-1);
        recevbuff[n]='\0';
        
	if 	(  !strcmp(recevbuff, "status"	)  )
		return 	0;
        else if (  !strcmp(recevbuff,"imagereq" )  )
		return 	1;
        else if (  !strcmp(recevbuff,"stopservice\r\n" )  )
		return 	255;
        else if (  !strcmp(recevbuff,"stopservice" )  )
		return 	255;
	else
		return -1;
}


void   write_net_string(int fd, char *str){
    
    write(fd,str,strlen(str));
    
}

void    write_net_image(int fd,char *img){
    
    int readn, data_fd = open(img,O_RDONLY);
    char data[MAX_DATA];
    
    if(data_fd == -1)
        return;
    else
    {    
    while (    (readn = read(data_fd, data, MAX_DATA - 1)) > 0   )
            {
                write(fd, data, readn);
                memset(data, 0, sizeof(data));
            }
    
    }
    close(data_fd);
    
}
