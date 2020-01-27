#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>

#define F_N CONFIG_FRAC_BITWIDTH
#define F_ONE (1 << F_N)
#define F_K (1 << (F_N - 1))
#define F_MASK (~(F_ONE | (F_ONE - 1)))

#ifdef CONFIG_TEST
    #define F_LIT(f) (fixed)f
    #ifndef CONFIG_RISCV
        #define F_TO_FLOAT(f) (float)f
    #endif
#else
    #define F_LIT(f) (fixed)(f * F_ONE)
    #ifndef CONFIG_RISCV
        #define F_TO_FLOAT(f) (float)(f) / F_ONE 
    #endif
#endif
#define F_ADD(a, b) a + b
#define F_SUB(a, b) a - b
#define F_MUL(a, b) f_mul(a, b)
#define F_DIV(a, b) f_div(a, b)
#define F_ROUND(a) f_round(a)
#define F_SQRT(a) f_sqrt(a)
#define F_SIN(a) f_sin(a)
#define F_COS(a) f_cos(a)
#define F_LT(a, b) a < b
#define F_LTE(a, b) a <= b
#define F_GT(a, b) a > b
#define F_GTE(a, b) a >= b
#define F_EQ(a, b) a == b
#define F_NEQ(a, b) a != b
#define F_MAX  f_max
#define F_SIGN_MASK sign_mask

#define F_PI F_LIT(3.1415926)
#define F_TWO_PI F_LIT(6.2831853)
#define F_HALF_PI F_LIT(1.5707963)

#if CONFIG_BITWIDTH == 8
typedef int8_t fixed;
#define f_max 127
#define sign_mask 0x80
#else
#define f_max 32767
#define sign_mask 0x8000
typedef int16_t fixed;
#endif

#define USE_LONG 1
// Comment out middle two lines for int arithmetic to work
static inline fixed f_mul(fixed a, fixed b) {
#ifdef CONFIG_TEST
    return a * b;
#else
		uint8_t test = (((a & F_SIGN_MASK) > 0) == ((b & F_SIGN_MASK) > 0));
		//printf("Test is :%i, a: %i, b:%i, %i, %i\r\n",test,a,b,a & F_SIGN_MASK, b &
		//F_SIGN_MASK);
    //signed int tmp = (signed int)a * (signed int) b;
#if USE_LONG
    signed long tmp = (signed long)a * (signed long)b;
		// Check for double overflow...
		if (tmp >> F_N > 65535) {
			//printf("Double overflow incoming!\r\n");
			//tmp = 1048576;
			// Just go straight to the ceiling
			tmp = F_MAX;
		}
		if (tmp >> F_N < -65535) {
			//printf("Double overflow mixed signs\r\n");
			//tmp = -1048576;
			tmp = -1 * F_MAX;
		}
#else
    signed int tmp = a * b;
#endif
		//printf("prod: %d, a=%i b=%i, sizeof=%i\r\n",tmp,a,b,sizeof(signed int));
    tmp += F_K;
    tmp >>= F_N;
		//printf("after shift: %d, fixed: %i\r\n",tmp, (fixed)tmp);
#if USE_LONG
		// Works for both + * + and - * -
		if (test && (fixed)tmp < 0) {
			tmp = F_MAX;
			//printf("Error! multiplication overflow %i * %i, new out=%i\r\n",a,b,tmp);
		}
		if (!test && (fixed)tmp > 0) {
			tmp = -1 *F_MAX;
			//printf("Error! Overflow with one pos and one neg: %i * %i, new out = %i\r\n",a,b,tmp);
		}
#endif
    return (fixed)tmp;
#endif
};

static inline fixed f_div(fixed a, fixed b) {
#ifdef CONFIG_TEST
    return a / b;
#else
    signed long tmp = a << F_N;
    if((tmp >= 0 && b >= 0) || (tmp < 0 && b < 0)) {
    	tmp += b / 2;
    } else {
    	tmp -= b / 2;
    }
    return (fixed)(tmp / b);
#endif
}

static inline fixed f_round(fixed a) {
	return a & (F_MASK);
}

fixed f_sqrt(fixed);
fixed f_sin(fixed); 
fixed f_cos(fixed); 

#endif
