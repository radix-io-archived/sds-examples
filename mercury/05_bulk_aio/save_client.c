#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <mercury.h>
#include "types.h"

typedef struct {
	hg_class_t* 	hg_class;
	hg_context_t*	hg_context;
	hg_id_t			save_rpc_id;
	int 			completed;
} engine_state;

typedef struct {
	engine_state* 	engine;
	hg_bulk_t     	bulk_handle;
	void*			buffer;
	size_t			size;
	char*			filename;
} save_operation;

hg_return_t lookup_callback(const struct hg_cb_info *callback_info);
hg_return_t save_completed(const struct hg_cb_info *info);

int main(int argc, char** argv)
{
	if(argc != 4) {
		fprintf(stderr,"Usage: %s <protocol> <server address> <filename>\n", argv[0]);
		exit(0);
	}

	const char* protocol = argv[1];

	hg_return_t ret;

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
	stt.save_rpc_id = MERCURY_REGISTER(stt.hg_class, "save", save_in_t, save_out_t, NULL);

	// Create the save_operation structure
	save_operation save_op;
	save_op.engine = &stt;
	save_op.filename = argv[3];
	if(access(save_op.filename, F_OK) == -1) {
    	fprintf(stderr,"File %s doesn't exist or cannot be accessed.\n",save_op.filename);
		exit(-1);
	} 

	char* server_address = argv[2];
	ret = HG_Addr_lookup(stt.hg_context, lookup_callback, &save_op, server_address, HG_OP_ID_IGNORE);

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
	save_operation* save_op = (save_operation*)(callback_info->arg);
	engine_state* state = save_op->engine;

	/* Check file size to allocate buffer. */
	FILE* file = fopen(save_op->filename,"r");
	fseek(file, 0L, SEEK_END);
	save_op->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	save_op->buffer = calloc(1, save_op->size);
	fread(save_op->buffer,1,save_op->size,file);
	fclose(file);

	hg_addr_t addr = callback_info->info.lookup.addr;
	hg_handle_t handle;
	ret = HG_Create(state->hg_context, addr, state->save_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	save_in_t in;
	in.filename = save_op->filename;
	in.size		= save_op->size; 

	ret = HG_Bulk_create(state->hg_class, 1, (void**) &(save_op->buffer), &(save_op->size),
					HG_BULK_READ_ONLY, &(save_op->bulk_handle));
	assert(ret == HG_SUCCESS);
	in.bulk_handle = save_op->bulk_handle;

	/* The state pointer is passed along as user argument. */
	ret = HG_Forward(handle, save_completed, save_op, &in);
	assert(ret == HG_SUCCESS);

	/* Free the address. */
	ret = HG_Addr_free(state->hg_class, addr);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}

hg_return_t save_completed(const struct hg_cb_info *info)
{
	hg_return_t ret;

	/* Get the state pointer from the user-provided arguments. */
	save_operation* save_op = (save_operation*)(info->arg);
	engine_state* state = (engine_state*)(save_op->engine);
	
	save_out_t out;
	assert(info->ret == HG_SUCCESS);

	ret = HG_Get_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Got response: %d\n", out.ret);

	ret = HG_Bulk_free(save_op->bulk_handle);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_output(info->info.forward.handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(info->info.forward.handle);
	assert(ret == HG_SUCCESS);

	state->completed = 1;

	return HG_SUCCESS;
}
