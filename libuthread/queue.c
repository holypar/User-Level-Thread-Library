#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"


struct queue {
	// void* front; 
	// void* end;
	// actual value would be an array* 
	// dataList: void* listName[]
	// splitList: char* splitList[]
	// data type void
	// int length 
};

queue_t queue_create(void)
{
	/* TODO */
	// we create the queue and allocate memory here theQueue
	// theQueue->front = theQueue
	// rest of the values would be none.
	// return theQueue
	// struct queue *queueName = malloc(sizeof *queueName);
	// queue_t is the same as (queue*)
	// queue_t myqueue = (queue_t) malloc(sizeof(queue))
	return NULL;
}

int queue_destroy(queue_t queue)
{

	//if length of queue is > 0 raise error :it shoudlnt run
	//else free(queue)

	/* TODO */
	return -1;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO */
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
	/* TODO */
	return -1;
}

