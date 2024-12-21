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

#include <string.h>
#include <inttypes.h>

#if defined (__APPLE__) && defined (__aarch64__)
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

#include "bench.h"

/* Get CPU cycles */
unsigned long long cpucycles(void)
{
#if defined(__x86_64__)
    unsigned long long result;
    __asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
                   : "=a"(result)::"%rdx");
    return result;

#elif defined (__APPLE__) && defined (__aarch64__)
    uint64_t time_value = mach_absolute_time();
    mach_timebase_info_data_t timebase_info;
    mach_timebase_info(&timebase_info);
    uint64_t clock_cycles = (time_value * timebase_info.numer) / timebase_info.denom;
    return clock_cycles;

#elif defined(__aarch64__)
    unsigned long long cycles;
    __asm__ __volatile__(
        "mrs %0, PMCCNTR_EL0"
        : "=r"(cycles));
    return cycles;

#else
#error "Unsupported architecture"
#endif
}

/******************************************************************
 * Benchmarking Functions
 ******************************************************************/
void bench_init(bench_t *bench, int ntests, FILE *logger) {
    bench->ntests = ntests;
    bench->logger = logger;
    bench->t = malloc(sizeof(unsigned long long) * (bench->ntests + 1));
    if(bench->t == NULL) {
        printf("Error: Memory allocation failed\n");
        return;
    }
}


void bench_free(bench_t *bench) {
    if (bench == NULL) {
        printf("Error: It is already freed\n");
        return;
    }

    free(bench->t);
}



/******************************************************************
 * Statistic Functions
 ******************************************************************/

/* Statistic Functions */
static int cmp_llu(const void *a, const void *b)
{
    if (*(unsigned long long *)a < *(unsigned long long *)b)
        return -1;
    if (*(unsigned long long *)a > *(unsigned long long *)b)
        return 1;
    return 0;
}

/* Return the median of a given array of numbers */
static unsigned long long median(unsigned long long *t, size_t tlen)
{
    // qsort(t, tlen, sizeof(unsigned long long), cmp_llu);
    if (tlen % 2)
        return t[tlen / 2];
    else
        return (t[tlen / 2 - 1] + t[tlen / 2]) / 2;
}

/* Return the average of a given array of numbers */
static unsigned long long average(unsigned long long *t, size_t tlen)
{
    unsigned long long acc = 0;
    size_t i;
    for (i = 0; i < tlen; i++)
    {
        acc += t[i];
    }
    return acc / (tlen);
}

/* Return the lower-quartile of a given array of numbers */
static unsigned long long lower_quartile(unsigned long long *t, size_t tlen)
{
    return t[tlen / 4];
}

/* Return the upper-quartile of a given array of numbers */
static unsigned long long upper_quartile(unsigned long long *t, size_t tlen)
{
    return t[3 * tlen / 4];
}

/* In-place computation of t[i] <- t[i+1] - t[i] of a given array of numbers */
static void delta(unsigned long long *t, size_t tlen)
{
    unsigned int i;
    for (i = 0; i < tlen - 1; i++)
    {
        t[i] = t[i + 1] - t[i];
    }
}

void bench_stats(bench_t *bench) {
    int tlen = bench->ntests + 1;
    delta(bench->t, tlen);
    qsort(bench->t, tlen, sizeof(unsigned long long), cmp_llu);

    bench->stats.med = median(bench->t, tlen - 1);
    bench->stats.ave = average(bench->t, tlen - 1);
    bench->stats.min = bench->t[0];
    bench->stats.max = bench->t[tlen - 2];
    bench->stats.q1 = lower_quartile(bench->t, tlen - 1);
    bench->stats.q3 = upper_quartile(bench->t, tlen - 1);
}


/******************************************************************
 * Print Functions
 ******************************************************************/

/* Display Results Functions */
static void printfcomma(unsigned long long n)
{
    if (n < 1000)
    {
        printf("%llu", n);
        return;
    }
    printfcomma(n / 1000);
    printf(",%03llu", n % 1000);
}

static void printfalignedcomma(unsigned long long n, int len)
{
    unsigned long long ncopy = n;
    int i = 0;

    while (ncopy > 9)
    {
        len -= 1;
        ncopy /= 10;
        i += 1; // to account for commas
    }
    i = i / 3 - 1; // to account for commas
    for (; i < len; i++)
    {
        printf(" ");
    }
    printfcomma(n);
}

void print_results(bench_t *bench) {
    unsigned long long med = bench->stats.med;
    unsigned long long ave = bench->stats.ave;
    unsigned long long min = bench->stats.min;
    unsigned long long max = bench->stats.max;
    unsigned long long q1  = bench->stats.q1;
    unsigned long long q3  = bench->stats.q3;

    FILE *logger = bench->logger;

    // print to screen
    printf("\tmedian        :");
    printfalignedcomma(bench->stats.med, 12);
    printf(" cc \n");
    printf("\taverage       :");
    printfalignedcomma(bench->stats.ave, 12);
    printf(" cc \n");
    printf("\tFive-number summary (%lu)\n", bench->ntests);
    printf("\t\tmin         :");
    printfalignedcomma(bench->stats.min, 12);
    printf(" cc \n");
    printf("\t\tq1          :");
    printfalignedcomma(bench->stats.q1, 12);
    printf(" cc \n");
    printf("\t\tmedian      :");
    printfalignedcomma(bench->stats.med, 12);
    printf(" cc \n");
    printf("\t\tq3          :");
    printfalignedcomma(bench->stats.q3, 12);
    printf(" cc \n");
    printf("\t\tmax         :");
    printfalignedcomma(bench->stats.max, 12);
    printf(" cc \n");

    // print to logger
    if (logger != NULL)
    {
        fprintf(logger, "    ave_cc: %llu\n", bench->stats.ave);
        fprintf(logger, "    min: %llu\n", bench->stats.min);
        fprintf(logger, "    q1: %llu\n", bench->stats.q1);
        fprintf(logger, "    median: %llu\n", bench->stats.med);
        fprintf(logger, "    q3: %llu\n", bench->stats.q3);
        fprintf(logger, "    max: %llu\n", bench->stats.max);
    }
    else
    {
        printf("Logger file cannot be accessed\n");
    }
}
