#include <uk/profiling.h>

#include <uk/config.h>
#include <uk/plat/time.h>
#include <uk/component.h>
#include <uk/print.h>

UK_COMP_PUBLIC_SECTION(".", ".bss")
char const *uk_prf_names[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", ".bss")
__nsec uk_prf_delays[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", ".bss")
size_t uk_prf_counts[CONFIG_LIBUKTIME_PROFILING_ARRAY_SIZE];

UK_COMP_PUBLIC_SECTION(".", ".bss")
size_t uk_prf_id_count = 0;

UK_COMP_PUBLIC_SECTION(".", ".bss")
void print_profiling_results() {
	for (size_t i = 0; i < uk_prf_id_count; i++) {
		uk_pr_info("Profiling: %s: %llu ns, %llu times\n",
			   uk_prf_names[i], uk_prf_delays[i], uk_prf_counts[i]);
	}
}