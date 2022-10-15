#include "sys_string.h"
#include "sys_sanitizer.h"
#include "sys_tools.h"

#define CSTRING_TERMINATOR '\0'

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len] != CSTRING_TERMINATOR)
    {
		++len;
    }
	return len;
}

char* strcpy(char* dest, const char* src)
{
    for (;;)
    {
        *dest = *src;
        if (*src == CSTRING_TERMINATOR)
        {
            break;
        }
        ++dest;
        ++src;
    }
    return dest;
}

void* sys_memset(void* dst, int value, size_t num)
{
    SYS_ASSERT_NOT_NULL(dst);

    for (size_t i = 0; i < num; ++i)
    {
        CAST_TO_UINT8_PTR(dst)[i] = value;
    }

    return dst;
}
