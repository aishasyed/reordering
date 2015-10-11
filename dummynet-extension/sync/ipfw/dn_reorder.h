
#ifndef _REORDER_H
#define _REORDER_H

#ifdef __cplusplus
extern "C" {
#endif



#define DN_SCHED_REORDER	6 //XXX


#define MAX_REORDERED_PACKETS 1024
#define MAX_REORDERINGS 10

struct my_queue {
    int num_of_reorderings;
    int capacities [MAX_REORDERINGS];
    
	int capacity; //
	int input_index;
	int output_index;
    
	int packet_reordering [MAX_REORDERINGS] [MAX_REORDERED_PACKETS];
	struct mbuf* circular_buffer [MAX_REORDERINGS] [MAX_REORDERED_PACKETS];
	
    struct dn_queue mq;
};

/*
 * Tests whether a packet can be queued (i.e., is there capacity)
 */
int my_can_enqueue(struct my_queue* q);

/*
 * Queues a packet. Returns 1 on success, 0 on failure
 */
int my_enqueue(struct my_queue* q, struct mbuf* m);

/*
 * De-queues a packet. Returns the packet, or returns NULL if no packet can be returned
 */
struct mbuf* my_dequeue(struct my_queue* q);

/*
 * Initializes the queue structure
 */
void my_initqueue(struct my_queue* q);


void my_reinitqueue(struct my_queue* q);
    
//////////////////////////////////
// Re-ordered queue implementation

int my_enqueue(struct my_queue* q, struct mbuf* m) {
	int index;
	printf("reorder: my_enqueue\n");
	printf("reorder: my_enqueue: input_index = %d\n", q->input_index);
	index = q->packet_reordering[q->input_index];
	printf("reorder: my_enqueue: index = %d\n", index);

	if (q->circular_buffer[index]) {
		printf("reorder: my_enqueue: buffer full!\n");
		return 0; // we already have a packet at this location, can't handle
	}

	printf("reorder: my_enqueue: adding to buffer\n");
	q->circular_buffer[index] = m;
	q->input_index = (q->input_index + 1) % q->capacity;  ////change HERE!!!!
	printf("reorder: my_enqueue: input_index = %d\n", q->input_index);

	return 1;
}

int my_can_enqueue(struct my_queue* q) {
	printf("reorder: my_can_enqueue\n");
	printf("reorder: my_can_enqueue: q->input_index = %d\n", q->input_index);
	printf("reorder: my_can_enqueue: q->packet_reordering[q->input_index] = %d\n", q->packet_reordering[q->input_index]);
	printf("reorder: my_can_enqueue: result = %d\n", q->circular_buffer[q->packet_reordering[q->input_index]] ? 1 : 0);
	return q->circular_buffer[q->packet_reordering[q->input_index]] ? 1 : 0;
}

struct mbuf* my_dequeue(struct my_queue* q) {
	struct mbuf* m;
	printf("reorder: my_dequeue\n");
	printf("reorder: my_dequeue: output_index = %d\n", q->output_index);

	if (!q->circular_buffer[q->output_index]) {
		printf("reorder: my_dequeue: buffer empty!\n");
		return 0;
	}

	m = q->circular_buffer[q->output_index];
	q->circular_buffer[q->output_index] = 0;
	q->output_index = (q->output_index + 1) % q->capacity;
	printf("reorder: my_dequeue: output_index = %d\n", q->output_index);

	return m;
}

void my_initqueue2(struct my_queue* q) {
        
        
}
    
void my_initqueue(struct my_queue* q) {
	//bzero(q, sizeof(struct my_queue));

	// TODO this hardcoded reordering should be coming from the usermode config
	// scanner.c can be used to generate this
	{
		/*struct my_queue _q =
		{
			3, // capacity
			0, // input_index
			0, // output_index
			{ // packet_reordering
				1, 0, 2
			},
			{ // circular_buffer
				0, 0, 0
			}
		};
		*q = _q;*/
		#ifdef REORDER_USERMODE_SCANNER
		q->capacity = 3;
		q->input_index = 0;
		q->output_index = 0;
		q->packet_reordering[0] = 2; q->packet_reordering[1] = 1; q->packet_reordering[2] = 0;
		q->circular_buffer[0] = q->circular_buffer[0] = q->circular_buffer[0] = 0;
		#else
		#	include "reordering.h"
		#endif
		
		printf("reorder: my_initqueue\n");
		printf("reorder: my_initqueue: q->capacity = %d\n", q->capacity);
		printf("reorder: my_initqueue: q->input_index = %d\n", q->input_index);
		printf("reorder: my_initqueue: q->output_index = %d\n", q->output_index);
		printf("reorder: my_initqueue: q->packet_reordering = %d,%d,%d\n", q->packet_reordering[0], q->packet_reordering[1], q->packet_reordering[2]);
		printf("reorder: my_initqueue: q->circular_buffer = %d,%d,%d\n", (int)q->circular_buffer[0], (int)q->circular_buffer[1], (int)q->circular_buffer[2]);
	}
}

#ifdef __cplusplus
}
#endif

#endif /* _REORDER_H */
