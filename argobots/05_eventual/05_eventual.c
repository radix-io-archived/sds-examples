#include <stdio.h>
#include <unistd.h>
#include <abt.h>

#define NUM_XSTREAMS 4

static ABT_eventual my_eventual;

void first_thread(void *arg)
{
		int rank;
		ABT_xstream_self_rank(&rank);
		printf("ULT %d in XSTREAM %d doing some computation\n", (int)(size_t)arg, rank);
		sleep(1);
		int r = 42;
		ABT_eventual_set(my_eventual, &r, sizeof(r));
}

void waiter_thread(void *arg)
{
		int rank;
		int* r;
		ABT_eventual_wait(my_eventual, (void**)&r);
		ABT_xstream_self_rank(&rank);
		printf("ULT %d in XSTREAM %d waited for result: %d\n", (int)(size_t)arg, rank, *r);
}

int main(int argc, char *argv[])
{
		ABT_xstream xstreams[NUM_XSTREAMS];
		ABT_pool    pools[NUM_XSTREAMS];
		ABT_thread  threads[NUM_XSTREAMS];
		size_t i;

		ABT_init(argc, argv);

		/* Create eventual */
		ABT_eventual_create(sizeof(int), &my_eventual);

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
		ABT_thread_create(pools[0], first_thread, (void *)0,
						ABT_THREAD_ATTR_NULL, &threads[0]);
		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_thread_create(pools[i], waiter_thread, (void *)i,
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

		/* Free the eventual */
		ABT_eventual_free(&my_eventual);

		ABT_finalize();

		return 0;
}
