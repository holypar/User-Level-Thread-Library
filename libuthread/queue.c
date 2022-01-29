#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define ERROR -1;
#define SUCCESS 0;

struct queue {
	void* front; 
	void* end;
	void** data; 
	int length; 
	// actual value would be an array* 
	// dataList: void* listName[]
	// splitList: char* splitList[]
	// data type void
	// int length 
};

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

int queue_enqueue(queue_t queue, void *data)
{
	
	return -1;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO */
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

