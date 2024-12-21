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


// Multiplication of two 64-bit polynomial block
// by using PCLMULQDQ instruction on x86, or
// by using PMULL instruction on ARM assembly,
// based on CPU architecture
#if defined(__x86_64__) || defined(_M_X64)
#include <x86intrin.h>
    static inline void mul64 (
        uint64_t *in1, 
        uint64_t *in2, 
        uint64_t *out
    ) {
        __m128i A = _mm_set_epi64x(0, *(in1 + 0));
        __m128i B = _mm_set_epi64x(0, *(in2 + 0));
        __m128i P0 = _mm_clmulepi64_si128(A, B, 0x00);
        *(out + 0) = _mm_extract_epi64(P0, 0);
        *(out + 1) = _mm_extract_epi64(P0, 1);
    }

#elif defined(__arm__) || defined(__aarch64__)
#include <arm_neon.h>
    static inline void mul64 (
        uint64_t *in1, 
        uint64_t *in2, 
        uint64_t *out
    ) {
        uint64x1_t va = vld1_u64(in1);
        uint64x1_t vb = vld1_u64(in2);

        poly128_t  prod = vmull_p64(va, vb);
        uint64x2_t prod_64x2 = vreinterpretq_u64_p128(prod);

        *(out + 0) = vgetq_lane_u64(prod_64x2, 0);
        *(out + 1) = vgetq_lane_u64(prod_64x2, 1);
    }

#else
    #error "Unsupported architecture" 
#endif


// Polynomial multiplication
// c <- a * b
void gf2x_poly_mul(
    IN  poly_t *a, 
    IN  poly_t *b,
    OUT poly_t *c
) {
    // Required for countint functial call
    PRINT_FUNCTION_NAME("gf2x_poly_mul");
    
    uint64_t out[2] = {0};

    for (int i = 0; i < a->size64; i++) {
        for (int j = 0; j < b->size64; j++) {
            PRINT_FUNCTION_NAME("mul64");
            mul64(&a->data[i], &b->data[j], out);
            c->data[i + j    ] ^= out[0];
            c->data[i + j + 1] ^= out[1];
        }
    }
}


// Modular multiplication of polynomials
// c <- (a * b) mod (x^EXT_DEG - 1)
void gf2x_mod_mul(
    IN  poly_t *a,
    IN  poly_t *b,
    OUT poly_t *c
) {
    // Required for countint functial call
    PRINT_FUNCTION_NAME("gf2x_mod_mul");

    // Verify the polynomial degrees &  block sizes
    assert(a->deg <= EXT_DEG - 1);
    assert(b->deg <= EXT_DEG - 1);
    assert(a->size64 == b->size64);

    // Initialize the product polynomial h
    #if defined(USE_STATIC_POLY)
    poly_t tmp = {
        .deg = 2 * (EXT_DEG-1), 
        .size64 = a->size64 + b->size64
        };
    gf2x_poly_zeroize(&tmp);
    #else
    poly_t tmp = {
        .deg = 2 * (EXT_DEG-1), 
        .size64 = a->size64 + b->size64,
        .data = malloc( (a->size64 + b->size64) * sizeof(uint64_t))
        };
    gf2x_poly_zeroize(&tmp);
    #endif


    // Regular polynomial multiplication
    gf2x_poly_mul(a, b, &tmp);

    // Reduction
    gf2x_red(&tmp, c);

    gf2x_poly_free(&tmp);
}
