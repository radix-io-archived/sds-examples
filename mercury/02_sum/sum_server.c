#include <assert.h>
#include <stdio.h>
#include <mercury.h>
#include "types.h"

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;

static const int TOTAL_RPCS = 10;
static int num_rpcs = 0;

hg_return_t sum(hg_handle_t h);

int main(int argc, char** argv)
{
	hg_return_t ret;

	hg_class = HG_Init("bmi+tcp://localhost:1234", NA_TRUE);
    assert(hg_class != NULL);

    hg_context = HG_Context_create(hg_class);
    assert(hg_context != NULL);

	hg_id_t rpc_id = MERCURY_REGISTER(hg_class, "sum", sum_in_t, sum_out_t, sum);
	do
	{
		unsigned int count;
		do {
			ret = HG_Trigger(hg_context, 0, 1, &count);
		} while((ret == HG_SUCCESS) && count);

		HG_Progress(hg_context, 100);
	} while(num_rpcs < TOTAL_RPCS);

	ret = HG_Context_destroy(hg_context);
	assert(ret == HG_SUCCESS);

	ret = HG_Hl_finalize();
	assert(ret == HG_SUCCESS);

	return 0;
}

hg_return_t sum(hg_handle_t handle)
{
	hg_return_t ret;
	sum_in_t in;
	sum_out_t out;
	
	ret = HG_Get_input(handle, &in);
	assert(ret == HG_SUCCESS);

	out.ret = in.x + in.y;
	printf("%d + %d = %d\n",in.x,in.y,in.x+in.y);
	num_rpcs += 1;

	ret = HG_Respond(handle,NULL,NULL,&out);
	assert(ret == HG_SUCCESS);

	ret = HG_Free_input(handle, &in);
	assert(ret == HG_SUCCESS);
	ret = HG_Destroy(handle);
	assert(ret == HG_SUCCESS);

	return HG_SUCCESS;
}
