# This is the report #

## Queue Data Structure ## 

### Implementation ### 
For our implementation, we decided to create the queue using an implementation of a doubly linked list. 
Therefore, our queue works with a series of nodes. Each nodes has a pointer to the next node and a pointer to the previous node. 
Therefore, our queue has 3 properties: 
1. node pointer to the top of the queue
2. node pointer to the bottom of the queue 
3. length of the queue 

In this implementation each step works with queues similar to the way it is implemented for a doubly linked list. 

To create a queue, we simply allocate memory on the heap for it and set default node values as NULL and the length to be 0. 

To enqueue, we make a new node (using malloc) with the data from the parameter. Then we attach the pointer of the node at the bottom with the pointers of the new node appropriately. To dequeue, it is similar, but instead detach the node at the bottom, set the value of the data, and the free the node. 

To delete, we would search the entire queue going from each node to the next. Then if the data matches, we save the previous and next nodes. 
Then we reattach the pointers of the previous and next nodes to each other and the free the node to be deleted. 

To iterate through, it is similar to delete function but instead, we go through each node and apply the function one by one to the data. If the function returns a one, we stop premmaturely, set the data value, and break out of the loop. 

### Why we choose this implementation ### 
We decided to use a doubly linked list implementation for a queue because it was easier for our group as a whole to understand. We wanted a data structure that worked with nodes more than a data structure like a dynamic array because it felt easier to think of individual nodes attaching and detaching than working with an array using indexes. 

Additionally, we felt that a dynamic array implementation would have our enqueue function be O(1) amoritized since we would have to dynamically allocate more space at specific intervals of the enqueue which would not make it explicitly O(1). Additionally, it would be impossible to free only parts of the dynamic array and we would have to keep the capacity of the queue large if there was too much enqueuing. 