#include <stdio.h>
#include <stdlib.h>
#include <abt.h>

#define NUM_XSTREAMS 4

static ABT_future my_future;

void future_is_ready(void** args)
{
	int rank;
	printf("Future is ready!\n");
	ABT_xstream_self_rank(&rank);
	printf("XSTREAM %d executed the future's callback, got values:\n", rank);
	int i;
	for(i = 0; i < NUM_XSTREAMS; i++) {
		int* x = (int*)(args[i]);
		printf("   %d\n",*x);
		free(x);
	}
}

void feeder_thread(void *arg)
{
	int rank;
	ABT_xstream_self_rank(&rank);
	int* r = (int*)malloc(sizeof(int));
	*r = 42+rank;
	printf("ULT %d in XSTREAM %d set future to: %d\n", (int)(size_t)arg, rank, *r);
	ABT_future_set(my_future,(void*)r);
	ABT_future_wait(my_future);
	printf("ULT %d in XSTREAM %d continuing after future was set\n", (int)(size_t)arg, rank);
}

int main(int argc, char *argv[])
{
	ABT_xstream xstreams[NUM_XSTREAMS];
	ABT_pool    pools[NUM_XSTREAMS];
	ABT_thread  threads[NUM_XSTREAMS];
	size_t i;

	ABT_init(argc, argv);

	/* Create eventual */
	ABT_future_create(NUM_XSTREAMS, future_is_ready, &my_future);

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
		ABT_thread_create(pools[i], feeder_thread, (void *)i,
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
	ABT_future_free(&my_future);

	ABT_finalize();

	return 0;
}
