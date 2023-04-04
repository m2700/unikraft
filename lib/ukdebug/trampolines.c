#include <uk/auto-trampoline.h>
#include <uk/config.h>

#if CONFIG_LIBUKDEBUG_PRINTK
AUTO_TRAMPOLINE(_uk_printk)
AUTO_TRAMPOLINE(_uk_printd)
#endif /* CONFIG_LIBUKDEBUG_PRINTK */