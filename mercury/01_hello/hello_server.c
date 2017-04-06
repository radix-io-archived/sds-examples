#include <assert.h>
#include <stdio.h>
#include <mercury.h>

static hg_class_t*     hg_class 	= NULL;
static hg_context_t*   hg_context 	= NULL;

static const int TOTAL_RPCS = 10;
static int num_rpcs = 0;

hg_return_t hello_world(hg_handle_t h);

int main(int argc, char** argv)
{
	hg_return_t ret;

	hg_class = HG_Init("bmi+tcp://localhost:1234", NA_TRUE);
    assert(hg_class != NULL);

    hg_context = HG_Context_create(hg_class);
    assert(hg_context != NULL);

	hg_id_t rpc_id = HG_Register_name(hg_class, "hello", NULL, NULL, hello_world);
	HG_Registered_disable_response(hg_class, rpc_id, HG_TRUE);
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

hg_return_t hello_world(hg_handle_t h)
{
	printf("Hello World!\n");
	num_rpcs += 1;
	return HG_SUCCESS;
}
