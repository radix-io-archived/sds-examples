#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include <mercury.h>
#include "types.h"

typedef struct {
	hg_id_t			save_rpc_id;
	margo_instance_id mid;
	hg_addr_t       svr_addr;
} engine_state;

typedef struct {
	engine_state* 	engine;
	hg_bulk_t     	bulk_handle;
	void*			buffer;
	size_t			size;
	char*			filename;
} save_operation;

static void send_file(save_operation *arg);

int main(int argc, char** argv)
{
	if(argc < 2) {
		fprintf(stderr,"Usage: %s filename1 filename2 ...\n", argv[0]);
		exit(0);
	}

	int num_files = argc-1;

	/* Local instance of the engine_state. */
	engine_state stt;

	// Start Margo
	stt.mid = margo_init("bmi+tcp", MARGO_CLIENT_MODE, 0, 0);

	// Register a RPC function
	stt.save_rpc_id = MARGO_REGISTER(stt.mid, "save", save_in_t, save_out_t, NULL);

	margo_addr_lookup(stt.mid, "bmi+tcp://localhost:1234", &(stt.svr_addr));

	int i;
	for(i=0; i<num_files; i++) {

		save_operation operation;
		// Create the save_operation structure
		operation.engine = &stt;
		operation.filename = argv[i+1];

		// Check that file exists
		if(access(operation.filename, F_OK) == -1) {
			fprintf(stderr,"File %s doesn't exist or cannot be accessed.\n", operation.filename);
			exit(-1);
		}

		send_file(&operation);
	}

	/* free address */
	margo_addr_free(stt.mid, stt.svr_addr);

	/* Finalize Margo */
	margo_finalize(stt.mid);
	return 0;
}

void send_file(save_operation* save_op)
{
	hg_return_t ret;
	/* We get the pointer to the engine_state here. */
	engine_state* state = save_op->engine;

	/* Check file size to allocate buffer. */
	FILE* file = fopen(save_op->filename,"r");
	fseek(file, 0L, SEEK_END);
	save_op->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	save_op->buffer = calloc(1, save_op->size);
	fread(save_op->buffer,1,save_op->size,file);
	fclose(file);

	hg_handle_t handle;
	ret = margo_create(state->mid, state->svr_addr, state->save_rpc_id, &handle);
	assert(ret == HG_SUCCESS);
	
	save_in_t in;
	in.filename = save_op->filename;
	in.size		= save_op->size; 

	// TODO use margo_addr_self and margo_addr_to_string to
	// add the client address into the "in" structure

	ret = margo_bulk_create(state->mid, 1, (void**) &(save_op->buffer), &(save_op->size),
					HG_BULK_READ_ONLY, &(save_op->bulk_handle));
	assert(ret == HG_SUCCESS);
	in.bulk_handle = save_op->bulk_handle;

	margo_forward(handle, &in);

	save_out_t out;

	ret = margo_get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Saving file %s => got response: %d\n", in.filename, out.ret);

	ret = margo_bulk_free(save_op->bulk_handle);
	assert(ret == HG_SUCCESS);

	ret = margo_free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_destroy(handle);
	assert(ret == HG_SUCCESS);
}
