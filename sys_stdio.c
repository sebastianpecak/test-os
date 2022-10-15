#include "sys_stdio.h"
#include "sys_stdint.h"
#include "sys_tools.h"
#include "terminal.h"
#include "sys_stdlib.h"
#include "sys_multiboot.h"
#include "sys_stdarg.h"
#include "sys_sanitizer.h"
#include "sys_string.h"

struct PrintfFormatInfo_s
{
    // Flags.
    uint32_t flag_left_just             : 1;
    uint32_t flag_force_sign            : 1;
    uint32_t flag_space_instead_of_sign : 1;
    uint32_t flag_is_hash               : 1;
    uint32_t flag_left_pad_zero         : 1;
    // Specifier.
    uint32_t specifier_int_dec    : 1;
    uint32_t specifier_uint_dec   : 1;
    uint32_t specifier_uint_oct   : 1;
    uint32_t specifier_uint_hex   : 1;
    uint32_t specifier_lower_case : 1;
    uint32_t specifier_upper_case : 1;
    uint32_t specifier_cstr       : 1;
};

/**
 * @brief Returns number of characters read from format string.
 *        Returns 0 if no flag present.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int sys_printf_parse_format_flag(const char *format, struct PrintfFormatInfo_s* output)
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
        //terminal_writestring_warning("sys_printf_parse_format_flag: unknown flag\n");
        return 0;
    }

    return 1;
}

/**
 * @brief Returns number of characters read from format string.
 *        Returns -1 on error.
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int sys_printf_parse_format_specifier(const char *format, struct PrintfFormatInfo_s* output)
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
    default:
        terminal_writestring_error("sys_printf_parse_format_specifier: return -1\n");
        return -1;
    }

    return 1;
}

/**
 * @brief Returns number of characters read from format string.
 *        %[flags][width][.precision][length]specifier
 * 
 * @param format 
 * @param output 
 * @return int 
 */
static int sys_printf_parse_format(const char *format, struct PrintfFormatInfo_s* output)
{
    int result = 0;
    int total_read = 0;

    sys_memset(output, 0, sizeof(struct PrintfFormatInfo_s));

    // Try to parse FLAGS.
    result = sys_printf_parse_format_flag(format + total_read, output);
    if (result < 0)
    {
        terminal_writestring_error("sys_printf_parse_format: return -1\n");
        return -1;
    }
    total_read += result;

    // Try to parse SPECIFIERS.
    result = sys_printf_parse_format_specifier(format + total_read, output);
    if (result < 0)
    {
        terminal_writestring_error("sys_printf_parse_format: return -1\n");
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
static int sys_printf_format_and_append(const struct PrintfFormatInfo_s* format, char* output, size_t output_size, uint32_t arg)
{
    int result = 0;

    if (format->specifier_cstr)
    {
        const char* cstr = CAST_TO_CHAR_PTR(arg);
        strcpy(output, cstr);
        return strlen(cstr);
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

    result += uint32_to_string(
        arg,
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

/**
 * @brief Just one thing to remember when dealing with va_args
 *        Types narrower than 4bytes are expanded to 4bytes.
 * 
 * @param format 
 * @param ... 
 * @return int 
 */
int sys_printf(const char *format, ...)
{
    int buffer_iter = 0;
    int result = 0;
    uint32_t varg_buffer = 0;
    char buffer[1024];
    va_list args;
    struct PrintfFormatInfo_s format_info;

    SYS_ASSERT_NOT_NULL(format);
    va_start(args, format);
    sys_memset(&format_info, 0, sizeof(format_info));
    sys_memset(buffer, 0, sizeof(buffer));

    // Iterate over format string.
    for (; *format != '\0'; /*++format*/)
    {
        switch (*format)
        {
        case '%':
            result = sys_printf_parse_format(format + 1, &format_info);
            format += result + 1;
            varg_buffer = va_arg(args, uint32_t);
            result = sys_printf_format_and_append(&format_info, buffer + buffer_iter, sizeof(buffer) - buffer_iter, varg_buffer);
            buffer_iter += result;
            break;

        default:
            buffer[buffer_iter++] = *format;
            ++format;
            break;
        }
    }

    // const struct MultibootBootInfo_s* boot_info = va_arg(args, struct MultibootBootInfo_s*);
    // const uint32_t varg1 = va_arg(args, uint32_t);
    // const uint32_t varg2 = va_arg(args, uint32_t);
    // const uint32_t varg3 = va_arg(args, uint32_t);
    // const uint32_t varg4 = va_arg(args, uint32_t);
    // const uint32_t varg5 = va_arg(args, uint32_t);

    //SYS_VARG_INIT(format, sizeof(format));

    //SYS_VARG_INIT();
    // const struct MultibootBootInfo_s* boot_info = (struct MultibootBootInfo_s*)_varg_head;
    // _varg_head += sizeof(struct MultibootBootInfo_s);
    // const uint32_t varg1 = SYS_VARG_GET_UINT32();
    // const uint32_t varg2 = SYS_VARG_GET_UINT32();
    // const uint32_t varg3 = SYS_VARG_GET_UINT32();
    // const uint32_t varg4 = SYS_VARG_GET_UINT32();
    // const uint32_t varg5 = SYS_VARG_GET_UINT32();
   // const struct MultibootBootInfo_s* boot_info = (struct MultibootBootInfo_s*)SYS_VARG_GET_UINT32();
    //uint32_t* arg1 = (uint32_t*)(_varg_head);

    // terminal_writestring_warning("format: "); terminal_writestring(format); terminal_putchar('\n');

    // uint32_to_string(varg1, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("varg1: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(varg2, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("varg2: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(varg3, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("varg3: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(varg4, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("varg4: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(varg5, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("varg5: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(boot_info->flags, BASE_HEX, buffer, sizeof(buffer));
    // terminal_writestring_warning("flags: "); terminal_writestring(buffer); terminal_putchar('\n');

    // const uint32_t firstStackArgAddress = ADDRESS_TO_UINT32(format);
    // // Let's assume that we have two uint32_t args passed.
    // terminal_writestring_warning("First stack arg: "); terminal_writestring(format); terminal_putchar('\n');

    // uint32_t* secondStackArgAddress = (uint32_t*)(firstStackArgAddress + sizeof(uint32_t));
    // uint32_t* thirdStackArgAddress = (uint32_t*)(firstStackArgAddress + sizeof(uint32_t) + sizeof(uint32_t));

    // uint32_to_string(*secondStackArgAddress, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("Second stack arg: "); terminal_writestring(buffer); terminal_putchar('\n');

    // uint32_to_string(*thirdStackArgAddress, BASE_DECIMAL, buffer, sizeof(buffer));
    // terminal_writestring_warning("Third stack arg: "); terminal_writestring(buffer); terminal_putchar('\n');

    va_end(args);

    terminal_writestring(buffer);

    return buffer_iter;
}
