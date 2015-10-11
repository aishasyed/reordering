
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mbuf { char str[1024]; };
struct dn_queue { int i; };
//#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
/*void bzero(void* b, int len) {
	memset(b, '\0', len);
}*/

#define REORDER_USERMODE_SCANNER
#include "ipfw/dn_reorder.h"

void fatal(char *s) {
	perror(s);
	exit(1);
}

void read_packet_reordering(const char* filename, int* packets, int*capacity) {
	FILE* fd;

	//int packets[MAX_REORDERED_PACKETS];
	int reverse_index[MAX_REORDERED_PACKETS];
	int n = 0;
	int i;

	fd = fopen(filename, "r");
	if (!fd) fatal("failed to open file");

	while (1) {
		int packet;
		if (fscanf(fd, "%d", &packet) == EOF) break;
		if (n == MAX_REORDERED_PACKETS)
			fatal("too many packets");

		packet--; // our input starts with 1, so subtract 1
		packets[n++] = packet;
	}
	if (n < 1)
		fatal("zero packets");

	for (i = 0; i < n; i++) {
		reverse_index[i] = -1;
	}
	for (i = 0; i < n; i++) {
		int packet = packets[i];
		if (packet < 0)
			fatal("packet < 1");
		if (packet >= n)
			fatal("packet > packet_count");
		if (reverse_index[packet] != -1)
			fatal("duplicate packet");
		reverse_index[packet] = i;
	}
	*capacity = n;
}

void dump_queue(struct my_queue* q) {
	int i;
	printf("{\n");
	printf("\tq->capacity = %d;\n", q->capacity);
	printf("\tq->input_index = 0;\n");
	printf("\tq->output_index = 0;\n");
	printf("\n");
	for (i = 0; i < q->capacity; i++) {
		printf("\tq->packet_reordering[%d] = %d;\n", i, q->packet_reordering[i]);
	}
	printf("\n");
	for (i = 0; i < q->capacity; i++) {
		printf("\tq->circular_buffer[%d] = 0;\n", i);
	}
	/*printf("\tq->input_index = 0;\n");
	printf("\tq->input_index = 0;\n");
	printf("\t%d, // capacity\n", q->capacity);
	printf("\t%d, // input_index\n", 0);
	printf("\t%d, // output_index\n", 0);
	printf("\t{ // packet_reordering\n\t\t");
	for (i = 0; i < q->capacity; i++) {
		printf("%s%d", i==0 ? "" : ", ", q->packet_reordering[i]);
	}
	printf("\n\t},\n");
	printf("\t{ // circular_buffer\n\t\t");
	for (i = 0; i < q->capacity; i++) {
		printf("%s%d", i==0 ? "" : ", ", 0); // we only print zero, allowing easy copy paste of struct initializer
	}
	printf("\n\t}\n");*/
	printf("}\n");
}

void test(struct my_queue* q) {
	char c;
	for (c = 'a'; c <= 'z'; c++) {
		struct mbuf* m = malloc(sizeof(struct mbuf));
		m->str[0] = c; m->str[1] = 0;
		if (!my_enqueue(q, m))
			fatal("enqueue failed\n");
		printf("+%s\n", m->str);
		while ((m = my_dequeue(q)) != NULL) {
			printf("-%s\n", m->str);
		}
	}
}
int main(int argc, char **argv) {
	struct my_queue q;
	q.input_index = q.output_index = 0; //my_initqueue(&q);

	if (argc != 2) {
		fatal("Usage: ./scanner input.txt\n\tinput.txt should have a list of space separated reordering sequence numbers starting from 1");
		return 1;
	}

	read_packet_reordering("input.txt", &q.packet_reordering[0], &q.capacity);
	
	//test(&q);
	
	dump_queue(&q);
	return 0;
}



