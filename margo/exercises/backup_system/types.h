#ifndef PARAM_H
#define PARAM_H

#include <mercury.h>
#include <mercury_bulk.h>
#include <mercury_proc_string.h>
#include <mercury_macros.h>

MERCURY_GEN_PROC(save_in_t,
		((hg_string_t)(filename))\
		((hg_size_t)(size))\
		((hg_bulk_t)(bulk_handle)))

MERCURY_GEN_PROC(save_out_t, ((int32_t)(ret)))

MERCURY_GEN_PROC(getsize_in_t,
		((hg_string_t)(filename)))

MERCURY_GEN_PROC(getsize_out_t,
		((int32_t)(ret))\
		((hg_size_t)(size)))

MERCURY_GEN_PROC(load_in_t,
		((hg_string_t)(filename))\
		((hg_size_t)(size))\
		((hg_bulk_t)(bulk_handle)))

MERCURY_GEN_PROC(load_out_t, ((int32_t)(ret)))

#endif
