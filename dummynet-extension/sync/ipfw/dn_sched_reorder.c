

#ifdef _KERNEL
#include <sys/malloc.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/module.h>
#include <net/if.h>	/* IFNAMSIZ */
#include <netinet/in.h>
#include <netinet/ip_var.h>		/* ipfw_rule_ref */
#include <netinet/ip_fw.h>	/* flow_id */
#include <netinet/ip_dummynet.h>
#include <netinet/ipfw/dn_heap.h>
#include <netinet/ipfw/ip_dn_private.h>
#include <netinet/ipfw/dn_sched.h>
#else
#include <dn_test.h>
#endif

#include "dn_reorder.h"

/*
 * This file implements a reorder scheduler for a single queue.
 * The queue is allocated as part of the scheduler instance,
 * and there is a single flowset is in the template which stores
 * queue size and policy.
 * Enqueue and dequeue use the default library functions.
 */
static int 
reorder_enqueue(struct dn_sch_inst *si, struct dn_queue *q, struct mbuf *m)
{
	/* XXX if called with q != NULL and m=NULL, this is a
	 * re-enqueue from an existing scheduler, which we should
	 * handle.
	 */
	struct my_queue* my_q = (struct my_queue *)(si+1);
	printf("reorder: reorder_enqueue\n");
	return my_enqueue(my_q, m) ? 0 : 1;
}

static struct mbuf *
reorder_dequeue(struct dn_sch_inst *si)
{
	struct my_queue* my_q = (struct my_queue *)(si+1);
	printf("reorder: reorder_dequeue\n");
	return my_dequeue(my_q);
}

static int
reorder_new_sched(struct dn_sch_inst *si)
{
	/* This scheduler instance contains the queue */
	struct my_queue* my_q = (struct my_queue *)(si+1);
	printf("reorder: reorder_new_sched\n");
	my_initqueue(my_q);
	return 0;
}

static int
reorder_free_sched(struct dn_sch_inst *si)
{
	struct my_queue* my_q = (struct my_queue *)(si+1);
	struct mbuf* m;
	printf("reorder: reorder_free_sched\n");
	while ((m = my_dequeue(my_q)) != NULL)
		dn_free_pkts(m);
	my_initqueue(my_q);
	//{ int i; for (i = 0; i < my_q->capacity; i++) { if (my_q->circular_buffer[i]) dn_free_pkts(my_q->circular_buffer[i]); } }
	return 0;
}

/*
 * Reorder scheduler descriptor
 * contains the type of the scheduler, the name, the size of extra
 * data structures, and function pointers.
 */
static struct dn_alg reorder_desc = {
	_SI( .type = )  DN_SCHED_REORDER,
	_SI( .name = )  "REORDER",
	_SI( .flags = ) 0,

	_SI( .schk_datalen = ) 0,
	_SI( .si_datalen = )  sizeof(struct my_queue),
	_SI( .q_datalen = )  0,

	_SI( .enqueue = )  reorder_enqueue,
	_SI( .dequeue = )  reorder_dequeue,
	_SI( .config = )  NULL,
	_SI( .destroy = )  NULL,
	_SI( .new_sched = )  reorder_new_sched,
	_SI( .free_sched = )  reorder_free_sched,
	_SI( .new_fsk = )  NULL,
	_SI( .free_fsk = )  NULL,
	_SI( .new_queue = )  NULL,
	_SI( .free_queue = )  NULL,
};

DECLARE_DNSCHED_MODULE(dn_reorder, &reorder_desc);
