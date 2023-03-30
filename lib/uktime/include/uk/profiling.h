#pragma once

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/essentials.h>

#if CONFIG_LIBUKTIME_PROFILING

void print_profiling_results();

extern char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __nsec uk_prf_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_id_count;

#define UK_PRF_START(name)                                                     \
	UK_COMP_PUBLIC_SECTION(".", "bss")                                     \
	static __sz __uk_prf_id_##name = 0;                                    \
	__uk_prf_id_##name = __uk_prf_id_##name || ++uk_prf_id_count;          \
	uk_prf_counts[__uk_prf_id_##name - 1]++;                               \
	uk_prf_names[__uk_prf_id_##name - 1] = #name;                          \
	__nsec __uk_prf_ts_##name = ukplat_monotonic_clock()

#define UK_PRF_END(name)                                                       \
	uk_prf_delays[__uk_prf_id_##name - 1] +=                               \
	    ukplat_monotonic_clock() - __uk_prf_ts_##name

#else // CONFIG_LIBUKTIME_PROFILING

#include <uk/profiling-dummy.h>

#endif // CONFIG_LIBUKTIME_PROFILING