/***************************************************************************
 * Copyright (C) 2012-2020 Andes Technology Corporation                    *
 * All rights reserved.                                                    *
 ***************************************************************************/
#ifndef _SHARE_H_
#define _SHARE_H_

#include <stdint.h>
#include <stdio.h>
#include <nds_intrinsic.h>
#include "nds_type.h"

char * get_version(void);

// Performance counter for RV64
uint64_t inst;
uint64_t cycle;

#define startPFM \
__nds__mtsr(0, NDS_MINSTRET); \
__nds__mtsr(0, NDS_MCYCLE);

#define stopPFM \
inst = __nds__mfsr(NDS_MINSTRET); \
cycle = __nds__mfsr(NDS_MCYCLE); \

__attribute__ ((noinline)) void readResult()
{
	uint32_t max_32b = 0xffffffff;
	if(inst > max_32b)
		printf("The inst count is 0x%08x%08x(hex)\n", (uint32_t)(inst>>32), (uint32_t)inst);
	else
		printf("The inst count is %u\n", (uint32_t)inst);

	if(inst > max_32b)
		printf("The cycle count is 0x%08x%08x(hex)\n", (uint32_t)(cycle>>32), (uint32_t)cycle);
	else
		printf("The cycle count is %u\n", (uint32_t)cycle);
}

#ifdef ENA_VEC_ISA
static inline void enable_fs()
{
    //Set MSTATUS.FS = 2 (clean) to avoid post FPU instructions raise exception in other simulator (Imperas).
    uint32_t csr_mstatus;
    const uint32_t mask_fs = (3L << 13);
    const uint32_t fs_clean = (2L << 13);

    csr_mstatus = __nds__read_and_clear_csr(mask_fs, NDS_MSTATUS);

    csr_mstatus |= fs_clean;
    __nds__write_csr(csr_mstatus, NDS_MSTATUS);
}

static inline void enable_vs()
{
    //Enable VPU
    uint32_t csr_mstatus;
    const uint32_t mask_vs = (3L << 9);
    const uint32_t vs_clean = (2L << 9);

    csr_mstatus = __nds__read_and_clear_csr(mask_vs, NDS_MSTATUS);

    csr_mstatus |=  vs_clean;
    __nds__write_csr(csr_mstatus, NDS_MSTATUS);
}
#define ENA_FS_VS enable_fs(); enable_vs();
#else
#define ENA_FS_VS
#endif

static inline uint32_t test_Fopen(char *fName, char *mode, FILE **fp)
{
    *fp = fopen(fName, mode);
    if(*fp == NULL)
    {
        printf("Error on openning %s!!!\n", fName);
        return 1;
    }
    return 0;
}

static inline uint32_t test_Fread(void *buf, uint32_t numByte, uint32_t bSize, FILE *fp, char *bufName)
{
    if (fread(buf, numByte, bSize, fp) != bSize)
    {
        printf("Error on reading %s!!!\n", bufName);
        return 1;
    }
    return 0;
}

void verify_buffer_f32(float * src1, float * src2, uint32_t count)
{
    uint32_t i, pass = 1;
    for(i=0; i<count; i++)
    {
        if(src1[i] != src2[i])
        {
        	pass = 0;
            break;
        }
    }

    if(pass)
		printf("accuracy checking ... PASS\n");
	else
		printf("accuracy checking ... FAIL !!!\n");
}

void verify_buffer_q7(q7_t * src1, q7_t * src2, uint32_t count)
{
    uint32_t i, pass = 1;
    for(i=0; i<count; i++)
    {
        if(src1[i] != src2[i])
        {
        	pass = 0;
            break;
        }
    }

    if(pass)
		printf("accuracy checking ... PASS\n");
	else
		printf("accuracy checking ... FAIL !!!\n");
}

static inline void print_version()
{
    char * version = get_version();
    printf("version: <%s>\n\n", version);
}
#define PRINT_VER print_version();

#endif
