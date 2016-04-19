/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * modular_arithmetic.c
 *
 * This file contains all the function needed for the RSA-algorithm and DH-calculations
 * all numbers are in little endian for easier loop counters
 */

#include <string.h>
#include "global.h"


/*
 * multi-precision multiplication defined as in chapter 14 in the handbook of applied cryptography
 *
 * res          pointer to res array with size >= SIZE1*SIZE2
 * in1, in2     pointer to input arrays of size resp SIZE1 and SIZE2
 */
void mp_multiplication(unsigned char *res, const unsigned char *in1, const unsigned int SIZE1, const unsigned char *in2, const unsigned int SIZE2)
{
	unsigned char xi;
	unsigned char yi;
	unsigned char u;
	unsigned char v;
	unsigned int i;
	unsigned int j;
	unsigned char c;

	for (i=0; i<SIZE1; i++){
		c=0;
		xi = in1[i];
		for (j=0; j<SIZE2; j++){
			yi = in2[j];
			u = (res[i+j]+xi*yi+c)>>8;
			v = res[i+j]+xi*yi+c;
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
void mp_subtraction(unsigned char *res, const unsigned char *in1, const unsigned char *in2, const unsigned int size)
{
	unsigned char xi;
	unsigned char yi;
	unsigned int i;
	unsigned char c;

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
void mp_addition_special2(unsigned char *res, unsigned char *carrier, const unsigned char *in1, const unsigned char *x,
                          const unsigned char *y, const unsigned char *u, const unsigned char *m, const unsigned int size)
{
	unsigned int i;
	unsigned int c;

    c = (in1[0]+x[0]*y[0]+u[0]*m[0]) >> 8;
	for (i=1; i<size; i++){
        res[i-1] = in1[i] + x[0]*y[i] + u[0]*m[i] + c;
        c = (in1[i] + x[0]*y[i] + u[0]*m[i] + c)>> 8;
	}
    res[size-1] = c+carrier[0];
    carrier[0] = (carrier[0]+c)>> 8;
}

/*
 * computes res = res = (x[0]*y + u[0]*m)/b = (A+B+C)/b for the montgomery-multiplication
 *
 * res              pointer to res array of size size
 * carrier          pointer to carrier array of size 1
 * x, y,u,m         pointer to input arrays of size size
 */
void mp_addition_special1(unsigned char *res, unsigned char *carrier, const unsigned char *x, const unsigned char *y,
                          const unsigned char *u, const unsigned char *m, const unsigned int size)
{
	unsigned int i;
	unsigned int c;

    c = (x[0]*y[0]+u[0]*m[0]) >> 8;
	for (i=1; i<size; i++){
        res[i-1] = x[0]*y[i] + u[0]*m[i] + c;
        c = (x[0]*y[i] + u[0]*m[i] + c)>> 8;
	}
	res[size-1] = c;
	carrier[0] = c>>8;

}

/*
 * check if in1[size] is bigger then in2[size]
 * return 1 in in1>=in2, else returns 0
 *
 * in1, in2         pointer to input arrays of size size
 */
char is_bigger(const unsigned char *in1, const unsigned char *in2, const unsigned int size){
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

/*
 * montgomery-multiplication defined as in chapter 14 in the handbook of applied cryptography
 *
 * res              pointer to res array of size size
 * x, y,m           pointer to input arrays of size size
 * m_               pointer to input array of size 1
 */
void montgomery_multiplication(unsigned char *res, const unsigned char *x, const unsigned char *y,
                               const unsigned char *m, const unsigned char *m_ , const unsigned int size)
{
	unsigned int i;
	unsigned char u;
	unsigned char carrier;


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
void montgomery_exponentiation(unsigned char *res, const unsigned char *x, const unsigned char *e,
                               const unsigned int e_size, const unsigned char *m, const unsigned int m_size, const unsigned char *m_,
                               const unsigned char *R_mod_m, const unsigned char *R2_mod_m)
{   unsigned char x_[MAX_MODULUS_SIZE], temp1[MAX_MODULUS_SIZE];
    signed int i= e_size-1;
    signed int j=7;
    unsigned char bits;

    montgomery_multiplication(x_,x,R2_mod_m,m,m_,m_size);
    memcpy(temp1,R_mod_m,m_size);

    bits = e[i];
    while(((bits>>j)&0x01)==0){
        j = j-1;
    };

    while(j >=0 ){
        montgomery_multiplication(res,temp1,temp1,m,m_,m_size);
        memcpy(temp1,res,m_size);
        if(((bits>>j)&0x01)==1){
            montgomery_multiplication(temp1,res,x_,m,m_,m_size);
            memcpy(res,temp1,m_size);
        };
        j = j-1;
    };

    for (i=e_size-2; i>=0; i--){
        bits = e[i];
        for(j=7; j>=0; j--){
            montgomery_multiplication(res,temp1,temp1,m,m_,m_size);
            memcpy(temp1,res,m_size);
            if(((bits>>j)&0x01)==1){
                montgomery_multiplication(temp1,res,x_,m,m_,m_size);
                memcpy(res,temp1,m_size);
            };
        };
    }

    memset(x_,0x00, m_size);
    memset(x_,0x01,1);
    memcpy(temp1,res,m_size);
    montgomery_multiplication(res,temp1,x_,m,m_,m_size);
}

/*
 *check of in1 is not equal to 0,1 or in2-1. Otherwise the DH protocol will be corrupted
 *
 * in1, in2         pointer to input arrays of size size
 */
char is_save_message(const unsigned char *in1, const unsigned char *in2, const unsigned int size){
	unsigned int i;
	unsigned char temp1[MAX_PRIME_SIZE];

	memset(temp1,0x01,1);
	memset(temp1+1,0x00,size-1);
	mp_subtraction(temp1, in2, temp1, size);

	if (in1[0]==0x00){
        i = 1;
        while (in1[i]==0x00 && i <= size-1){
            i++;
        }
        if (i == size) {return 0;}
	}
	if (in1[0]==0x01){
        i = 1;
        while (in1[i]==0x00 && i <= size-1){
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
void calculate_n_inv(int a, unsigned char *n_)
{
	int b=0x100, t, q;
	int x0 = 0, x1 = 1;
	while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if (x1 < 0) x1 += 0x100;
	t = 0x100 - x1;
	n_[0] = t;
}
/*
 * calculates 256^(m_size) mod m
 *
 * rmodn    pointer to output array of size m_size
 * m        pointer to input array of m_size
 */
void calculate_rmodn(unsigned char* rmodn,const unsigned char *m, const unsigned int m_size)
{
    unsigned char temp[MAX_MODULUS_SIZE];

    memset(temp,0x00,m_size);
    mp_subtraction(rmodn,temp,m,m_size);
    while(is_bigger(rmodn,m,m_size))
    {
        mp_subtraction(rmodn,rmodn,m,m_size);
    }
};

/*
 * calculate y<<8*positions (is the same as y*(256^positions))
 */

void shift(unsigned char *res,const unsigned char *y, const unsigned int y_size, const unsigned int positions)
{
    unsigned int i;

    memset(res,0x00,y_size+positions);

    for(i=0;i<y_size;i++)
    {
        res[i+positions] = y[i];
    }
}

/*
 * calculates (256^(m_size))^2 mod m
 */
void calculate_r2modn(unsigned char* r2modn,const unsigned char *m, const unsigned int m_size)
{
    unsigned char temp1[2*MAX_MODULUS_SIZE+1];
    unsigned char temp2[2*MAX_MODULUS_SIZE+1];
    unsigned char temp3[2*MAX_MODULUS_SIZE+1];
    unsigned int i;
    unsigned char div;

    memset(temp1,0x00,2*m_size);
    memset(temp1+2*m_size,0x01,1);
    memset(temp2,0x00,2*m_size+1);
    shift(temp2,m,m_size,m_size+1);
    if(is_bigger(temp1,temp2, 2*m_size+1)){
            mp_subtraction(temp1,temp1,temp2,2*m_size+1);
    }

    for(i=2*m_size;i>=m_size; i--)
    {
        if (temp1[i]==m[m_size-1]){
                div = 0xFF;
        }
        else {
                div = (temp1[i]*256+temp1[i-1])/m[m_size-1];
        }
        while (div*(m[m_size-1]*256 + m[m_size-2])>temp1[i]*0x010000+ temp1[i-1]*256 + temp1[i-2]){
                div = div-1;
        }
        memset(temp2,0x00,2*m_size+1);
        mp_multiplication(temp2,m,m_size,&div,1);
        memset(temp3,0x00,2*m_size+1);
        shift(temp3,temp2,m_size+1,i-m_size);
        if (is_bigger(temp1,temp3, 2*m_size+1)){
            mp_subtraction(temp1,temp1, temp3,2*m_size+1);
        }
        else{
                div = div -1;
                memset(temp2,0x00,2*m_size+1);
                mp_multiplication(temp2,m,m_size,&div,1);
                memset(temp3,0x00,2*m_size+1);
                shift(temp3,temp2,m_size+1,i-m_size);
                mp_subtraction(temp1,temp1,temp3,2*m_size+1);
        }
    }
    memcpy(r2modn,temp1,m_size);
};

/*
 * calculates in mod m
 * assumes that in is not bigger then m_size characters
 */

void mod_reduction(unsigned char* out,unsigned char* in, const unsigned char *m, const unsigned int m_size)
{
   out = in;
    while(is_bigger(out,m,m_size))
    {
        mp_subtraction(out,out,m,m_size);
    }
};

