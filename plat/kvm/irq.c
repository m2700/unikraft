/* SPDX-License-Identifier: ISC */
/*
 * Authors: Dan Williams
 *          Martin Lucina
 *          Ricardo Koller
 *          Costin Lupu <costin.lupu@cs.pub.ro>
 *
 * Copyright (c) 2015-2017 IBM
 * Copyright (c) 2016-2017 Docker, Inc.
 * Copyright (c) 2018, NEC Europe Ltd., NEC Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* Taken from solo5 intr.c */

#include <stdlib.h>
#include <uk/alloc.h>
#include <uk/plat/lcpu.h>
#include <uk/plat/time.h>
#include <uk/plat/irq.h>
#include <uk/plat/common/irq.h>
#include <kvm/intctrl.h>
#include <uk/assert.h>
#include <uk/print.h>
#include <errno.h>
#include <uk/bitops.h>
#include <uk/component.h>
#include <uk/config.h>
#include <uk/dyn-trampoline.h>

/* IRQ handlers declarations */
struct irq_handler {
	irq_handler_func_t func;
	void *arg;
};

UK_COMP_PUBLIC_SECTION(".", "bss")
static struct irq_handler irq_handlers[__MAX_IRQ]
				[CONFIG_KVM_MAX_IRQ_HANDLER_ENTRIES];

static inline struct irq_handler *allocate_handler(unsigned long irq)
{
	UK_ASSERT(irq < __MAX_IRQ);
	for (int i = 0; i < CONFIG_KVM_MAX_IRQ_HANDLER_ENTRIES; i++)
		if (irq_handlers[irq][i].func == NULL)
			return &irq_handlers[irq][i];
	return NULL;
}

int ukplat_irq_register(unsigned long irq, irq_handler_func_t func, void *arg)
{
	struct irq_handler *h;
	unsigned long flags;

	UK_ASSERT(func);
	if (irq >= __MAX_IRQ)
		return -EINVAL;

	flags = ukplat_lcpu_save_irqf();
	h = allocate_handler(irq);
	if (!h) {
		ukplat_lcpu_restore_irqf(flags);
		return -ENOMEM;
	}

	h->func = func;
	h->arg = arg;

	ukplat_lcpu_restore_irqf(flags);

	intctrl_clear_irq(irq);
	return 0;
}

/*
 * TODO: This is a temporary solution used to identify non TSC clock
 * interrupts in order to stop waiting for interrupts with deadline.
 */
extern unsigned long sched_have_pending_events;

UK_COMP_PUBLIC_SECTION(".", "text") // irq handler
void _ukplat_irq_handle(unsigned long irq)
{
	DYN_TRAMPOLINE_INIT;

	struct irq_handler *h;
	int i;

	UK_ASSERT(irq < __MAX_IRQ);

	for (i = 0; i < CONFIG_KVM_MAX_IRQ_HANDLER_ENTRIES; i++) {
		if (irq_handlers[irq][i].func == NULL)
			break;
		h = &irq_handlers[irq][i];
		if (irq != ukplat_time_get_irq())
			/* ukplat_time_get_irq() gives the IRQ reserved for a timer,
			 * responsible to wake up cpu from halt, so it can check if
			 * it has something to do. Effectively it is OS ticks.
			 *
			 * If interrupt comes not from the timer, the
			 * chances are some work have just
			 * arrived. Let's kick the scheduler out of
			 * the halting loop, and let it take care of
			 * that work.
			 */
			__uk_test_and_set_bit(0, &sched_have_pending_events);

		if (h->func(h->arg) == 1)
			goto exit_ack;
	}
	/*
	 * Acknowledge interrupts even in the case when there was no handler for
	 * it. We do this to (1) compensate potential spurious interrupts of
	 * devices, and (2) to minimize impact on drivers that share one
	 * interrupt line that would then stay disabled.
	 */
	uk_pr_crit("Unhandled irq=%lu\n", irq);

exit_ack:
	intctrl_ack_irq(irq);

	DYN_TRAMPOLINE_FINI;
}

int ukplat_irq_init(struct uk_alloc *a __unused)
{
	UK_ASSERT(ukplat_lcpu_irqs_disabled());

	/* Nothing for now */
	return 0;
}
