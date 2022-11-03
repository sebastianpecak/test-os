#include "sys_stdio.h"
#include "sys_stdint.h"
#include "sys_stdlib.h"
#include "sys_stdarg.h"
#include "sys_sanitizer.h"
#include "sys_string.h"
#include "sys_limits.h"

/**
 * @brief Just one thing to remember when dealing with va_args
 *        Types narrower than 4bytes are expanded to 4bytes.
 * 
 */

struct PrintfFormatInfo_s
{
    // Flags.
    uint32_t flag_left_just             : 1;
    uint32_t flag_force_sign            : 1;
    uint32_t flag_space_instead_of_sign : 1;
    uint32_t flag_is_hash               : 1;
    uint32_t flag_left_pad_zero         : 1;
    // Length.
    uint32_t length_long      : 1;
    uint32_t length_long_long : 1;
    // Specifier.
    uint32_t specifier_int_dec    : 1;
    uint32_t specifier_uint_dec   : 1;
    uint32_t specifier_uint_oct   : 1;
    uint32_t specifier_uint_hex   : 1;
    uint32_t specifier_lower_case : 1;
    uint32_t specifier_upper_case : 1;
    uint32_t specifier_cstr       : 1;
    uint32_t specifier_char       : 1;
};

/**
 * @brief Returns number of characters read from format string.
 *        Returns 0 if no flag present.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int _parse_format_flag(const char *format, struct PrintfFormatInfo_s* output)
{
    switch (*format)
    {
    case '-':
        output->flag_left_just = 1;
        break;
    case '+':
        output->flag_force_sign = 1;
        break;
    case ' ':
        output->flag_space_instead_of_sign = 1;
        break;
    case '#':
        output->flag_is_hash = 1;
        break;
    case '0':
        output->flag_left_pad_zero = 1;
        break;
    default:
        return 0;
    }

    return 1;
}

/**
 * @brief Parses optional [length] field.
 *        Returns number of characters read from format.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int _parse_format_length(const char *format, struct PrintfFormatInfo_s* output)
{
    // long.
    if (format[0] == 'l')
    {
        // long-long.
        if (format[1] == 'l')
        {
            output->length_long_long = 1;
            return 2;
        }
        else
        {
            output->length_long = 1;
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Returns number of characters read from format string.
 *        Returns -1 on error.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int _parse_format_specifier(const char *format, struct PrintfFormatInfo_s* output)
{
    switch (*format)
    {
    case 'd':
    case 'i':
        output->specifier_int_dec = 1;
        break;
    case 'u':
        output->specifier_uint_dec = 1;
        break;
    case 'X':
        output->specifier_upper_case = 1;
        output->specifier_uint_hex   = 1;
        break;
    case 'x':
        output->specifier_lower_case = 1;
        output->specifier_uint_hex   = 1;
        break;
    case 'o':
        output->specifier_uint_oct = 1;
        break;
    case 's':
        output->specifier_cstr = 1;
        break;
    case 'c':
        output->specifier_char = 1;
        break;
    default:
        terminal_writestring_error("_parse_format_specifier: unknown format specifier '");
        terminal_putchar(*format);
        terminal_writestring_error("'!\n");
        return -1;
    }

    return 1;
}

/**
 * @brief Returns number of characters read from format string.
 *        %[flags][width][.precision][length]specifier
 *        Returns negative number on error.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int _parse_format(const char *format, struct PrintfFormatInfo_s* output)
{
    int result     = 0;
    int total_read = 0;

    sys_memset(output, 0, sizeof(struct PrintfFormatInfo_s));

    // Try to parse FLAGS.
    result = _parse_format_flag(format + total_read, output);
    total_read += result;

    // Try to parse LENGTH.
    result = _parse_format_length(format + total_read, output);
    total_read += result;

    // Try to parse SPECIFIERS.
    result = _parse_format_specifier(format + total_read, output);
    if (result < 0)
    {
        terminal_writestring_error("_parse_format: parsing specifier failed!\n");
        return -1;
    }
    total_read += result;

    return total_read;
}

/**
 * @brief Returns number of characters writen to output buffer.
 * 
 * @param format 
 * @param output 
 * @param output_size 
 * @param arg 
 * @return int 
 */
static int _format_and_append(const struct PrintfFormatInfo_s* format, char* output, size_t output_size, va_list* vargs)
{
    int result = 0;
    uint64_t number = 0;

    if (format->specifier_char)
    {
        output[0] = va_arg(*vargs, uint32_t);
        return 1;
    }

    if (format->specifier_cstr)
    {
        const char* cstr = va_arg(*vargs, char*);
        sys_strcpy(output, cstr);
        return sys_strlen(cstr);
    }

    // Append 0x if needed.
    if (format->specifier_uint_hex && format->flag_is_hash)
    {
        //terminal_writestring_error("sys_printf_format_and_append: hex");
        output[0] = '0'; output[1] = 'x';
        result += 2;
    }
    // Append 0 if needed.
    else if (format->specifier_uint_oct && format->flag_is_hash)
    {
        output[0] = '0';
        result += 1;
    }

    if (format->length_long_long)
    {
        //terminal_writestring_warning("sys_printf_format_and_append: UINT64\n");
        number = va_arg(*vargs, uint64_t);
        if (number & UINT32_MAX_VALUE)
        {
            terminal_writestring_warning("sys_printf_format_and_append: lower is max\n");
        }
        if ((number >> 32) & UINT32_MAX_VALUE)
        {
            terminal_writestring_warning("sys_printf_format_and_append: upper is max\n");
        }
    }
    else
    {
        //terminal_writestring_warning("sys_printf_format_and_append: UINT32\n");
        number = va_arg(*vargs, uint32_t);
    }
    //(format->length_long_long ? va_arg(*vargs, uint64_t) : va_arg(*vargs, uint32_t));
    result += uint_to_string(
        number,
        //BASE_HEX,
        (format->specifier_uint_hex ? BASE_HEX : (
            format->specifier_uint_dec ? BASE_DECIMAL : (
                format->specifier_uint_oct ? BASE_OCTAL : BASE_NONE
            )
        )),
        output + result,
        output_size
    );

    return result;
}

int MAKE_FUNC_NAME(vsprintf)(char* output, const char* format, va_list args)
{
    int output_iter;
    int result;
    struct PrintfFormatInfo_s format_info;

    SYS_ASSERT_NOT_NULL(output);
    SYS_ASSERT_NOT_NULL(format);
    SYS_ASSERT_NOT_NULL(args);

    output_iter = 0;
    result = 0;
    sys_memset(&format_info, 0, sizeof(format_info));

    // Iterate over format string.
    for (; *format != '\0';)
    {
        // Process format string.
        if (*format == '%')
        {
            result = _parse_format(format + 1, &format_info);
            if (result < 0)
            {
                terminal_writestring_error("vsprintf: format parsing failed!\n");
                return -1;
            }
            format += result + 1;
            result = _format_and_append(&format_info, output + output_iter, 0, &args);
            output_iter += result;
            continue;
        }
        // Process printable character.
        output[output_iter++] = *format;
        ++format;
    }

    // Zero-terminate string.
    output[output_iter] = '\0';

    return output_iter;
}

int MAKE_FUNC_NAME(printf)(const char *format, ...)
{
    char buffer[1024];
    va_list args;

    SYS_ASSERT_NOT_NULL(format);

    sys_memset(buffer, 0, sizeof(buffer));
    va_start(args, format);
    const int result = sys_vsprintf(buffer, format, args);
    va_end(args);

    if (result > 0)
    {
        terminal_writestring(buffer);
    }

    return result;
}

int MAKE_FUNC_NAME(sprintf)(char* str, const char* format, ...)
{
    va_list args;

    SYS_ASSERT_NOT_NULL(str);
    SYS_ASSERT_NOT_NULL(format);

    va_start(args, format);
    const int result = sys_vsprintf(str, format, args);
    va_end(args);

    return result;
}
