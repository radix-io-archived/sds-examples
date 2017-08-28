#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

/* This structure will encapsulate data about the server. */

hg_id_t fwd_rpc_id;
hg_id_t save_rpc_id;

static hg_return_t forward_save(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(forward_save);

int main(int argc, char** argv)
{
	margo_instance_id mid = margo_init("bmi+tcp://localhost:1234", MARGO_SERVER_MODE, 0, 0);
	assert(mid);

	fwd_rpc_id  = MARGO_REGISTER(mid, "forward_save", save_in_t, save_out_t, forward_save);
	save_rpc_id = MARGO_REGISTER(mid, "save", save_in_t, save_out_t, NULL);

	margo_wait_for_finalize(mid);

	return 0;
}

hg_return_t forward_save(hg_handle_t handle)
{
	hg_return_t ret;
	save_in_t in;

	margo_instance_id mid = margo_hg_handle_get_instance(handle);

	ret = margo_get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	save_out_t out;
	out.ret = 0;

	printf("Forwarding request to backup %s...\n", in.filename);

	hg_addr_t svr_addr;
	ret = margo_addr_lookup(mid, "bmi+tcp://localhost:1235", &svr_addr);
	assert(ret == HG_SUCCESS);

	hg_handle_t save_handle;
	ret = margo_create(mid, svr_addr, save_rpc_id, &save_handle);
	assert(ret == HG_SUCCESS);

	ret = margo_forward(mid, save_handle, &in);
	assert(ret == HG_SUCCESS);

	ret = HG_Get_output(save_handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_respond(mid, handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_free_input(handle, &in);
	assert(ret == HG_SUCCESS);

	margo_destroy(handle);
	margo_destroy(save_handle);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(forward_save)
