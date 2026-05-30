#include "../engine.h"

#include <windows.h>
#include <fileapi.h>

funcdef void *
os_reserve(u64 size)
{
	void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
	return result;
}

funcdef bool
os_commit(void *ptr, u64 size)
{
	void *result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	return result != 0;
}

funcdef void
os_decommit(void *ptr, u64 size)
{
	VirtualFree(ptr, size, MEM_DECOMMIT);
}

funcdef void
os_release(void *ptr, u64 size)
{
	(void) size;
	VirtualFree(ptr, 0, MEM_RELEASE);
}

funcdef OS_TimeStamp
os_time_now()
{
	static LARGE_INTEGER frequency = {};
	static bool initialized = false;

	if (!initialized)
	{
		QueryPerformanceFrequency(&frequency);
		initialized = true;
	}

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (u64)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
}

funcdef OS_FileData
os_file_data(string path)
{
	OS_FileData result = {};

	Temp t = {};
	defer(temp_end(t));

	string cstr = string_to_cstring(scratch(&t), path);

	DWORD attrs = GetFileAttributesA((char *)cstr.raw);
	if (attrs == INVALID_FILE_ATTRIBUTES)
	{
		return result;
	}

	result.flags |= File_Exists;

	if (attrs & FILE_ATTRIBUTE_DIRECTORY)
	{
		result.flags |= File_Directory;
	}

	WIN32_FILE_ATTRIBUTE_DATA data = {};
	if (GetFileAttributesExA(
		(char *)cstr.raw,
		GetFileExInfoStandard,
		&data))
	{
		ULARGE_INTEGER size;
		size.HighPart = data.nFileSizeHigh;
		size.LowPart  = data.nFileSizeLow;
		result.size = size.QuadPart;
	}

	char const *ext = nullptr;
	for (char const *c = (char *)cstr.raw; *c; ++c)
	{
		if (*c == '.')
		{
			ext = c;
		}
	}

	if (ext)
	{
		if (_stricmp(ext, ".png")  == 0 ||
			_stricmp(ext, ".jpg")  == 0 ||
			_stricmp(ext, ".jpeg") == 0)
		{
			result.kind = OS_FileKind::Image;
		}
		else if (_stricmp(ext, ".glsl") == 0 ||
				 _stricmp(ext, ".vert") == 0 ||
				 _stricmp(ext, ".frag") == 0)
		{
			result.kind = OS_FileKind::Shader;
		}
		else if (_stricmp(ext, ".ttf") == 0)
		{
			result.kind = OS_FileKind::Font;
		}
		else if (_stricmp(ext, ".dll") == 0)
		{
			result.kind = OS_FileKind::Dynamic_Library;
		}

		if (_stricmp(ext, ".exe") == 0 ||
			_stricmp(ext, ".bat") == 0 ||
			_stricmp(ext, ".cmd") == 0 ||
			_stricmp(ext, ".com") == 0)
		{
			result.flags |= File_Executable;
		}
	}

	return result;
}