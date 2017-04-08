#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

static hg_class_t*     hg_class 	= NULL; /* Pointer to the Mercury class */
static hg_context_t*   hg_context 	= NULL; /* Pointer to the Mercury context */
static margo_instance_id mid		= MARGO_INSTANCE_NULL;
static hg_id_t         sum_rpc_id;		/* ID of the RPC */
static hg_addr_t       svr_addr;

typedef struct {
	sum_in_t in;	
} thread_args;

static void run_my_rpc(void *arg);

/* Main function. */
int main(int argc, char** argv)
{
	hg_return_t ret;
	ABT_xstream xstream;
	ABT_pool pool;
	ABT_thread threads[4];
	thread_args args[4];

	/* 
	 * Initialize an hg_class.
	 * Here we only specify the protocal since this is a client
	 * (no need for an address and a port). HG_FALSE indicates that
	 * the client will not listen for incoming requests.
	 */
	hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(hg_class != NULL);

	/* Creates a context for the hg_class. */
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	/* Initialie Argobots */
	ABT_init(argc, argv);

	/* set primary ES to idle without polling */
	ABT_snoozer_xstream_self_set();

	/* get main ES */
	ABT_xstream_self(&xstream);

	/* get main pool */
	ABT_xstream_get_main_pools(xstream, 1, &pool);

	/* Start Margo */
	mid = margo_init(0, 0, hg_context);

	/* Register a RPC function.
	 * The first two NULL correspond to what would be pointers to
	 * serialization/deserialization functions for input and output datatypes
	 * (not used in this example).
	 * The third NULL is the pointer to the function (which is on the server,
	 * so NULL here on the client).
	 */
	sum_rpc_id = MERCURY_REGISTER(hg_class, "sum", sum_in_t, sum_out_t, NULL);
	
	/* Lookup the address of the server, this is asynchronous and
	 * the result will be handled by lookup_callback once we start the progress loop.
	 * NULL correspond to a pointer to user data to pass to lookup_callback (we don't use
	 * any here). The 4th argument is the address of the server.
	 * The 5th argument is a pointer a variable of type hg_op_id_t, which identifies the operation.
	 * It can be useful to get this identifier if we want to be able to cancel it using
	 * HG_Cancel. Here we don't use it so we pass HG_OP_ID_IGNORE.
	 */
	margo_addr_lookup(mid, "bmi+tcp://localhost:1234", &svr_addr);
		
	int i;
	for(i=0; i<4; i++) {
			args[i].in.x = 42+i*2;
			args[i].in.y = 42+i*2+1;
			ABT_thread_create(pool, run_my_rpc, &args[i],
						ABT_THREAD_ATTR_NULL, &threads[i]);
	}

	/* yield to one of the threads */
    ABT_thread_yield_to(threads[0]);

	for(i=0; i<4; i++)
    {
        ABT_thread_join(threads[i]);
        ABT_thread_free(&threads[i]);
    }

	/* shut down Margo */
    margo_finalize(mid);

	/* Finalize Argobots */
	ABT_finalize();

	/* Destroy the context. */
	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	/* Finalize the hg_class. */
	hg_return_t err = HG_Finalize(hg_class);
	assert(err == HG_SUCCESS);
	return 0;
}

void run_my_rpc(void *arg)
{
	thread_args* targs = (thread_args*)arg;
	hg_handle_t handle;
	hg_return_t ret;
	ABT_thread self;
	ABT_thread_id id;
	
	/* create handle */
    ret = HG_Create(hg_context, svr_addr, sum_rpc_id, &handle);
    assert(ret == HG_SUCCESS);

	/* forward RPC */
	margo_forward(mid, handle, &(targs->in));

	/* Deserialize the data returned by the server. */
	sum_out_t out;
	ret = HG_Get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d+%d = %d\n", targs->in.x, targs->in.y, out.ret);

	/* Free the output */
	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	/* destroy the handle */
	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);

	ABT_thread_self(&self);
	ABT_thread_get_id(self,&id);
	printf("ULT [%d] done.\n", (int)id);
}
