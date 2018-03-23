#include <assert.h>
#include <mercury.h>
#include <margo.h>
#include <stdio.h>
#include "types.h"

/* after serving this number of rpcs, the server will shut down. */
static const int TOTAL_RPCS = 16;
/* number of RPCS already received. */
static int num_rpcs = 0;

/* All Mercury RPCs must have a signature
 *   hg_return_t f(hg_handle_t h)
 */

hg_return_t hash(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(hash)


int main(int argc, char **argv)
{
    hg_addr_t my_address;
    char addr_str[128];
    size_t addr_str_size = 128;

    /* initialize margo */
    margo_instance_id mid = margo_init(argv[1], MARGO_SERVER_MODE, 0, 0);
    assert(mid);

    /* when we initialize margo we only give it a transport (e.g."bmi+tcp").
     * We want to be able to report to clients the actual server address */
    margo_addr_self(mid, &my_address);
    margo_addr_to_string(mid, addr_str, &addr_str_size, my_address);
    margo_addr_free(mid, my_address);
    printf("Server running at address %s\n", addr_str);

    MARGO_REGISTER(mid, "hash", hash_in_t, hash_out_t, hash);

    /* that's all the server does:
     * - initialie margo
     * - register an RPC
     * the only thing left to do is listen for clients and shutdown when
     * requested */
    margo_wait_for_finalize(mid);

    return 0;
}

hg_return_t hash(hg_handle_t h)
{
    hg_return_t ret;
    hash_in_t in;
    hash_out_t out;

    num_rpcs += 1;

    margo_instance_id mid = margo_hg_handle_get_instance(h);

    /* deserialize input from the handle */
    ret = margo_get_input(h, &in);
    assert(ret == HG_SUCCESS);

    /* operate on input */
    /* the 'djb2' string hash */
    uint64_t hash = 5381;
    int c;
    char *p = in.word;
    while ( (c = *p++))
        hash = ((hash << 5) + hash) + c;

    out.hash = hash;
    out.ret = HG_SUCCESS;
    /* if you want to verify the remote end is working like you expect,
     * uncomment this line */
    //printf("hashing %s to %lx\n", in.word, out.hash);

    ret = margo_respond(h, &out);
    assert(ret == HG_SUCCESS);

    /* free input data */
    ret = margo_free_input(h, &in);
    assert (ret == HG_SUCCESS);

    ret = margo_destroy(h);
    assert (ret == HG_SUCCESS);

    /* for this demo we don't need the server to run forever. after a few
     * requests we'll shut down.  A full-fledged service would register a
     * finalize callback */
    if (num_rpcs == TOTAL_RPCS) {
        margo_finalize(mid);
    }

    return HG_SUCCESS;
}
DEFINE_MARGO_RPC_HANDLER(hash);





