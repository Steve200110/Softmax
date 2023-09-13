/***************************************************************************
 * Copyright (C) 2012-2020 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/
#include "nds_type.h"

//pure C algorithm
int v_f32_add_c(float32_t * src, const float32_t cst, uint32_t count)
{
    // src[i] = src[i] + cst;
    for (long itr = 0; itr < count; itr++ )
    {
        src[ itr ] = src[ itr ] + cst;
    }

    return 0;
}

//v-ext algorithm
int v_f32_div_v(float32_t * src, const float32_t cst, uint32_t count)
{
    // src[i] = src[i] + cst;
    long avl = count;
    long vl;
    __asm__ __volatile__ ("vsetvli %[out], %[avl], " "e32" ", " "m8" ", " "tu" ", " "mu" "\n" : [out] "=r" (vl) : [avl] "r" (avl));

    long cnt_avl = vl << 1;
    float32_t *in1 = src;
    float32_t *in2 = src + vl;;
    while (avl != 0)
    {
        __asm__ __volatile__("vle32.v" " " "v0" ", %[ptr]\n" :: [ptr] "A" (*(in1)));
        __asm__ __volatile__("vle32.v" " " "v8" ", %[ptr]\n" :: [ptr] "A" (*(in2)));
        __asm__ __volatile__("vfdiv.vf" " " "v0" ", " "v0" ", %[rs1]\n" :: [rs1] "f" (cst));
        __asm__ __volatile__("vfdiv.vf" " " "v8" ", " "v8" ", %[rs1]\n" :: [rs1] "f" (cst));
        __asm__ __volatile__("vse32.v" " " "v0" ", %[ptr]\n" :: [ptr] "A" (*(in1)));
        __asm__ __volatile__("vse32.v" " " "v8" ", %[ptr]\n" :: [ptr] "A" (*(in2)));
        in1 += cnt_avl;
        in2 += cnt_avl;
        avl -= cnt_avl;
    }

    return 0;
}
int v_f32_add_v(float32_t * src, const float32_t cst, uint32_t count)
{
    // src[i] = src[i] + cst;
    long avl = count;
    long vl;
    __asm__ __volatile__ ("vsetvli %[out], %[avl], " "e32" ", " "m8" ", " "tu" ", " "mu" "\n" : [out] "=r" (vl) : [avl] "r" (avl));

    long cnt_avl = vl << 1;
    float32_t *in1 = src;
    float32_t *in2 = src + vl;;
    while (avl != 0)
    {
        __asm__ __volatile__("vle32.v" " " "v0" ", %[ptr]\n" :: [ptr] "A" (*(in1)));
        __asm__ __volatile__("vle32.v" " " "v8" ", %[ptr]\n" :: [ptr] "A" (*(in2)));
        __asm__ __volatile__("vfadd.vf" " " "v0" ", " "v0" ", %[rs1]\n" :: [rs1] "f" (cst));
        __asm__ __volatile__("vfadd.vf" " " "v8" ", " "v8" ", %[rs1]\n" :: [rs1] "f" (cst));
        __asm__ __volatile__("vse32.v" " " "v0" ", %[ptr]\n" :: [ptr] "A" (*(in1)));
        __asm__ __volatile__("vse32.v" " " "v8" ", %[ptr]\n" :: [ptr] "A" (*(in2)));
        in1 += cnt_avl;
        in2 += cnt_avl;
        avl -= cnt_avl;
    }

    return 0;
}
