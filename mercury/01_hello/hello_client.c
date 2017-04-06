#include <assert.h>
#include <stdio.h>
#include <mercury.h>

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;
static hg_id_t         hello_rpc_id;
static int completed = 0;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);

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
	hello_rpc_id = HG_Register_name(hg_class, "hello", NULL, NULL, NULL);
	HG_Registered_disable_response(hg_class, hello_rpc_id, HG_TRUE);
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
	hg_addr_t addr = callback_info->info.lookup.addr;

	hg_handle_t handle;
	ret = HG_Create(hg_context, addr, hello_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	ret = HG_Forward(handle, NULL, NULL, NULL);
	assert(ret == HG_SUCCESS);

	completed = 1;
	return HG_SUCCESS;
}
