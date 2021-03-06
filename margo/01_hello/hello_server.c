#include <assert.h>
#include <stdio.h>
#include <margo.h>

/* after serving this number of rpcs, the server will shut down. */
static const int TOTAL_RPCS = 4;
/* number of RPCS already received. */
static int num_rpcs = 0;

/* 
 * hello_world function to expose as an RPC.
 * This function just prints "Hello World"
 * and increment the num_rpcs variable.
 *
 * All Mercury RPCs must have a signature
 *   hg_return_t f(hg_handle_t h)
 */
hg_return_t hello_world(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(hello_world)

/*
 * main function.
 */
int main(int argc, char** argv)
{
	/* Initialize Margo */
	margo_instance_id mid = margo_init("tcp", MARGO_SERVER_MODE, 0, -1);
    assert(mid);

	hg_addr_t my_address;
	margo_addr_self(mid, &my_address);
	char addr_str[128];
	size_t addr_str_size = 128;
	margo_addr_to_string(mid, addr_str, &addr_str_size, my_address);
	margo_addr_free(mid,my_address);
	printf("Server running at address %s\n", addr_str);

	/* Register the RPC by its name ("hello") */
	hg_id_t rpc_id = MARGO_REGISTER(mid, "hello", void, void, hello_world);

	/* We call this function to tell Mercury that hello_world will not
	 * send any response back to the client.
	 */
	margo_registered_disable_response(mid, rpc_id, HG_TRUE);

	/* NOTE: there isn't anything else for the server to do at this point
     * except wait for itself to be shut down.  The
     * margo_wait_for_finalize() call here yields to let Margo drive
     * progress until that happens.
	 */
	margo_wait_for_finalize(mid);

	return 0;
}

/* Implementation of the hello_world RPC. */
hg_return_t hello_world(hg_handle_t h)
{
	hg_return_t ret;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

	printf("Hello World!\n");
	num_rpcs += 1;

    /* We are not going to use the handle anymore, so we should destroy it. */
    ret = margo_destroy(h);
    assert(ret == HG_SUCCESS);

	if(num_rpcs == TOTAL_RPCS) {
		/* NOTE: we assume that the server daemon is using
		 * margo_wait_for_finalize() to suspend until this RPC executes, so there
		 * is no need to send any extra signal to notify it.
		 */
        /* Finalize Margo */
		margo_finalize(mid);
	}

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(hello_world)
