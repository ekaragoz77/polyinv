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

#define LAST_BLOCK_MASK ((1ULL << LAST_BLOCK_BITSIZE) - 1)

// c <- h mod x^r - 1
void gf2x_red(
    IN  poly_t *h,
    OUT poly_t *c
) {
    for(int i = 0; i < LAST_BLOCK_IDX; i++) {
        c->data[i] = h->data[i];
    }

    c->data[LAST_BLOCK_IDX] = h->data[LAST_BLOCK_IDX] & LAST_BLOCK_MASK;

    for(int i = LAST_BLOCK_IDX; i < h->size64; i++) {
        c->data[i - LAST_BLOCK_IDX] ^= ((h->data[i] >> LAST_BLOCK_BITSIZE) | (h->data[i + 1] << (64 - LAST_BLOCK_BITSIZE)));
    }
}