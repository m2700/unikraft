#include <uk/auto-trampoline.h>
#include <uk/config.h>

#if CONFIG_LIBUKDEBUG_PRINTK
AUTO_TRAMPOLINE(_uk_printk)
#endif /* CONFIG_LIBUKDEBUG_PRINTK */