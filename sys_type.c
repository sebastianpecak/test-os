#include "sys_type.h"

#define TO_LOWER_UPPER_OFFSET 32

int tolower(int c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return (c + TO_LOWER_UPPER_OFFSET);
    }

    return c;
}

int toupper(int c)
{
    if (c >= 'a' && c <= 'z')
    {
        return (c - TO_LOWER_UPPER_OFFSET);
    }

    return c;
}
