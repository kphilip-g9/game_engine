#include "engine.h"

funcdef string
string_from_bytes(bytes data)
{
	return string {
		data.raw, data.len
	};
}


funcdef string
string_concat(Arena *arena, string a, string b)
{
	u64 total_len = a.len + b.len;
	
	bytes data = alloc_slice(arena, u8, total_len);

	memcpy(data.raw, a.raw, a.len);
	memcpy(data.raw + a.len, b.raw, b.len);

	return string_from_bytes(data);
}


// like printf formating, but returns a new string
funcdef string
string_format(Arena *arena, const char *fmt_string, ...)
{
	va_list args;

	va_start(args, fmt_string);
	int len = vsnprintf(nullptr, 0, fmt_string, args);
	va_end(args);

	if (len < 0) return {};

	bytes buf = alloc_slice(arena, u8, len + 1);

	va_start(args, fmt_string);
	vsnprintf((char *)buf.raw, len + 1, fmt_string, args);
	va_end(args);

	return { buf.raw, (u64) len };
}


funcdef string
string_from_cstring(Arena *arena, char *cstring)
{
	bytes data = alloc_slice(arena, u8, strlen(cstring));
	memcpy(data.raw, cstring, data.len);
	return string_from_bytes(data);
}


funcdef slice<string>
strings_from_cstrings(Arena *arena, int count, char **cstrings)
{
	slice<string> result = alloc_slice(arena, string, count);

	for (int i=0; i<count; ++i)
	{
		char *cstring = cstrings[i];
		string str = string_from_cstring(arena, cstring);
		result[i] = str;
	}

	return result;
}

funcdef string
string_to_cstring(Arena *arena, string s)
{
	bytes data = alloc_slice(arena, u8, s.len + 1);
	memcpy(data.raw, s.raw, s.len);
	data[s.len] = '\0';

	return string_from_bytes(data);
}


// ~karun

funcdef bool
string_equal(string a, string b)
{
	if (a.len != b.len) return false;
	return memcmp(a.raw, b.raw, a.len) == 0;
}

funcdef string
string_strip(string s)
{
	u64 start = 0;
	u64 end   = s.len;

	// trim leading whitespace
	while (start < end &&
		  (s.raw[start] == ' '  ||
		   s.raw[start] == '\t' ||
		   s.raw[start] == '\n' ||
		   s.raw[start] == '\r'))
	{
		start++;
	}

	// trim trailing whitespace
	while (end > start &&
		  (s.raw[end - 1] == ' '  ||
		   s.raw[end - 1] == '\t' ||
		   s.raw[end - 1] == '\n' ||
		   s.raw[end - 1] == '\r'))
	{
		end--;
	}

	return string { s.raw + start, end - start };
}

funcdef slice<string>
string_as_lines(Arena *arena, string s)
{
	// first pass: count lines
	u64 line_count = 0;
	for (u64 i = 0; i < s.len; i++) {
		if (s.raw[i] == '\n') line_count++;
	}
	// last line may not end with \n
	if (s.len > 0 && s.raw[s.len - 1] != '\n') line_count++;

	slice<string> result = alloc_slice(arena, string, line_count);

	u64 line_idx   = 0;
	u64 line_start = 0;

	for (u64 i = 0; i < s.len; i++) {
		if (s.raw[i] == '\n') {
			u64 end = i;
			// handle windows \r\n line endings
			if (end > line_start && s.raw[end - 1] == '\r') end--;
			result[line_idx++] = string { s.raw + line_start, end - line_start };
			line_start = i + 1;
		}
	}

	// last line if file doesn't end with \n
	if (line_idx < line_count) {
		result[line_idx] = string { s.raw + line_start, s.len - line_start };
	}

	return result;
}