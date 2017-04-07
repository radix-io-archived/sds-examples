#include <stdio.h>
#include <abt.h>

#define NUM_XSTREAMS    4
#define TASKS_PER_XTREAM 2

void task_hello(void *arg)
{
		printf("TASK%d: Hello, world!\n", (int)(size_t)arg);
}

int main(int argc, char *argv[])
{
		ABT_xstream xstreams[NUM_XSTREAMS];
		ABT_pool    pools[NUM_XSTREAMS];
		size_t i;

		ABT_init(argc, argv);

		/* Execution Streams */
		ABT_xstream_self(&xstreams[0]);
		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_create(ABT_SCHED_NULL, &xstreams[i]);
		}

		/* Get the first pool associated with each ES */
		for (i = 0; i < NUM_XSTREAMS; i++) {
				ABT_xstream_get_main_pools(xstreams[i], 1, &pools[i]);
		}

		/* Create Tasks */
		for (i = 0; i < (NUM_XSTREAMS*TASKS_PER_XTREAM); i++) {
				ABT_task_create(pools[i % NUM_XSTREAMS], task_hello, (void *)i, NULL);
		}

		/* Free */
		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_join(xstreams[i]);
				ABT_xstream_free(&xstreams[i]);
		}

		ABT_finalize();

		return 0;
}
