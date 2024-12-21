/* 
 * MIT License
 *
 * Copyright (c) 2024 Emrah Karagoz
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

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

unsigned long long cpucycles(void);

typedef struct {
    unsigned long long ave;
    // Five-number summary
    unsigned long long min;
    unsigned long long q1;
    unsigned long long med;
    unsigned long long q3;
    unsigned long long max;
} stat_t;

typedef struct {
    int ntests;                 // Number of Tests
    unsigned long long *t;      // Tick values
    struct timespec start;      // Start of benchmarking        
    struct timespec stop;       // End of benchmarking
    double result;              // Stop - Start
    stat_t stats;               // Statistic Summary
    FILE *logger;               // Logger file
} bench_t;

/******************************************************************
 * Benchmarking Functions
 ******************************************************************/
void bench_init(bench_t *bench, int ntests, FILE *logger);
void bench_free(bench_t *bench);
void bench_stats(bench_t *bench);
// void print_results(FILE *logger, unsigned long long *t, size_t tlen);
void print_results(bench_t *bench);


#define BENCHFUNC(bench, func) \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench.start); \
    for (int i = 0; i < bench.ntests; i++) { \
        bench.t[i] = cpucycles(); \
        func; \
    } \
    bench.t[bench.ntests] = cpucycles(); \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench.stop); \
    bench.result = (bench.stop.tv_sec - bench.start.tv_sec) * 1e6 + (bench.stop.tv_nsec - bench.start.tv_nsec) / 1e3; \
    bench.result = bench.result / (bench.ntests + 1); \
    bench_stats(&bench);

#endif /* BENCHMARK_H */
