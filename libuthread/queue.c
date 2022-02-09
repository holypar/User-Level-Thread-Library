#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Defining integers for Errors and Success results */
#define ERROR -1
#define SUCCESS 0

/* Queue Implementation similar to a doubly linked list */

/* 
Structure for queues: 
	pointer to the front, 
	pointer to the back, 
	length of the queue 
*/
struct queue {
	struct node* front; 
	struct node* end; 
	int length; 
};

/* 
Structure for nodes: 
	pointer to the next node, 
	pointer to the previous node, 
	storage for data
*/

struct node {
	struct node* next; 
	struct node* prev; 
	void* data; 
};

queue_t queue_create(void)
{
	/* Mallocs the queue and sets the default values */
	queue_t myQueue = (queue_t) malloc(sizeof(struct queue));
	myQueue->front = NULL; 
	myQueue->end = NULL; 
	myQueue->length = 0; 
	
	return myQueue;
}

int queue_destroy(queue_t queue)
{
	/* Give an error if the queue is not empty or is NULL, otherwise free it*/
	if (queue->length > 0 || queue == NULL)
		return ERROR;

	free(queue);
	
	return SUCCESS; 

}

int queue_enqueue(queue_t queue, void *data)    
{
	/* Checks if queue or data is NULL */
	if (queue == NULL || data == NULL) 
		return ERROR;
	
	/* Creates new node  */
	struct node* newNode = malloc(sizeof(struct node)); 
	newNode->next = NULL;
	newNode->data = data;

	/* Two cases: if the queue is empty or if the queue has nodes  */
	if (queue->length == 0) {
		/* If queue is empty, set the previous pointer to NULL */
		newNode->prev = NULL; 

		/* Sets the queue */
		queue->front = newNode; 
		queue->end = newNode; 
		queue->length = 1; 
	} else {

		/* Reroute the pointers of the end and new node */
		newNode->prev = queue->end;
		queue->end->next = newNode; 

		/* Append the pointer to the end */
		queue->end = newNode; 
		queue->length = queue->length + 1; 
	}
	
	return SUCCESS;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* Checking for a potential error */
	if (queue == NULL || data == NULL || queue->length == 0) 
		return ERROR;
	
	/* Saving the next node */
	struct node* nextNode = queue->front->next; 

	/* Setting the data, free the data and set the pointer */
	*data = queue->front->data; 
	free(queue->front); 
	queue->front = nextNode; 
	
	/* Set the next node's previous pointer */
	if (nextNode != NULL)
		nextNode->prev = NULL; 
	queue->length = queue->length - 1;

	return SUCCESS;
}

int queue_delete(queue_t queue, void *data)
{
	/* Checking for a potential error */
	if (queue == NULL || data == NULL || queue->length == 0) 
		return -1;

	struct node* currentNode; 
	currentNode = queue->front; 

	while(currentNode != NULL) {
		if (currentNode->data == data) {
			/* Save the next and previous nodes */
			struct node* prevNode = currentNode->prev; 
			struct node* nextNode = currentNode->next;  
			
			/* Next we handle edges cases */

			/* If it is not the first item, reattach the pointer */
			if (prevNode != NULL) 
				prevNode->next = nextNode; 
			
			/* If it is not the last item reattach the pointer*/
			if (nextNode != NULL)
				nextNode->prev = prevNode; 
			
			/* If it is the first item reattach the front pointer */
			if (currentNode == queue->front) 
				queue->front = nextNode; 
			
			/* If it is the last item reattach the end pointer */
			if (currentNode == queue->end) 
				queue->end = prevNode;
			
			/* Free the memory */
			free(currentNode); 
			queue->length = queue->length - 1;
			break; 
		}
		/* Iterate through the linked list */
		currentNode = currentNode->next; 
	}
	
	return SUCCESS;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{

	if (queue == NULL || func == NULL || queue->length == 0) 
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

		/* Iterate through the linked list */
		currentNode = currentNode->next;
	}
	
	return SUCCESS;
}

int queue_length(queue_t queue)
{
	return queue->length;
}

