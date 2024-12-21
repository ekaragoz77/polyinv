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
#include <string.h>

#include "gf2x.h"


/************************************
 * Polynomial Initialization
 ************************************/

// Initialize polynomial for a given degree "deg"
// and allocate memory for the polynomial
void inline gf2x_poly_init(INPLACE poly_t *p, IN int deg) {
    
    p->deg = deg;
    int size64 = (deg + 1 + 63) / 64;

    #if defined(USE_STATIC_POLY)        
        p->size64 = size64;
        memset(&(p->data), 0, MAX_POLY_SIZE * sizeof(uint64_t));
    #else
        // Dynamic memory allocation if USE_STATIC_POLY is not defined
        p->size64 = size64;
        p->data = (uint64_t *) calloc(p->size64, sizeof(uint64_t));
    #endif
}


// Zeroize the blocks in the polynomial
void inline gf2x_poly_zeroize(INPLACE poly_t *p) {
    memset(p->data, 0, p->size64 * sizeof(uint64_t));
}


// Free the memory allocated for the polynomial
void inline gf2x_poly_free(INPLACE poly_t *p) {
    #if !defined(USE_STATIC_POLY)
    free(p->data);
    #endif
}


// Get the coefficient of x^[idx] of a polynomial p
int inline gf2x_poly_getcoef(IN poly_t *p, IN int idx) {
    return (p->data[idx / 64] >> ((idx) % 64)) & 1;
}


// Set the coefficient of x^[idx] of a polynomial p to "val" (0 or 1)
void inline gf2x_poly_setcoef(INPLACE poly_t *p, IN int idx, IN int val) {
    if (val == 1) {
        p->data[idx / 64] |= (1ULL << (idx % 64));
    } else {
        p->data[idx / 64] &= ~(1ULL << (idx % 64));
    }
}


// Returns the degree of a polynomial 
// (returns -1 for a zero polynomial)
int gf2x_poly_deg(IN poly_t *p) {
    for (int i = 64 * (p->size64) - 1; i >= 0; --i) {
        if (gf2x_poly_getcoef(p, i)) {
            return i;
        }
    }
    return -1;
}


// Return 0 if p is a zero polynomial (i.e. p = 0), 
// otherwise 1
int gf2x_poly_isZero(IN poly_t *p) {
    for (int i = 0; i < p->size64; i++) {
        if (p->data[i]) {
            return 1;
        }            
    }
    return 0;
}

// Copy a to b: b <- a
void gf2x_poly_copy(OUT poly_t *b, IN poly_t *a) {
    for (int i = 0; i < a->size64; i++) {
        b->data[i] = a->data[i];
    }
}
