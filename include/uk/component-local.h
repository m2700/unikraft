#pragma once

#ifdef CONFIG_COMPONENT_SPLITTING
#define UK_COMPONENT_LOCAL_NAME(sym)                             \
	UK_CONCAT(__component_local_, sym)
#else // CONFIG_COMPONENT_SPLITTING
#define UK_COMPONENT_LOCAL_NAME(sym) sym
#endif // CONFIG_COMPONENT_SPLITTING