/* 
 * MIT License
 *
 * Copyright (c) 2024 Emrah Karagoz, Pakize Sanal, Abhraneel Dutta, Edoardo Persichetti
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GF2X_H
#define GF2X_H

#include <stdint.h>
#include <assert.h>

#include "config.h"

// Determines if an argument/variable is provided as input (IN) 
// or output (OUT) to the function, or updated in-place (INPLACE)
#define IN 
#define OUT 
#define INPLACE 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * poly_t : Definition of a Polynomial                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
typedef struct {
    int         deg;            // Degree of the Polynomial
    int         size64;         // Size64 of the Polynomial
    // For a staticly defined polynomial
    // use MAX_POLY_SIZE blocks
    #if defined(USE_STATIC_POLY)
    uint64_t    data[MAX_POLY_SIZE];
    // otherwise, use pointer for polynomial blocks
    #else
    uint64_t    *data;
    #endif
} poly_t;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Basic Polynomial Functions                                          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Initialize the blocks of a polynomial p of degree deg
void gf2x_poly_init(INPLACE poly_t *p, IN int deg);

// Zeroize the blocks of a polynomial p
void gf2x_poly_zeroize(INPLACE poly_t *p);

// Free the blocks of a polynomial p
void gf2x_poly_free(INPLACE poly_t *p);

// Polynomial Coefficients
int  gf2x_poly_getcoef(IN poly_t *p, IN int idx);
void gf2x_poly_setcoef(INPLACE poly_t *p, IN int idx, IN int val);

// Polynomial Degree
int  gf2x_poly_deg(IN poly_t *p);

// Copy a to b: b <- a
void gf2x_poly_copy(OUT poly_t *b, IN poly_t *a);

// Generate a random polynomial (of degree)
void gf2x_poly_random(INPLACE poly_t *p); 

// Generate a random polynomial (of degree) coprime to x^r - 1
void gf2x_poly_random_coprime(INPLACE poly_t *p);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Polynomial Arithmetic                                               *
 * r <- EXT_DEG                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// h <- f + g
void gf2x_poly_add(IN poly_t *f, IN poly_t *g, OUT poly_t *h);

// h <- f * g
void gf2x_poly_mul(IN poly_t *f, IN poly_t *g, OUT poly_t *h);

// c = a+b mod (x^r - 1)
void gf2x_mod_add(OUT poly_t *c, IN poly_t *a, IN poly_t *b);

// c = a*b mod (x^r - 1) 
void gf2x_mod_mul(IN poly_t *a, IN poly_t *b, OUT poly_t *c);

// c <- a^2 mod (x^r - 1)
void gf2x_mod_sqr(IN  poly_t *a, OUT poly_t *c);

// c <- c^(2^k) mod (x^r - 1)
void gf2x_mod_sqr_k_inplace(INPLACE poly_t *c, IN int k); 

// c <- h mod x^r - 1
void gf2x_red(IN  poly_t *h, OUT poly_t *c);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Polynomial Inversions                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define POLY_INV_TYT_MAX_K  10
#define POLY_INV_SAC_MAX_C  20
#define POLY_INV_SAC_MAX_A  (2 * POLY_INV_SAC_MAX_C)

typedef struct _ctx_t {
    // f = x^p - 1
    int p;
    // a & b in CEA st p - 2 = a * b
    int cea_a;  int cea_b;  
    // h, k = number of ri's, list of ri's
    int tyt_h;  int tyt_k;  int tyt_r[POLY_INV_TYT_MAX_K]; 
    // p - 2  = r * n + k
    int sac_r;  int sac_n;  int sac_h;  int sac_h_idx;
    // Addition Chain C in SAC
    int sac_lenC;
    int sac_C[POLY_INV_SAC_MAX_C];
    int sac_A[POLY_INV_SAC_MAX_A];
} ctx_t;

// g^-1 mod (x^p - 1) using Euclid's GCD algorithm 
void gf2x_mod_inv_eea(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv);

// g^-1 mod (x^p - 1) using Bernstein and Yang's Polynomial Inversion
// void gf2x_mod_inv_by(IN int d, IN poly_t *f, IN poly_t *g, OUT poly_t *ginv); 
void gf2x_mod_inv_byi(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv); 

// g^-1 mod (x^p - 1) using FLT-based Inversion
void gf2x_mod_inv_flt(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv); 

// g^-1 mod (x^p - 1) using CEA Inversion
void gf2x_mod_inv_cea(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv); 

// g^-1 mod (x^p - 1) using TYT Inversion
void gf2x_mod_inv_tyt(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv);

// g^-1 mod (x^p - 1) using SAC Inversion
void gf2x_mod_inv_sac(IN ctx_t *ctx, IN poly_t *g, OUT poly_t *ginv);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Helper functions                                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Print polynomial
void gf2x_poly_print(char *name, poly_t *f);

// Bitlength
#define bitlength(x) ((x) == 0 ? 0 : (32 - __builtin_clz(x)))

// Count the ones, i.e. hamming weight
#define count_ones(x) _Generic((x), \
    unsigned char: __builtin_popcount,  \
    unsigned short: __builtin_popcount, \
    unsigned int: __builtin_popcount,   \
    unsigned long: __builtin_popcountl, \
    unsigned long long: __builtin_popcountll \
    )(x)

#endif /* GF2X_H */