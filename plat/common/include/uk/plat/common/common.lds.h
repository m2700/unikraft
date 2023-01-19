/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2019, NEC Laboratories Europe GmbH, NEC Corporation,
 *                     University Politehnica of Bucharest.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __UK_COMMON_LDS_H
#define __UK_COMMON_LDS_H

#include <uk/arch/limits.h> /* for __PAGE_SIZE */

#ifdef CONFIG_COMPONENT_SPLITTING
#include <uk/kvm/components.lds.h>
#define COMPONENT_LOCAL_DATA_SECTION \
	__clocal_data_start = ALIGN(__PAGE_SIZE); \
	. = ALIGN(__PAGE_SIZE); \
	.clocal.data : {*(.clocal.data)} \
	. = __clocal_data_start + (ALIGN(__PAGE_SIZE) - __clocal_data_start) * UK_COMPONENT_COUNT; \
	__clocal_data_end = .;
#define COMPONENT_LOCAL_BSS_SECTIONS \
	__clocal_bss_start = ALIGN(__PAGE_SIZE); \
	. = ALIGN(__PAGE_SIZE); \
	.clocal.bss : {*(.clocal.bss)} \
	. = __clocal_bss_start + (ALIGN(__PAGE_SIZE) - __clocal_bss_start) * UK_COMPONENT_COUNT; \
	__clocal_bss_end = .;
#else // CONFIG_COMPONENT_SPLITTING
#define COMPONENT_TEXT_SECTIONS
#define COMPONENT_RODATA_SECTIONS
#define COMPONENT_TDATA_SECTIONS
#define COMPONENT_TBSS_SECTIONS
#define COMPONENT_DATA_SECTIONS
#define COMPONENT_BSS_SECTIONS
#define COMPONENT_LOCAL_DATA_SECTION
#define COMPONENT_LOCAL_BSS_SECTIONS
#endif // CONFIG_COMPONENT_SPLITTING

/* DWARF debug sections.  Symbols in the DWARF debugging sections are
 * relative to the beginning of the section so we begin them at 0.
 */
#define DEBUG_SYMBOLS							\
	/* DWARF 1 */							\
	.debug          0 : { *(.debug) }				\
	.line           0 : { *(.line) }				\
	/* GNU DWARF 1 extensions */					\
	.debug_srcinfo  0 : { *(.debug_srcinfo) }			\
	.debug_sfnames  0 : { *(.debug_sfnames) }			\
	/* DWARF 1.1 and DWARF 2 */					\
	.debug_aranges  0 : { *(.debug_aranges) }			\
	.debug_pubnames 0 : { *(.debug_pubnames) }			\
	/* DWARF 2 */							\
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }	\
	.debug_abbrev   0 : { *(.debug_abbrev) }			\
	.debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) } \
	.debug_frame    0 : { *(.debug_frame) }				\
	.debug_str      0 : { *(.debug_str) }				\
	.debug_loc      0 : { *(.debug_loc) }				\
	.debug_macinfo  0 : { *(.debug_macinfo) }			\
	/* SGI/MIPS DWARF 2 extensions */				\
	.debug_weaknames 0 : { *(.debug_weaknames) }			\
	.debug_funcnames 0 : { *(.debug_funcnames) }			\
	.debug_typenames 0 : { *(.debug_typenames) }			\
	.debug_varnames  0 : { *(.debug_varnames) }			\
	/* DWARF 3 */							\
	.debug_pubtypes 0 : { *(.debug_pubtypes) }			\
	.debug_ranges   0 : { *(.debug_ranges) }			\
	/* DWARF Extension.  */						\
	.debug_macro    0 : { *(.debug_macro) }				\
	.gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }

#define EXCEPTION_SECTIONS						\
	. = ALIGN(__PAGE_SIZE);						\
	__eh_frame_start = .;						\
	.eh_frame :							\
	{								\
		*(.eh_frame)						\
		*(.eh_frame.*)						\
	}								\
	__eh_frame_end = .;						\
									\
	__eh_frame_hdr_start = .;					\
	.eh_frame_hdr :							\
	{								\
		*(.eh_frame_hdr)					\
		*(.eh_frame_hdr.*)					\
	}								\
	__eh_frame_hdr_end = .;

#define CTORTAB_SECTION							\
	. = ALIGN(__PAGE_SIZE);						\
	uk_ctortab_start = .;						\
	.uk_ctortab :							\
	{								\
		KEEP(*(SORT_BY_NAME(.uk_ctortab[0-9])))			\
	}								\
	uk_ctortab_end = .;

#define INITTAB_SECTION							\
	uk_inittab_start = .;						\
	.uk_inittab :							\
	{								\
		KEEP(*(SORT_BY_NAME(.uk_inittab[1-6][0-9])))		\
	}								\
	uk_inittab_end = .;

#define EVENTTAB_SECTION						\
	. = ALIGN(0x8);							\
	.uk_eventtab :							\
	{								\
		KEEP(*(SORT_BY_NAME(.uk_event_*)))			\
	}

#define TLS_SECTIONS							\
	. = ALIGN(TLS_ALIGN);						\
	_tls_start = .;							\
	.tdata :							\
	{								\
		*(.tdata)						\
		*(.tdata.*)						\
		*(.gnu.linkonce.td.*)					\
	}								\
	COMPONENT_TDATA_SECTIONS		\
	_etdata = .;							\
	.tbss :								\
	{								\
		*(.tbss)						\
		*(.tbss.*)						\
		*(.gnu.linkonce.tb.*)					\
		*(.tcommon)						\
	}								\
	. = . + SIZEOF(.tbss);			\
	COMPONENT_TBSS_SECTIONS			\
	_tls_end = .;					\
	/*								\
	 * NOTE: Because the .tbss section is zero-sized in the final	\
	 *       ELF image, we need to reset . to it's original state	\
	 *       after incrementing		\
	 */								\
	. = ADDR(.tbss);

#define DATA_SECTIONS							\
	/* Read-write data (initialized) */				\
	. = ALIGN(__PAGE_SIZE);						\
	_data = .;							\
	.data :								\
	{								\
		*(.data)						\
		*(.data.*)						\
	}								\
	COMPONENT_DATA_SECTIONS				\
	COMPONENT_LOCAL_DATA_SECTION		\
	_edata = .;							\
									\
	/*								\
	 * NOTE: linker will insert any extra sections here,		\
	 * just before .bss						\
	 */								\
									\
	/* Read-write data (uninitialized) */				\
	. = ALIGN(__PAGE_SIZE);						\
	__bss_start = .;						\
	.bss :								\
	{								\
		*(.bss)							\
		*(.bss.*)						\
		*(COMMON)						\
	}										\
	. = ALIGN(__PAGE_SIZE);			\
	COMPONENT_BSS_SECTIONS			\
	COMPONENT_LOCAL_BSS_SECTIONS

#endif /* __UK_COMMON_LDS_H */
