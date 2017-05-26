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

	state.hg_class = HG_Init("bmi+tcp://localhost:1234", HG_TRUE);
	assert(state.hg_class != NULL);

	state.hg_context = HG_Context_create(state.hg_class);
	assert(state.hg_context != NULL);

	ABT_init(argc,argv);

	ABT_snoozer_xstream_self_set();

	state.mid = margo_init(0, 0, state.hg_context);
	assert(state.mid);

	fwd_rpc_id = MERCURY_REGISTER(state.hg_class, "forward_save", save_in_t, save_out_t, forward_save_handler);
	save_rpc_id = MERCURY_REGISTER(state.hg_class, "save", save_in_t, save_out_t, NULL);

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
	// Get the server_state attached to the RPC.
	const struct hg_info* info = HG_Get_info(handle);
	server_state* stt = HG_Registered_data(info->hg_class, info->id);

	ret = HG_Get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	save_out_t out;
	out.ret = 0;

	printf("Forwarding request to backup %s...\n", in.filename);

	hg_addr_t svr_addr;
	ret = margo_addr_lookup(stt->mid, "bmi+tcp://localhost:1235", &svr_addr);
	assert(ret == HG_SUCCESS);

	hg_handle_t save_handle;
	ret = HG_Create(stt->hg_context, svr_addr, save_rpc_id, &save_handle);
	assert(ret == HG_SUCCESS);

	ret = margo_forward(stt->mid, save_handle, &in);
	assert(ret == HG_SUCCESS);

	ret = HG_Get_output(save_handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_respond(stt->mid, handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_input(handle, &in);
	assert(ret == HG_SUCCESS);

	HG_Destroy(handle);
	HG_Destroy(save_handle);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(forward_save)
