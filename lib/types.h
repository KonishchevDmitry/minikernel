#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stddef.h>

typedef __INT8_TYPE__   i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;
typedef __INT64_TYPE__ i64;

typedef __UINT8_TYPE__   u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

typedef __SIZE_TYPE__   size_t;
typedef __INTPTR_TYPE__ ssize_t;

typedef unsigned long register_t;
typedef i64 time_ms_t;

typedef const char* error;

#define UINT32_MAX 0xFFFFFFFF
#define __must_check __attribute__((warn_unused_result))

#endif