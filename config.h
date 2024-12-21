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

#ifndef CONFIG_H
#define CONFIG_H

/* If EXT_DEG is not defined yet, either set it to a default value,
 * or error the program and ask for its value */
#if !defined(EXT_DEG)
    #define EXT_DEG         (255)
    // #error "EXT_DEG is not defined"
#endif

/* Number of Tests */
#define TEST_INV_NUM_TESTS  (10)
#define TEST_SPEED_NUM_TESTS (10)

/* Rounding functions */
#define CEIL(A, N)          ((A + N - 1) / N)
#define FLOOR(A, N)         (A / N)
#define CEIL_N(A, N)        (N * CEIL(A, N))

/* Determine the maximum number of 64-bit blocks (MAX_POLY_SIZE) */
#define NUM_BLOCKS          CEIL(EXT_DEG, 64)
#define MAX_POLY_SIZE       (2 * NUM_BLOCKS)

/* Determine the last block info */
#define LAST_BLOCK_IDX      FLOOR(EXT_DEG, 64)
#define LAST_BLOCK_BITSIZE  (EXT_DEG % 64)

/* Print a polynomial */
#define POLY_PRINT_DELIM    4   // Number of 64-bit blocks to print in a single line
#define POLY_PRINT_PAD_TYPE 1   // 0:Zero 1:Dot 2:Short+Zero 3:Short+Dot

/* Used for counting the function calls (i.e. Test Count) */
#ifndef TEST_COUNT
    #define TEST_COUNT 0
#endif
#define PRINT_FUNCTION_NAME(x)  (TEST_COUNT ? printf("%s\n", x) : (void)0)

#endif /* CONFIG_H */
