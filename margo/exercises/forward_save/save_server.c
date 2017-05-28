#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

/* This structure will encapsulate data about the server. */
typedef struct {
	hg_class_t*     hg_class;
	hg_context_t*   hg_context;
	margo_instance_id mid;
} server_state;

static hg_return_t save(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(save);

int main(int argc, char** argv)
{
	hg_return_t ret;

	server_state state; // Instance of the server's state

	// TODO change the port number on which the server is listening
	state.hg_class = HG_Init("bmi+tcp://localhost:1234", HG_TRUE);
	assert(state.hg_class != NULL);

	state.hg_context = HG_Context_create(state.hg_class);
	assert(state.hg_context != NULL);

	ABT_init(argc,argv);

	ABT_snoozer_xstream_self_set();

	state.mid = margo_init(0, 0, state.hg_context);
	assert(state.mid);

	hg_id_t rpc_id = MERCURY_REGISTER(state.hg_class, "save", save_in_t, save_out_t, save_handler);

	/* Attach the local server_state to the RPC so we can get a pointer to it when
	 * the RPC is invoked. */
	ret = HG_Register_data(state.hg_class, rpc_id, &state, NULL);

	margo_wait_for_finalize(state.mid);

	ABT_finalize();

	ret = HG_Context_destroy(state.hg_context);
	assert(ret == HG_SUCCESS);

	ret = HG_Finalize(state.hg_class);
	assert(ret == HG_SUCCESS);

	return 0;
}

hg_return_t save(hg_handle_t handle)
{
	hg_return_t ret;
	save_in_t in;
	// Get the server_state attached to the RPC.
	const struct hg_info* info = HG_Get_info(handle);
	server_state* stt = HG_Registered_data(info->hg_class, info->id);

	ret = HG_Get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	void* buffer = calloc(1,in.size);
	hg_bulk_t bulk_handle;

	ret = HG_Bulk_create(stt->hg_class, 1, &buffer,
			&in.size, HG_BULK_WRITE_ONLY, &bulk_handle);
	assert(ret == HG_SUCCESS);

	/* get address of client */
	hg_addr_t client_addr;
	// TODO uncomment this line after having added the "address" field in types.h
	//ret = margo_addr_lookup(stt->mid, in.address, &client_addr);
	assert(ret == HG_SUCCESS);

	/* initiate bulk transfer from client to server */
	ret = margo_bulk_transfer(stt->mid, HG_BULK_PULL,
			client_addr, in.bulk_handle, 0,
			bulk_handle, 0, in.size);
	assert(ret == 0);

	HG_Addr_free(stt->hg_class, client_addr);

	FILE* f = fopen(in.filename,"w+");
	fwrite(buffer, 1, in.size, f);
	fclose(f);

	printf("Writing file %s\n", in.filename);

	save_out_t out;
	out.ret = 0;

	ret = margo_respond(stt->mid, handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_input(handle, &in);
	assert(ret == HG_SUCCESS);

	HG_Bulk_free(bulk_handle);
	HG_Destroy(handle);
	free(buffer);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(save)
