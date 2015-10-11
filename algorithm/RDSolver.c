/*

This is an RD solver. It takes reorder density as input and generates a sequence of numbers with that reorder density.

Note: This is a strict solver. It will find a solution if at all possible, and will fail if it isn't possible. It does not
output approximate solutions.

This solver uses various optimizations to quickly get to a result, but will exhaustively scan the problem space. The
worst case time can be fairly high, but usually doesn't happen in practice.

The first iteration of this program was a max-flow solver. However the problem turned out to have additional constraints
not easily expressable in a simple max-flow graph. This solver was written with the max-flow solver as a model, but has
additional constraints and is specialized for this specific problem.

The main issue turned out to be that the graph generated is two-layered, and requires a uniqueness constraint in both 
layers. Simple max-flow was leading to duplicate packets in one of the layers. I reduced the max-flow problem to a
specialized one with that additional constraint.

*/

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 150000
#define MAX_RDS 128

struct RD {
	int displacement;
	int count;
};

struct Vertex {
	int id;
	int rdindex;
	int target;
};

struct RD rds[MAX_RDS];
int rds_length;
int N;

void load(char* filename) {
	FILE* f;
	int balance = 0;
	rds_length = N = 0;
	f = fopen(filename, "r");

	{
		int a; int b; float d = 0;
		while (fscanf(f, "RD[%d] %d %f\n", &a, &b, &d) == 3) {
			struct RD rd = {a, b};

			if (rds_length == MAX_RDS) {
				fprintf(stderr, "Too many RDs :(\n");
				exit(1);
			}
			rds[rds_length++] = rd;
			N += rd.count;
			if (N > MAX_N) {
				fprintf(stderr, "Too many packets :(\n");
				exit(1);
			}
			balance += rd.displacement * rd.count;
			//printf("[RD[%d] %d]\n", rd.displacement, rd.count);
		}
	}
	if (balance != 0) {
		fprintf(stderr, "Positive and negative packets are not balanced (sum(RDi*Ni)=%d != 0) :(", balance);
		exit(1);
	}
}

int bucket_lengths[MAX_N];
struct Vertex buckets[MAX_N][MAX_RDS];
struct Vertex* solution[MAX_N];
int remaining_rds[MAX_RDS];

void bipartite(int N, int displacement, int rdindex) {
	int so, si;
	for (so = 0; so < N; so++) {
		si = so + displacement;
		if (si >= 0 && si < N) {
			struct Vertex v = { so, rdindex, si };
			buckets[so][bucket_lengths[so]++] = v;
		}
	}
}

int solve(int n) {
	int i;
	if (n == N) {
		return 1;
	}
	for (i = 0; i < bucket_lengths[n]; i++) {
		struct Vertex source = buckets[n][i];
		int sink = source.target;
		if (solution[sink] == NULL && remaining_rds[source.rdindex] > 0) {
			solution[sink] = &buckets[n][i]; //source;
			remaining_rds[source.rdindex]--;
			if (solve(n + 1)) return 1;
			solution[sink] = NULL;
			remaining_rds[source.rdindex]++;
		}
	}
	return 0;
}

void gen() {
	int i;
	for (i = 0; i < MAX_N; i++) {
		bucket_lengths[i] = 0;
		solution[i] = NULL;
	}
	for (i = 0; i < rds_length; i++) {
		remaining_rds[i] = rds[i].count;
	}

	for (i = 0; i < rds_length; i++) {
		bipartite(N, rds[i].displacement, i);
	}

	if (solve(0)) {
		printf("Solved!\n");
		for (i = 0; i < N; i++) {
			printf("%d\n ", solution[i]->id + 1);
		}
		/*{
			int newrds[MAX_RDS];
			for (i = 0; i < rds_length; i++) {
				newrds[i] = 0;
			}
			for (i = 0; i < N; i++) {
				newrds[solution[i]->rdindex]++;
			}
			for (i = 0; i < rds_length; i++) {
				printf("[RD[%d] %d]\n", rds[i].displacement, newrds[i]);
			}
		}*/
	} else {
		printf("No solution :(");
		exit(1);
	}
}


int main(int argc, char** argv) {
	if (argc != 2 || argv[1][0] == '-') {
		printf("Usage: RDSolver <rds.txt>\n");
		printf("   Prints out a packet sequence based on given RD values.\n");
		printf("   <rds.txt> contains RD values in this format:\n");
		printf("       RD[-1] 7 0.7\n");
		printf("       RD[1] 2 0.2\n");
		printf("       RD[5] 1 0.1\n");
		exit(1);
	}
    
    printf("\n");

	load(argv[1]);
	gen();
    
    printf("\n\n");
	return 0;
}
