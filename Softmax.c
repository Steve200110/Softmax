//============================================================================
// Name        : softmax.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, ANSI-style
//============================================================================

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <nds_intrinsic.h>
#include "nds_type.h"
#include "math.h"
#include "v_f32_add.h"
#include <riscv_vector.h>
#include "time.h"
#include "common.h"
#define number 4;
int tmd=0;
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
/////vector vadd
void vadd0(uint32_t *a, const uint32_t *b, const uint32_t *c, size_t n) {
  while (n > 0) {
    size_t vl = vsetvl_e32m8(n);
    vuint32m8_t vb = vle32_v_u32m8(b, vl);
    vuint32m8_t vc = vle32_v_u32m8(c, vl);
    vuint32m8_t va = vadd(vb, vc, vl);
    vse32(a, va, vl);
    a += vl;
    b += vl;
    c += vl;
    n -= vl;
  }
}

void vadd1(float32_t *a, const float32_t *b, const float32_t *c, size_t n) {
  while (n > 0) {
    size_t vl = vsetvl_e32m8(n);
    vfloat32m8_t vb = vle32_v_f32m8(b, vl);
    vfloat32m8_t vc = vle32_v_f32m8(c, vl);
    vfloat32m8_t va = vfadd(vb, vc, vl);
    vse32(a, va, vl);
    a += vl;
    b += vl;
    c += vl;
    n -= vl;
  }
}

////////////
float v[128]={0};
float ex[128]={0};

//float srcf32_1[4] __attribute__ ((aligned(32)));
float srcf32_1[128]={0};
float srcf32_2=0;
float result[128];
float result1[128];
float resultt[128];
//ÀË¬d¿ù»~
void check(int i){
	double num=0;
	if(i==0){
		for(int i=0;i<128;i++){
			num+=result[i];
		}
	}else if(i==1){
		for(int i=0;i<128;i++){
			num+=srcf32_1[i];
		}
	}else{
		for(int i=0;i<128;i++){
			num+=resultt[i];
		}
	}

	if(num==1){
		printf("Check %d Pass\n",i);
	}else{
		printf("NO, your number is %.10f\n",num);
	}
}

//­ì¥»softmax
void softmax(){
	float total=0;
	for(int i=0;i<128;i++){
		ex[i]=exp(3);
	}
	for(int i=0;i<128;i++){
		total+=ex[i];
	}
	for(int i=0;i<128;i++){
		result[i]=ex[i]/total;
	}
}
// ex(i)/sigma(ex(i))
float exp1(float x) {
    x = 1.0 + x / 256.0;
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    return x;
}
double fastPrecisePow(double a, double b) {
  // calculate approximation with fraction of the exponent
  int e = (int) b;
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)((b - e) * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;

  // exponentiation by squaring with the exponent's integer part
  // double r = u.d makes everything much slower, not sure why
  double r = 1.0;
  while (e) {
    if (e & 1) {
      r *= a;
    }
    a *= a;
    e >>= 1;
  }

  return r * u.d;
}
float expfg(double x) {
    x = 1.0 + x / 1024;
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x; x *= x; x *= x;
    x *= x; x *= x;
    return x;
}
void softmax1(){
	for(int i=0;i<128;i++){
		srcf32_1[i]=exp(3);
	}

	//v_f32_add_v(srcf32_1, k, 128);
    //float
	float a0[16];
	float a1[16];
	float a2[16];
	float a3[16];
	float a4[16];
	float a5[16];
	float a6[16];
	float a7[16];
	float a00[16];
	float a11[16];
	float a22[16];
	float a33[16];
	float a000[16];
	float a111[16];
	float fin[16];
	for(int i=0;i<16;i++){
		a0[i]=ex[i];
		a1[i]=ex[(16*1)+i];
		a2[i]=ex[(16*2)+i];
		a3[i]=ex[(16*3)+i];
		a4[i]=ex[(16*4)+i];
		a5[i]=ex[(16*5)+i];
		a6[i]=ex[(16*6)+i];
		a7[i]=ex[(16*7)+i];
	}

	vadd1(a00, a0, a1, 16);

	vadd1(a11, a2, a3, 16);
	vadd1(a22, a4, a5, 16);
	vadd1(a33, a6, a7, 16);

	vadd1(a000,a00,a11,16);
	vadd1(a111,a22,a33,16);
    vadd1(fin,a000,a111,16);

    for(int i=0;i<16;i++){
    	srcf32_2+=fin[i];
    }

    v_f32_div_v(srcf32_1, srcf32_2, 128);

}

int main(){
	/*
	for(int i=0;i<128;i++){
		ex[i]=exp(3);
	}
    for(int i=0;i<128;i++){
    	srcf32_1[i]=ex[i];
    }
	*/
	uint64_t cycle_c=0,cycle_v=0;//cycle_t=0;

    startPFM;
    softmax();
    stopPFM;
    readResult();
    cycle_c = cycle;


    startPFM;
    softmax1();
    stopPFM;
    readResult();
    cycle_v = cycle;
/*
    startPFM;
    softmaxt();
    stopPFM;
    readResult();
    cycle_t = cycle;
    printf("%lf %lf\n",ex[0],v[0]);
    printf("Speed up : %fx\n",(float)cycle_c/cycle_v);
*/
	check(0);
	check(1);
    printf("Speed up : %fx\n",(float)cycle_c/cycle_v);
	//check(2);
    return 0;
}
