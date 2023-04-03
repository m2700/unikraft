#pragma once

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/essentials.h>
#include <stdio.h>
#include <stdbool.h>
#include <x86/cpu.h>

#if CONFIG_LIBUKTIME_PROFILING

void print_profiling_results(bool print_avg, bool print_total);

extern char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_id_count;

extern __u64 uk_prf_tsc_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

#define __MEASURE_TSC(name) __u64 __uk_prf_tsc_##name = rdtsc_ordered()
#define __SAVE_TSC(name)                                                       \
	uk_prf_tsc_delays[__uk_prf_id_##name - 1] +=                           \
	    rdtsc_ordered() - __uk_prf_tsc_##name

#define UK_PRF_START(name)                                                     \
	UK_COMP_PUBLIC_SECTION(".", "bss")                                     \
	static __sz __uk_prf_id_##name = 0;                                    \
	if (__uk_prf_id_##name == 0) {                                         \
		__uk_prf_id_##name = ++uk_prf_id_count;                        \
	}                                                                      \
	uk_prf_counts[__uk_prf_id_##name - 1]++;                               \
	uk_prf_names[__uk_prf_id_##name - 1] = #name;                          \
	__MEASURE_TSC(name)

#define UK_PRF_END(name)                                                       \
	__SAVE_TSC(name);                                                      \

#else // CONFIG_LIBUKTIME_PROFILING

#include <uk/profiling-dummy.h>

#endif // CONFIG_LIBUKTIME_PROFILING