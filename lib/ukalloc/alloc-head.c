#include <uk/config.h>
#include <uk/alloc_impl.h>

struct uk_alloc *_uk_alloc_head;

int uk_alloc_register(struct uk_alloc *a)
{
	struct uk_alloc *this = _uk_alloc_head;

	if (!_uk_alloc_head) {
		_uk_alloc_head = a;
		a->next = __NULL;
		return 0;
	}

	while (this && this->next)
		this = this->next;
	this->next = a;
	a->next = __NULL;
	return 0;
}
