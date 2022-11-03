#ifndef SYS_STDINT_H
#define SYS_STDINT_H

/**
 * @brief Adds uint type of arbitrary bit size.
 * 
 */
#define ADD_UINTN_TYPE(_uint_n) \
    struct _uint## _uint_n ## _t { \
        uint32_t value : (_uint_n); \
    }; \
    typedef struct _uint## _uint_n ## _t uint## _uint_n ## _t;

/**
 * @brief Standard int types.
 * 
 */
typedef signed char          int8_t;
typedef signed short int     int16_t;
typedef signed long int      int32_t;
typedef signed long long int int64_t;

/**
 * @brief Standard uint types.
 * 
 */
typedef unsigned char          uint8_t;
typedef unsigned short int     uint16_t;
typedef unsigned long int      uint32_t;
typedef unsigned long long int uint64_t;
//ADD_UINTN_TYPE(5);

/**
 * @brief Size types.
 * 
 */
typedef uint32_t size_t;

/**
 * @brief Integer pointer types.
 * 
 */
typedef int32_t  intptr_t;
typedef uint32_t uintptr_t;

#endif // SYS_STDINT_H
