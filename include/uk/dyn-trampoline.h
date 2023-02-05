#pragma once // __SRC_LIBNAME__ can be changed without re-including

#if CONFIG_VMFUNC0_TRAMPOLINES
#include <uk/trampoline.h>
#else // CONFIG_VMFUNC0_TRAMPOLINES
#define DYN_TRAMPOLINE_INIT
#define DYN_TRAMPOLINE_FINI
#endif // CONFIG_VMFUNC0_TRAMPOLINES