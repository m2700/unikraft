#!/usr/bin/env python3

import argparse
from typing import Iterable, Sequence
import itertools


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


def start_sym(section: str) -> str:
    return f"__{section.lstrip('.').replace('.', '_')}_start"


def end_sym(section: str) -> str:
    return f"__{section.lstrip('.').replace('.', '_')}_end"


def share_to_section_name(comp_share: Iterable[int], section: str) -> str:
    return f".component{'_'.join(map(str, comp_share))}.{section}"


def section_inner_script(lib_pats:  Iterable[str],
                         target_section: str, src_sections: Iterable[str],
                         extent_syms: bool = False, keep_target: bool = False) -> str:
    sec_inner_lines = []

    if extent_syms:
        sec_inner_lines.append(f"{start_sym(target_section)} = .;")

    for lib_pat in lib_pats:
        sec_inner_lines.extend(
            f"{lib_pat}({src_sec})" for src_sec in src_sections)

    if extent_syms:
        sec_inner_lines.append(f"{end_sym(target_section)} = .;")

    if keep_target:
        sec_inner_lines.append(f". = .;")

    sec_inner = "\n    ".join(sec_inner_lines)

    return sec_inner


def component_section_script(target_section: str, src_sections: Iterable[str],
                             comp_num: int | None = None,
                             comp_map: Iterable[tuple[int | Sequence[int],
                                                      Iterable[str]]] | None = None,
                             page_aligned: bool = False,
                             inner_extent_syms: bool = False, outer_extent_syms: bool = False,
                             component_src: bool = False, keep_orig_target: bool = False,
                             collect_share_section: bool = False, tbss: bool = False) -> str:
    s = ""

    if comp_num is not None:
        assert comp_map is None
        comp_map = ((comp_share, {"*"})
                    for comp_share in iter_component_tuples(comp_num))
    if keep_orig_target:
        assert not component_src  # not supported
        comp_map = itertools.chain(comp_map, [(None, {"*"})])

    full_comp_share = None
    if collect_share_section:
        assert comp_num is not None and component_src
        full_comp_share = tuple(range(comp_num))
    
    if comp_map is None:
        comp_map = [(None, {"*"})]

    for comp_share, lib_pats in comp_map:
        comp_src_sections = src_sections
        keep_target = False

        if comp_share is None:
            assert not component_src  # not supported
            comp_target_section = f".{target_section}"
            keep_target = keep_orig_target
        else:
            if isinstance(comp_share, int):
                comp_share = (comp_share,)

            comp_target_section = share_to_section_name(
                comp_share, target_section)

            if component_src:
                comp_src_sections = {share_to_section_name(comp_share, src_sec)
                                     for src_sec in src_sections}
                if comp_share == full_comp_share:
                    comp_src_sections.update(f".shared.{src_sec}"
                                             for src_sec in src_sections)

        sec_inner = section_inner_script(lib_pats, comp_target_section, comp_src_sections,
                                         extent_syms=inner_extent_syms, keep_target=keep_target)

        if outer_extent_syms:
            if page_aligned:
                s += f"{start_sym(comp_target_section)} = ALIGN(__PAGE_SIZE);\n"
                s += ". = ALIGN(__PAGE_SIZE);\n"
            else:
                s += f"{start_sym(comp_target_section)} = .;\n"

        s += f"""\
{comp_target_section} : {{
    {sec_inner}
}}
"""

        if tbss:
            s += f". = ADDR({comp_target_section}) + SIZEOF({comp_target_section});\n"
        if page_aligned:
            s += ". = ALIGN(__PAGE_SIZE);\n"
        if outer_extent_syms:
            s += f"{end_sym(comp_target_section)} = .;\n"

    return s.strip()


def symlist_mapping_expr(comp_share: Iterable[int], comp_num: int) -> str:
    comp_share = tuple(comp_share)

    return f"""\
    {{
        .share = {{{', '.join(map(str, comp_share + (0,) * (comp_num - len(comp_share))))}}},
        .text_extents = {{
            {start_sym(share_to_section_name(comp_share, "text"))},
            {end_sym(share_to_section_name(comp_share, "text"))},
        }},
        .rodata_extents = {{
            {start_sym(share_to_section_name(comp_share, "rodata"))},
            {end_sym(share_to_section_name(comp_share, "rodata"))},
        }},
        .tdata_extents = {{
            {start_sym(share_to_section_name(comp_share, "tdata"))},
            {end_sym(share_to_section_name(comp_share, "tdata"))},
        }},
        .tbss_extents = {{
            {start_sym(share_to_section_name(comp_share, "tbss"))},
            {end_sym(share_to_section_name(comp_share, "tbss"))},
        }},
        .data_extents = {{
            {start_sym(share_to_section_name(comp_share, "data"))},
            {end_sym(share_to_section_name(comp_share, "data"))},
        }},
        .bss_extents = {{
            {start_sym(share_to_section_name(comp_share, "bss"))},
            {end_sym(share_to_section_name(comp_share, "bss"))},
        }},
    }}
""".strip()


def generic_linker_defs(opt: argparse.Namespace) -> str:
    text_comp_sects = component_section_script(
        'text', ['text', 'text.*'], opt.comp_num,
        page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section
    ).replace('\n', '\t\\\n    ')
    rodata_comp_sects = component_section_script(
        'rodata', ['rodata', 'rodata.*'], opt.comp_num,
        page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section
    ).replace('\n', '\t\\\n    ')
    tdata_comp_sects = component_section_script(
        'tdata', ['tdata', 'tdata.*', 'gnu.linkonce.td.*'],
        opt.comp_num, page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section
    ).replace('\n', '\t\\\n    ')
    tbss_comp_sects = component_section_script(
        'tbss', ['tbss', 'tbss.*', 'tcommon', 'gnu.linkonce.tb.*'],
        opt.comp_num, page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section, tbss=True
    ).replace('\n', '\t\\\n    ')
    data_comp_sects = component_section_script(
        'data', ['data', 'data.*'], opt.comp_num,
        page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section
    ).replace('\n', '\t\\\n    ')
    bss_comp_sects = component_section_script(
        'bss', ['bss', 'bss.*', 'COMMON'], opt.comp_num,
        page_aligned=True, outer_extent_syms=True, component_src=True,
        collect_share_section=opt.collect_share_section
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


def link_remap_sections(opt: argparse.Namespace) -> str:
    lib_comp_mappings = opt.lib_comp_mappings
    if opt.wildcard_location:
        assert (not opt.single_match_location)
        lib_comp_mappings = (f"*/{lib_comp}" for lib_comp in lib_comp_mappings)
    comp_lib_map = {}
    for lib_comp in lib_comp_mappings:
        lib, comp = lib_comp.split("=")
        if opt.single_match_location:
            assert (not opt.wildcard_location)
            lib = f"{lib[:-1]}[{lib[-1]}]"
        comp_lib_map.setdefault(int(comp), set()).add(lib)

    comp_lib_map = sorted(comp_lib_map.items(), key=lambda lcp: lcp[0])

    text_comp_sects = component_section_script(
        'text', ['.text', '.text.*'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')
    rodata_shared_sect = component_section_script(
        'shared.rodata', ['.rodata.str1.1']
    ).replace('\n', '\n    ')
    rodata_comp_sects = component_section_script(
        'rodata', ['.rodata', '.rodata.*'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')
    tdata_comp_sects = component_section_script(
        'tdata', ['.tdata', '.tdata.*', '.gnu.linkonce.td.*'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')
    tbss_comp_sects = component_section_script(
        'tbss', ['.tbss', '.tbss.*', '.tcommon', '.gnu.linkonce.tb.*'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')
    data_comp_sects = component_section_script(
        'data', ['.data', '.data.*'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')
    bss_comp_sects = component_section_script(
        'bss', ['.bss', '.bss.*', 'COMMON'],
        comp_map=comp_lib_map, keep_orig_target=opt.keep_remapped
    ).replace('\n', '\n    ')

    link_s = f"""\
SECTIONS
{{
    {text_comp_sects}

    {rodata_shared_sect}
    {rodata_comp_sects}

    {tdata_comp_sects}

    {tbss_comp_sects}

    {data_comp_sects}

    {bss_comp_sects}
}}
"""

    return link_s


def symlist_impl(opt: argparse.Namespace) -> str:
    sections = ["text", "rodata", "tdata", "tbss", "data", "bss"]

    extent_attrs = []
    for section in sections:
        extent_attrs.append(f"char *{section}_extents[2];")
    extent_attrs = "\n    ".join(extent_attrs)

    symbol_decls = []
    mappings = []
    for comp_share in iter_component_tuples(opt.comp_num):
        for section in sections:
            for to_sym in [start_sym, end_sym]:
                symbol_decls.append(
                    f"extern char {to_sym(share_to_section_name(comp_share, section))}[];")

        mappings.append(f"{symlist_mapping_expr(comp_share, opt.comp_num)},")
    symbol_decls = "\n".join(symbol_decls)
    mappings = "\n    ".join(mappings)

    return f"""\
#pragma once

#define UK_COMPONENT_COUNT {opt.comp_num}

struct share_extent_mapping {{
    unsigned share[UK_COMPONENT_COUNT];
    {extent_attrs}
}};

{symbol_decls}

static struct share_extent_mapping uk_component_symlist[] = {{
    {mappings}
}};
"""


def main():
    parser = argparse.ArgumentParser(
        description="Generate linker script for library components")
    subparsers = parser.add_subparsers(dest="cmd", required=True)

    defs_parser = subparsers.add_parser(
        "defs", help="Generate linker script defines")
    remap_parser = subparsers.add_parser(
        "remap", help="Generate a library to component, section remapping, linker script")
    symlist_parser = subparsers.add_parser(
        "symlist", help="Generate C array, containing a mapping of component shares to extent symbols")

    defs_parser.add_argument('-a', '--collect-share-section',
                             action='store_true')
    defs_parser.add_argument("comp_num", help="Number of components",
                             metavar='component-count', type=int)

    remap_parser.add_argument('-w', '--wildcard-location',
                              action='store_true')
    remap_parser.add_argument(
        '-s', '--single-match-location', action='store_true')
    remap_parser.add_argument("-k", "--keep-remapped", action="store_true")
    remap_parser.add_argument(
        'lib_comp_mappings', metavar='library.o=component-id', nargs='*', type=str,
        help='Library Component Mappings'
    )

    symlist_parser.add_argument(
        "comp_num", help="Number of components", metavar="component-count", type=int)

    opt = parser.parse_args()

    if opt.cmd == 'defs':
        print(generic_linker_defs(opt))
    elif opt.cmd == 'remap':
        print(link_remap_sections(opt))
    else:
        print(symlist_impl(opt))


if __name__ == '__main__':
    main()
