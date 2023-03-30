#include <uk/profiling.h>

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/print.h>
#include <stdio.h>
#include <uk/dyn-trampoline.h>

#if CONFIG_LIBUKTIME_PROFILING_TSC
#include <x86/cpu.h>
#endif

UK_COMP_PUBLIC_SECTION(".", "bss")
char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", "bss")
__sz uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

#if CONFIG_LIBUKTIME_PROFILING_TSC
UK_COMP_PUBLIC_SECTION(".", "bss")
__u64 uk_prf_tsc_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
#endif

#if CONFIG_LIBUKTIME_PROFILING_MONOTONIC
UK_COMP_PUBLIC_SECTION(".", "bss")
__nsec uk_prf_ns_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];
#endif

UK_COMP_PUBLIC_SECTION(".", "bss")
__sz uk_prf_id_count = 0;

UK_COMP_PUBLIC_SECTION(".", "text")
void print_profiling_results()
{
	DYN_TRAMPOLINE_INIT;
	for (__sz i = 0; i < uk_prf_id_count; i++) {
		printf("Profiling: %s: %lu times", uk_prf_names[i],
		       uk_prf_counts[i]);
#if CONFIG_LIBUKTIME_PROFILING_TSC
		printf(", %lu tsc", uk_prf_tsc_delays[i]);
#endif
#if CONFIG_LIBUKTIME_PROFILING_MONOTONIC
		printf(", %lu ns", uk_prf_ns_delays[i]);
#endif
		printf("\n");
	}
	DYN_TRAMPOLINE_FINI;
}