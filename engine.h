#ifndef ENGINE_H
#define ENGINE_H

//////////
// ~gaureesh @NOTE: base types

#include <stdint.h> // for fixed size integers.
#include <assert.h> // for assert macro.
#include <string.h> // for memmove, memcpy etc. ( not for cstd string functions ).
#include <stdarg.h> // for va_args.
#include <stdio.h>  // standard io operations.

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

	T& operator[](u64 index) {
		assert(index < this->len);
		return this->raw[index];
	}
};

template<typename T>
struct list {
	T   *raw;
	u64  len;
	u64  capacity;

	T& operator[](u64 index) {
		assert(index < this->len);
		return this->raw[index];
	}
};

struct vec2 {
	f32 x, y;
};

struct vec4 {
	f32 x, y, z, w;
};


typedef slice<u8> bytes;
typedef slice<const u8> string;

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
#elif defined(_MSC_VER)
# define Compiler_CL
#else
# error "c++ compiler not supported"
#endif

#ifndef __cplusplus
# error "compiler should be c++"
#endif


//////////////
// ~gaureesh @NOTE: useful macro definitions

#ifndef DEBUG_BUILD
# define DEBUG_BUILD 1
#endif

#define funcdef       static
#define local_persist static
#define global        static

#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Clamp(min, val, max)  Min(Max((val), (min)), (max))

#define Align_Up_Power_2(val, alignment) (((val) + (alignment) - 1) & ~((alignment) - 1));

#define KB(x) (u64) (x << 10)
#define MB(x) (u64) (x << 20)
#define GB(x) (u64) (x << 30)

#define Flag_Check(__flags, __mask) !!((__flags) & (__mask))

//////////////
// ~gaureesh @NOTE: os

funcdef void *os_reserve(u64 size);
funcdef bool  os_commit(void *ptr, u64 size);
funcdef void  os_decommit(void *ptr, u64 size);
funcdef void  os_release(void *ptr, u64 size);

//////////////
// ~gaureesh @NOTE: arena

struct Arena {
	u64   reserved;
	u64   committed;
	u64   used;
};

struct Temp {
	Arena *arena;
	u64    mark;
};

funcdef Arena *arena_init(u64 reserve);
funcdef void  *arena_allocate(Arena *arena, void *old_ptr, u64 old_size, u64 new_size, u64 alignment);
funcdef void   arena_free(Arena *arena);

funcdef Temp   temp_begin(Arena *arena);
funcdef void   temp_end(Temp temp);

funcdef Arena *scratch(Temp *temp = nullptr);

#define alloc_struct(_arr, _T) (_T *) arena_allocate((_arr), nullptr, 0, sizeof(_T), alignof(_T))
#define alloc_slice(_arr, _T, _n) slice<_T> { (_T *) arena_allocate((_arr), nullptr, 0, sizeof(_T) * (_n), alignof(_T)), (u64) (_n) }

//////////////
// ~gaureesh @NOTE: string

#define S(x) string { (const u8 *) (x), sizeof(x) - 1 }
#define S_FMT(s) (int) s.len, (char *) s.raw

funcdef string string_from_bytes(bytes data);
funcdef string string_concat(Arena *arena, string a, string b);
funcdef string string_format(Arena *arena, const char *fmt_string, ...);
funcdef string string_from_cstring(Arena *arena, char *cstring);

funcdef slice<string> strings_from_cstrings(Arena *arena, int count, char **cstrings);

//////////////
// ~gaureesh @NOTE: main

#endif // ENGINE_H
