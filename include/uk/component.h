#pragma once

#include <uk/config.h>
#include <uk/essentials.h>

#ifndef CONFIG_COMPONENT_SPLITTING
#error "COMPONENT_SPLITTING must be enabled"
#endif

#define __UK_LIB_COMPONENT(lib) __##lib##_COMPONENT__
#define UK_LIB_COMPONENT(lib) __UK_LIB_COMPONENT(lib)

#define UK_COMPONENT UK_LIB_COMPONENT(__LIBNAME__)

#define UK_LIB_SECTION(lib, section)                                           \
	__section(".component" STRINGIFY(UK_LIB_COMPONENT(lib)) "." STRINGIFY( \
	    section))

#define UK_SHARE_SECTION(share, section)                                       \
	__section("." STRINGIFY(share) "." STRINGIFY(section))
