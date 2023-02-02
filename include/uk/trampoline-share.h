// include every time __SRC_LIBNAME__ changes

#include <uk/config.h>
#include <uk/component.h>

#ifndef __SRC_LIBNAME__
#ifndef __VARIANT__
#error "both __SRC_LIBNAME__ & __VARIANT__ are not defined"
#endif // __VARIANT__
#endif // __SRC_LIBNAME__

#ifdef UK_SRC_COMPONENT
#undef UK_SRC_COMPONENT
#endif

#ifdef UK_SRC_IS_SHARED
#undef UK_SRC_IS_SHARED
#endif

#ifdef UK_SRC_SHARE
#undef UK_SRC_SHARE
#endif

#ifdef UK_TRAMPOLINE_SHARE
#undef UK_TRAMPOLINE_SHARE
#endif

#ifdef UK_TRAMPOLINE_NEEDED
#undef UK_TRAMPOLINE_NEEDED
#endif

#ifdef __SRC_LIBNAME__
#define UK_SRC_COMPONENT UK_LIB_COMPONENT(__SRC_LIBNAME__)
#define UK_SRC_IS_SHARED UK_LIB_IS_SHARED(__SRC_LIBNAME__)
#define UK_SRC_SHARE UK_LIB_SHARE(__SRC_LIBNAME__)
#else // __SRC_LIBNAME__
#define UK_SRC_COMPONENT UK_LIB_COMPONENT(__VARIANT__)
#define UK_SRC_IS_SHARED UK_LIB_IS_SHARED(__VARIANT__)
#define UK_SRC_SHARE UK_LIB_SHARE(__VARIANT__)
#endif // __SRC_LIBNAME__

#if !defined(CONFIG_COMPONENT_SPLITTING)
// no shares defined without component splitting
#elif UK_IS_SHARED
#if __SHARE_HAS(UK_SHARE, UK_SRC_COMPONENT)
#define UK_TRAMPOLINE_SHARE UK_SHARE
#else
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(__SHARE_COMBI_, UK_SHARE),                         \
		  UK_CONCAT(UK_CONCAT(_, UK_SRC_COMPONENT), __))
#endif
#elif UK_SRC_COMPONENT < UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(UK_SRC_COMPONENT, _), UK_COMPONENT)
#elif UK_SRC_COMPONENT > UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(UK_COMPONENT, _), UK_SRC_COMPONENT)
#else
#define UK_TRAMPOLINE_SHARE UK_COMPONENT
#endif

#define UK_TRAMPOLINE_NEEDED                                                   \
	(!(UK_SRC_IS_SHARED && __SHARE_HAS(UK_SRC_SHARE, UK_SHARE))            \
	 && (UK_IS_SHARED || UK_COMPONENT != UK_SRC_COMPONENT))