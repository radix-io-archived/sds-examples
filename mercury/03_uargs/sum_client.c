#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "config.h"
#include "types.h"

#ifdef HAS_CCI
static const char* protocol = "cci+tcp";
#else
static const char* protocol = "bmi+tcp";
#endif

/*
 * This structure will help avoid using global/static variables.
 * One instance of it will be created locally in main, then a pointer
 * to this instance will be accessible in callbacks through user arguments.
 */
typedef struct {
	hg_class_t* 	hg_class;
	hg_context_t*	hg_context;
	hg_id_t			sum_rpc_id;
	int 			completed;
} engine_state;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
hg_return_t sum_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	hg_return_t ret;

	if(argc != 2) {
		printf("Usage: %s <server_address>\n",argv[0]);
		exit(0);
	}

	char* server_address = argv[1];

	/* Local instance of the engine_state. */
	engine_state stt;
	stt.completed = 0;
	// Initialize an hg_class.
	stt.hg_class = HG_Init(protocol, HG_FALSE);
	assert(stt.hg_class != NULL);

	// Creates a context for the hg_class.
	stt.hg_context = HG_Context_create(stt.hg_class);
	assert(stt.hg_context != NULL);

	// Register a RPC function
	stt.sum_rpc_id = MERCURY_REGISTER(stt.hg_class, "sum", sum_in_t, sum_out_t, NULL);
	// Lookup the address of the server, this is asynchronous and
	// the result will be handled by lookup_callback once we start the progress loop
	// The engine_state is passed as user-provided argument, so we can get it in the lookup_callback function.
	ret = HG_Addr_lookup(stt.hg_context, lookup_callback, &stt, server_address, HG_OP_ID_IGNORE);

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

	/* We get the pointer to the engine_state here. */
	engine_state* state = (engine_state*)(callback_info->arg);

	assert(callback_info->ret == 0);
	hg_addr_t addr = callback_info->info.lookup.addr;

	hg_handle_t handle;
	ret = HG_Create(state->hg_context, addr, state->sum_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	sum_in_t in;
	in.x = 42;
	in.y = 23;

	/* The state pointer is passed along as user argument. */
	ret = HG_Forward(handle, sum_completed, state, &in);
	assert(ret == HG_SUCCESS);

    ret = HG_Addr_free(state->hg_class, addr);
    assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}

hg_return_t sum_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;

	/* Get the state pointer from the user-provided arguments. */
	engine_state* state = (engine_state*)(info->arg);
	
	sum_out_t out;
	assert(info->ret == HG_SUCCESS);

	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	state->completed = 1;

	return HG_SUCCESS;
}
