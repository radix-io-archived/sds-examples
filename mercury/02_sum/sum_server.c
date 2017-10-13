#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "types.h"

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;

static const int TOTAL_RPCS = 10;
static int num_rpcs = 0;

/* Function to expose as RPC. */
hg_return_t sum(hg_handle_t h);

int main(int argc, char** argv)
{
	hg_return_t ret;

	if(argc != 2) {
		printf("Usage: %s <server address>\n", argv[0]);
		exit(0);
	}

	const char* server_address = argv[1];

	hg_class = HG_Init(server_address, HG_TRUE);
    assert(hg_class != NULL);

    hg_context = HG_Context_create(hg_class);
    assert(hg_context != NULL);

	char hostname[128];
	hg_size_t hostname_size;
	hg_addr_t self_addr;
	HG_Addr_self(hg_class,&self_addr);
	HG_Addr_to_string(hg_class, hostname, &hostname_size, self_addr);
	printf("Server running at address %s\n",hostname);

	/* 
	 * This time with use the macro MERCURY_REGISTE to register the RPC function
	 * along with the input/output structures and their serialization functions.
	 */
	MERCURY_REGISTER(hg_class, "sum", sum_in_t, sum_out_t, sum);
	do
	{
		unsigned int count;
		do {
			ret = HG_Trigger(hg_context, 0, 1, &count);
		} while((ret == HG_SUCCESS) && count);

		HG_Progress(hg_context, 100);
	} while(num_rpcs < TOTAL_RPCS);

	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	ret = HG_Finalize(hg_class);
	assert(ret == HG_SUCCESS);

	return 0;
}

/* Implementation of the sum RPC function. */
hg_return_t sum(hg_handle_t handle)
{
	hg_return_t ret;
	sum_in_t in;   /* input structure for the RPC  */
	sum_out_t out; /* output structure for the RPC */
	
	/* Deserialize the input from the received handle. */
	ret = HG_Get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	/* Compute the result. */
	out.ret = in.x + in.y;
	printf("%d + %d = %d\n",in.x,in.y,in.x+in.y);
	num_rpcs += 1;

	/* Sends a response, pointing the returned data to the out struct.
	   The first NULL correspond to a potential completion callback that
	   would be called once the response has been sent, and the second
	   NULL is a pointer to pass to this callback. */
	ret = HG_Respond(handle,NULL,NULL,&out);
	assert(ret == HG_SUCCESS);

	/* Free the input data. */
	ret = HG_Free_input(handle, &in);
	assert(ret == HG_SUCCESS);

	/* The response has been sent, we don't need the handle anymore. */
	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
