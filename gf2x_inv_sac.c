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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gf2x.h"

/*******************************************
 * SAC Poly Inversion
 * Input: a polynomial alpha
 * Return: alpha^-1 = alpha ^(2^(p-1)-2)
 * 
 * (R, N, h) <- SACDecomp(m) so that
 *      p-2 = m-1 = R * N + h
 * for N = 2^t for some positive integer t
 * 
 * Addition Chain of r1
 *      C = [ c0 = 1, c1, ..., c_(n-1) = r1 ]
 * Addition Chain Pair 
 *      A = [0] + [ (i1, i2) such that c_i1 + c_i2 = c_i for i = 1, .., n-1 ]
 * h in C
 * 
 * ******************************************/
void gf2x_mod_inv_sac(
    IN  ctx_t       *ctx,       // Context
    IN  poly_t      *g,         // input polynomial g
    OUT poly_t      *ginv       // the inverse of g
) {
    int p = ctx->p;

    int h = ctx->sac_h;
    int h_idx = ctx->sac_h_idx;
    int r = ctx->sac_r;
    int n =ctx->sac_n;

    int *C = ctx->sac_C;
    int *A = ctx->sac_A;
    int lenC = ctx->sac_lenC;
    
    // Polys
    poly_t delta, delta_r, delta_h, gamma;
    gf2x_poly_init(&delta, p-1);
    gf2x_poly_init(&delta_r, p-1);
    gf2x_poly_init(&delta_h, p-1);
    gf2x_poly_init(&gamma, p-1);

    // Chain Inverse
    poly_t tmp;
    gf2x_poly_init(&tmp, p-1);

    // Initiate the list of delta polynomials
    poly_t L[lenC];
    for(int i = 0; i < lenC; i++) {
        gf2x_poly_init(&L[i], p-1);
    }
    gf2x_poly_copy(&L[0], g);

    // Construct a list of delta's such that
    // delta[i] = alpha^(2^ci -1)
    int i1, i2;
    for (int i = 1; i < lenC; i++) {
        // i1, i2 = A[2*i-2], A[2*i-1]
        i1 = A[2*i-2];
        i2 = A[2*i-1];
        gf2x_poly_copy(&tmp, &L[i1]);
        gf2x_mod_sqr_k_inplace(&tmp, C[i2]);
        gf2x_mod_mul(&L[i2], &tmp, &L[i]);
    }

    gf2x_poly_copy(&delta_r, &L[lenC - 1]);
    gf2x_poly_copy(&delta_h, &L[h_idx]);

    // Factors
    // gamma <- delta_r
    gf2x_poly_copy(&gamma, &delta_r);

    for (int i = bitlength(n) - 2; i >= 0; i--) {
        // gamma <- gamma * gamma^(2^(r * 2^i))
        gf2x_poly_copy(&tmp, &gamma);
        gf2x_mod_sqr_k_inplace(&gamma, r * (1 << i));
        gf2x_mod_mul(&gamma, &tmp, &gamma);
        
        if ((n >> i) & 1) {
            gf2x_mod_sqr_k_inplace(&gamma, r * (1 << i));        
            gf2x_mod_mul(&gamma, &delta_r, &gamma);
        }
    }
    
    // Final Phase
    if (h == 0) {
        gf2x_poly_copy(ginv, &gamma);
    } else {
        // delta <- delta_h * gamma^(2^h)
        gf2x_mod_sqr_k_inplace(&gamma, h);
        gf2x_mod_mul(&delta_h, &gamma, &delta);
        // ginv = delta^2
        gf2x_mod_sqr(&delta, ginv);
    }

}