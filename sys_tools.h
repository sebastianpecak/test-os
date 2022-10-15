#ifndef SYS_TOOLS_H
#define SYS_TOOLS_H

#include "sys_stdint.h"

/**
 * @brief Get address of an object and cast it to uint32_t.
 * 
 */
#define ADDRESS_TO_UINT32(_object) \
    ((uint32_t)(&(_object)))

#define CAST_TO_UINT8_PTR(_object) \
    ((uint8_t*)_object)

#define CAST_TO_CHAR_PTR(_object) \
    ((char*)_object)

#endif // SYS_TOOLS_H
