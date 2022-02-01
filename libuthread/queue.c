#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define ERROR -1;
#define SUCCESS 0;

struct queue {
	struct node* front; 
	struct node* end; 
	int length; 
};

struct node {
	struct node* next; 
	struct node* prev; 
	void* data; 
};


queue_t queue_create(void)
{
	// struct queue *myQueue = malloc(sizeof(*queue_t));
	queue_t myQueue = (queue_t) malloc(sizeof(struct queue));
	myQueue->front = NULL; 
	myQueue->end = NULL; 
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
	/* Checks if queue or data is NULL */
	if (queue == NULL || data == NULL) 
		return -1;
	 
	if (queue->length == 0) {
		/* Creates new node  */
		struct node* newNode = malloc(sizeof(struct node)); 
		newNode->next = NULL; 
		newNode->prev = NULL;
		newNode->data = data; 
		/* Sets the queue */
		queue->front = newNode; 
		queue->end = newNode; 
		queue->length = 1; 
	} else {
		/* Creates new node  */
		struct node* newNode = malloc(sizeof(struct node)); 
		newNode->next = NULL;
		newNode->data = data; 

		/* Reroute the pointers */
		newNode->prev = queue->end;
		queue->end->next = newNode; 

		/* Sets the queue */
		queue->end = newNode; 
		queue->length = queue->length + 1; 
	}
	
	return SUCCESS;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->length == 0) 
		return -1;
	
	struct node* nextNode = queue->front->next; 
	free(queue->front); 
	queue->front = nextNode; 
	nextNode->prev = NULL; 
	
	return SUCCESS;
}

int queue_delete(queue_t queue, void *data)
{
	
	if (queue == NULL || data == NULL || queue->length == 0) 
		return -1;

	struct node* currentNode; 
	currentNode = queue->front; 
	
	while(currentNode != NULL) {
		if (currentNode->data == data) {
			/* Save the next and previous nodes */
			struct node* prevNode = currentNode->prev; 
			struct node* nextNode = currentNode->next;  
			/* Reset the pointers */
			prevNode->next = nextNode; 
			nextNode->prev = prevNode; 
			/* Free the memory */
			free(currentNode); 
		}
		/* Iterate through the linked list */
		currentNode = currentNode->next; 
	}
	
	return SUCCESS;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{

	if (queue == NULL || func == NULL) 
		return ERROR; 

	/* Creating node */
	struct node* currentNode; 
	currentNode = queue->front;

	while(currentNode != NULL) {

		/* Running the callback function and getting return value */
		int returnValue = func(queue, currentNode->data, arg);

		/* If return value is 1, we set the data to current node's data and stop iteration */
		if (returnValue) {
			*data = currentNode->data; 
			break; 
		}
	}
	
	return SUCCESS;
}

int queue_length(queue_t queue)
{
	return queue->length;
}

