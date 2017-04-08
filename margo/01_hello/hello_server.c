#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include <mercury.h>

static hg_class_t*     hg_class 	= NULL; /* the mercury class */
static hg_context_t*   hg_context 	= NULL; /* the mercury context */
static margo_instance_id mid 		= MARGO_INSTANCE_NULL;
/* after serving this number of rpcs, the server will shut down. */
static const int TOTAL_RPCS = 16;
/* number of RPCS already received. */
static int num_rpcs = 0;

/* 
 * hello_world function to expose as an RPC.
 * This function just prints "Hello World"
 * and increment the num_rpcs variable.
 *
 * All Mercury RPCs must have a signature
 *   hg_return_t f(hg_handle_t h)
 */
hg_return_t hello_world(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(hello_world)

/*
 * main function.
 */
int main(int argc, char** argv)
{
	hg_return_t ret;

	/* Initialize Mercury and get an hg_class handle.
	 * bmi+tcp is the protocol to use.
	 * localhost is the address of the server (not useful at the server itself).
	 * HG_TRUE is here to specify that mercury will listen for incoming requests.
	 * (HG_TRUE on servers, HG_FALSE on clients).
	 */
	hg_class = HG_Init("bmi+tcp://localhost:1234", HG_TRUE);
    assert(hg_class != NULL);

	/* Creates a Mercury context from the Mercury class. */
    hg_context = HG_Context_create(hg_class);
    assert(hg_context != NULL);

	/* Setup Argobots */
	ABT_init(argc, argv);

	/* set primary ES to idle without polling */
    ABT_snoozer_xstream_self_set();

	/* Initialize Margo */
	mid = margo_init(0, 0, hg_context);
    assert(mid);

	/* Register the RPC by its name ("hello").
	 * The two NULL arguments correspond to the functions user to
	 * serialize/deserialize the input and output parameters
	 * (hello_world doesn't have parameters and doesn't return anything, hence NULL).
	 */
	hg_id_t rpc_id = HG_Register_name(hg_class, "hello", NULL, NULL, hello_world_handler);

	/* We call this function to tell Mercury that hello_world will not
	 * send any response back to the client.
	 */
	HG_Registered_disable_response(hg_class, rpc_id, HG_TRUE);

	/* NOTE: there isn't anything else for the server to do at this point
     * except wait for itself to be shut down.  The
     * margo_wait_for_finalize() call here yields to let Margo drive
     * progress until that happens.
	 */
	margo_wait_for_finalize(mid);

	/* Finalize Argobots */
	ABT_finalize();
	
	/* Destroys the Mercury context. */
	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	/* Finalize Mercury. */
	ret = HG_Finalize(hg_class);
	assert(ret == HG_SUCCESS);

	return 0;
}

/* Implementation of the hello_world RPC. */
hg_return_t hello_world(hg_handle_t h)
{
	hg_return_t ret;

	printf("Hello World!\n");
	num_rpcs += 1;
	/* We are not going to use the handle anymore, so we should destroy it. */
	ret = HG_Destroy(h);
	assert(ret == HG_SUCCESS);

	if(num_rpcs == TOTAL_RPCS) {
		/* NOTE: we assume that the server daemon is using
		 * margo_wait_for_finalize() to suspend until this RPC executes, so there
		 * is no need to send any extra signal to notify it.
		 */
		margo_finalize(mid);
	}

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(hello_world)
