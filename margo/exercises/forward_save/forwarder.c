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

hg_id_t fwd_rpc_id;
hg_id_t save_rpc_id;

static hg_return_t forward_save(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(forward_save);

int main(int argc, char** argv)
{
	hg_return_t ret;

	server_state state; // Instance of the server's state

	// TODO make sure the forwarder is listening to the right port
	state.hg_class = HG_Init("bmi+tcp://localhost:1234", HG_TRUE);
	assert(state.hg_class != NULL);

	state.hg_context = HG_Context_create(state.hg_class);
	assert(state.hg_context != NULL);

	ABT_init(argc,argv);

	ABT_snoozer_xstream_self_set();

	state.mid = margo_init(0, 0, state.hg_context);
	assert(state.mid);

	// TODO register the forward_save and save RPCs
	// fwd_rpc_id = ...
	// save_rpc_id = ...

	/* Attach the local server_state to the RPC so we can get a pointer to it when
	 * the RPC is invoked. */
	ret = HG_Register_data(state.hg_class, fwd_rpc_id, &state, NULL);

	margo_wait_for_finalize(state.mid);

	ABT_finalize();

	ret = HG_Context_destroy(state.hg_context);
	assert(ret == HG_SUCCESS);

	ret = HG_Finalize(state.hg_class);
	assert(ret == HG_SUCCESS);

	return 0;
}

hg_return_t forward_save(hg_handle_t handle)
{
	hg_return_t ret;
	save_in_t in;
	save_out_t out;
	// Get the server_state attached to the RPC.
	const struct hg_info* info = HG_Get_info(handle);
	server_state* stt = HG_Registered_data(info->hg_class, info->id);

	// TODO get the input of the RPC in "in"
	
	printf("Forwarding request to backup %s...\n", in.filename);

	// TODO do a lookup of the save_server
	hg_addr_t svr_addr;

	// TODO create the RPC handle for a "save" RPC to the server
	hg_handle_t save_handle;

	// TODO forward the RPC using Margo

	// TODO get the output of the RPC sent to the server

	// TODO respond to the client

	// TODO free the out variable

	// TODO free the in variable

	// TODO destroy then "handle"

	// TODO destroy the "save_handle"

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(forward_save)
