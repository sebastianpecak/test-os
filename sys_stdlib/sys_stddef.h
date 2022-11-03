#ifndef SYS_STDDEF_H
#define SYS_STDDEF_H

#define NULL ((void*) 0)
#define NOP (do {} while (0))

#define TYPE_CAST(_type, _object) \
    ((_type)(_object))

#define TYPE_CAST_CONST(_type, _object) \
    ((const _type)(_object))

#define LOWER_16_BITS(_uint32) \
    ((uint16_t)(((uint32_t)(_uint32)) & 0xFFFFUL))
    //((uint16_t)(_uint32) & 0xFFFFUL)

#define UPPER_16_BITS(_uint32) \
    ((uint16_t)((((uint32_t)(_uint32)) >> 16) & 0xFFFFUL))
    //(((uint16_t)(_uint32) >> 16) & 0xFFFFUL)

#define LOWER_8_BITS(_uint16) \
    ((uint8_t)(_uint16) & 0xFF)

#define UPPER_8_BITS(_uint16) \
    (((uint8_t)(_uint16) >> 8) & 0xFF)

#endif // SYS_STDDEF_H
