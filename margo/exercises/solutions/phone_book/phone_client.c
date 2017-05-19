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
static hg_id_t         get_num_rpc_id;		/* ID of the RPC */
static hg_id_t         set_num_rpc_id;
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

	/* 
	 * Initialize an hg_class.
	 * Here we only specify the protocal since this is a client
	 * (no need for an address and a port). HG_FALSE indicates that
	 * the client will not listen for incoming requests.
	 */
	hg_class = HG_Init("bmi+tcp", HG_FALSE);
	assert(hg_class != NULL);

	/* Creates a context for the hg_class. */
	hg_context = HG_Context_create(hg_class);
	assert(hg_context != NULL);

	/* Initialie Argobots */
	ABT_init(argc, argv);

	/* set primary ES to idle without polling */
	ABT_snoozer_xstream_self_set();

	/* Start Margo */
	mid = margo_init(0, 0, hg_context);

	/* Register RPC functions */
	get_num_rpc_id = MERCURY_REGISTER(hg_class, "get_num", get_num_in_t, get_num_out_t, NULL);
	set_num_rpc_id = MERCURY_REGISTER(hg_class, "set_num", set_num_in_t, set_num_out_t, NULL);

	margo_addr_lookup(mid, "bmi+tcp://localhost:1234", &svr_addr);

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
	ret = HG_Create(hg_context, svr_addr, get_num_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	get_num_in_t in;
	in.name = name;

	margo_forward(mid, handle, &in);

	get_num_out_t out;
	ret = HG_Get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	if(out.ret == 0) {
		printf("%s's number is %s\n",name,out.phone);
	} else {
		printf("%s's number is not known\n",name);
	}

	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);
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
	ret = HG_Create(hg_context, svr_addr, set_num_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	set_num_in_t in;
	in.name = name;
	in.phone = phone; 

	margo_forward(mid, handle, &in);

	set_num_out_t out;
	ret = HG_Get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	if(out.ret == 0) {	
		printf("%s's number has been set to %s\n",name,phone);
	} else {
		printf("an error occured\n");
	}

	ret = HG_Free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);
}
