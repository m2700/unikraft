#include <uk/config.h>

#ifdef __HEADER_LIBNAME__

#ifdef UK_HEADER_COMPONENT
#undef UK_HEADER_COMPONENT
#endif

#ifdef UK_TRAMPOLINE_SHARE
#undef UK_TRAMPOLINE_SHARE
#endif

#define UK_HEADER_COMPONENT UK_LIB_COMPONENT(__HEADER_LIBNAME__)

#if !defined(CONFIG_COMPONENT_SPLITTING)
// no shares defined without component splitting
#elif UK_HEADER_COMPONENT < UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(component, UK_HEADER_COMPONENT),                   \
		  UK_CONCAT(_, UK_COMPONENT))
#elif UK_HEADER_COMPONENT > UK_COMPONENT
#define UK_TRAMPOLINE_SHARE                                                    \
	UK_CONCAT(UK_CONCAT(component, UK_COMPONENT),                          \
		  UK_CONCAT(_, UK_HEADER_COMPONENT))
#else
#define UK_TRAMPOLINE_SHARE UK_CONCAT(component, UK_COMPONENT)
#endif

#endif // __HEADER_LIBNAME__