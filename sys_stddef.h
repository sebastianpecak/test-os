#ifndef SYS_STDDEF_H
#define SYS_STDDEF_H

#define NULL ((void*) 0)
#define NOP (do {} while (0))

#define TYPE_CAST(_type, _object) \
    ((_type)(_object))

#endif // SYS_STDDEF_H
