#pragma once

#include <uk/config.h>
#include <uk/component-local.h>

#ifndef STRINGIFY
#ifndef __STRINGIFY
#define __STRINGIFY(x) #x
#endif
#define STRINGIFY(x) __STRINGIFY(x)
#endif

#ifndef UK_CONCAT
#define __UK_CONCAT_X(a, b) a##b
#define UK_CONCAT(a, b) __UK_CONCAT_X(a, b)
#endif

#ifndef __LIBNAME__
#error "__LIBNAME__ not defined"
#endif

#define __SHARE_HAS(share, cs)                                                 \
	UK_CONCAT(UK_CONCAT(__SHARE_, share),                                  \
		  UK_CONCAT(_HAS_, UK_CONCAT(cs, __)))

#ifdef CONFIG_COMPONENT_SPLITTING
#define __UK_LIB_COMPONENT(lib) __##lib##_COMPONENT__
#define __UK_LIB_IS_SHARED(lib) __##lib##_IS_SHARED__
#define __UK_LIB_SHARE(lib) __##lib##_SHARE__
#else // CONFIG_COMPONENT_SPLITTING
#define __UK_LIB_COMPONENT(lib) 0
#define __UK_LIB_IS_SHARED(lib) 0
#define __UK_LIB_SHARE(lib) 0
#endif // CONFIG_COMPONENT_SPLITTING

#define UK_LIB_COMPONENT(lib) __UK_LIB_COMPONENT(lib)
#define UK_LIB_IS_SHARED(lib) __UK_LIB_IS_SHARED(lib)
#define UK_LIB_SHARE(lib) __UK_LIB_SHARE(lib)

#ifdef __USE_VARIANT_COMPONENT__
#define UK_COMPONENT __VARIANT__
#else // __USE_VARIANT_COMPONENT__
#define UK_COMPONENT UK_LIB_COMPONENT(__LIBNAME__)
#endif // __USE_VARIANT_COMPONENT__

#define UK_IS_SHARED UK_LIB_IS_SHARED(__LIBNAME__)
#define UK_SHARE UK_LIB_SHARE(__LIBNAME__)

#ifdef __ASSEMBLY__

#ifdef CONFIG_COMPONENT_SPLITTING
#define UK_COMP_PREFIX_SECTION(prefix, dot, section) .prefix.section
#else // CONFIG_COMPONENT_SPLITTING
#define UK_COMP_PREFIX_SECTION(prefix, dot, section) dot##section
#endif // CONFIG_COMPONENT_SPLITTING

#define UK_COMP_PUBLIC_SECTION(dot, section)                                   \
	UK_COMP_PREFIX_SECTION(shared, dot, section)
#define UK_COMP_CLOCAL_SECTION(dot, section)                                   \
	UK_COMP_PREFIX_SECTION(clocal, dot, section)
#define UK_COMP_SECTION(comp, dot, section)                                    \
	UK_COMP_PREFIX_SECTION(UK_CONCAT(component, comp), dot, section)

#else // __ASSEMBLY__

#ifdef CONFIG_COMPONENT_SPLITTING
#define UK_COMP_PREFIX_SECTION(prefix, dot, section)                           \
	__section("." prefix "." section)
#else // CONFIG_COMPONENT_SPLITTING
#define UK_COMP_PREFIX_SECTION(prefix, dot, section) __section(dot section)
#endif // CONFIG_COMPONENT_SPLITTING

#define UK_COMP_PUBLIC_SECTION(dot, section)                                   \
	UK_COMP_PREFIX_SECTION("shared", dot, section)
#define UK_COMP_CLOCAL_SECTION(dot, section)                                   \
	UK_COMP_PREFIX_SECTION("clocal", dot, section)
#define UK_COMP_SECTION(comp, dot, section)                                    \
	UK_COMP_PREFIX_SECTION("component" STRINGIFY(comp), dot, section)

#endif // __ASSEMBLY__

#define UK_COMP_LIB_SECTION(lib, dot, section)                                 \
	UK_COMP_SECTION(UK_SHARE(lib), dot, section)