#include <stdio.h>
#include <abt.h>

void thread1(void *arg)
{
		int rank;
		ABT_xstream_self_rank(&rank);
		printf("ULT %d in XSTREAM %d: Step 1\n", (int)(size_t)arg, rank);
		ABT_thread_yield();
		printf("ULT %d in XSTREAM %d: Step 2\n", (int)(size_t)arg, rank);
		ABT_thread_yield();
		printf("ULT %d in XSTREAM %d: Step 3\n", (int)(size_t)arg, rank);
}

void thread2(void *arg)
{
		int rank;
		ABT_xstream_self_rank(&rank);
		printf("ULT %d in XSTREAM %d: Step 1\n", (int)(size_t)arg, rank);
		ABT_thread_yield();
		printf("ULT %d in XSTREAM %d: Step 2\n", (int)(size_t)arg, rank);
		ABT_thread_yield();
		printf("ULT %d in XSTREAM %d: Step 3\n", (int)(size_t)arg, rank);
}

int main(int argc, char *argv[])
{
		ABT_xstream xstream;
		ABT_pool    pool;
		ABT_thread  threads[2];

		ABT_init(argc, argv);

		/* Execution Streams */
		ABT_xstream_self(&xstream);

		/* Get the first pool associated with each ES */
		ABT_xstream_get_main_pools(xstream, 1, &pool);

		/* Create ULTs */
		ABT_thread_create(pool, thread1, (void *)0,
								ABT_THREAD_ATTR_NULL, &threads[0]);
		ABT_thread_create(pool, thread2, (void *)1,
								ABT_THREAD_ATTR_NULL, &threads[1]);

		/* Join & Free */
		int i;
		for (i = 0; i < 2; i++) {
				ABT_thread_join(threads[i]);
				ABT_thread_free(&threads[i]);
		}

		ABT_finalize();

		return 0;
}
