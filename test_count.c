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

int main(void)
{
    // Print the test info
    printf("Testing Count:\n");
    printf("- EXT_DEG               : %d\n", EXT_DEG);
    printf("- INVERSE METHOD        : %d\n", INVERSE_METHOD);
    printf("- NUM_BLOCKS            : %d\n", NUM_BLOCKS);
    printf("- MAX_POLY_SIZE         : %d\n", MAX_POLY_SIZE);

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

    // Required for randomization, no need a random seed
    srand(42);
            
    // Choose a random input
    gf2x_poly_random_coprime(&g);

    // Test BYI
    #if (INVERSE_METHOD == BYI)
    gf2x_mod_inv_byi(&ctx, &g, &ginv);
    #endif
   
    // Test FLT
    #if (INVERSE_METHOD == FLT)
    gf2x_mod_inv_flt(&ctx, &g, &ginv);
    #endif
        
    // Test CEA
    #if (INVERSE_METHOD == CEA)
    gf2x_mod_inv_cea(&ctx, &g, &ginv);
    #endif

    // Test TYT
    #if (INVERSE_METHOD == TYT)
    gf2x_mod_inv_tyt(&ctx, &g, &ginv);
    #endif

    // Test SAC
    #if (INVERSE_METHOD == SAC)
    gf2x_mod_inv_sac(&ctx, &g, &ginv);
    #endif

    printf("\n\n");

    return 0;
}