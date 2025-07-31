#pragma once
#include <stdint.h>

typedef float f32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef f32 [[clang::matrix_type(3, 3)]] mat3;
typedef f32 [[clang::matrix_type(3, 1)]] mat3x1;
typedef f32 [[clang::matrix_type(4, 4)]] mat4;
typedef f32 [[clang::ext_vector_type(2)]] vec2;
typedef f32 [[clang::ext_vector_type(3)]] vec3;
typedef f32 [[clang::ext_vector_type(4)]] vec4;
