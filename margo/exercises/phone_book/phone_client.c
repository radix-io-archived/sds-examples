#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

static hg_class_t*     hg_class 	= NULL; /* Pointer to the Mercury class */
static hg_context_t*   hg_context 	= NULL; /* Pointer to the Mercury context */
static margo_instance_id mid		= MARGO_INSTANCE_NULL;
<<<<<<< HEAD
static hg_id_t         get_num_rpc_id;		/* ID of the RPCs */
=======
static hg_id_t         get_num_rpc_id;		/* ID of the RPC */
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
static hg_id_t				 set_num_rpc_id;
static hg_addr_t       svr_addr;

typedef enum {
	SET_PHONE,
	GET_PHONE,
	STOP,
	UNKNOWN
} op_type;

static op_type get_op_type(const char* line);
static void get_num(char* line);
static void set_num(char* line);

/* Main function. */
int main(int argc, char** argv)
{
	hg_return_t ret;
<<<<<<< HEAD
=======
//	ABT_xstream xstream;
//	ABT_pool pool;
//	ABT_thread threads[4];
//	thread_args args[4];
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7

	/* 
	 * Initialize an hg_class.
	 * Here we only specify the protocal since this is a client
	 * (no need for an address and a port). HG_FALSE indicates that
	 * the client will not listen for incoming requests.
	 */
<<<<<<< HEAD

	// TODO: Initialize Mercury (hg_class) to use bmi+tcp and listening=false

	// TODO: Create the hg_context from the initialized hg_class

	// TODO: Initialize Argobots
=======
	hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(hg_class != NULL);

	/* Creates a context for the hg_class. */
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	/* Initialie Argobots */
	ABT_init(argc, argv);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7

	/* set primary ES to idle without polling */
	ABT_snoozer_xstream_self_set();

<<<<<<< HEAD
	// TODO: Initialize Margo (get mid). We will use 0 progress thread and 0 RPC thread

	// TODO: Call MERCURY_REGISTER to initialize get_num_rpc_id and set_num_rpc_id

	// TODO: Do an address lookup to initialize svr_addr, using Margo

	// Here we read lines entered by the user. These lines can start with
  // set, get, or stop, followed by arguments
=======
	/* get main ES */
//	ABT_xstream_self(&xstream);

	/* get main pool */
//	ABT_xstream_get_main_pools(xstream, 1, &pool);

	/* Start Margo */
	mid = margo_init(0, 0, hg_context);

	/* Register a RPC function.
	 * The first two NULL correspond to what would be pointers to
	 * serialization/deserialization functions for input and output datatypes
	 * (not used in this example).
	 * The third NULL is the pointer to the function (which is on the server,
	 * so NULL here on the client).
	 */
	get_num_rpc_id = MERCURY_REGISTER(hg_class, "get_num", get_num_in_t, get_num_out_t, NULL);
	set_num_rpc_id = MERCURY_REGISTER(hg_class, "set_num", set_num_in_t, set_num_out_t, NULL);

	/* Lookup the address of the server, this is asynchronous and
	 * the result will be handled by lookup_callback once we start the progress loop.
	 * NULL correspond to a pointer to user data to pass to lookup_callback (we don't use
	 * any here). The 4th argument is the address of the server.
	 * The 5th argument is a pointer a variable of type hg_op_id_t, which identifies the operation.
	 * It can be useful to get this identifier if we want to be able to cancel it using
	 * HG_Cancel. Here we don't use it so we pass HG_OP_ID_IGNORE.
	 */
	margo_addr_lookup(mid, "bmi+tcp://localhost:1234", &svr_addr);

>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
	char line[256];
	op_type t = UNKNOWN;
	while(t != STOP) {
		char* s = fgets(line,256,stdin);
		if(s != NULL) {
			t = get_op_type(line);
			switch(t) {
			case GET_PHONE:
				get_num(line);
				break;
			case SET_PHONE:
				set_num(line);
				break;
			case UNKNOWN:
				printf("Unknown operation\n");
				break;
			case STOP:
				break;
			}
		} else {
			t = STOP;
		}
	}	
<<<<<<< HEAD

	// TODO: Shut down Margo 

	// TODO: Finalize Argobots

	// TODO: Destroy the mercury context

	// TODO: Finalize mercury
=======
/*	for(i=0; i<4; i++) {
			args[i].in.x = 42+i*2;
			args[i].in.y = 42+i*2+1;
			ABT_thread_create(pool, run_my_rpc, &args[i],
						ABT_THREAD_ATTR_NULL, &threads[i]);
	}
*/

	/* yield to one of the threads */
//    ABT_thread_yield_to(threads[0]);
/*
	for(i=0; i<4; i++)
    {
        ABT_thread_join(threads[i]);
        ABT_thread_free(&threads[i]);
    }
*/
	/* shut down Margo */
    margo_finalize(mid);

	/* Finalize Argobots */
	ABT_finalize();

	/* Destroy the context. */
	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	/* Finalize the hg_class. */
	hg_return_t err = HG_Finalize(hg_class);
	assert(err == HG_SUCCESS);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
	return 0;
}

static op_type get_op_type(const char* line)
{
	if(strstr(line, "get") == line) {
		return GET_PHONE;
	} else if(strstr(line, "set") == line) {
		return SET_PHONE;
	} else if(strstr(line, "stop") == line) {
		return STOP;
	}
	return UNKNOWN;
}

static void get_num(char* line)
{
	char* name = strstr(line,"get")+3;
	while(!isalnum(*name)) name += 1;
	char* end = name;
	while(isalnum(*end)) end += 1;
	*end = '\0';
	
	hg_return_t ret;
	hg_handle_t handle;
<<<<<<< HEAD

	// TODO: Create the handle for the get_num rpc

	// TODO: Declare and set the get_num_in_t argument for the RPC

	// TODO: Forward the RPC handle using Margo


	get_num_out_t out;
	// TODO: Get the output of the RPC

=======
	ret = HG_Create(hg_context, svr_addr, get_num_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	get_num_in_t in;
	in.name = name;

	margo_forward(mid, handle, &in);

	get_num_out_t out;
	ret = HG_Get_output(handle, &out);
	assert(ret == HG_SUCCESS);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7

	if(out.ret == 0) {
		printf("%s's number is %s\n",name,out.phone);
	} else {
		printf("%s's number is not known\n",name);
	}

<<<<<<< HEAD
	// TODO: Free the "out" variable

	// TODO: Destroy the RPC handle	
=======
	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);
	
	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
}

static void set_num(char* line)
{
	char* name = strstr(line,"set")+3;
	while(!isalnum(*name)) name += 1;
	char* end = name;
	while(isalnum(*end)) end += 1;
	*end = '\0';
	char* phone = end+1;
	while(!isalnum(*phone)) phone += 1;
	end = phone;
	while(isalnum(*end)) end += 1;
	*end = '\0';

	hg_return_t ret;
  hg_handle_t handle;
<<<<<<< HEAD

	// TODO: Create the RPC handle for the set_num RPC

	// TODO: Declare and set the input argument

	// TODO: Forward the RPC using Margo

	// TODO: Get the RPC's output
  set_num_out_t out;
=======
  ret = HG_Create(hg_context, svr_addr, set_num_rpc_id, &handle);
  assert(ret == HG_SUCCESS);

  set_num_in_t in;
  in.name = name;
 	in.phone = phone; 

  margo_forward(mid, handle, &in);

  set_num_out_t out;
  ret = HG_Get_output(handle, &out);
  assert(ret == HG_SUCCESS);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7

	if(out.ret == 0) {	
		printf("%s's number has been set to %s\n",name,phone);
	} else {
		printf("an error occured\n");
	}
<<<<<<< HEAD

	// TODO: Free the RPC output  

	// TODO: Destroy the RPC handle
=======
  
  ret = HG_Free_output(handle, &out);
  assert(ret == HG_SUCCESS);

  ret = HG_Destroy(handle);
  assert(ret == HG_SUCCESS);
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7
}
