#ifndef PARAM_H
#define PARAM_H

#include <mercury.h>
#include <mercury_macros.h>
#include <mercury_proc_string.h>

<<<<<<< HEAD
/* TODO
  Use MERCURY_GEN_PROC to define the following structures
  and their serialization functions:
  - get_num_in_t with a "name" field (we will use hg_string_t)
  - get_num_out_t with a "phone" (string) and a "ret" (int32_t) field
  - set_num_in_t with "name" and "phone" (both strings)
  - set_num_out_t with a "ret" (int32_t)
 */
=======
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
>>>>>>> 2c6bfd9ce8c1bb89e4b4de45499859715aa6a2c7

#endif
