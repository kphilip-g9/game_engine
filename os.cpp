#include "engine.h"

#if OS_Linux
# include "linux/os_linux.cpp"
#elif OS_Windows
# include "win32/os_windows.cpp"
#else
# error "platform implementation missing"
#endif
