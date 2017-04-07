#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "types.h"

typedef struct {
	hg_class_t* 	hg_class;
	hg_context_t*	hg_context;
	hg_id_t			save_rpc_id;
	hg_bulk_t 		bulk_handle;
	char*			buffer;
	size_t			buffer_size;
	int 			completed;
} engine_state;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
hg_return_t save_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	hg_return_t ret;

	/* Local instance of the engine_state. */
	engine_state stt;
	stt.completed = 0;
	// Initialize an hg_class.
	stt.hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(stt.hg_class != NULL);

	// Creates a context for the hg_class.
	stt.hg_context = HG_Context_create(stt.hg_class);
	assert(stt.hg_context != NULL);

	// Register a RPC function
	stt.save_rpc_id = MERCURY_REGISTER(stt.hg_class, "save", save_in_t, save_out_t, NULL);

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

	assert(callback_info->ret == 0);

	/* We get the pointer to the engine_state here. */
	engine_state* state = (engine_state*)(callback_info->arg);

	state->buffer_size = 512;
	state->buffer = calloc(1, 512);
	int i;
	for(i=0; i<512; i++) state->buffer[i] = 65 + (i % 26);

	hg_addr_t addr = callback_info->info.lookup.addr;
	hg_handle_t handle;
	ret = HG_Create(state->hg_context, addr, state->save_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	save_in_t in;
	in.filename = "save.txt"; //(hg_string_t)calloc(1,128);
	//sprintf(in.filename,"save.txt");

	ret = HG_Bulk_create(state->hg_class, 1, (void**) &(state->buffer), &(state->buffer_size),
					HG_BULK_READ_ONLY, &in.bulk_handle);
	if(ret != HG_SUCCESS) printf("%s\n",HG_Error_to_string(ret));
	assert(ret == HG_SUCCESS);		

	/* The state pointer is passed along as user argument. */
	ret = HG_Forward(handle, save_completed, state, &in);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}

hg_return_t save_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;

	/* Get the state pointer from the user-provided arguments. */
	engine_state* state = (engine_state*)(info->arg);
	
	save_out_t out;
	assert(info->ret == HG_SUCCESS);

	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	HG_Bulk_free(state->bulk_handle);
	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	state->completed = 1;

	return HG_SUCCESS;
}
