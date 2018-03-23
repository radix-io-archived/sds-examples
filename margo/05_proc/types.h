#include <mercury.h>
#include <mercury_macros.h>

typedef struct {
    char * word;
    size_t size;
} hash_in_t;

/* the whole point of this example. We have a word of varying length and we need to pack this data into the buffer to send to the target.
 * 'proc' is the state of the mercury type processing engine
 * 'data' is the raw buffer we are processing */
static inline hg_return_t hg_proc_hash_in_t(hg_proc_t proc, void *data)
{
    hg_return_t ret;
    /* standard practice when dealing with 'void *' data: immediately get it
     * into a real type */
    hash_in_t *in = (hash_in_t*)data;

    ret = hg_proc_hg_size_t(proc, &in->size);
    if (in->size) {
        switch(hg_proc_get_op(proc)) {
            /* ENCODE: taking a user-provided buffer and converting it to wire
             * format */
            case HG_ENCODE:
                /* The prototype is
                 * hg_proc_raw(hg_proc_t proc, void *data, hg_size_t data_size)
                 * and moves 'data_size' bytes from the processing state into
                 * 'data' */
                ret = hg_proc_raw(proc, in->word, in->size);
                break;
            /* DECODE: the wire-formatted buffer is unpacked into a user's
             * struct  */
            case HG_DECODE:
                /* we know 'size' because it was the first thing we operated
                 * on before this switch statement */
                in->word = malloc(in->size);
                /* with space allocated, move 'in->size' bytes into it */
                ret = hg_proc_raw(proc, in->word, in->size);
                break;
            /* FREE: anything we allocated gets cleaned up here */
            case HG_FREE:
                free(in->word);
            default:
                break;
        }
    }
    return ret;
}

MERCURY_GEN_PROC(hash_out_t,
        ((int32_t)(ret))
        ((uint64_t)(hash)) )
