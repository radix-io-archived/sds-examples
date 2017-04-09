#include <iostream>

#include <abt.h>

#include <sds-utils/abt-utils.hh>

#define NUM_XSTREAMS    4

/* This is the function executed by each of the threads */
int hello(int i, const std::string& message){
	int rank;
	ABT_xstream_self_rank(&rank);
	std::cout << "ULT " << i << " in XSTREAM " << rank << " says " << message << std::endl;
	return 42+i*rank;
}
 
int main(int argc, char *argv[])
{
		ABT_xstream xstreams[NUM_XSTREAMS];
		ABT_pool    pools[NUM_XSTREAMS];
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

		for(i = 0; i < NUM_XSTREAMS; i++) {
			int r = sdsu::dispatch_to_ult(pools[i], hello, i, std::string("Hello World using Thread"));
			/* see sdsu::dispatch_to_ult_void for functions that return void */
			std::cout << " Returned " << r << std::endl;
			std::string msg("Hello World using Tasks");
			r = sdsu::dispatch_to_task(pools[i], hello, i, msg);
			std::cout << " Returned " << r << std::endl;
			/* see sdsu::dispatch_to_task_void for functions that return void */
		}

		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_join(xstreams[i]);
				ABT_xstream_free(&xstreams[i]);
		}

		/* Finalize */
		ABT_finalize();

		return 0;
}
