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

#ifndef PARAMS_H
#define PARAMS_H

#define BYI 1
#define FLT 2
#define CEA 3
#define TYT 4
#define SAC 5


#if defined(EXT_DEG)
    #if EXT_DEG == 10499
        ctx_t ctx = { 
            .p = 10499, 
            .cea_a =   3,  .cea_b = 3499,
            .tyt_h =   1,  .tyt_k =   2,  .tyt_r = {41, 256},
            .sac_r =  41,  .sac_n = 256,  .sac_h = 1, .sac_h_idx = 0,
            .sac_lenC = 8,
            .sac_C = {1,    2,    3,    5,   10,   20,   40,   41},
            .sac_A = {    0,0,  0,1,  1,2,  3,3,  4,4,  5,5,  0,6},
            };

    // BIKE's prime
    #elif EXT_DEG == 12323
        ctx_t ctx = { 
            .p = 12323, 
            .cea_a =    9,  .cea_b =     1369,
            .tyt_h =   32,  .tyt_k =        1,  .tyt_r = {12289},
            .sac_r =   48,  .sac_n = (1 << 8),  .sac_h = 33, .sac_h_idx = 6,
            .sac_lenC = 8,
            .sac_C = {1,    2,    4,    8,   16,   32,   33,   48},
            .sac_A = {    0,0,  1,1,  2,2,  3,3,  4,4,  0,5,  4,5},
            };    
    
    // BIKE's prime
    #elif EXT_DEG == 24659
        ctx_t ctx = { 
            .p = 24659, 
            .cea_a =    3,  .cea_b =     8219,
            .tyt_h = 4097,  .tyt_k =        2,  .tyt_r = {4112, 5},
            .sac_r =   96,  .sac_n = (1 << 8),  .sac_h = 81, .sac_h_idx = 9,
            .sac_lenC = 11,
            .sac_C = {1,    2,    3,    6,    9,   12,   24,   33,   48,   81,   96},
            .sac_A = {    0,0,  0,1,  2,2,  2,3,  2,4,  5,5,  4,6,  6,6,  7,8,  8,8},
            };
    
    #elif EXT_DEG == 24781
        ctx_t ctx = { 
            .p = 24781, 
            .cea_a =  71,  .cea_b = 349,
            .tyt_h =   8,  .tyt_k =   2,  .tyt_r = {8257, 3},
            .sac_r = 193,  .sac_n = 128,  .sac_h = 75, .sac_h_idx = 8,
            .sac_lenC = 12,
            .sac_C = {1,    2,    3,    6,   12,   24,   48,   72,   75,   96,  192,   193},
            .sac_A = {    0,0,  0,1,  2,2,  3,3,  4,4,  5,5,  5,6,  2,7,  6,6,  9,9,  0,10},
            };
    
    #elif EXT_DEG == 27067
        ctx_t ctx = { 
            .p = 27067, 
            .cea_a =   5,  .cea_b = 5413,
            .tyt_h =  64,  .tyt_k =   2,  .tyt_r = {67, 403},
            .sac_r = 211,  .sac_n = 128,  .sac_h = 57, .sac_h_idx = 9,
            .sac_lenC = 13,
            .sac_C = {1,    2,    3,    5,    6,   12,   13,   26,   52,   57,  104,    208,   211},
            .sac_A = {    0,0,  0,1,  1,2,  2,2,  4,4,  0,5,  6,6,  7,7,  3,8,  8,8,  10,10,  2,11},
            };
    #elif EXT_DEG == 27581
        ctx_t ctx = { 
            .p = 27581, 
            .cea_a =   3,  .cea_b = 9193,
            .tyt_h =  32,  .tyt_k =   2,  .tyt_r = {163, 169},
            .sac_r = 215,  .sac_n = 128,  .sac_h = 59, .sac_h_idx = 9,
            .sac_lenC = 13,
            .sac_C = {1,    2,    3,    6,    7,   12,   13,   26,   52,   59,  104,    208,   215},
            .sac_A = {    0,0,  0,1,  2,2,  0,3,  3,3,  0,5,  6,6,  7,7,  4,8,  8,8,  10,10,  4,11},
            };
    
    //  BIKE's prime
    #elif EXT_DEG == 40973
        ctx_t ctx = { 
            .p = 40973, 
            .cea_a =  3,  .cea_b = 13657,
            .tyt_h =  1,  .tyt_k =    2,  .tyt_r = {10, 4097},
            .sac_r = 20,  .sac_n = 2048,  .sac_h = 11, .sac_h_idx = 5,
            .sac_lenC = 7,
            .sac_C = {1,    2,    3,    5,   10,   11,   20},
            .sac_A = {    0,0,  0,1,  1,2,  3,3,  0,4,  4,4},
            };
    #else
        #error "Invalid EXT_DEG value"
    #endif
#else
    #error "EXT_DEG not defined"    
#endif

#endif /* PARAMS_H */