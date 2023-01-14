#include <uk/component.h>
#include <uk/trampoline.h>
#define __SRC_LIBNAME__ libnolibc
#include <uk/trampoline-share.h>

#if UK_COMPONENT != UK_SRC_COMPONENT
TRAMPOLINE(printf, ())
#endif // CONFIG_VMFUNC0_TRAMPOLINES