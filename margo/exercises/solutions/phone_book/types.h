#ifndef PARAM_H
#define PARAM_H

#include <mercury.h>
#include <mercury_macros.h>
#include <mercury_proc_string.h>

/* We use the Mercury macros to define the input
 * and output structures along with the serialization
 * functions.
 */
MERCURY_GEN_PROC(get_num_in_t,
	((hg_string_t)(name)))

MERCURY_GEN_PROC(get_num_out_t,
	((hg_string_t)(phone))\
	((int32_t)(ret)))

MERCURY_GEN_PROC(set_num_in_t,
	((hg_string_t)(name))\
	((hg_string_t)(phone)))

MERCURY_GEN_PROC(set_num_out_t,
	((int32_t)(ret)))

#endif
