#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <abt.h>
#include <abt-snoozer.h>
#include <margo.h>
#include "types.h"

static margo_instance_id mid		= MARGO_INSTANCE_NULL;
static hg_id_t    get_num_rpc_id;		/* ID of the RPC */
static hg_id_t    set_num_rpc_id;
static hg_addr_t  svr_addr;

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

	// TODO: Initialize Margo (get mid). We will use 0 progress thread and 0 RPC thread

	// TODO: Call MARGO_REGISTER to initialize get_num_rpc_id and set_num_rpc_id

	// TODO: Do an address lookup to initialize svr_addr, using Margo

	// Here we read lines entered by the user. These lines can start with
	// set, get, or stop, followed by arguments
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

	// TODO: Shut down Margo 
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

	// TODO: Get the output of the RPC

	// TODO: Create the handle for the get_num rpc

	// TODO: Declare and set the get_num_in_t argument for the RPC

	// TODO: Forward the RPC handle using Margo

	// TODO: Declare get_num_out_t out; and get the output of the RPC

	// TODO uncomment the following
	/*
	if(out.ret == 0) {
		printf("%s's number is %s\n",name,out.phone);
	} else {
		printf("%s's number is not known\n",name);
	}
	*/

	// TODO: Free the "out" variable

	// TODO: Destroy the RPC handle	
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

	// TODO: Create the RPC handle for the set_num RPC

	// TODO: Declare and set the input argument

	// TODO: Forward the RPC using Margo

	// TODO: Get the RPC's output
	// set_num_out_t out;

	// TODO: uncomment this
	/*
	if(out.ret == 0) {	
		printf("%s's number has been set to %s\n",name,phone);
	} else {
		printf("an error occured\n");
	}
	*/

	// TODO: Free the RPC output  

	// TODO: Destroy the RPC handle
}
