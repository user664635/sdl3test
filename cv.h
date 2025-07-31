#pragma once
#include "def.h"
#ifdef __cplusplus
extern "C"{
#endif

void cv_init();
void cv_run();
void cv_pixel(u8 *, u32, u32);

#ifdef __cplusplus
}
#endif
