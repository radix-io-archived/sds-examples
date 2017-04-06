#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "types.h"

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;
static hg_id_t         sum_rpc_id;
static hg_addr_t       addr;
static int completed = 0;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
hg_return_t sum_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	hg_return_t ret;

	// Initialize an hg_class.
	hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(hg_class != NULL);

	// Creates a context for the hg_class.
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	// Register a RPC function
	sum_rpc_id = MERCURY_REGISTER(hg_class, "sum", sum_in_t, sum_out_t, NULL);
	// Lookup the address of the server, this is asynchronous and
	// the result will be handled by lookup_callback once we start the progress loop
	ret = HG_Addr_lookup(hg_context, lookup_callback, NULL, "bmi+tcp://localhost:1234", HG_OP_ID_IGNORE);

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


hg_return_t lookup_callback(const struct hg_cb_info *callback_info)
{
	hg_return_t ret;

	assert(callback_info->ret == 0);
	addr = callback_info->info.lookup.addr;

	hg_handle_t handle;
	ret = HG_Create(hg_context, addr, sum_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	sum_in_t in;
	in.x = 42;
	in.y = 23;

	ret = HG_Forward(handle, sum_completed, NULL, &in);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}

hg_return_t sum_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;
	
	sum_out_t out;
	assert(info->ret == HG_SUCCESS);

	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Addr_free(hg_class, addr);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	completed = 1;

	return HG_SUCCESS;
}
