#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define ERROR -1;
#define SUCCESS 0;

struct queue {
	int front; 
	int end;
	void** data; 
	int length; 
	
};

/*
front and back are indexes 
front of the space = data[front] 
back of the space = data[back]

enqueue: 
If there is no data, malloc for one element, 
Front and back = 0; 
If there is data, realloc for back + 1 element, back = back + 1

dequeue: 
If there is data, we just move the pointers, front = front + 1
If there is only one item left, (length == 1), return the value 
Front and back = NULL, free(queue->data), queue->data = NULL 


*/

queue_t queue_create(void)
{
	// struct queue *myQueue = malloc(sizeof(*queue_t));
	queue_t myQueue = (queue_t) malloc(sizeof(struct queue));
	myQueue->front = NULL; 
	myQueue->end = NULL; 
	myQueue->data = NULL; 
	myQueue->length = 0; 
	
	return myQueue;
}

int queue_destroy(queue_t queue)
{
	if (queue->length > 0 || queue == NULL)
		return ERROR;
	
	free(queue);
	
	return SUCCESS; 

}

int queue_enqueue(queue_t queue, void *data)    // 
{
	if (queue == NULL || data == NULL) 
		return -1;
	 
	if (queue->data == NULL) {
		queue->data = (void**) malloc(sizeof(void*)); 
		queue->front = 0; 
		queue->end = 0;
		queue->data[0] = data;
		queue->length = 1; 
	} else {
		queue->end = queue->end + 1;
		queue->data = (void**) realloc(queue->data, queue->end);
		queue->data[queue->end] = data;
		queue->length = queue->length + 1;  
	}
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL) 
		return -1;

    *data = queue->data[queue->front]; 
	queue->front = queue->front + 1; 
	queue->length = queue->length - 1;
	
	if (queue->length = 0) {
		free(queue->data); 
		queue->data = NULL;
		queue->front = 0; 
		queue->end = 0; 
	}

	return -1;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO */
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	/* TODO */
	return -1;
}

int queue_length(queue_t queue)
{
	return queue->length;
}

