#include <assert.h>
#include <stdio.h>
#include <abt.h>
#include <margo.h>
#include <mercury.h>
#include "types.h"
#include "phone_book.h"

static margo_instance_id mid 		= MARGO_INSTANCE_NULL;
static phonebook myphonebook;

/* 
 * hello_world function to expose as an RPC.
 * This function just prints "Hello World"
 * and increment the num_rpcs variable.
 *
 * All Mercury RPCs must have a signature
 *   hg_return_t f(hg_handle_t h)
 */
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

	// TODO: Initialize Margo (0 progress thread, 0 RPC thread)

	// TODO: Register the get_num and set_num RPCs

	// TODO: Call Margo's main loop function (waiting for finalize)

	return 0;
}

/* Implementation of the RPC. */
hg_return_t get_num(hg_handle_t h)
{
	hg_return_t ret;

	// get_num_in_t in;
	// get_num_out_t out;

	// TODO: Deserialize the input into "in"

	/* Compute the result. */
	/*
	out.ret = 0;
	out.phone = phonebook_find(myphonebook, in.name);
	if(out.phone == NULL) out.ret = -1;
	*/

	// TODO: Respond to the RPC using Margo

	// TODO: Free the input

	// TODO: Destroy the RPC handle

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(get_num)

hg_return_t set_num(hg_handle_t h)
{
	hg_return_t ret;

	// set_num_in_t in;
	// set_num_out_t out;

	// TODO: Get the input

	// TODO: uncomment
	// phonebook_insert(myphonebook, in.name, in.phone);

	//out.ret = 0;

	// TODO: Respond to the RPC using Margo

	// TODO: Free the input

	// TODO: Destroy the handle

	return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(set_num)
