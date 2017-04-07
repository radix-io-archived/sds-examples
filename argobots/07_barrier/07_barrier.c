#include <stdio.h>
#include <stdlib.h>
#include <abt.h>

#define NUM_XSTREAMS 4

static ABT_barrier my_barrier;

void my_thread(void *arg)
{
	int rank;
	ABT_xstream_self_rank(&rank);
	printf("ULT %d in XSTREAM %d is before the barrier\n", (int)(size_t)arg, rank);
	ABT_barrier_wait(my_barrier);
	printf("ULT %d in XSTREAM %d is after the barrier\n", (int)(size_t)arg, rank);
}

int main(int argc, char *argv[])
{
	ABT_xstream xstreams[NUM_XSTREAMS];
	ABT_pool    pools[NUM_XSTREAMS];
	ABT_thread  threads[NUM_XSTREAMS];
	size_t i;

	ABT_init(argc, argv);

	/* Create eventual */
	ABT_barrier_create(NUM_XSTREAMS, &my_barrier);

	/* Execution Streams */
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
		ABT_thread_create(pools[i], my_thread, (void *)i,
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

	/* Free the future */
	ABT_barrier_free(&my_barrier);

	ABT_finalize();

	return 0;
}
