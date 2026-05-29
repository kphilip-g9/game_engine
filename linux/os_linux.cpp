#include "../engine.h"


funcdef void *
os_reserve(u64 size)
{
	return nullptr;
}

funcdef bool
os_commit(void *ptr, u64 size)
{
	return false;
}

funcdef void
os_decommit(void *ptr, u64 size)
{
}

funcdef void
os_release(void *ptr, u64 size)
{
}

