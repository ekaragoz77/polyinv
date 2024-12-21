# Polynomial Inversion Algorithms in Constant Time for Post-Quantum Cryptography

This repository contains the C implementation of polynomial inversion algorithms:
1. Bernstein-Yang's Inversion (BYI) a.k.a. `POLY_INV_BYI`
2. Regular Inversion via Fermat's Little Theorem (FLT) a.k.a. `POLY_INV_FLT`
3. CEA Inversion a.k.a. `POLY_INV_CEA`
4. TYT Inversion a.k.a. `POLY_INV_TYT`
5. Shortest Addition Chain (SAC) Inversion a.k.a.`POLY_INV_SAC`

and their benchmarking. Here, Bernstein-Yang's Inversion is based on Extended Euclidean Algorithm (EEA), while the others are based on Fermat's Little Theorem (FLT). 

The polynomials are randomly selected from a polynomial ring $\mathbb{F}_2[x] / (x^r - 1)$, defined by `EXTDEG` flag (set using`-DEXTDEG=<r>` in the compiler). Predefined primes in `params.h` file include: 10499, 12323, 24659, 24781, 27067, 27581, and 40973.

The benchmarking of these algorithms are included in our paper **Polynomial Inversion Algorithms in Constant Time for Post-Quantum Cryptography**, which is presented in [IndoCrypt 2024](https://setsindia.in/indocrypt2024/indocrypt), and published in [Progress in Cryptology – INDOCRYPT 2024](https://link.springer.com/chapter/10.1007/978-3-031-80311-6_12).


## How to Compile and Run
Compilation details are specified in the `Makefile`. The required functions are defined in `gf2x.h`, and `bench.h` includes the benchmarking functions to measure performance in terms of clock cycles.

There are three tests that can be run directly (through their script `.sh` files):

1. `run_test_inv`: Tests the correctness of the polynomial inversion algorithms (through source file `test_inv.c`), 
2. `run_test_count`: Counts the number of function calls (through source file `test_count.c`),
3. `run_test_speed`: Benchmarks the performance of polynomial inversion algorithms (through source file `test_speed.c`)

for each prime and each polynomial inversion algorithm. It is easy to modify the script files for selected primes and polynomial inversion algorithms.

## Benchmarking of Polynomial Inversion Algorithms
 
The polynomial inversion algorithms are benchmarked on
`x86` (AMD Ryzen 5800 @ 3.4 GHz) and `arm64` (Apple Mac Pro M2 @ 2.4 GHz)
architectures. The code is compiled using `GCC 9.4.0` with the `march=native` and `-O3` optimization flags. 

For each prime `p`, the benchmarking results table includes:
- the clock cycles (in millions) in both architectures `x86` and `arm64` in the 1st and 2nd rows,
- the number of high-level modular polynomial multiplication (`gf2x_mod_mul`) and squaring (`gf2x_mod_sqr`) function calls in the 3rd and 4th rows,
- the number of the block multiplication (`mul64`) and squaring (`sqr64`) function calls in 5th and 6th rows.

 
| **p (size64)** | **Metric**              | **BYI**      | **ITI**      | **CEA**      | **TYT**      | **SAC**      |
|-----------------|-------------------------|--------------|--------------|--------------|--------------|--------------|
| **10499 (165)** | x86 (Mcc)          | **4.35**     | **12.85**    | **8.38**     | **15.02**    | **7.66**     |
|                 | arm64 (Mcc)       | **7.37**     | **16.32**    | **13.78**    | **17.35**    | **12.02**    |
|                 | `# gf2x_mod_mul`       | N/A          | 16           | 20           | 16           | 16           |
|                 | `# gf2x_mod_sqr`       | N/A          | 18,688       | 10,497       | 20,992       | 10,538       |
|                 | `# mul64`              | ~1.10M       | ~0.44M       | ~0.54M       | ~0.44M       | ~0.44M       |
|                 | `# sqr64`              | -            | ~3.10M       | ~1.74M       | ~3.48M       | ~1.75M       |
| **12323 (193)** | x86 (Mcc)          | **5.79**     | **17.02**    | **10.89**    | **20.07**    | **10.93**    |
|                 | arm64 (Mcc)       | **12.23**    | **21.91**    | **18.74**    | **25.25**    | **16.92**    |
|                 | `# gf2x_mod_mul`       | N/A          | 16           | 19           | 16           | 16           |
|                 | `# gf2x_mod_sqr`       | N/A          | 20,152       | 12,321       | 24,578       | 12,369       |
|                 | `# mul64`              | ~1.51M       | ~0.60M       | ~0.71M       | ~0.60M       | ~0.60M       |
|                 | `# sqr64`              | -            | ~3.98M       | ~2.39M       | ~4.77M       | ~2.40M       |
| **24659 (386)** | x86 (Mcc)          | **21.03**    | **66.25**    | **42.51**    | **64.32**    | **42.61**    |
|                 | arm64 (Mcc)       | **45.73**    | **95.18**    | **77.74**    | **96.50**    | **77.88**    |
|                 | `# gf2x_mod_mul`       | N/A          | 18           | 19           | 18           | 19           |
|                 | `# gf2x_mod_sqr`       | N/A          | 41,040       | 24,657       | 41,121       | 24,739       |
|                 | `# mul64`              | ~6.05M       | ~2.68M       | ~2.83M       | ~2.68M       | ~2.83M       |
|                 | `# sqr64`              | -            | ~15.88M      | ~9.54M       | ~15.91M      | ~9.57M       |
| **24781 (388)** | x86 (Mcc)          | **22.11**    | **65.64**    | **44.13**    | **76.33**    | **42.18**    |
|                 | arm64 (Mcc)       | **46.30**    | **101.52**   | **86.50**    | **107.21**   | **80.28**    |
|                 | `# gf2x_mod_mul`       | N/A          | 20           | 22           | 18           | 19           |
|                 | `# gf2x_mod_sqr`       | N/A          | 41,162       | 24,779       | 49,542       | 25,091       |
|                 | `# mul64`              | ~6.10M       | ~3.01M       | ~3.31M       | ~2.71M       | ~2.86M       |
|                 | `# sqr64`              | -            | ~16.01M      | ~9.64M       | ~19.27M      | ~9.76M       |
| **27067 (423)** | x86 (Mcc)          | **24.43**    | **76.65**    | **50.19**    | **91.81**    | **51.94**    |
|                 | arm64 (Mcc)       | **54.42**    | **125.42**   | **98.09**    | **137.71**   | **101.62**   |
|                 | `# gf2x_mod_mul`       | N/A          | 22           | 20           | 21           | 20           |
|                 | `# gf2x_mod_sqr`       | N/A          | 43,448       | 27,065       | 54,002       | 27,337       |
|                 | `# mul64`              | ~7.09M       | ~3.94M       | ~3.58M       | ~3.76M       | ~3.58M       |
|                 | `# sqr64`              | -            | ~18.42M      | ~11.48M      | ~22.90M      | ~11.59M       |
| **27581 (431)** | x86 (Mcc)          | **25.34**    | **81.50**    | **53.40**    | **94.94**    | **53.40**    |
|                 | arm64 (Mcc)       | **57.34**    | **139.49**   | **111.14**   | **146.93**   | **103.03**   |
|                 | `# gf2x_mod_mul`       | N/A          | 24           | 22           | 21           | 20           |
|                 | `# gf2x_mod_sqr`       | N/A          | 43,962       | 27,579       | 55,094       | 27,850       |
|                 | `# mul64`              | ~7.34M       | ~4.46M       | ~4.09M       | ~3.90M       | ~3.72M       |
|                 | `# sqr64`              | -            | ~18.99M      | ~11.91M      | ~23.80M      | ~12.03M       |
| **40973 (641)** | x86 (Mcc)          | **58.14**    | **191.2**    | **116.62**   | **208.12**   | **113.43**   |
|                 | arm64 (Mcc)       | **127.29**   | **284.38**   | **246.86**   | **300.02**   | **217.01**   |
|                 | `# gf2x_mod_mul`       | N/A          | 19           | 22           | 18           | 18           |
|                 | `# gf2x_mod_sqr`       | N/A          | 73,738       | 40,971       | 81,940       | 40,983       |
|                 | `# mul64`              | ~17.02M      | ~7.81M       | ~9.04M       | ~7.40M       | ~7.40M       |
|                 | `# sqr64`              | -            | ~47.34M      | ~26.30M      | ~52.61M      | ~26.31M      |

## Contributors
 
- Emrah Karagoz, Florida Atlantic University, Boca Raton, USA, [ekaragoz2017@fau.edu](mailto:ekaragoz2017@fau.edu)

- Pakize Sanal, Florida Atlantic University, Boca Raton, USA, [psanal2018@fau.edu](mailto:psanal2018@fau.edu)

- Abhraneel Dutta, Florida Atlantic University, Boca Raton, USA, [adutta2016@fau.edu](mailto:adutta2016@fau.edu)

- Edoardo Persichetti, Florida Atlantic University, Boca Raton, USA, [epersichetti@fau.edu](mailto:epersichetti@fau.edu)

 