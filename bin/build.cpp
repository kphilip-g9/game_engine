// ~gaureesh @NOTE: This file acts as our 'build system'
// it selectively includes and compiles all the required
// source files. We do this to avoid the use of build systems
// like CMake, premake etc. which complicates our project
// beyond what is required.

#include "../engine.h"

#include "../os.cpp"
#include "../main.cpp"


funcdef void
handle_runtime_error(Runtime_Error err)
{
	switch(err) {
	default:
		break;
	}
}




#if OS_Linux || OS_Mac 

int main(int argc, char **argv) {
	string_list args = {};

	

	
	entry_point(args);
}

#elif OS_Windows

~gaureesh @NOTE: windows has it's own main function that
is meant for desktop applications. otherwise the
application will have the standard output console visible
always.

#endif
