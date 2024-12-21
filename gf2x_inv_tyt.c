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

/******************************
 * Improved TYT Inversion
 *  
 * Input: 
 *      g(x) in F_(2^p) = F2[x] / (x^p-1)
 * where p−2 = \prod_i=0^(k-1) r_i + h 
 * 
 * Output: g^(-1)
 * 
 * Consider
 * 
 * r - 2 = r_0 * r_1 * ... * r_(k-1) + h so that
 * 
 *      ri = (1 ri_(q1-2) .. ri_1 ri_0)_2 
 *      qi  = bitlength(ri) 
 * 
 * for i = 0 .. k-1, and
 * 
 *      h = (1 h_(t-2) h_(t-3) ... h_1 h_0)_2
 *      t = bitlength of h
 * 
 * In addition, initalize an array (of length q) F of polynomials
 * where q = max(qi)
 * 
 * ***************************/
void gf2x_mod_inv_tyt(
    IN  ctx_t       *ctx,       // Context
    IN  poly_t      *g,         // input polynomial g
    OUT poly_t      *ginv       // the inverse of g
) {
    int p = ctx->p;
    int h = ctx->tyt_h;
    int k = ctx->tyt_k;
    int *r = ctx->tyt_r;

    // Checking decomposition
    int prod = 1;
    for(int i = 0; i < k; i++) {
        prod *= r[i];
    }
    assert( (p-2) == (prod + h) );

    // Compute qi's and max_q
    int q[k];
    for(int i = 0; i < k; i++) {
        q[i] = bitlength(r[i]);
    }

    int max_q = q[0];
    for(int i = 1; i < k; i++) {
        if (q[i] > max_q) {
            max_q = q[i];
        }
    }

    int t = bitlength(h);

    // First phase: Construction of F    
    poly_t F[max_q];

    gf2x_poly_init(&F[0], p-1);
    gf2x_poly_copy(&F[0], g);

    for(int i = 1; i < max_q; i++) {
        gf2x_poly_init(&F[i], p-1);
    }

    for(int i = 1; i < q[0]; i++) {
        // F[i] <- F[i-1]
        gf2x_poly_copy(&F[i], &F[i-1]);
        // F[i] <- F[i-1]^(2^(i-1) + 1)
        gf2x_mod_sqr_k_inplace(&F[i], 1 << (i-1));
        gf2x_mod_mul(&F[i], &F[i-1], &F[i]);
    }

    // Second phase: Compute delta    
    poly_t delta;
    gf2x_poly_init(&delta, p-1);
    gf2x_poly_copy(&delta, &F[q[0]-1]);

    for(int i = q[0]-2; i >= 0; i--) {
        if ((r[0] >> i) & 1) {
            gf2x_mod_sqr_k_inplace(&delta, (1 << i));
            gf2x_mod_mul(&delta, &F[i], &delta);
        }
    }

    // Third phase: Compute gamma
    poly_t gamma;
    gf2x_poly_init(&gamma, p-1);
    gf2x_poly_copy(&gamma, &F[t-1]);

    for (int i = t-2; i >= 0; i--) {
        if ((h >> i) & 1) {
            gf2x_mod_sqr_k_inplace(&gamma, (1 << i));
            gf2x_mod_mul(&gamma, &F[i], &gamma);
        }
    }

    // Fourth phase: Update the list F and delta
    int N = r[0];
    gf2x_poly_copy(&F[0], &delta);

    for(int j = 1; j < k; j++) {
        
        for(int i = 1; i < q[j]; i++) {
            // F[i] <- F[i-1]
            gf2x_poly_copy(&F[i], &F[i-1]);
            // F[i] <- F[i-1]^(2^(i-1) + 1)
            gf2x_mod_sqr_k_inplace(&F[i], N * (1 << (i-1)));
            gf2x_mod_mul(&F[i], &F[i-1], &F[i]);
        }

        // delta <- F[qj-1]
        gf2x_poly_copy(&delta, &F[q[j]-1]);

        for (int i = q[j] - 2; i >= 0; i--) {
            if ((r[j] >> i) & 1) {
                gf2x_mod_sqr_k_inplace(&delta, N * (1 << i));
                gf2x_mod_mul(&delta, &F[i], &delta);
            }
        }

        // F[0} <- delta
        gf2x_poly_copy(&F[0], &delta);

        // Update N
        N *= r[j];
    }

    // Final phase
    gf2x_mod_sqr_k_inplace(&gamma, p-2-h);
    gf2x_mod_mul(&gamma, &delta, &gamma);
    gf2x_mod_sqr(&gamma, ginv);

}
