#!/usr/bin/env python3

import argparse
from typing import Iterable


def iter_component_tuples(comp_num: int, min_comp: int = 0, max_tuple_size: int = 0) -> Iterable[tuple[int]]:
    if max_tuple_size == 0:
        max_tuple_size = comp_num
    for comp_i in range(min_comp, comp_num):
        yield (comp_i,)
    if max_tuple_size > 1:
        for comp_i in range(min_comp, comp_num):
            for sub_comp_tuple in iter_component_tuples(comp_num, comp_i + 1, max_tuple_size - 1):
                assert (len(sub_comp_tuple) <= max_tuple_size - 1)
                yield (comp_i,) + sub_comp_tuple


def component_section_script(target_section: str, src_sections, comp_num: int | None = None,
                             comp_map: Iterable[tuple[str, int]] | None = None,
                             page_aligned: bool = False, extent_syms: bool = False,
                             component_src: bool = False) -> str:
    s = ""
    if comp_num is not None:
        assert comp_map is None
        comp_map = (("*", comp_share)
                    for comp_share in iter_component_tuples(comp_num))

    for lib_pat, comp_share in comp_map:
        comp_share_name = f"component{'_'.join(map(str, comp_share))}"
        comp_share_prefix = f".{comp_share_name}." if component_src else ""

        src_section_pats = "\n    ".join(
            f"{lib_pat}({comp_share_prefix}{src_sec})" for src_sec in src_sections)

        start_sym = end_sym = ""
        if extent_syms:
            start_sym = f"__{comp_share_name}_{target_section}"
            end_sym = f"__{comp_share_name}_{target_section}_end"

        sec_inner = "\n    ".join(
            filter(None, [start_sym, src_section_pats, end_sym]))

        s += f"""\
.{comp_share_name}.{target_section} : {{
    {sec_inner}
}}
"""
        if page_aligned:
            s += ". = ALIGN(__PAGE_SIZE);\n"

    return s.strip()


def generic_linker_defs(opt: argparse.Namespace):
    text_comp_sects = component_section_script(
        'text', ['text', 'text.*'], opt.comp_num,
        page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')
    rodata_comp_sects = component_section_script(
        'rodata', ['rodata', 'rodata.*'], opt.comp_num,
        page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')
    tdata_comp_sects = component_section_script(
        'tdata', ['tdata', 'tdata.*', 'gnu.linkonce.td.*'],
        opt.comp_num, page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')
    tbss_comp_sects = component_section_script(
        'tbss', ['tbss', 'tbss.*', 'tcommon', 'gnu.linkonce.tb.*'],
        opt.comp_num, page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')
    data_comp_sects = component_section_script(
        'data', ['data', 'data.*'], opt.comp_num,
        page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')
    bss_comp_sects = component_section_script(
        'bss', ['bss', 'bss.*', 'COMMON'], opt.comp_num,
        page_aligned=True, extent_syms=True, component_src=True
    ).replace('\n', '\t\\\n    ')

    header_s = f"""\
#pragma once

#define COMPONENT_TEXT_SECTIONS \\
    {text_comp_sects}

#define COMPONENT_RODATA_SECTIONS \\
    {rodata_comp_sects}

#define COMPONENT_TDATA_SECTIONS \\
    {tdata_comp_sects}

#define COMPONENT_TBSS_SECTIONS \\
    {tbss_comp_sects}

#define COMPONENT_DATA_SECTIONS \\
    {data_comp_sects}

#define COMPONENT_BSS_SECTIONS \\
    {bss_comp_sects}
"""

    return header_s


def link_remap_sections(opt: argparse.Namespace):
    lib_comp_map = opt.lib_comp_mappings
    if opt.wildcard_location:
        lib_comp_map = (f"*/{lib_comp}" for lib_comp in lib_comp_map)
    lib_comp_map = [lib_comp.split("=")
                    for lib_comp in lib_comp_map]

    text_comp_sects = component_section_script(
        'text', ['.text', '.text.*'], comp_map=lib_comp_map
    ).replace('\n', '\n    ')
    rodata_comp_sects = component_section_script(
        'rodata', ['.rodata', '.rodata.*'], comp_map=lib_comp_map
    ).replace('\n', '\n    ')
    tdata_comp_sects = component_section_script(
        'tdata', ['.tdata', '.tdata.*', '.gnu.linkonce.td.*'],
        comp_map=lib_comp_map
    ).replace('\n', '\n    ')
    tbss_comp_sects = component_section_script(
        'tbss', ['.tbss', '.tbss.*', '.tcommon', '.gnu.linkonce.tb.*'],
        comp_map=lib_comp_map
    ).replace('\n', '\n    ')
    data_comp_sects = component_section_script(
        'data', ['.data', '.data.*'], comp_map=lib_comp_map
    ).replace('\n', '\n    ')
    bss_comp_sects = component_section_script(
        'bss', ['.bss', '.bss.*', 'COMMON'], comp_map=lib_comp_map
    ).replace('\n', '\n    ')

    link_s = f"""\
SECTIONS
{{
    {text_comp_sects}

    {rodata_comp_sects}

    {tdata_comp_sects}

    {tbss_comp_sects}

    {data_comp_sects}

    {bss_comp_sects}
}}
"""

    return link_s


def main():
    parser = argparse.ArgumentParser(
        description="Generate linker script for library components")
    subparsers = parser.add_subparsers(dest="cmd", required=True)
    defs_parser = subparsers.add_parser(
        "defs", help="Generate linker script defines")
    remap_parser = subparsers.add_parser(
        "remap", help="Generate a library to component, section remapping, linker script")
    defs_parser.add_argument("comp_num", help="Number of components",
                             metavar='component-count', type=int)
    remap_parser.add_argument('-w', '--wildcard-location',
                              action='store_true')
    remap_parser.add_argument(
        'lib_comp_mappings', metavar='library.o=component-id', nargs='*', type=str,
        help='Library Component mappings'
    )
    opt = parser.parse_args()

    if opt.cmd == 'defs':
        print(generic_linker_defs(opt))
    else:
        print(link_remap_sections(opt))


if __name__ == '__main__':
    main()
