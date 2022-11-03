#include "sys_string.h"
#include "sys_sanitizer.h"

#define CSTRING_TERMINATOR '\0'

/////////////////////////////////////////////////////
// MEM functions
/////////////////////////////////////////////////////

void* sys_memset(void* dst, int value, size_t num)
{
    SYS_ASSERT_NOT_NULL(dst);
    SYS_ASSERT(num > 0);

    for (size_t i = 0; i < num; ++i)
    {
        TYPE_CAST(uint8_t*, dst)[i] = value;
    }

    return dst;
}

void* sys_memcpy(void* dst, const void* src, size_t num)
{
    SYS_ASSERT_NOT_NULL(dst);
    SYS_ASSERT_NOT_NULL(src);
    SYS_ASSERT(num > 0);

    for (size_t i = 0; i < num; ++i)
    {
        TYPE_CAST(uint8_t*, dst)[i] = TYPE_CAST_CONST(uint8_t*, src)[i];
    }

    return dst;
}

int sys_memcmp(const void* ptr1, const void* ptr2, size_t num)
{
    SYS_ASSERT_NOT_NULL(ptr1);
    SYS_ASSERT_NOT_NULL(ptr2);
    SYS_ASSERT(num > 0);

    for (size_t i = 0; i < num; ++i)
    {
        if (*TYPE_CAST(uint8_t*, ptr1) < *TYPE_CAST(uint8_t*, ptr2))
        {
            return -1;
        }
        else if (*TYPE_CAST(uint8_t*, ptr1) > *TYPE_CAST(uint8_t*, ptr2))
        {
            return 1;
        }
    }

    return 0;
}

/////////////////////////////////////////////////////
// STR functions
/////////////////////////////////////////////////////

size_t sys_strlen(const char* str) 
{
    SYS_ASSERT_NOT_NULL(str);

	size_t len = 0;
	while (str[len] != CSTRING_TERMINATOR)
    {
		++len;
    }
	return len;
}

char* sys_strcpy(char* dst, const char* src)
{
    SYS_ASSERT_NOT_NULL(dst);
    SYS_ASSERT_NOT_NULL(src);

    for (;;)
    {
        *dst = *src;
        if (*src == CSTRING_TERMINATOR)
        {
            break;
        }
        ++dst;
        ++src;
    }
    return dst;
}
