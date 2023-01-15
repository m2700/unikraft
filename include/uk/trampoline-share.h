#include <uk/config.h>

#ifndef __SRC_LIBNAME__
#error "__SRC_LIBNAME__ is not defined"
#endif // __SRC_LIBNAME__

#ifdef UK_SRC_COMPONENT
#undef UK_SRC_COMPONENT
#endif

#ifdef UK_TRAMPOLINE_SHARE
#undef UK_TRAMPOLINE_SHARE
#endif

#define UK_SRC_COMPONENT UK_LIB_COMPONENT(__SRC_LIBNAME__)

#if !defined(CONFIG_COMPONENT_SPLITTING)
// no shares defined without component splitting
#elif UK_SRC_COMPONENT < UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(component, UK_SRC_COMPONENT),                      \
		  UK_CONCAT(_, UK_COMPONENT))
#elif UK_SRC_COMPONENT > UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(component, UK_COMPONENT),                          \
		  UK_CONCAT(_, UK_SRC_COMPONENT))
#else
#define UK_TRAMPOLINE_SHARE UK_CONCAT(component, UK_COMPONENT)
#endif