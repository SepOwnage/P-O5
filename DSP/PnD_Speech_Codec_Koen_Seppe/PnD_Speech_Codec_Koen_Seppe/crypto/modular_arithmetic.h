/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * modular_arithmetic.h
 *
 * This file contains all the function needed for the RSA-algorithm and DH-calculations
 * all numbers are in little endian for easier loop counters
 */

#ifndef __MODULAR_ARITHMETIC_H
#define __MODULAR_ARITHMETIC_H

void montgomery_multiplication(void *resin, const void *xin, const void *yin,
                               const void *min, const void *m_in , const unsigned int size);

void montgomery_exponentiation(void *resin, const void *xin, const void *ein,
                               const unsigned int e_size, const void *min, const unsigned int m_size, const void *m_in,
                               const void *R_mod_min, const void *R2_mod_min);

void calculate_rmodn(void* rmodnin,const void *min, const int m_size);

void calculate_r2modn(void* r2modnin,const void *min, const unsigned int m_size);

void calculate_n_inv(int a, void *n_in);

void mod_reduction(void* outin,void* inin, const void *min, const unsigned int m_size);

char is_save_message(const void *in1in, const void *in2in, const unsigned int size);

#endif
