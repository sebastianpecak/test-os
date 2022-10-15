#include "sys_stdlib.h"

/**
 * @brief Returns number of characters writen to output.
 * 
 * @param number 
 * @param base 
 * @param output 
 * @param output_size 
 * @return int 
 */
int uint32_to_string(uint32_t number, uint8_t base, char* output, size_t output_size)
{
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
