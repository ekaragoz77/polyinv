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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "gf2x.h"


// Block Squaring of a 64-bit polynomial block
// by using PCLMULQDQ instruction on x86, or
// by using PMULL instruction on ARM assembly,
// based on CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
#include <x86intrin.h>
    static inline void sqr64 (
        uint64_t *in,
        uint64_t *out
    ) {
        PRINT_FUNCTION_NAME("sqr");
        __m128i A = _mm_set_epi64x(0, *(in + 0));

        __m128i P0 = _mm_clmulepi64_si128(A, A, 0x00);

        *(out + 0) = _mm_extract_epi64(P0, 0);
        *(out + 1) = _mm_extract_epi64(P0, 1);
    }

#elif defined(__arm__) || defined(__aarch64__)
#include <arm_neon.h>
    static inline void sqr64 (
        uint64_t *in, 
        uint64_t *out
    ) {
        uint64x1_t va = vld1_u64(in);

        poly128_t  prod = vmull_p64(va, va);
        uint64x2_t prod_64x2 = vreinterpretq_u64_p128(prod);

        *(out + 0) = vgetq_lane_u64(prod_64x2, 0);
        *(out + 1) = vgetq_lane_u64(prod_64x2, 1);
    }

#else
    #error "Unsupported architecture"
#endif


// Modular squarring 
// Input : a <- polynomial of degree <= (EXT_DEG - 1)
// Output: c <- a^2 mod (x^EXT_DEG - 1)
void gf2x_mod_sqr(
    IN  poly_t *a,
    OUT poly_t *c
) {
    // Required for counting function call
    PRINT_FUNCTION_NAME("gf2x_mod_sqr");

    // tmp <- 0
    poly_t tmp;
    gf2x_poly_init(&tmp, 2 * (EXT_DEG - 1));

    // Block squaring
    for (int i = 0; i < a->size64; i++) {
        // Required for counting function call
        PRINT_FUNCTION_NAME("sqr64");

        sqr64(&a->data[i], &tmp.data[2*i]);
    }

    // c <- tmp mod (x^EXT_DEG - 1)
    gf2x_red(&tmp, c);

    // Free tmp
    gf2x_poly_free(&tmp);
}


// In-place repeatitive modular squarring 
// c <- c^(2^k) mod (x^EXT_DEG - 1)
// without calling "mod_sqr" function
void gf2x_mod_sqr_k_inplace(
    INPLACE poly_t *c,
    IN int k
) {
    poly_t tmp;
    gf2x_poly_init(&tmp, 2 * (EXT_DEG - 1));

    for(int j = 0; j < k; j++) {
        // Required for counting function call
        PRINT_FUNCTION_NAME("gf2x_mod_sqr");

        // tmp <- 0
        gf2x_poly_zeroize(&tmp);

        // Block Squarring
        for (int i = 0; i < c->size64; i++) {
            // Required for counting function call
            PRINT_FUNCTION_NAME("sqr64");
            
            sqr64(&c->data[i], &tmp.data[2*i]);
        }

        // c <- tmp mod (x^EXT_DEG - 1)
        gf2x_red(&tmp, c);
    }

    // Free tmp
    gf2x_poly_free(&tmp);
}
