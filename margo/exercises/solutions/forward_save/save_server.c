#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

static hg_return_t save(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(save);

int main(int argc, char** argv)
{
	margo_instance_id mid = margo_init("bmi+tcp://localhost:1235", MARGO_SERVER_MODE, 0, 0);
	assert(mid);

	MARGO_REGISTER(mid, "save", save_in_t, save_out_t, save);
	
	margo_wait_for_finalize(mid);

	return 0;
}

hg_return_t save(hg_handle_t handle)
{
	hg_return_t ret;
	save_in_t in;

	margo_instance_id mid = margo_hg_handle_get_instance(handle);

	ret = margo_get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	void* buffer = calloc(1,in.size);
	hg_bulk_t bulk_handle;

	ret = margo_bulk_create(mid, 1, &buffer,
				&in.size, HG_BULK_WRITE_ONLY, &bulk_handle);
    assert(ret == HG_SUCCESS);

	/* get address of client */
	hg_addr_t client_addr;
    ret = margo_addr_lookup(mid, in.address, &client_addr);

    /* initiate bulk transfer from client to server */
	ret = margo_bulk_transfer(mid, HG_BULK_PULL,
        client_addr, in.bulk_handle, 0,
        bulk_handle, 0, in.size);
    assert(ret == 0);

	FILE* f = fopen(in.filename,"w+");
	fwrite(buffer, 1, in.size, f);
	fclose(f);

	printf("Writing file %s\n", in.filename);

	save_out_t out;
	out.ret = 0;

	ret = margo_respond(handle, &out);
    assert(ret == HG_SUCCESS);

	ret = margo_free_input(handle, &in);
	assert(ret == HG_SUCCESS);

    margo_bulk_free(bulk_handle);
    margo_destroy(handle);
    free(buffer);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(save)
