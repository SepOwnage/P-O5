/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * modular_arithmetic_short.c
 *
 * This file contains all the function needed for the RSA-algorithm and DH-calculations
 * all numbers are in little endian for easier loop counters
 */

#include <string.h>
#include "global.h"
#include "util.h"

/*
 * multi-precision multiplication defined as in chapter 14 in the handbook of applied cryptography
 *
 * res          pointer to res array with size >= SIZE1*SIZE2
 * in1, in2     pointer to input arrays of size resp SIZE1 and SIZE2
 */

void mp_multiplication(void *resin, const void *in1in, const unsigned int SIZE1, const void *in2in, const unsigned int SIZE2)
{   unsigned short *res = resin;
    const unsigned short *in1 = in1in;
    const unsigned short *in2 = in2in;

	unsigned short xi;
	unsigned short yi;
	unsigned short u;
	unsigned short v;
	unsigned int i;
	unsigned int j;
	unsigned short c;

	for (i=0; i<SIZE1; i++){
		c=0;
		xi = in1[i];
		for (j=0; j<SIZE2; j++){
			yi = in2[j];
			u = (res[i+j]+xi*yi+c)>>16;
			v = (unsigned short) (res[i+j]+xi*yi+c);
			res[i+j] = v;
			c = u;
			}
		res[i+SIZE2]=u;
    }
}

/*
 * multi-precision subtraction defined as in chapter 14 in the handbook of applied cryptography
 *
 * res          pointer to res array with size >= SIZE
 * in1, in2     pointer to input arrays of size resp SIZE with in1>= in2
 */
void mp_subtraction(void *resin, const void *in1in, const void *in2in, const unsigned int size)
{   unsigned short *res = resin;
    const unsigned short *in1 = in1in;
    const unsigned short *in2 = in2in;

	unsigned short xi;
	unsigned short yi;
	unsigned int i;
	unsigned short c;

	c=0;
	for (i=0; i<size; i++){
        xi = in1[i];
        yi = in2[i];
        res[i] = xi - yi - c;
        if (xi - yi - c >=0){
                c = 0;
        }
        else {
                c=1;
        }
	}

}

/*
 * computes res = (in1 + x[0]*y + u[0]*m)/b = (A+B+C)/b for the montgomery-multiplication
 *
 * res              pointer to res array of size size
 * carrier          pointer to carrier array of size 1
 * in1,x,y,u,m      pointer to input arrays of size size
 */
inline void mp_addition_special2(void *resin, void * /*restrict*/ carrierin, const void *in1in, const void * /*restrict*/ xin,
                          const void * /*restrict*/ yin, const void * /*restrict*/ uin, const void * /*restrict*/ min, const unsigned int size)

{   unsigned short * res = resin;
    unsigned short * /*restrict*/ carrier = carrierin;
    const unsigned short * in1 = in1in;
    const unsigned short * /*restrict*/ x = xin;
    const unsigned short * /*restrict*/ y = yin;
    const unsigned short * /*restrict*/ u = uin;
    const unsigned short * /*restrict*/ m = min;

	unsigned int i;
	unsigned int c;
	unsigned short * /*restrict*/ res/*restrict*/ed = res-1;
	unsigned long long temp;
    unsigned int temp1;
    unsigned int temp2;

	temp1 = x[0]*y[0] + in1[0];
    temp2 = u[0]*m[0];
    temp = (unsigned long long) (temp1) + (unsigned long long) (temp2);
    c = temp>>16;

	for (i=1; i<size; i++){
        temp1 = x[0]*y[i] + in1[i];
        temp2 = u[0]*m[i];
        temp = (unsigned long long) (temp1) + (unsigned long long) (temp2) + (unsigned long long) (c);
		res/*restrict*/ed[i] = temp;
        c = temp>> 16;
	}
    temp1 = carrier[0];
	temp = (unsigned long long) (c) + (unsigned long long) (temp1);
    res[size-1] = temp;
    carrier[0] = temp>> 16;
}

/*
 * computes res = res = (x[0]*y + u[0]*m)/b = (A+B+C)/b for the montgomery-multiplication
 *
 * res              pointer to res array of size size
 * carrier          pointer to carrier array of size 1
 * x, y,u,m         pointer to input arrays of size size
 */
void mp_addition_special1(void *resin, void *carrierin, const void *xin, const void *yin,
                          const void *uin, const void*min, const unsigned int size)
{   unsigned short *res = resin;
    unsigned short *carrier = carrierin;
    const unsigned short *x = xin;
    const unsigned short *y = yin;
    const unsigned short *u = uin;
    const unsigned short *m = min;

	unsigned int i;
	unsigned int c;
    unsigned long long temp;
    unsigned int temp1;
    unsigned int temp2;

    temp1 = x[0]*y[0];
    temp2 = u[0]*m[0];
    temp = (unsigned long long) (temp1) + (unsigned long long) (temp2);
    c = temp>>16;

	for (i=1; i<size; i++){
        temp1 = x[0]*y[i];
        temp2 = u[0]*m[i];
        temp = (unsigned long long) (temp1) + (unsigned long long) (temp2) + (unsigned long long) (c);
		res[i-1] = temp;
        c = temp>> 16;
	}
	res[size-1] = c;
	carrier[0] = c>>16;

}

/*
 * check if in1[size] is bigger then in2[size]
 * return 1 in in1>=in2, else returns 0
 *
 * in1, in2         pointer to input arrays of size size
 */
char is_bigger(const void *in1in, const void *in2in, const unsigned int size){
    const unsigned short *in1 = in1in;
    const unsigned short *in2 = in2in;

	unsigned int i;

	i = size -1;
	while ((in1[i] == in2[i]) && i > 0 ){
        i=i-1;
        }
	if (in1[i]>=in2[i]){
            return 1;
    }
    else {
            return 0;
    }
}

char is_strict_bigger(const void *in1in, const void *in2in, const unsigned int size){
    const unsigned short *in1 = in1in;
    const unsigned short *in2 = in2in;

	unsigned int i;

	i = size -1;
	while ((in1[i] == in2[i]) && i > 0 ){
        i=i-1;
        }
	if (in1[i]>in2[i]){
            return 1;
    }
    else {
            return 0;
    }
}

/*
 * montgomery-multiplication defined as in chapter 14 in the handbook of applied cryptography
 *
 * res              pointer to res array of size size
 * x, y,m           pointer to input arrays of size size
 * m_               pointer to input array of size 1
 */
void montgomery_multiplication(void *resin, const void *xin, const void *yin,
                               const void *min, const void *m_in , const unsigned int size)
{   unsigned short *res = resin;
    const unsigned short *x = xin;
    const unsigned short *y = yin;
    const unsigned short *m = min;
    const unsigned short *m_ = m_in;

	unsigned int i;
	unsigned short u;
	unsigned short carrier;


	u = x[0]*y[0]*m_[0];
	mp_addition_special1(res,&carrier,x,y,&u,m,size);

	for (i=1; i<size; i++){
       u = (res[0]+ x[i]*y[0])*m_[0];
       mp_addition_special2(res,&carrier, res,x+i,y,&u,m, size);
	};
	if (is_bigger(res,m, size) || carrier != 0){
            mp_subtraction(res,res,m, size);
    };
}

/*
 * montgomery-exponentiation defined as in chapter 14 in the handbook of applied cryptography
 *
 * res               pointer to res array of size m_size
 * x,m               pointer to input arrays of size m_size
 * e                 pointer to input arrays of size e_size
 * R_mod_m, R2_mod_m pointer to input arrays of size m_size
 * m_                pointer to input array of size 1
 */
void montgomery_exponentiation(void *resin, const void *xin, const void *ein,
                               const unsigned int e_size, const void *min, const unsigned int m_size, const void *m_in,
                               const void *R_mod_min, const void *R2_mod_min)
{   unsigned short *res = resin;
    const unsigned short *x = xin;
    const unsigned short *e = ein;
    const unsigned short *m = min;
    const unsigned short *m_ = m_in;
    const unsigned short *R_mod_m = R_mod_min;
    const unsigned short *R2_mod_m = R2_mod_min;

    unsigned short x_[MAX_MODULUS_SIZE], temp1[MAX_MODULUS_SIZE];
    signed int i= e_size-1;
    signed int j=15;
    unsigned short bits;
    montgomery_multiplication(x_,x,R2_mod_m,m,m_,m_size);
    memcpy(temp1,R_mod_m,2*m_size);

    bits = e[i];
    while(((bits>>j)&0x0001)==0){
        j = j-1;
    };

    while(j >=0 ){
        montgomery_multiplication(res,temp1,temp1,m,m_,m_size);
        memcpy(temp1,res,2*m_size);
        if(((bits>>j)&0x0001)==1){
            montgomery_multiplication(temp1,res,x_,m,m_,m_size);
            memcpy(res,temp1,2*m_size);
        };
        j = j-1;
    };

    for (i=e_size-2; i>=0; i--){
        bits = e[i];
        for(j=15; j>=0; j--){
            montgomery_multiplication(res,temp1,temp1,m,m_,m_size);
            memcpy(temp1,res,2*m_size);
            if(((bits>>j)&0x0001)==1){
                montgomery_multiplication(temp1,res,x_,m,m_,m_size);
                memcpy(res,temp1,2*m_size);
            };
        };
    }

    memset_short(x_,0x0000, m_size);
    memset_short(x_,0x0001,1);
    memcpy(temp1,res,2*m_size);
    montgomery_multiplication(res,temp1,x_,m,m_,m_size);
}

/*
 *check of in1 is not equal to 0,1 or in2-1. Otherwise the DH protocol will be corrupted
 *
 * in1, in2         pointer to input arrays of size size
 */
char is_save_message(const void *in1in, const void *in2in, const unsigned int size){
    const unsigned short *in1 = in1in;
    const unsigned short *in2 = in2in;

	unsigned int i;
	unsigned short temp1[MAX_PRIME_SIZE];

	memset_short(temp1,0x01,1);
	memset_short(temp1+1,0x00,size-1);
	mp_subtraction(temp1, in2, temp1, size);

	if (in1[0]==0x0000){
        i = 1;
        while (in1[i]==0x0000 && i <= size-1){
            i++;
        }
        if (i == size) {return 0;}
	}
	if (in1[0]==0x0001){
        i = 1;
        while (in1[i]==0x0000 && i <= size-1){
             i++;
        }
        if (i == size) {return 0;}
	}
	if (in1[0]==temp1[0]){
	    i = 1;
        while (in1[i]==temp1[i] && i <= size-1){
            i++;
        }
        if (i == size) {return 0;}
	}
    return 1;
}

/*
 * calculates the modular inverse of -a mod b = 256
 *
 * a    input integer a
 * n_   pointer to output array of size 1
 */
void calculate_n_inv(int a, void *n_in)
{	unsigned short *n_= n_in;

    int b=0x10000, t, q;
	int x0 = 0, x1 = 1;
	while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if (x1 < 0) x1 += 0x10000;
	t = 0x10000 - x1;
	n_[0] = t;
}
/*
 * calculates 256^(m_size) mod m
 *
 * rmodn    pointer to output array of size m_size
 * m        pointer to input array of m_size
 */
void calculate_rmodn(void* rmodnin,const void *min, const int m_size)
{   unsigned short *rmodn = rmodnin;
    const unsigned short *m = min;

    unsigned short temp[MAX_MODULUS_SIZE];

    memset(temp,0x00,2*m_size);
    mp_subtraction(rmodn,temp,m,m_size);
    while(is_bigger(rmodn,m,m_size))
    {
        mp_subtraction(rmodn,rmodn,m,m_size);
    }
}

/*
 * calculate y<<8*positions (is the same as y*(256^positions))
 */
void shift(void *resin,const void *yin, const unsigned int y_size, const unsigned int positions)
{   unsigned short *res = resin;
    const unsigned short *y = yin;

    unsigned int i;

    memset_short(res,0x0000,y_size+positions);

    for(i=0;i<y_size;i++)
    {
        res[i+positions] = y[i];
    }
}

char test(const unsigned short div,const unsigned short *m, const unsigned short *temp, unsigned int i, const unsigned int m_size){
    unsigned short temp1[3] = {0x0000,0x0000,0x0000};
    mp_multiplication(temp1,m + m_size -2,2,&div,1);
    return is_strict_bigger(temp1,temp+i-2,3);

}

/*
 * calculates (256^(m_size))^2 mod m
 */
void calculate_r2modn(void* r2modnin,const void *min, const unsigned int m_size)
{   unsigned short *r2modn = r2modnin;
    const unsigned short *m = min;

    unsigned short temp1[2*MAX_MODULUS_SIZE+1];
    unsigned short temp2[2*MAX_MODULUS_SIZE+1];
    unsigned short temp3[2*MAX_MODULUS_SIZE+1];
    unsigned int i;
    unsigned int div_int;
    unsigned short div;

    memset_short(temp1,0x0000,2*m_size);
    memset_short(temp1+2*m_size,0x0001,1);
    memset_short(temp2,0x0000,2*m_size+1);
    shift(temp2,m,m_size,m_size+1);

    for(i=2*m_size;i>=m_size; i--)
    {
        if (temp1[i]==m[m_size-1]){
                div = 0xFFFF;
        }
        else {
                div_int = (temp1[i]*256*256+temp1[i-1]);
                div = div_int/m[m_size-1];

        }
        while (test(div,m,temp1,i,m_size)){
                div = div-1;
        }
        memset_short(temp2,0x0000,2*m_size+1);
        mp_multiplication(temp2,m,m_size,&div,1);
        memset_short(temp3,0x0000,2*m_size+1);
        shift(temp3,temp2,m_size+1,i-m_size);
        if (is_bigger(temp1,temp3, 2*m_size+1)){
            mp_subtraction(temp1,temp1, temp3,2*m_size+1);
        }
        else{
                div = div -1;
                memset_short(temp2,0x0000,2*m_size+1);
                mp_multiplication(temp2,m,m_size,&div,1);
                memset_short(temp3,0x0000,2*m_size+1);
                shift(temp3,temp2,m_size+1,i-m_size);
                mp_subtraction(temp1,temp1,temp3,2*m_size+1);
        }
    }
    memcpy(r2modn,temp1,2*m_size);
}

/*
 * calculates in mod m
 * assumes that in is not bigger then m_size characters
 */

void mod_reduction(void* outin,void* inin, const void *min, const unsigned int m_size)
{  unsigned short *out = outin;
   unsigned short *in = inin;
   const unsigned short *m = min;

   out = in;
    while(is_bigger(out,m,m_size))
    {
        mp_subtraction(out,out,m,m_size);
    }
}
