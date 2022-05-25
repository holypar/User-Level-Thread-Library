# This is the report #

## Queue Data Structure ## 

## Phase 1: Queue API ##

### Implementation ### 
For our implementation, we decided to create the queue using an implementation
of a doubly linked list. 

Therefore, our queue works with a series of nodes. Each nodes has a pointer to
the next node and a pointer to the previous node. 

Therefore, our queue has 3 properties: 
1. node pointer to the top of the queue
2. node pointer to the bottom of the queue 
3. length of the queue 

In this implementation each step works with queues similar to the way it is
implemented for a doubly linked list. 

To create a queue, we simply allocate memory on the heap for it and set default
node values as NULL and the length to be 0. 

To enqueue, we make a new node (using malloc) with the data from the parameter.
Then we attach the pointer of the node at the bottom with the pointers of the
new node appropriately. To dequeue, it is similar, but instead detach the node
at the bottom, set the value of the data, and the free the node. 

To delete, we would search the entire queue going from each node to the next.
Then if the data matches, we save the previous and next nodes. Then we reattach
the pointers of the previous and next nodes to each other and the free the node
to be deleted.

To iterate through, it is similar to delete function but instead, we go through
each node and apply the function one by one to the data. If the function
returns a one, we stop premmaturely, set the data value, and break out of the
loop. 

### Why we choose this implementation ### 
We decided to use a doubly linked list implementation for a queue because it
was easier for our group as a whole to understand. We wanted a data structure
that worked with nodes more than a data structure like a dynamic array because
it felt easier to think of individual nodes attaching and detaching than
working with an array using indexes.

Additionally, we felt that a dynamic array implementation would have our
enqueue function be O(1) amoritized since we would have to dynamically
allocate more space at specific intervals of the enqueue which would not make 
it explicitly O(1). Additionally, it would be impossible to free only parts of 
the dynamic array and we would have to keep the capacity of the queue large if 
there was too much enqueuing.

## Phase 2: uthread API ##

### Implementation ### 
For our implementation, we started by creating a TCB struct simply called
thread. 

This struct contains the essentials needed to bring the thread library
to life. 

We have a thread which defines all the characteristics that a thread would 
need. 

Properties of the Thread: 
1. tid: id of thread
2. context: context of thread 
3. stackPointer: pointer to the top of the stack 
4. state: an integer represent what state the thread is in 
5. returnValue: a storage space to store the return value of the thread 
6. dependsOn: a tid which the thread depends on the finish 

We additionally have a scheduler which has queues for different states and 
tracks the active thread running along with others.  

Properties of a Scheduler
1. preempt: integer value of the preemption 
2. threadCounter: number of threads created 
3. readyQueue: queue for threads that are ready 
4. zombieQueue: queue for threads that are zombie which need collection 
5. blockQueue: queue for threads that are blocked and waiting
6. activeThread: thread that is active and running 

For the uthread_start function we start it by checking whether premption is
enabled or not. From there we create our necessary ready, zombie, and blocking
queues. Then we initialize our scheduler and create out mainThread and set its
properties such as having the TID of 0. 

For the uthread_stop function we disable the preemption and then check if there
is something in our ready queue and the thread that is calling it is the main
thread since the threading is complete. From there we free the current thread,
destroy the stack and destroy all the queues. If there are uncollected threads
in the zombie queue when uthread_stop is being called we dequeue all the zombie
threads and then destroy the zombie queue. This process is basically freeing
all the memory that was being used in the heap that was necessary for the
thread library to run.

For the uthread_create function we disable the preemption while a thread is
being created. Our process of creating a thread is basically creating a new
thread using our thread TCB, creating a separate stack for that thread, and
initilizing its context and state. After that process is completed we enable
preemption and return the newly created thread's TID.

For our uthread_yield function we first check if there is a thread in the ready
queue that is ready to become the active thread. Our process consists of
creating a thread pointer for the next thread and setting that equal to the
thread that is in the front of the ready queue. From there we add the current
thread to the back of the ready queue and context switch from the old thread to
the new thread. 
 
For the uthread_self function it simply returns the current active thread's
tid which is located in our scheduler struct.

For the uthread_exit we first disable the preemption process. Then we change
the active thread's state to ZOMBIE because it has returned. We save its
return value and send the active thread into the zombie queue. From there we 
find if a thread was waiting for this thread to complete in the block queue 
using its dependency attritbute, and then send that thread from the blocking 
queue to the ready queue. We found each threads dependency using our helper
function findDependency that bascially saves the threads TID when it is
called. After this process is complete, we send save the next ready thread
from the ready queue, set it to active, and context switch to it.

### Why we implemented this way? ### 

We chose to use three queues to simplify our thought process for the
uthread_join. It was easier for us to think of having queues with similar type
of threads instead of putting everything into one queue where all the threads
have varying states. Since we created the queue_iterate function in our queue 
API, it doesn't hurt to use three queues.

We then created a scheduler which checks if preemption is enabled, a thread
counter, and the three queues which reperesent which states a thread can be in
its life cycle: ready queue, zombie queue, block queue. We also kept track on
the currently active thread in our scheduler. 

## Phase 3: uthread_join() ##

### Implementation ###

For uthread_join there are two possible scenarios. 
The first scenario is that we find the function given tid in the zombie queue 
using queue_iterate, it collects the return value,  deletes that thread from 
the queue, and frees the thread using our helper function freeThread which 
basically frees the context of the thread, destroys the stackm, and then 
finally frees the thread.

The second scenario is that the tid we are looking for was not found in the
zombie queue meaning that process is not fished AKA it is in the ready queue.
Then what happens is that the current thread becomes in the BLOCK state and we
set its dependency to the tid given. From there we enqueue the active thread 
to the back of the blocking queue and schedule the next available ready 
thread to run. The next time it is the blocked process' time to run it will
check the zombie queue for its dependency and collect its return value and do
the same process as the first scenario for freeing the thread.

### Why we implementated this way? ###

We decided to implement join this way in order to deal with the two distinct 
options. Additionally, it was easier to queue iterate through distinct queues
instead of one queue. We stored a dependency tid in the thread which made it
much easier to track which thread depended on which to finish. Additionally, 
it was easy to iterate through everything since we had distinct queues holding threads with separate states. However, the real reason why we implemented this
way was because we went to office hours and the professor recommended this 
implementation. 

### How test_uthread_lib tests the join ###
This tester is very similar to the test preemption tester which we discuss down below, but this tester specifically checks whether the blocking mechanism works correctly when a thread calls join on another thread that has not completed yet. The tester works great with prememption enabled or disabled. Basically whath happens is that main creates thread1 and join on it waiting for it to finish. When the library context switches to thread 1, it creates thread 2 and joins upon that thread. This sends thread1 into the blocking queue as well with main. So thread 2 becomes the active thread outputs it has finished and sends it return value to thread 1 to be collected. From there thread1 outputs that it has finished and main collects its return value and finishes by outputting "This is main"

## Phase 4 ##
For this phase, we set up a timer and a sigaction SIGVTALRM. In preempt_start, we just initialize these steps. We make sure to have preemption off while doing this process and turn it back on during uthread_start(). 

For preempt_stop, it is just restoring the previous sigaction and timer values back to their original phases. 

Preempt_disable and preempt_enable are the same in which they block and unblock signals of SIGVTALRM respectively. 

Since the majority of our code dealt with scheduler which was a main sharing data strucuture, it was hard to use it discretely. Therefore, wherever we started to use the scheduler, we did preempt disable. Then, we would enable it back afterwards usually just before context switching or near the end of the function. 

### Why we implementated this way? ###

We implemented this way just simply by following what was recommended from the 
prompt. Using a signal action along with a timer was very simple in terms of 
complexity. All we had to do was block the signal or unblock the signal.
Since everything was already set up at the start, the rest of the code was easy
to understand and implement so therefore we chose this method. 

### How test_preempt showcases preemption ###

We included a test_preempt.c file which showcases our preemption working just 
fine. It is already added as part of the Makefile in apps to produce an 
executable. We create thread 1 which creates thread 2. In thread one, we have 
an infinite while loop so it never finishes. In the case of preemption, we
would get an infinite loop with nothing printing. However, we see that if preemption is turned on, thread 2 finishes, and then the infinite loop happens.
Therefore, this proves the thread 2 was yield to by the preemption timer thus, allowing it to print. 
