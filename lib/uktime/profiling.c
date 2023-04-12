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

static void print_prf_line(__sz count, __u64 tsc_delay, bool print_avg,
			   bool print_total)
{
	printf("%lu times", count);
	if (print_total) {
		printf(", %lu tsc", tsc_delay);
		printf(", %lu ns", tsc_to_ns(tsc_delay));
	}
	if (print_avg) {
		printf(", %lu tsc/it", tsc_delay / count);
		printf(", %lu ns/it", tsc_to_ns(tsc_delay) / count);
	}
	printf("\n");
}
static void print_prf_line_named(char const *name, __sz count, __u64 tsc_delay,
				 bool print_avg, bool print_total)
{
	printf("Profiling: %s: ", name);
	print_prf_line(count, tsc_delay, print_avg, print_total);
}

UK_COMP_PUBLIC_SECTION(".", "text")
void print_profiling_results(bool print_avg, bool print_total)
{
	DYN_TRAMPOLINE_INIT;
	for (__sz i = 0; i < uk_prf_id_count; i++) {
		if (uk_prf_counts[i] == 0)
			continue;
		print_prf_line_named(uk_prf_names[i], uk_prf_counts[i],
				     uk_prf_tsc_delays[i], print_avg,
				     print_total);
	}
	DYN_TRAMPOLINE_FINI;
}

#define nl_strcmp(s1, s2)                                                      \
	(s1 == NULL ? (s2 == NULL ? 0 : -s2[0])                                \
		    : (s2 == NULL ? s1[0] : strcmp(s1, s2)))

UK_COMP_PUBLIC_SECTION(".", "text")
void print_profiling_results_grouped(bool print_avg, bool print_total)
{
	DYN_TRAMPOLINE_INIT;
	bool needs_newline = false;
	for (__sz ig = 0; ig < uk_prf_id_count; ig++) {
		char const *group_name = uk_prf_names[ig];

		for (__sz i = 0; i < ig; i++) {
			if (nl_strcmp(group_name, uk_prf_names[i]) == 0) {
				goto end_group_loop;
			}
		}

		__sz group_count = uk_prf_counts[ig];
		__u64 group_tsc = uk_prf_tsc_delays[ig];
		bool has_multiple = false;

		for (__sz i = ig + 1; i < uk_prf_id_count; i++) {
			if (nl_strcmp(group_name, uk_prf_names[i]) == 0) {
				group_count += uk_prf_counts[i];
				group_tsc += uk_prf_tsc_delays[i];
				has_multiple = true;
			}
		}
		if (group_count == 0) {
			continue;
		}

		if (ig != 0 && (needs_newline || has_multiple)) {
			needs_newline = false;
			printf("\n");
		}
		print_prf_line_named(group_name, group_count, group_tsc,
				     print_avg, print_total);

		if (has_multiple) {
			for (__sz i = ig; i < uk_prf_id_count; i++) {
				if (nl_strcmp(group_name, uk_prf_names[i]) == 0
				    && uk_prf_counts[i] != 0) {
					printf("    ");
					print_prf_line(uk_prf_counts[i],
						       uk_prf_tsc_delays[i],
						       print_avg, print_total);
				}
			}
			needs_newline = true;
		}

	end_group_loop:;
	}
	DYN_TRAMPOLINE_FINI;
}

UK_COMP_PUBLIC_SECTION(".", "text")
void uk_prf_clear(void)
{
	DYN_TRAMPOLINE_INIT;
	for (__sz i = 0; i < uk_prf_id_count; i++) {
		uk_prf_counts[i] = 0;
		uk_prf_tsc_delays[i] = 0;
	}
	DYN_TRAMPOLINE_FINI;
}