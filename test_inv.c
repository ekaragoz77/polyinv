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
#include <string.h>
#include <assert.h>
#include <time.h>

#include "gf2x.h"
#include "params.h"

#if defined(USE_STATIC_POLY)
    #define TEST_INV_BYI    0
    #define TEST_INV_FLT    1
    #define TEST_INV_CEA    1
    #define TEST_INV_TYT    1
    #define TEST_INV_SAC    1
#else
    #define TEST_INV_BYI    1
    #define TEST_INV_FLT    0
    #define TEST_INV_CEA    0
    #define TEST_INV_TYT    0
    #define TEST_INV_SAC    0
#endif


static int isZeroPoly(poly_t *a) {
    for (int i = 0; i < a->size64; i++) {
        if (a->data[i] != 0) return 0;
    }
    return 1;
}


static int isOnePoly(poly_t *a) {
    if (a->data[0] != 1) return 0;

    for (int i = 1; i < a->size64; i++) {
        if (a->data[i] != 0) return 0;
    }

    return 1;
}


int main(void)
{
    // Print the test info
    printf("Testing Correctness:\n");
    printf("  EXT_DEG       : %d\n", EXT_DEG);
    printf("  NUM_BLOCKS    : %d\n", NUM_BLOCKS);
    printf("  MAX_POLY_SIZE : %d\n", MAX_POLY_SIZE);
    printf("  NUM_TESTS     : %d\n", TEST_INV_NUM_TESTS);

    // Variables
    int p = EXT_DEG;

    // Input Polynomial g
    poly_t g;
    gf2x_poly_init(&g, p-1);
   
    // Output polynomial ginv
    poly_t ginv;
    gf2x_poly_init(&ginv, p-1);

    // Dummy polynomial for testing
    poly_t tmp;
    gf2x_poly_init(&tmp, p-1);

    // Number of correct inversion computations 
    int correct_byi = 0;
    int correct_flt = 0;
    int correct_cea = 0;
    int correct_tyt = 0;
    int correct_sac = 0;

    // Required for randomization
    srand(time(NULL));
    // srand(42);

    for (int i = 0; i < TEST_INV_NUM_TESTS; i++) {   
        // Choose a random input
        gf2x_poly_random_coprime(&g);
        
        // Test BYI
        #if TEST_INV_BYI
            gf2x_mod_inv_byi(&ctx, &g, &ginv);
            gf2x_mod_mul(&g, &ginv, &tmp);
            if(isOnePoly(&tmp)) correct_byi++;
        #endif

        // Test FLT
        #if TEST_INV_FLT
            gf2x_mod_inv_flt(&ctx, &g, &ginv);
            gf2x_mod_mul(&g, &ginv, &tmp);
            if(isOnePoly(&tmp)) correct_flt++;
        #endif
        
        // Test CEA
        #if TEST_INV_CEA
            gf2x_mod_inv_cea(&ctx, &g, &ginv);
            gf2x_mod_mul(&g, &ginv, &tmp);
            if(isOnePoly(&tmp)) correct_cea++;
        #endif

        // Test TYT
        #if TEST_INV_TYT
            gf2x_mod_inv_tyt(&ctx, &g, &ginv);
            gf2x_mod_mul(&g, &ginv, &tmp);
            if(isOnePoly(&tmp)) correct_tyt++;
        #endif

        // Test SAC
        #if TEST_INV_SAC
            gf2x_mod_inv_sac(&ctx, &g, &ginv);
            gf2x_mod_mul(&g, &ginv, &tmp);
            if(isOnePoly(&tmp)) correct_sac++;
        #endif
    }

    // Print the results
    printf("\nResults (Number of Correct Computations / Number of Tests):\n");
    #if TEST_INV_BYI
        printf("  BYI : %d / %d \n", correct_byi, TEST_INV_NUM_TESTS);
    #endif

    #if TEST_INV_FLT
        printf("  FLT : %d / %d \n", correct_flt, TEST_INV_NUM_TESTS);
    #endif
    
    #if TEST_INV_CEA
        printf("  CEA : %d / %d \n", correct_cea, TEST_INV_NUM_TESTS);
    #endif
    
    #if TEST_INV_TYT
        printf("  TYT : %d / %d \n", correct_tyt, TEST_INV_NUM_TESTS);
    #endif
    
    #if TEST_INV_SAC
        printf("  SAC : %d / %d \n", correct_sac, TEST_INV_NUM_TESTS);
    #endif
    printf("\n\n");

    // Free polynomials
    gf2x_poly_free(&g);
    gf2x_poly_free(&ginv);
    gf2x_poly_free(&tmp);

    return 0;
}