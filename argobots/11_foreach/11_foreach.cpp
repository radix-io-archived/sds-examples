#include <iostream>
#include <vector>
#include <string>

#include <abt.h>

#include <sds-utils/foreach.hpp>

#define NUM_XSTREAMS    4

/* This is the function executed by each of the threads */
void hello(const std::string& message){
	int rank;
	ABT_xstream_self_rank(&rank);
	std::cout << "ULT in XSTREAM " << rank << " says " << message << std::endl;
}
 
int main(int argc, char *argv[])
{
		std::vector<ABT_xstream> xstreams(NUM_XSTREAMS);
		std::vector<ABT_pool> pools(NUM_XSTREAMS);
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

		std::vector<std::string> v;
		v.emplace_back("Hello");
		v.emplace_back("Bonjour");
		v.emplace_back("Hallo");
		v.emplace_back("Hola");

		sdsu::ult_foreach(pools.begin(), pools.end(), v.begin(), v.end(), hello);

		for (i = 1; i < NUM_XSTREAMS; i++) {
				ABT_xstream_join(xstreams[i]);
				ABT_xstream_free(&xstreams[i]);
		}

		/* Finalize */
		ABT_finalize();

		return 0;
}
