#include <stdio.h>
#include <unistd.h>

#include <string.h>

#include <margo.h>
#include <mercury.h>

#include "types.h"

#define MAX_LINE 255
#define MAX_PREFIX 64

int main(int argc, char **argv)
{
    FILE *words;
    char line[255];
    char prefix[MAX_PREFIX] = "";
    char *addr;
    margo_instance_id mid;
    hg_addr_t svr_addr;
    hg_id_t hash_rpc_id;
    int count = 0;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server address> <word list>\n", argv[0]);
        exit(0);
    }

    /* pick off the transport identifier -- the part before the : .  Many
     * examples hard-code a transport, so here's a way to do it more flexibliy
     */
    addr = argv[1];
    for (int i = 0; i<MAX_PREFIX && addr[i] != '\0' && addr[i] != ':'; i++)
        prefix[i] = addr[i];

    /* start margo */
    mid = margo_init(prefix, MARGO_CLIENT_MODE, 0, 0);

    /* Register an RPC function */
    hash_rpc_id  = MARGO_REGISTER(mid, "hash", hash_in_t, hash_out_t, NULL);

    /* Lookup address of the server */
    margo_addr_lookup(mid, addr, &svr_addr);

    /* work begins here:
     * - extract words from a list
     * - get back a hash
     * - because each word is variable length we have to process the buffer by
     *   hand */
    words = fopen(argv[2], "r");
    hash_in_t args;
    hash_out_t resp;

    while (fgets(line, MAX_LINE, words) && count++ < 16 )
    {
        hg_handle_t h;
        args.word = line;
        args.size = strlen(line);
        /* chop off the newline */
        if (args.word[args.size-1] == '\n')
            args.word[args.size-1] = '\0';
        margo_create(mid, svr_addr, hash_rpc_id, &h);
        margo_forward(h, &args);

        margo_get_output(h, &resp);

        printf("Word %20s hashed to %016lx\n", line, resp.hash);
        margo_free_output(h, &resp);
        margo_destroy(h);
    }


    /* free resources */
    margo_addr_free(mid, svr_addr);

    /* shutdown */
    margo_finalize(mid);

}
