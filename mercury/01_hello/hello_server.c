#include <assert.h>
#include <stdio.h>
#include <mercury.h>

static hg_class_t*     hg_class 	= NULL; /* the mercury class */
static hg_context_t*   hg_context 	= NULL; /* the mercury context */

/* after serving this number of rpcs, the server will shut down. */
static const int TOTAL_RPCS = 10;
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

	/* Register the RPC by its name ("hello").
	 * The two NULL arguments correspond to the functions user to
	 * serialize/deserialize the input and output parameters
	 * (hello_world doesn't have parameters and doesn't return anything, hence NULL).
	 */
	hg_id_t rpc_id = HG_Register_name(hg_class, "hello", NULL, NULL, hello_world);

	/* We call this function to tell Mercury that hello_world will not
	 * send any response back to the client.
	 */
	HG_Registered_disable_response(hg_class, rpc_id, HG_TRUE);
	
	/* Main loop listening for incoming RPCs. */
	do
	{
		/* count will keep track of how many RPCs were treated in a given
		 * call to HG_Trigger.
		 */
		unsigned int count;
		do {
			/* Executes callbacks.
			 * 0 = no timeout, the function just returns if there is nothing to process.
			 * 1 = the max number of callbacks to execute before returning.
			 * After the call, count will hold the number of callbacks executed.
			 */
			ret = HG_Trigger(hg_context, 0, 1, &count);
		} while((ret == HG_SUCCESS) && count);
		/* Exit the loop if no event has been processed. */

		/* Make progress on receiving/sending data.
		 * 100 is the timeout in milliseconds, for which to wait for network events. */
		HG_Progress(hg_context, 100);
	} while(num_rpcs < TOTAL_RPCS);
	/* Exit the loop if we have reached the given number of RPCs. */

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
	return HG_SUCCESS;
}
