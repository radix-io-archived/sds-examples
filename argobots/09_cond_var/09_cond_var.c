#include <stdio.h>
#include <abt.h>

#define NUM_XSTREAMS    2

static ABT_cond  my_cond;
static ABT_mutex my_mutex;
static int count = 0;

void my_thread(void *arg)
{
		int rank;
		ABT_xstream_self_rank(&rank);
		if(rank == 0) {
			sleep(1);
			ABT_mutex_lock(my_mutex);
			printf("ULT %d in XSTREAM %d: Before signaling the condition variable!\n", (int)(size_t)arg, rank);
				if(count == 0)
					ABT_cond_signal(my_cond);
				count = count + 1;
			printf("ULT %d in XSTREAM %d: After signaling the condition variable!\n", (int)(size_t)arg, rank);
			ABT_mutex_unlock(my_mutex);
		} else if (rank == 1) {
			ABT_mutex_lock(my_mutex);
			printf("ULT %d in XSTREAM %d: Before waiting on the condition variable!\n", (int)(size_t)arg, rank);
			while(count == 0)
				ABT_cond_wait(my_cond,my_mutex);
			count = count - 1;
			printf("ULT %d in XSTREAM %d: After signaling the condition variable!\n", (int)(size_t)arg, rank);
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

		ABT_cond_create(&my_cond);
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

		ABT_mutex_free(&my_mutex);
		ABT_cond_free(&my_cond);

		ABT_finalize();

		return 0;
}
