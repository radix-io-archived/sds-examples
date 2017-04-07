#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "types.h"

typedef struct {
	hg_class_t* 	hg_class;
	hg_context_t*	hg_context;
	hg_id_t			sum_rpc_id;
	hg_addr_t		addr;
	int 			completed;
} engine_state;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
hg_return_t sum_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	hg_return_t ret;

	engine_state stt;
	stt.completed = 0;
	// Initialize an hg_class.
	stt.hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(stt.hg_class != NULL);

	// Creates a context for the hg_class.
	stt.hg_context = HG_Context_create(stt.hg_class);
	assert(stt.hg_context != NULL);

	// Register a RPC function
	stt.sum_rpc_id = MERCURY_REGISTER(stt.hg_class, "sum", sum_in_t, sum_out_t, NULL);
	// Lookup the address of the server, this is asynchronous and
	// the result will be handled by lookup_callback once we start the progress loop
	ret = HG_Addr_lookup(stt.hg_context, lookup_callback, &stt, "bmi+tcp://localhost:1234", HG_OP_ID_IGNORE);

	// Main event loop
	while(!stt.completed)
	{
		unsigned int count;
		do {
			ret = HG_Trigger(stt.hg_context, 0, 1, &count);
		} while((ret == HG_SUCCESS) && count && !stt.completed);
		HG_Progress(stt.hg_context, 100);
	}

	// Destroy the context
	ret = HG_Context_destroy(stt.hg_context);
	assert(ret == HG_SUCCESS);

	// Finalize the hg_class.
	hg_return_t err = HG_Finalize(stt.hg_class);
	assert(err == HG_SUCCESS);
	return 0;
}


hg_return_t lookup_callback(const struct hg_cb_info *callback_info)
{
	hg_return_t ret;

	engine_state* state = (engine_state*)(callback_info->arg);

	assert(callback_info->ret == 0);
	state->addr = callback_info->info.lookup.addr;

	hg_handle_t handle;
	ret = HG_Create(state->hg_context, state->addr, state->sum_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	sum_in_t in;
	in.x = 42;
	in.y = 23;

	ret = HG_Forward(handle, sum_completed, state, &in);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}

hg_return_t sum_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;

	engine_state* state = (engine_state*)(info->arg);
	
	sum_out_t out;
	assert(info->ret == HG_SUCCESS);

	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Addr_free(state->hg_class, state->addr);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	state->completed = 1;

	return HG_SUCCESS;
}
