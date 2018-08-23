#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct _queue_element{
int pid; //pid=0 network, pid=1, cam pid=2, ....
int req; //request
int flg;
} queue_element;

typedef struct _queue_node{

queue_element *data;
struct _queue_node *lnode;
struct _queue_node *rnode;
} queue_node;

typedef struct _queue_t{
queue_node* front;
queue_node* back;
int size;
} queue_t;


void queue_create(queue_t *queue);
void queue_enqueue(queue_t *queue ,queue_element data);
queue_node* queue_dequeue(queue_t* queue);
void queue_print(queue_t* queue);
void queue_free(queue_node* node);

//Define IRQ REQUEST
#define     DETECTED    1
#define     SAFE        0



#define     IRQ_NET     0
#define     IRQ_CAM     1
#define     IRQ_DET     2


#define     IRQ_NET_STATUS_REQ          0
#define     IRQ_NET_IMAGE_REQ           1
#define     IRQ_NET_INIT_ERR            2
#define     IRQ_NET_WAIT_ERR            3
#define     IRQ_NET_SHUTDOWN            255


#define     IRQ_DET_DETECT    1
#define     IRQ_DET_NODETECT    0


#endif
