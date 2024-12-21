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

// Compute g^-1 mod x^r -1
// where g is a polynomials over GF(2)
void gf2x_mod_inv_flt(
    IN ctx_t *ctx,
    IN poly_t *g,
    OUT poly_t *ginv
) {   
    // r - 2
    int r = ctx->p;
    int r2 = r - 2;
    assert( (r2 & 1) == 1 );
    r2 >>= 1;

    // Create b and c, and copy g to b & c.    
    poly_t b;
    gf2x_poly_init(&b, r-1);
    gf2x_poly_copy(&b, g);
        
    poly_t c;
    gf2x_poly_init(&c, r-1);
    gf2x_poly_copy(&c, g);

    // Recursive computations
    int i = 1;
    int k;

    poly_t tmp;
    gf2x_poly_init(&tmp, r-1);


    while (r2 > 0) {
        k = 1 << (i-1);

        // tmp <- c
        gf2x_poly_copy(&tmp, &c);

        // c <- c^(2^k) 
        gf2x_mod_sqr_k_inplace(&c, k);

        // c <- c^(2^k) * c
        gf2x_mod_mul(&c, &tmp, &c);

        if (r2 & 1) {
            // k = 2^i
            k <<= 1;

            // b <- b^(2^k)
            gf2x_mod_sqr_k_inplace(&b, k);

            // b <- b * c
            gf2x_mod_mul(&b, &c, &b);
        }

        i += 1;
        r2 >>= 1;
    }

    // ginv = b^2
    gf2x_mod_sqr(&b, ginv);

    // free
    gf2x_poly_free(&b);
    gf2x_poly_free(&c);
    gf2x_poly_free(&tmp);
}
