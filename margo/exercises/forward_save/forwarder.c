#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

hg_id_t fwd_rpc_id;
hg_id_t save_rpc_id;

static hg_return_t forward_save(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(forward_save);

int main(int argc, char** argv)
{
	margo_instance_id mid = margo_init("bmi+tcp://localhost:1234", MARGO_SERVER_MODE, 0, 0);
	assert(mid);

	// TODO register the forward_save and save RPCs
	// fwd_rpc_id = ...
	// save_rpc_id = ...

	margo_wait_for_finalize(mid);

	return 0;
}

hg_return_t forward_save(hg_handle_t handle)
{
	hg_return_t ret;
	save_in_t in;
	save_out_t out;

	// TODO get the margo_instance_id attached to the RPC.

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
