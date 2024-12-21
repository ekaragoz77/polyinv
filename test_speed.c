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
#include <time.h>

#include "bench.h"
#include "gf2x.h"
#include "params.h"

#define TEST_SPEED_FIXED_INPUT              1
#define TEST_SPEED_RANDOM_INPUT             0

#if defined(USE_STATIC_POLY)
    #define TEST_SPEED_BYI    0
    #define TEST_SPEED_FLT    1
    #define TEST_SPEED_CEA    1
    #define TEST_SPEED_TYT    1
    #define TEST_SPEED_SAC    1
#else
    #define TEST_SPEED_BYI    1
    #define TEST_SPEED_FLT    0
    #define TEST_SPEED_CEA    0
    #define TEST_SPEED_TYT    0
    #define TEST_SPEED_SAC    0
#endif


void print_table_head() {
    printf("\n");
    printf("+-----------------+-----------------+-----------------+-----------------+\n");
    printf("|     Ext Deg     |    Poly Inv     |    Ave (msec)   |   Median (Mcc)  |\n");
    printf("+-----------------+-----------------+-----------------+-----------------+\n");
}


void print_table_row(bench_t *bench, char *name) {
    printf("| %-15d | %-15s | %-15.2f | %-15.2f |\n", EXT_DEG, name, bench->result / 1e3, bench->stats.med / 1e6);
    printf("+-----------------+-----------------+-----------------+-----------------+\n");
}


int main(void)
{
    // Print the test info
    printf("Test Speed:\n");
    printf("- EXT_DEG               : %d\n", EXT_DEG);
    printf("- NUM_BLOCKS            : %d\n", NUM_BLOCKS);
    printf("- MAX_POLY_SIZE         : %d\n", MAX_POLY_SIZE);
    printf("- NUM_TESTS             : %d\n", TEST_SPEED_NUM_TESTS);

    // Benchmarking Parameters
    bench_t bench;
    bench_init(&bench, TEST_SPEED_NUM_TESTS, NULL);    

    // Variables
    int p = EXT_DEG;

    poly_t ginv;
    gf2x_poly_init(&ginv, p-1);
    gf2x_poly_zeroize(&ginv);

    poly_t tmp;
    gf2x_poly_init(&tmp, p-1);
    gf2x_poly_zeroize(&tmp);

    // Required for randomization
    srand(time(NULL));
    
    // Fixed Input
    poly_t g;
    gf2x_poly_init(&g, p-1);
    gf2x_poly_zeroize(&g);
    gf2x_poly_random_coprime(&g);

    // Print the table head
    print_table_head();

    // BYI
    #if TEST_SPEED_BYI
    BENCHFUNC(bench, gf2x_mod_inv_byi(&ctx, &g, &ginv));
    print_table_row(&bench, "BYI");
    #endif

    // FLT
    #if TEST_SPEED_FLT
    BENCHFUNC(bench, gf2x_mod_inv_flt(&ctx, &g, &ginv));
    print_table_row(&bench, "FLT");
    #endif    

    // CEA
    #if TEST_SPEED_CEA
    BENCHFUNC(bench, gf2x_mod_inv_cea(&ctx, &g, &ginv));
    print_table_row(&bench, "CEA");
    #endif
  
    // TYT
    #if TEST_SPEED_TYT
    BENCHFUNC(bench, gf2x_mod_inv_tyt(&ctx, &g, &ginv));
    print_table_row(&bench, "TYT");
    #endif

    // SAC
    #if TEST_SPEED_SAC
    BENCHFUNC(bench, gf2x_mod_inv_sac(&ctx, &g, &ginv));
    print_table_row(&bench, "SAC");
    #endif
    
    // Free the allocated memory
    printf("\n\n");
    bench_free(&bench);
   
    return 0;
}