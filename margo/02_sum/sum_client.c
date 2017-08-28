#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

/* Main function. */
int main(int argc, char** argv)
{
	/* Start Margo */
	margo_instance_id mid = margo_init("bmi+tcp", MARGO_CLIENT_MODE, 0, 0);

	/* Register a RPC function */
	hg_id_t sum_rpc_id = MARGO_REGISTER(mid, "sum", sum_in_t, sum_out_t, NULL);
	
	/* Lookup the address of the server */
	hg_addr_t svr_addr;
	margo_addr_lookup(mid, "bmi+tcp://localhost:1234", &svr_addr);
		
	int i;
	sum_in_t args;
	for(i=0; i<4; i++) {
		args.x = 42+i*2;
		args.y = 42+i*2+1;

		hg_handle_t h;
		margo_create(mid, svr_addr, sum_rpc_id, &h);
		margo_forward(mid, h, &args);
		
		sum_out_t resp;
		margo_get_output(h, &resp);

		printf("Got response: %d+%d = %d\n", args.x, args.y, resp.ret);

		margo_free_output(h,&resp);
		margo_destroy(h);
	}

	/* free the address */
	margo_addr_free(mid, svr_addr);

	/* shut down Margo */
    margo_finalize(mid);

	return 0;
}
