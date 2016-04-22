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

void montgomery_multiplication(unsigned char *res, const unsigned char *x, const unsigned char *y,
                               const unsigned char *m, const unsigned char *m_ , const unsigned int size);

void montgomery_exponentiation(unsigned char *res, const unsigned char *x, const unsigned char *e,
                               const unsigned int e_size, const unsigned char *m, const unsigned int m_size, const unsigned char *m_,
                               const unsigned char *R_mod_m, const unsigned char *R2_mod_m);

void calculate_rmodn(unsigned char* rmodn,const unsigned char *m, const unsigned int m_size);

void calculate_r2modn(unsigned char* r2modn,const unsigned char *m, const unsigned int m_size);

void calculate_n_inv(int a, unsigned char *n_);

void mod_reduction(unsigned char* out, unsigned char* in, const unsigned char *m, const unsigned int m_size);

char is_save_message(const unsigned char *in1, const unsigned char *in2, const unsigned int size);

#endif
