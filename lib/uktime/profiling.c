#include <uk/profiling.h>

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/print.h>
#include <stdio.h>
#include <uk/dyn-trampoline.h>
#include <stdbool.h>
#include <x86/cpu.h>

UK_COMP_PUBLIC_SECTION(".", "bss")
char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", "bss")
__sz uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", "bss")
__u64 uk_prf_tsc_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", "bss")
__sz uk_prf_id_count = 0;

UK_COMP_PUBLIC_SECTION(".", "text")
void print_profiling_results(bool print_avg, bool print_total)
{
	DYN_TRAMPOLINE_INIT;
	for (__sz i = 0; i < uk_prf_id_count; i++) {
		printf("Profiling: %s: %lu times", uk_prf_names[i],
		       uk_prf_counts[i]);
		if (print_total) {
			printf(", %lu tsc", uk_prf_tsc_delays[i]);
			printf(", %lu ns", tsc_to_ns(uk_prf_tsc_delays[i]));
		}
		if (print_avg) {
			printf(", %lu tsc/it",
			       uk_prf_tsc_delays[i] / uk_prf_counts[i]);
			printf(", %lu ns/it", tsc_to_ns(uk_prf_tsc_delays[i])
						  / uk_prf_counts[i]);
		}
		printf("\n");
	}
	DYN_TRAMPOLINE_FINI;
}