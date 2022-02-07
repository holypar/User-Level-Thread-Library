#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Enqueue/Dequeue Difficult */
void test_queue_difficult(void) 
{
	/* Enqueues 3 data, and then dequeues  */
	int data1 = 1; 
	int data2 = 2; 
	int data3 = 3; 
	int *ptr; 
	queue_t q; 

	fprintf(stderr, "*** TEST queue_difficult ***\n");
	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3); 
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data1);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data2);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data3);
	queue_destroy(q); 
}

/* Enqueue/Dequeue Moderate */
void test_queue_moderate(void) {
	int data1 = 1; 
	int data2 = 2; 
	int data3 = 3; 
	int *ptr; 
	queue_t q; 

	fprintf(stderr, "*** TEST queue_moderate ***\n");
	q = queue_create();

	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data1);
	queue_enqueue(q, &data3); 
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data2);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data3);
	queue_destroy(q); 
}

/* Enqueue/Dequeue Destroy Errors*/
void test_queue_destroy_errors() {
	int data1 = 1;
	int *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_destroy_errors ***\n");
	q = queue_create();
	queue_enqueue(q, &data1);
	TEST_ASSERT(queue_length(q) == 1); 

	int result1 = queue_destroy(q); 
	TEST_ASSERT(result1 == -1); 

	queue_dequeue(q, (void**)&ptr);

	int result2 = queue_destroy(q);
	TEST_ASSERT(result2 == 0); 

}

/* Testing Enqueue Errors */
void test_queue_enqueue_errors(void) {
	queue_t q = NULL;
	
	fprintf(stderr, "*** TEST queue_enqueue_errors ***\n");
	q = queue_create();

	/* Testing to enqueue nothing*/
	int result = queue_enqueue(q, NULL); 
	TEST_ASSERT(result == -1); 
	TEST_ASSERT(queue_length(q) == 0);
	queue_destroy(q); 
}

/* Testing Dequeue Errors */
void test_queue_dequeue_errors(void) {
	queue_t q = NULL;
	int data1 = 1;
	int *ptr;

	fprintf(stderr, "*** TEST queue_dequeue_errors ***\n");
	/* Testing dequeue if nothing is inside */
	q = queue_create();
	int result1 = queue_dequeue(q, (void**)&ptr); 
	TEST_ASSERT(result1 == -1); 
	TEST_ASSERT(queue_length(q) == 0);

	queue_enqueue(q, &data1);
	TEST_ASSERT(queue_length(q) == 1); 

	/* Testing dequeue if data I passed is a null*/
	int result2 = queue_dequeue(q, NULL);
	TEST_ASSERT(result2 == -1); 

	/* Testing normal dequeue */
	int result3 = queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(result3 == 0);
	TEST_ASSERT(queue_length(q) == 0); 
	queue_destroy(q);
}


/* Testing delete function */
void test_queue_delete(void) {
	queue_t q = NULL;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	int *ptr;

	fprintf(stderr, "*** TEST queue_delete ***\n");
	q = queue_create();
	queue_enqueue(q,&data1);
	queue_enqueue(q,&data2);
	queue_enqueue(q,&data3);
	queue_delete(q,&data2);
	queue_dequeue(q, (void**)&ptr); 
	TEST_ASSERT(ptr == &data1); 
	queue_dequeue(q, (void**)&ptr); 
	TEST_ASSERT(ptr == &data3); 
	queue_destroy(q); 
}

/* Testing delete function with duplicates */
void test_queue_delete_duplicates(void) {
	queue_t q = NULL;
	int data1 = 1;
	int data2 = 2;
	int data3 = 3;
	int *ptr;

	fprintf(stderr, "*** TEST queue_delete duplicate ***\n");
	q = queue_create();
	queue_enqueue(q,&data1);
	queue_enqueue(q,&data2);
	queue_enqueue(q,&data3);
	queue_enqueue(q,&data1);
	
	queue_delete(q,&data1);
	queue_dequeue(q, (void**)&ptr); 
	TEST_ASSERT(ptr == &data2); 

	queue_dequeue(q, (void**)&ptr); 
	queue_dequeue(q, (void**)&ptr); 
	queue_destroy(q); 
}

/* Testing delete function with errors */
void test_queue_delete_errors(void) {
	queue_t q = NULL;
	
	fprintf(stderr, "*** TEST queue_delete_errors ***\n");
	q = queue_create();

	/* Testing to delete with NULL */
	int result = queue_delete(q, NULL); 
	TEST_ASSERT(result == -1); 
	TEST_ASSERT(queue_length(q) == 0);
	queue_destroy(q); 
}

void test_queue_delete_nothing(void) {
	queue_t q = NULL;
	int data1 = 1; 
	int data2 = 2; 
	int* ptr; 

	/* Testing to delete something else */
	fprintf(stderr, "*** TEST queue_delete_nothing ***\n");
	q = queue_create();
	queue_enqueue(q, &data1); 
	queue_delete(q, &data2); 
	queue_dequeue(q, (void**)&ptr); 
	TEST_ASSERT(&data1 == ptr); 
	queue_destroy(q); 
	
}

static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += inc;

    return 0;
}

static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (*a == match)
        return 1;

    return 0;
}

void test_queue_iterator_errors(void) {
	queue_t q = NULL;
	int data1 = 1; 
	int* ptr;

	fprintf(stderr, "*** TEST queue_iterator_errors ***\n");

	q = queue_create(); 
	int result1 = queue_iterate(q, inc_item, (void*)1, NULL); 
	TEST_ASSERT(result1 == -1); 
	queue_enqueue(q, &data1); 
	int result2 = queue_iterate(q, NULL, (void*)1, NULL);
	TEST_ASSERT(result2 == -1); 
	queue_dequeue(q, (void**)&ptr);
	queue_destroy(q); 
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;
    int *ptr;

    /* Initialize the queue and enqueue items */
	fprintf(stderr, "*** TEST test_iterator ***\n");
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Add value '1' to every item of the queue, delete item '42' */
    queue_iterate(q, inc_item, (void*)1, NULL);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT(queue_length(q) == 9);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;     // result pointer *must* be reset first
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    TEST_ASSERT(ptr != NULL);
    TEST_ASSERT(*ptr == 5);
    TEST_ASSERT(ptr == &data[3]);
}


int main(void)
{
	//test_create();
	//test_queue_simple();
	//test_queue_difficult();
	//test_queue_moderate(); 
	//test_queue_destroy_errors(); 
	//test_queue_enqueue_errors(); 
	//test_queue_dequeue_errors(); 
	test_queue_delete();
	//test_iterator(); 
	test_queue_delete_duplicates(); 
	test_queue_delete_errors(); 
	test_queue_delete_nothing(); 
	test_queue_iterator_errors(); 
	return 0;
}
