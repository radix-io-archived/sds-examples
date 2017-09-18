#include <assert.h>
#include <stdio.h>
#include <margo.h>

/* Main function. */
int main(int argc, char** argv)
{
	hg_return_t ret;
	margo_instance_id mid = MARGO_INSTANCE_NULL;
	

	/* Start Margo */
	mid = margo_init("bmi+tcp",MARGO_CLIENT_MODE, 0, 0);
	assert(mid);

	/* Register a RPC function */
	hg_id_t hello_rpc_id = MARGO_REGISTER(mid, "hello", void, void, NULL);
	
	/* Indicate Mercury that we shouldn't expect a response from the server
	 * when calling this RPC
	 */
	margo_registered_disable_response(mid, hello_rpc_id, HG_TRUE);

	/* Lookup the address of the server */
	hg_addr_t svr_addr;
	margo_addr_lookup(mid, "bmi+tcp://localhost:1234", &svr_addr);
	
	/* Create a RPC handle to send to the server */
	hg_handle_t handle;
	ret = margo_create(mid, svr_addr, hello_rpc_id, &handle);
	assert(ret == HG_SUCCESS);	

	/* forward RPC */
	margo_forward(mid, handle, NULL);

	/* destroy the handle */
	margo_destroy(mid, handle);

	/* free address */
	margo_addr_free(mid, svr_addr);

	/* Shut down Margo */
	margo_finalize(mid);

	return 0;
}
