#include <stdio.h>
#include <abt.h>

#define NUM_XSTREAMS    4

/* This is the function executed by each of the threads */
void thread_hello(void *arg)
{
	/* Get the rank of the ES */
	int rank;
	ABT_xstream_self_rank(&rank);
	printf("ULT %d in XSTREAM %d: Hello, world!\n", (int)(size_t)arg, rank);
}

int main(int argc, char *argv[])
{
		ABT_xstream xstreams[NUM_XSTREAMS];
		ABT_pool    pools[NUM_XSTREAMS];
		ABT_thread  threads[NUM_XSTREAMS];
		size_t i;

		/* Initialize Argobots */
		ABT_init(argc, argv);

		/* Execution Streams
		 * xtrseam[0] will be the current ES, no need to create it.
		 */
		ABT_xstream_self(&xstreams[0]);
		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_create(ABT_SCHED_NULL, &xstreams[i]);
		}

		/* Get the first pool associated with each ES */
		for (i = 0; i < NUM_XSTREAMS; i++) {
				ABT_xstream_get_main_pools(xstreams[i], 1, &pools[i]);
		}

		/* Create ULTs */
		for (i = 0; i < NUM_XSTREAMS; i++) {
				ABT_thread_create(pools[i], thread_hello, (void *)i,
								ABT_THREAD_ATTR_NULL, &threads[i]);
		}

		/* Join & Free */
		for (i = 0; i < NUM_XSTREAMS; i++) {
				ABT_thread_join(threads[i]);
				ABT_thread_free(&threads[i]);
		}
		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_join(xstreams[i]);
				ABT_xstream_free(&xstreams[i]);
		}

		/* Finalize */
		ABT_finalize();

		return 0;
}
