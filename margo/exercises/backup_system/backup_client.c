#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include <mercury.h>
#include "types.h"

typedef struct {
	hg_class_t* 	hg_class;
	hg_context_t*	hg_context;
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

static void run_my_rpc(void *arg);

int main(int argc, char** argv)
{
	if(argc < 2) {
		fprintf(stderr,"Usage: %s filename1 filename2 ...\n", argv[0]);
		exit(0);
	}

	hg_return_t ret;
	ABT_xstream xstream;
	ABT_pool pool;
	ABT_thread* threads;
	save_operation* operations;
	int num_files = argc-1;

	/* Local instance of the engine_state. */
	engine_state stt;
	// Initialize an hg_class.
	stt.hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(stt.hg_class != NULL);

	// Creates a context for the hg_class.
	stt.hg_context = HG_Context_create(stt.hg_class);
	assert(stt.hg_context != NULL);

	// Initialize Argobots
	ABT_init(argc, argv);

	// set primary ES to idle without polling
	ABT_snoozer_xstream_self_set();

	// get main ES
	ABT_xstream_self(&xstream); 

	// get main pool
	ABT_xstream_get_main_pools(xstream, 1, &pool);

	// Start Margo
	stt.mid = margo_init(0, 0, stt.hg_context);

	// Register a RPC function
	stt.save_rpc_id = MERCURY_REGISTER(stt.hg_class, "save", save_in_t, save_out_t, NULL);

	margo_addr_lookup(stt.mid, "bmi+tcp://localhost:1234", &(stt.svr_addr));

	threads = (ABT_thread*)malloc(num_files*sizeof(ABT_thread));
	operations = (save_operation*)malloc(num_files*sizeof(save_operation));
	int i;
	for(i=0; i<num_files; i++) {
		// Create the save_operation structure
		operations[i].engine = &stt;
		operations[i].filename = argv[i+1];

		// Check that file exists
		if(access(operations[i].filename, F_OK) == -1) {
			fprintf(stderr,"File %s doesn't exist or cannot be accessed.\n",operations[i].filename);
			exit(-1);
		}
	}
	// Start threads
	for(i=0; i<num_files; i++) {
		ABT_thread_create(pool, run_my_rpc, &operations[i],
			ABT_THREAD_ATTR_NULL, &threads[i]);
	}

	/* Yield to one of the threads */
	ABT_thread_yield_to(threads[0]);

	for(i=0; i<num_files; i++) {
		ABT_thread_join(threads[i]);
		ABT_thread_free(&threads[i]);
	}

	/* Finalize Margo */
	margo_finalize(stt.mid);

	/* Finalize Argobots */
	ABT_finalize();

	free(threads);
	free(operations);

	// Destroy the context
	ret = HG_Context_destroy(stt.hg_context);
	assert(ret == HG_SUCCESS);

	// Finalize the hg_class.
	hg_return_t err = HG_Finalize(stt.hg_class);
	assert(err == HG_SUCCESS);
	return 0;
}

void run_my_rpc(void *arg)
{
	hg_return_t ret;
	/* We get the pointer to the engine_state here. */
	save_operation* save_op = (save_operation*)arg;
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
	ret = HG_Create(state->hg_context, state->svr_addr, state->save_rpc_id, &handle);
	assert(ret == HG_SUCCESS);
	
	save_in_t in;
	in.filename = save_op->filename;
	in.size		= save_op->size; 

	ret = HG_Bulk_create(state->hg_class, 1, (void**) &(save_op->buffer), &(save_op->size),
					HG_BULK_READ_ONLY, &(save_op->bulk_handle));
	assert(ret == HG_SUCCESS);
	in.bulk_handle = save_op->bulk_handle;

	margo_forward(state->mid, handle, &in);

	save_out_t out;

	ret = HG_Get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	printf("Saving file %s => got response: %d\n", in.filename, out.ret);

	ret = HG_Bulk_free(save_op->bulk_handle);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);
}
