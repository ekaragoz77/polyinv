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
#include "gf2x.h"

// 32-bit mask value
#define BLOCK_MASK32 0xFFFFFFFF
#define BLOCK_MASK64 0xFFFFFFFFFFFFFFFF

static inline void rand64(uint64_t *x) {
    *x = rand();
    *x <<= 32;
    *x |= rand();
}

// Generate a random polynomial
void gf2x_poly_random(INPLACE poly_t *p) {
    int num_bits = p->deg + 1;
    int actual_size64 = (num_bits + 63) / 64;

    for (int i = 0; i < actual_size64; i++)
    {
        rand64(&(p->data[i]));
    }

    for(int i = actual_size64; i < p->size64; i++) {
        p->data[i] = 0;
    }

    // Trim the last block
    int last_block_bits = (num_bits) & 0x3F;
    if (last_block_bits > 0) {
        uint64_t mask = ((1ULL << last_block_bits) - 1);
        p->data[actual_size64 - 1] &= mask;
    }

    gf2x_poly_setcoef(p, p->deg, 1);

}

// Generate a random polynomial coprime to x^r - 1
// Compute g(1) to determine if x+1 divides g(x)
// by counting number of 1s in the blocks
void gf2x_poly_random_coprime(INPLACE poly_t *p) {
    
    // Generate a random polynomial
    gf2x_poly_random(p);

    // Compute g(1)
    int g1 = 0;

    for (int i = 0; i < p->size64; i++) { 
        g1 += count_ones(p->data[i]);
    }

    g1 &= 1;

    // If g1 == 0 then g += 1
    if (g1 == 0)
    {
        p->data[0] ^= 1;
    }

}