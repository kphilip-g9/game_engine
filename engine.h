#ifndef ENGINE_H
#define ENGINE_H

//////////
// ~gaureesh @NOTE: base types

#include <stdint.h>
#include <assert.h>

typedef uint8_t  u8;
typedef  int8_t  s8;
typedef uint16_t u16;
typedef  int16_t s16;
typedef uint32_t u32;
typedef  int32_t s32;
typedef uint64_t u64;
typedef  int64_t s64;

typedef float  f32;
typedef double f64;

template<typename T>
struct slice {
	T   *raw;
	u64  len;
	
	slice<T> range(u64 begin, u64 end)
	{
		assert(begin <= len);
		assert(begin <= end);
		assert(end <= len);
		
		return {
			raw + begin,
			end - begin
		};
	}
};

template<typename T>
struct list {
	T   *data;
	u64  len;
	u64  capacity;
};

struct vec2 {
	f32 x, y;
};

struct vec4 {
	f32 x, y, z, w;
};


typedef slice<u8> bytes;
typedef slice<const u8> string;

typedef slice<string> string_list;


//////////
// ~gaureesh @NOTE: os and compiler detection

#ifdef _WIN32
# define OS_Windows 1
# define OS_Linux   0
# define OS_Mac     0
#elif __linux__
# define OS_Windows 0
# define OS_Linux   1
# define OS_Mac     0
#elif __APPLE__
# define OS_Windows 0
# define OS_Linux   0
# define OS_Mac     1
#else
# error "target platform unsupported"
#endif

#if defined(__clang__)
# define Compiler_Clang
#elif defined(__GNUC__) || defined(__GNUG__)
# define Compiler_GCC
#elif defined(__MSC_VER)
# define Compiler_CL
#else
# error "c++ compiler not supported"
#endif

#ifndef __cplusplus
# error "compiler should be c++"
#endif


//////////////
// ~gaureesh @NOTE: useful macro definitions

#define funcdef       static
#define local_persist static
#define global        static

#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Clamp(min, val, max)  Min(Max((val), (min)), (max))

//////////////
// ~gaureesh @NOTE: os

funcdef void *os_reserve(u64 size);
funcdef bool  os_commit(void *ptr, u64 size);
funcdef void  os_decommit(void *ptr, u64 size);
funcdef void  os_release(void *ptr, u64 size);


//////////////
// ~gaureesh @NOTE: arena

struct Arena {
	bytes reserved;
	u64   committed;
	u64   used;
};

//////////////
// ~gaureesh @NOTE: main

enum class Runtime_Error {
	None,
	Window_Creation_Failure,
	Opengl_Init_Failure,
}; 

#endif // ENGINE_H
