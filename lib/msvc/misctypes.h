#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

/* should be in some equivalent to <sys/types.h> */

// typedef __int8            int8_t;
// typedef __int16           int16_t; 
// typedef __int32           int32_t;
// typedef __int64           int64_t;
// typedef unsigned __int8   uint8_t;
// typedef unsigned __int16  uint16_t;
// typedef unsigned __int32  uint32_t;
// typedef unsigned __int64  uint64_t;

#ifndef __u_char_defined
// typedef unsigned char __u_char;
// typedef unsigned short int __u_short;
typedef unsigned int __u_int;
// typedef unsigned long int __u_long;

// typedef __u_char u_char;
// typedef __u_short u_short;
typedef __u_int u_int;
// typedef __u_long u_long;
#define __u_char_defined
#endif /* __u_char_defined */

#ifndef __daddr_t_defined
typedef char *__caddr_t;
// typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
#define __daddr_t_defined
#endif /* __daddr_t_defined */

#endif /* _SYS_TYPES_H */
