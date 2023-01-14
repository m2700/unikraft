#pragma once

#include <uk/config.h>
#include <uk/essentials.h>

#ifndef __LIBNAME__
#error "__LIBNAME__ not defined"
#endif

#ifdef CONFIG_COMPONENT_SPLITTING
#define __UK_LIB_COMPONENT(lib) __##lib##_COMPONENT__
#else // CONFIG_COMPONENT_SPLITTING
#define __UK_LIB_COMPONENT(lib) 0
#endif // CONFIG_COMPONENT_SPLITTING

#define UK_LIB_COMPONENT(lib) __UK_LIB_COMPONENT(lib)

#define UK_COMPONENT UK_LIB_COMPONENT(__LIBNAME__)

#ifdef CONFIG_COMPONENT_SPLITTING
#define UK_COMP_LIB_SECTION(lib, dot, section)                                 \
	__section(".component" STRINGIFY(UK_LIB_COMPONENT(lib)) "." section)
#define UK_COMP_SHARE_SECTION(share, dot, section)                             \
	__section("." STRINGIFY(share) "." section)
#else // CONFIG_COMPONENT_SPLITTING
#define UK_COMP_LIB_SECTION(lib, dot, section) __section(dot section)
#define UK_COMP_SHARE_SECTION(share, dot, section) __section(dot section)
#endif // CONFIG_COMPONENT_SPLITTING
