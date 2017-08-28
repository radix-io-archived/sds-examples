#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include <mercury.h>
#include "types.h"
#include "phone_book.h"

static margo_instance_id mid 		= MARGO_INSTANCE_NULL;
static phonebook myphonebook;

hg_return_t set_num(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(set_num)

hg_return_t get_num(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(get_num)

/*
 * main function.
 */
int main(int argc, char** argv)
{
	phonebook_create(&myphonebook);

	/* Initialize Margo */
	mid = margo_init("bmi+tcp://localhost:1234", MARGO_SERVER_MODE, 0, 0);
	assert(mid);

	/* Register the RPCs by their names */
	MARGO_REGISTER(mid, "get_num", get_num_in_t, get_num_out_t, get_num);
	MARGO_REGISTER(mid, "set_num", set_num_in_t, set_num_out_t, set_num);

	/* NOTE: there isn't anything else for the server to do at this point
	 * except wait for itself to be shut down.  The
	 * margo_wait_for_finalize() call here yields to let Margo drive
	 * progress until that happens.
	 */
	margo_wait_for_finalize(mid);

	return 0;
}

/* Implementation of the RPC. */
hg_return_t get_num(hg_handle_t h)
{
	hg_return_t ret;

	get_num_in_t in;
	get_num_out_t out;

	/* Deserialize the input from the received handle. */
	ret = margo_get_input(h, &in);
	assert(ret == HG_SUCCESS);

	/* Compute the result. */
	out.ret = 0;
	out.phone = (char*)phonebook_find(myphonebook, in.name);
	if(out.phone == NULL) out.ret = -1;

	ret = margo_respond(mid, h, &out);
	assert(ret == HG_SUCCESS);

	/* Free the input data. */
	ret = margo_free_input(h, &in);
	assert(ret == HG_SUCCESS);

	/* We are not going to use the handle anymore, so we should destroy it. */
	ret = margo_destroy(h);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(get_num)

hg_return_t set_num(hg_handle_t h)
{
	hg_return_t ret;

	set_num_in_t in;
	set_num_out_t out;

	ret = margo_get_input(h, &in);
	assert(ret == HG_SUCCESS);

	phonebook_insert(myphonebook, in.name, in.phone);

	out.ret = 0;

	ret = margo_respond(mid, h, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_free_input(h, &in);
	assert(ret == HG_SUCCESS);

	ret = margo_destroy(h);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(set_num)
