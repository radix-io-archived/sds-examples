#include <stdio.h>
#include <abt.h>

#define NUM_XSTREAMS    4

static int my_shared_counter = 0;
static ABT_mutex my_mutex = ABT_MUTEX_NULL;

void increment_counter(void *arg)
{
		int i;
		for(i=0; i<10000; i++) {
			ABT_mutex_lock(my_mutex);
			my_shared_counter += 1;
			ABT_mutex_unlock(my_mutex);
		}
}

int main(int argc, char *argv[])
{
		ABT_xstream xstreams[NUM_XSTREAMS];
		ABT_pool    pools[NUM_XSTREAMS];
		ABT_thread  threads[NUM_XSTREAMS];
		size_t i;

		ABT_init(argc, argv);

		/* Create the mutex */
		ABT_mutex_create(&my_mutex);

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
				ABT_thread_create(pools[i], increment_counter, (void *)i,
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

		/* Free the mutex */
		ABT_mutex_free(&my_mutex);

		ABT_finalize();

		return 0;
}
