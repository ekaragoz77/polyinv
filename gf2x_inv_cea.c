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

/***********************************************************
 * CEA Inversion
 *  
 * Input: g(x) in F2[x] / (x^p-1)
 * where m - 1 = p − 2 = a * b when p − 2 is not a prime
 * 
 * Output: g^(-1) = g^(2^p−2)
 * 
 * Consider
 * a = (1 a_(s-2) a_(s-3) ... a_1 a_0)_2 as binary representation of a
 * b = (1 b_(t-2) b_(t-3) ... b_1 b_0)_2 as binary representation of b
 * 
 * ********************************************************/

void gf2x_mod_inv_cea(
    IN ctx_t *ctx,
    IN poly_t *g,
    OUT poly_t *ginv
) {
    int r = ctx->p;
    int a = ctx->cea_a;
    int b = ctx->cea_b;

    // r - 2 = a * b
    int r2 = r - 2;
    assert( (a * b) == r2 );


    // Create gamma and copy g to gamma.
    poly_t gamma;
    gf2x_poly_init(&gamma, r-1);
    gf2x_poly_copy(&gamma, g);

    // Find s and t
    int s = 0, t = 0;
    int tmp = a;
    while (tmp > 0) {
        s += 1;
        tmp >>= 1;
    }
    tmp = b;
    while (tmp > 0) {
        t += 1;
        tmp >>= 1;
    }

    // Recursive computations
    poly_t h;
    gf2x_poly_init(&h, r-1);

    int k;

    for(int i = s-2; i >= 0; i--) {
        // h <- gamma
        gf2x_poly_copy(&h, &gamma);

        // gamma <- gamma * gamma^(2^(2^i))
        k = (1 << i);
        gf2x_mod_sqr_k_inplace(&gamma, k);
        gf2x_mod_mul(&gamma, &h, &gamma);

        if (a & (1 << i)) {
            // gamma <- g * gamma^(2^(2^i))
            gf2x_mod_sqr_k_inplace(&gamma, k);
            gf2x_mod_mul(&gamma, g, &gamma);
        }

    }

    // gamma <- gamma^2
    gf2x_mod_sqr_k_inplace(&gamma, 1);

    // Create delta and copy gamma to delta.
    poly_t delta;
    gf2x_poly_init(&delta, r-1);
    gf2x_poly_copy(&delta, &gamma);

    for(int i = t-2; i >= 0; i--) {
        // h <- delta
        gf2x_poly_copy(&h, &delta);

        // delta <- delta * delta^(2^(2^i))
        k = a * (1 << i);
        gf2x_mod_sqr_k_inplace(&delta, k);
        gf2x_mod_mul(&delta, &h, &delta);

        if (b & (1 << i)) {
            // delta <- g * delta^(2^(2^i))
            gf2x_mod_sqr_k_inplace(&delta, k);
            gf2x_mod_mul(&delta, &gamma, &delta);
        }

    }

    // ginv <- delta
    gf2x_poly_copy(ginv, &delta);
}
