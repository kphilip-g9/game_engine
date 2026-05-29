#include "engine.h"

#include <stdio.h>

funcdef Runtime_Error
entry_point(string_list args) {

	printf("hello, world\n");

	return Runtime_Error::None;
}


