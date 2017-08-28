#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

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
	/* Start Margo */
	mid = margo_init("bmi+tcp", MARGO_CLIENT_MODE, 0, 0);

	/* Register RPC functions */
	get_num_rpc_id = MARGO_REGISTER(mid, "get_num", get_num_in_t, get_num_out_t, NULL);
	set_num_rpc_id = MARGO_REGISTER(mid, "set_num", set_num_in_t, set_num_out_t, NULL);

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

	margo_addr_free(mid, svr_addr);
	/* shut down Margo */
    margo_finalize(mid);

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
	ret = margo_create(mid, svr_addr, get_num_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	get_num_in_t in;
	in.name = name;

	margo_forward(mid, handle, &in);

	get_num_out_t out;
	ret = margo_get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	if(out.ret == 0) {
		printf("%s's number is %s\n",name,out.phone);
	} else {
		printf("%s's number is not known\n",name);
	}

	ret = margo_free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_destroy(handle);
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
	ret = margo_create(mid, svr_addr, set_num_rpc_id, &handle);
	assert(ret == HG_SUCCESS);

	set_num_in_t in;
	in.name = name;
	in.phone = phone; 

	margo_forward(mid, handle, &in);

	set_num_out_t out;
	ret = margo_get_output(handle, &out);
	assert(ret == HG_SUCCESS);

	if(out.ret == 0) {	
		printf("%s's number has been set to %s\n",name,phone);
	} else {
		printf("an error occured\n");
	}

	ret = margo_free_output(handle, &out);
	assert(ret == HG_SUCCESS);

	ret = margo_destroy(handle);
	assert(ret == HG_SUCCESS);
}
