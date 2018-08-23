#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"
#include "service.h"

extern queue_t irq_queue;
extern int current_status;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern int thread_cont;

static void perform_task(int,int,int);


void* irq_service(void* arg)
{

THREAD_LOCK; //Block until all services are deployed..
THREAD_UNLOCK;

while(thread_cont)
{
	if( irq_queue.size > 0 )
	{
	queue_node *task = queue_dequeue(&irq_queue);
	perform_task(task->data->pid,task->data->req,task->data->flg); 
	queue_free(task);
	}

}

}


static void perform_task(int pid, int req, int clnt_fd)
{

switch(pid)
{
	case IRQ_NET:
           switch(req)
           {
               //THREAD_LOCK; //Program may not work properly when preemtped by network service
               case IRQ_NET_STATUS_REQ: 
                   if(current_status == DETECTED){
                       write_net_string(clnt_fd,"status not ok");
                       close(clnt_fd);
                   }
                   else{
                       write_net_string(clnt_fd,"status ok");
                       close(clnt_fd);
                   }
                   break;
               case IRQ_NET_IMAGE_REQ:
               {
                       write_net_image(clnt_fd,"./Temp/image2.bmp");
                       close(clnt_fd);
               }
                   break;
               case IRQ_NET_SHUTDOWN:   
               {
                        close(clnt_fd);
                        stop_thread_cont(&thread_cont);   
               }
                    break;
               default:
			close(clnt_fd);
	           break;
                 //THREAD_UNLOCK;
           } 
            break;
            
	case IRQ_CAM:		

	break;

	case IRQ_DET:		
            switch(req)
            {
                case IRQ_DET_DETECT :
                    current_status = 1;
                    system("php ./php/push_notification.php >/dev/null 2>&1");
                break;
                default :   
                    current_status =0;       
                break;        
            }
            
	break;

	case 3:		//others...

		break;

}
}
