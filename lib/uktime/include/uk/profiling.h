#pragma once

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/essentials.h>
#include <stdio.h>

#if CONFIG_LIBUKTIME_PROFILING_TSC
#include <x86/cpu.h>
#endif

#if CONFIG_LIBUKTIME_PROFILING

void print_profiling_results(bool print_avg, bool print_total);

extern char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
extern __sz uk_prf_id_count;

#if CONFIG_LIBUKTIME_PROFILING_TSC
extern __u64 uk_prf_tsc_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

#define __MEASURE_TSC(name) __u64 __uk_prf_tsc_##name = rdtsc()
#define __SAVE_TSC(name)                                                       \
	uk_prf_tsc_delays[__uk_prf_id_##name - 1] +=                           \
	    rdtsc() - __uk_prf_tsc_##name
#else // CONFIG_LIBUKTIME_PROFILING_TSC
#define __MEASURE_TSC(name)
#define __SAVE_TSC(name)
#endif // CONFIG_LIBUKTIME_PROFILING_TSC

#if CONFIG_LIBUKTIME_PROFILING_MONOTONIC
extern __nsec uk_prf_ns_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

#define __MEASURE_NS(name) __nsec __uk_prf_ns_##name = ukplat_monotonic_clock()
#define __SAVE_NS(name)                                                        \
	uk_prf_ns_delays[__uk_prf_id_##name - 1] +=                            \
	    ukplat_monotonic_clock() - __uk_prf_ns_##name
#else // CONFIG_LIBUKTIME_PROFILING_MONOTONIC
#define __MEASURE_NS(name)
#define __SAVE_NS(name)
#endif // CONFIG_LIBUKTIME_PROFILING_MONOTONIC

#define UK_PRF_START(name)                                                     \
	UK_COMP_PUBLIC_SECTION(".", "bss")                                     \
	static __sz __uk_prf_id_##name = 0;                                    \
	if (__uk_prf_id_##name == 0) {                                         \
		__uk_prf_id_##name = ++uk_prf_id_count;                        \
	}                                                                      \
	uk_prf_counts[__uk_prf_id_##name - 1]++;                               \
	uk_prf_names[__uk_prf_id_##name - 1] = #name;                          \
	__MEASURE_NS(name);                                                    \
	__MEASURE_TSC(name)

#define UK_PRF_END(name)                                                       \
	__SAVE_TSC(name);                                                      \
	__SAVE_NS(name)

#else // CONFIG_LIBUKTIME_PROFILING

#include <uk/profiling-dummy.h>

#endif // CONFIG_LIBUKTIME_PROFILING