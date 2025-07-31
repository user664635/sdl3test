#pragma once
#include "def.h"
#ifdef __cplusplus
extern "C" {
#endif

void cv_init();
void cv_run();
vec2 cv_pixel(u8 *, u8 **);

#ifdef __cplusplus
}
#endif
