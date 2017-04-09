#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <mercury.h>
#include "config.h"
#include "types.h"

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;
static hg_id_t         sum_rpc_id;
static hg_addr_t       addr;
static int completed = 0;

#ifdef HAS_CCI
static const char* protocol = "cci+tcp";
#else
static const char* protocol = "bmi+tcp";
#endif

/* This callback is called when the address lookup operation completed. */
hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
/* This callback is called when the server has sent its response. */
hg_return_t sum_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	hg_return_t ret;

	if(argc != 2) {
		printf("Usage: %s <server_address>\n",argv[0]);
		exit(0);
	}
	char* server_address = argv[1];

	// Initialize an hg_class.
	hg_class = HG_Init(protocol, HG_FALSE);
	assert(hg_class != NULL);

	// Creates a context for the hg_class.
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	// Register a RPC function
	sum_rpc_id = MERCURY_REGISTER(hg_class, "sum", sum_in_t, sum_out_t, NULL);
	// Lookup the address of the server, this is asynchronous and
	// the result will be handled by lookup_callback once we start the progress loop
	ret = HG_Addr_lookup(hg_context, lookup_callback, NULL, server_address, HG_OP_ID_IGNORE);

	// Main event loop
	while(!completed)
	{
		unsigned int count;
		do {
			ret = HG_Trigger(hg_context, 0, 1, &count);
		} while((ret == HG_SUCCESS) && count && !completed);
		HG_Progress(hg_context, 100);
	}

	// Destroy the context
	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	// Finalize the hg_class.
	hg_return_t err = HG_Finalize(hg_class);
	assert(err == HG_SUCCESS);
	return 0;
}

/* Implementation of the callback called when address lookup is done. */
hg_return_t lookup_callback(const struct hg_cb_info *callback_info)
{
	hg_return_t ret;

	assert(callback_info->ret == 0);
	addr = callback_info->info.lookup.addr;

	hg_handle_t handle;
	ret = HG_Create(hg_context, addr, sum_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	/* Initialize the input parameters of the RPC call */
	sum_in_t in;
	in.x = 42;
	in.y = 23;

	/* Forward the call with its parameters. */
	ret = HG_Forward(handle, sum_completed, NULL, &in);
	assert(ret == HG_SUCCESS);

	/* Free the address */
    ret = HG_Addr_free(hg_class, addr);
    assert(ret == HG_SUCCESS);	

	/* We don't free the handle because it will be 
	   destroyed in sum_completed later. */

	return HG_SUCCESS;
}

/* Implementation of the callback called when receiving 
 * a response from the server. */
hg_return_t sum_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;
	
	assert(info->ret == HG_SUCCESS);

	/* Deserialize the data returned by the server. */
	sum_out_t out;
	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	/* Free the output */
	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	/* Free the handle. */
	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	completed = 1;

	return HG_SUCCESS;
}
