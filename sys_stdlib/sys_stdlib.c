#include "sys_stdlib.h"

// int uint_to_string(uint64_t number, uint8_t base, char* output, size_t output_size)
// {
//     (void)output_size;

//     static const char allDigits[] = "0123456789ABCDEF";
//     char buffer[64];
//     int bufferIter = 0;
//     const uint8_t shift = (
//         base == BASE_BINARY ? 1 :
//         base == BASE_OCTAL ? 3 :
//         base == BASE_HEX ? 4 : -1
//     );

//     do {
//         //const uint32_t left = number / base;
//         // Base is either 2 or 8 or 10 or 16. Won't work for base10.
//         const uint32_t left = number >> shift;
//         //const uint8_t reminder = number - (left * base);
//         const uint8_t reminder = number - (left << shift);
//         buffer[bufferIter++] = allDigits[reminder];
//         number = left;
//     } while (number > 0);
//     // Revert string.
//     for (uint8_t i = 0; i < bufferIter; ++i)
//     {
//         output[i] = buffer[bufferIter - 1 - i];
//     }
//     output[bufferIter] = 0;

//     return bufferIter;
// }

/**
 * @brief Returns number of characters writen to output.
 * 
 * @param number 
 * @param base 
 * @param output 
 * @param output_size 
 * @return int 
 */
int uint_to_string(uint32_t number, uint8_t base, char* output, size_t output_size)
{
    //return uint_to_string(number, base, output, output_size);
    (void)output_size;

    static const char allDigits[] = "0123456789ABCDEF";
    char buffer[32];
    uint8_t bufferIter = 0;

    do {
        const uint32_t left = number / base;
        const uint32_t reminder = number - (left * base);
        buffer[bufferIter++] = allDigits[reminder];
        number = left;
    } while (number > 0);
    // Revert string.
    for (uint8_t i = 0; i < bufferIter; ++i)
    {
        output[i] = buffer[bufferIter - 1 - i];
    }
    output[bufferIter] = 0;

    return bufferIter;
}
