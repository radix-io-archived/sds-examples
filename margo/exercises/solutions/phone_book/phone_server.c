#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include <mercury.h>
#include "types.h"
#include "phone_book.h"

static hg_class_t*     hg_class 	= NULL; /* the mercury class */
static hg_context_t*   hg_context 	= NULL; /* the mercury context */
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
	hg_return_t ret;

	phonebook_create(&myphonebook);

	/* Initialize Mercury and get an hg_class handle.
	 * bmi+tcp is the protocol to use.
	 * localhost is the address of the server (not useful at the server itself).
	 * HG_TRUE is here to specify that mercury will listen for incoming requests.
	 * (HG_TRUE on servers, HG_FALSE on clients).
	 */
	hg_class = HG_Init("bmi+tcp://localhost:1234", HG_TRUE);
	assert(hg_class != NULL);

	/* Creates a Mercury context from the Mercury class. */
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	/* Setup Argobots */
	ABT_init(argc, argv);

	/* set primary ES to idle without polling */
	ABT_snoozer_xstream_self_set();

	/* Initialize Margo */
	mid = margo_init(0, 0, hg_context);
	assert(mid);

	/* Register the RPCs by their names */
	MERCURY_REGISTER(hg_class, "get_num", get_num_in_t, get_num_out_t, get_num_handler);
	MERCURY_REGISTER(hg_class, "set_num", set_num_in_t, set_num_out_t, set_num_handler);

	/* NOTE: there isn't anything else for the server to do at this point
	 * except wait for itself to be shut down.  The
	 * margo_wait_for_finalize() call here yields to let Margo drive
	 * progress until that happens.
	 */
	margo_wait_for_finalize(mid);

	/* Finalize Argobots */
	ABT_finalize();
	/* Destroys the Mercury context. */
	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	/* Finalize Mercury. */
	ret = HG_Finalize(hg_class);
	assert(ret == HG_SUCCESS);

	return 0;
}

/* Implementation of the RPC. */
hg_return_t get_num(hg_handle_t h)
{
	hg_return_t ret;

	get_num_in_t in;
	get_num_out_t out;

	/* Deserialize the input from the received handle. */
	ret = HG_Get_input(h, &in);
	assert(ret == HG_SUCCESS);

	/* Compute the result. */
	out.ret = 0;
	out.phone = phonebook_find(myphonebook, in.name);
	if(out.phone == NULL) out.ret = -1;

	ret = margo_respond(mid, h, &out);
	assert(ret == HG_SUCCESS);

	/* Free the input data. */
	ret = HG_Free_input(h, &in);
	assert(ret == HG_SUCCESS);

	/* We are not going to use the handle anymore, so we should destroy it. */
	ret = HG_Destroy(h);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(get_num)

hg_return_t set_num(hg_handle_t h)
{
	hg_return_t ret;

	set_num_in_t in;
	set_num_out_t out;

	ret = HG_Get_input(h, &in);
	assert(ret == HG_SUCCESS);

	phonebook_insert(myphonebook, in.name, in.phone);

	out.ret = 0;

	ret = margo_respond(mid, h, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_input(h, &in);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(h);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(set_num)
