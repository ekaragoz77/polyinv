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

// Print polynomial f in 64 bit blocks (in hex)
static inline void gf2x_poly_print64_zero_padded(
    char    *name,   // input poly name
    poly_t  *f        // input poly
) {
    printf("%s (%05d) = \n", name, f->size64);
    for (int i = f->size64 - 1; i >= 0 ; i--) {
        
        #if !defined(USE_STATIC_POLY)
        printf("%016lx ", *(f->data + i));
        #else
        printf("%016lx ", f->data[i]);
        #endif
        
        #ifdef POLY_PRINT_DELIM
            if ((f->size64 - i) % POLY_PRINT_DELIM == 0) 
                printf("\n");
        #endif
    }
    printf("\n\n");
}

// Print polynomial f in 64 bit blocks (in hex)
static inline void gf2x_poly_print64_dot_padded(
    char    *name,   // input poly name
    poly_t  *f        // input poly
) {
    char buf[17];
    int num_len;
    int padding_len;

    printf("%s (%5d) = \n", name, f->size64);

    for (int i = f->size64 - 1; i >= 0 ; i--) {

        #if !defined(USE_STATIC_POLY)
        num_len = snprintf(buf, sizeof(buf), "%lx", *(f->data + i));
        #else
        num_len = snprintf(buf, sizeof(buf), "%lx", f->data[i]);
        #endif 
        
        padding_len = 16 - num_len;
        for (int j = 0; j < padding_len; j++) {
            putchar('.');
        }
        printf("%s ", buf);
        #ifdef POLY_PRINT_DELIM
            if ((f->size64 - i) % POLY_PRINT_DELIM == 0) 
                printf("\n");
        #endif
    }
    printf("\n\n");

}

// Print polynomial f in 64 bit blocks (in hex)
static inline void gf2x_poly_print64_short_zero_padded(
    char    *name,   // input poly name
    poly_t  *f        // input poly
) {
    printf("%s (%05d) = \n", name, f->size64);
    for (int i = f->size64 - 1; i >= f->size64 - 8 ; i--) {
        
        #if !defined(USE_STATIC_POLY)
        printf("%016lx ", *(f->data + i));
        #else
        printf("%016lx ", f->data[i]);
        #endif
        
        #ifdef POLY_PRINT_DELIM
            if ((f->size64 - i) % POLY_PRINT_DELIM == 0) 
                printf("\n");
        #endif
    }
    printf("\n\n");
}


// Short 
// Print polynomial f in 64 bit blocks (in hex)
static inline void gf2x_poly_print64_short_dot_padded(
    char    *name,   // input poly name
    poly_t  *f        // input poly
) {
    char buf[17];
    int num_len;
    int padding_len;

    printf("%s (%5d) = \n", name, f->size64);

    for (int i = f->size64 - 1; i >= f->size64 - 8 ; i--) {

        #if !defined(USE_STATIC_POLY)
        num_len = snprintf(buf, sizeof(buf), "%lx", *(f->data + i));
        #else
        num_len = snprintf(buf, sizeof(buf), "%lx", f->data[i]);
        #endif 
        
        padding_len = 16 - num_len;
        for (int j = 0; j < padding_len; j++) {
            putchar('.');
        }
        printf("%s ", buf);
        #ifdef POLY_PRINT_DELIM
            if ((f->size64 - i) % POLY_PRINT_DELIM == 0) 
                printf("\n");
        #endif
    }
    printf("\n\n");

}

void gf2x_poly_print(
    char    *name,   // input poly name
    poly_t  *f        // input poly
) {
    #if   (POLY_PRINT_PAD_TYPE == 0)
        gf2x_poly_print64_zero_padded(name, f);
    #elif (POLY_PRINT_PAD_TYPE == 1)
        gf2x_poly_print64_dot_padded(name, f);
    #elif (POLY_PRINT_PAD_TYPE == 2)
        gf2x_poly_print64_short_zero_padded(name, f);
    #elif (POLY_PRINT_PAD_TYPE == 3)
        gf2x_poly_print64_short_dot_padded(name, f);
    #else
        #error "Invalid POLY_PRINT_PAD_TYPE"
    #endif
}