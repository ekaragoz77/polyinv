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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gf2x.h"

/*********************************************************
 * Bernstein and Yang's Polynomial Inversion for GF(2^m)
 * Input: f(x) irreducible polynomial of GF(2^m), m >= 2,
 *        a(x) invertible element of GF(2^m)
 * Output: V(x), such that a(x)^−1 = V(x) in GF(2^m)
 * Data: w: machine word size
 * 
**********************************************************/

/* 2x2 matrix of polynomials */
typedef struct { 
    int         denom;
    poly_t      p0;
    poly_t      p1;
    poly_t      p2;
    poly_t      p3;
} polymat_t;

/* JumpDivStep Nodes (as a binary tree) */
typedef struct jnode {
    int         n;
    int         delta;
    poly_t      f;
    poly_t      g;
    polymat_t   P;
    struct jnode *left;
    struct jnode *right;
} jnode;

// Block-shifted polynomial addition
// r <- (p + q) >> (block-shift)
// Assumed that p.size64 = q.size64
static void gf2x_poly_add_blockshift(
    IN poly_t *p, 
    IN poly_t *q,
    IN int bshift,
    OUT poly_t *r
) {
    assert(p->size64 == q->size64);
    assert(bshift <= p->size64);
    assert(r->size64 >= (p->size64-bshift));

    for (int i = bshift; i < p->size64; i++) {
        r->data[i-bshift] = p->data[i] ^ q->data[i];
    }

    // zeroize the remaining blocks
    for (int i = p->size64; i < r->size64; i++) {
        r->data[i] = 0;
    }
}

static inline void free_jnode(jnode *node) {
    if (node != NULL) {
        // gf2x_poly_free(&node->f);
        gf2x_poly_free(&node->g);
        gf2x_poly_free(&node->P.p0);
        gf2x_poly_free(&node->P.p1);
        gf2x_poly_free(&node->P.p2);
        gf2x_poly_free(&node->P.p3);
    }
}

// Return n, the maximum power of 2
// so that n < x for a given number x
static inline uint64_t maxpow2(uint64_t x) {
    uint64_t n = 1;
    while (x > n) {
        n <<= 1;
    }
    return (uint64_t) (n >> 1);
}

static inline void truncate(poly_t *f, int n) {
    for (int i = f->size64 - 1; i >= n; i--) {
        f->data[i] = 0;
    }
    f->size64 = n;
}


// In-place left multiplication of a polynomial vector 
// of length 2 with a 2x2 polynomial matrix 
// vec(f, g) <- P * vec(f, g)
static inline void MatPolyMul (
    IN polymat_t *P,
    INPLACE poly_t *f, 
    INPLACE poly_t *g
) {
    // Check if the sizes of the polynomials in the matrix
    int s64 = P->p0.size64;
    assert(s64 == P->p1.size64);
    assert(s64 == P->p2.size64);
    assert(s64 == P->p3.size64);
    
    int sizet = P->p0.size64 + f->size64;
    poly_t t0 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };
    poly_t t1 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };
    poly_t t2 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };
    poly_t t3 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };

    gf2x_poly_zeroize(&t0);
    gf2x_poly_zeroize(&t1);
    gf2x_poly_zeroize(&t2);
    gf2x_poly_zeroize(&t3);

    gf2x_poly_mul(&(P->p0), f, &t0);
    gf2x_poly_mul(&(P->p1), g, &t1);
    gf2x_poly_mul(&(P->p2), f, &t2);
    gf2x_poly_mul(&(P->p3), g, &t3);

    gf2x_poly_add_blockshift(&t0, &t1, P->denom, f);
    gf2x_poly_add_blockshift(&t2, &t3, P->denom, g);

    // Free ti's
    // gf2x_poly_free(&t0);
    // gf2x_poly_free(&t1);
    // gf2x_poly_free(&t2);
    // gf2x_poly_free(&t3);
}

// P <- P2 * P1
static inline void MatMatMul (
    polymat_t *P,           // out
    polymat_t *P1,          // in
    polymat_t *P2           // in
) {
    // Check if the sizes of the polynomials in the matrix
    assert(P1->p0.size64 == P1->p1.size64);
    assert(P1->p0.size64 == P1->p2.size64);
    assert(P1->p0.size64 == P1->p3.size64);

    assert(P2->p0.size64 == P2->p1.size64);
    assert(P2->p0.size64 == P2->p2.size64);
    assert(P2->p0.size64 == P2->p3.size64);
   
    // Temporary polynomials   
    int sizet = P1->p0.size64 + P2->p0.size64;
    poly_t t0 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };
    poly_t t1 = { .data = malloc(sizet * sizeof(uint64_t)), .size64 = sizet };    
    P->denom = t0.size64;

    // p0 <-- (P20, P21) * (P10, P12)
    gf2x_poly_zeroize(&t0);
    gf2x_poly_zeroize(&t1);
    gf2x_poly_mul(&(P2->p0), &(P1->p0), &t0);
    gf2x_poly_mul(&(P2->p1), &(P1->p2), &t1);
    gf2x_poly_add_blockshift(&t0, &t1, 0, &(P->p0));
    
    // p1 <-- (P20, P21) * (P11, P13)
    gf2x_poly_zeroize(&t0);
    gf2x_poly_zeroize(&t1);
    gf2x_poly_mul(&(P2->p0), &(P1->p1), &t0);
    gf2x_poly_mul(&(P2->p1), &(P1->p3), &t1);
    gf2x_poly_add_blockshift(&t0, &t1, 0, &(P->p1));

    // p2 <-- (P22, P23) * (P10, P12)
    gf2x_poly_zeroize(&t0);
    gf2x_poly_zeroize(&t1);
    gf2x_poly_mul(&(P2->p2), &(P1->p0), &t0);
    gf2x_poly_mul(&(P2->p3), &(P1->p2), &t1);
    gf2x_poly_add_blockshift(&t0, &t1, 0, &(P->p2));

    // p3 <-- (P22, P23) * (P11, P13)
    gf2x_poly_zeroize(&t0);
    gf2x_poly_zeroize(&t1);
    gf2x_poly_mul(&(P2->p2), &(P1->p1), &t0);
    gf2x_poly_mul(&(P2->p3), &(P1->p3), &t1);
    gf2x_poly_add_blockshift(&t0, &t1, 0, &(P->p3));

    gf2x_poly_free(&t0);
    gf2x_poly_free(&t1);
}

// Reverse 64-bit blocks
uint64_t rev64(uint64_t n) {
    n = ((n >> 1) & 0x5555555555555555) | ((n & 0x5555555555555555) << 1);
    n = ((n >> 2) & 0x3333333333333333) | ((n & 0x3333333333333333) << 2);
    n = ((n >> 4) & 0x0F0F0F0F0F0F0F0F) | ((n & 0x0F0F0F0F0F0F0F0F) << 4);
    n = ((n >> 8) & 0x00FF00FF00FF00FF) | ((n & 0x00FF00FF00FF00FF) << 8);
    n = ((n >> 16) & 0x0000FFFF0000FFFF) | ((n & 0x0000FFFF0000FFFF) << 16);
    n = (n >> 32) | (n << 32);
    return n;
}

// Reverse p in terms of d
// (i.e. r = p.reverse(d) )
static inline void reverse(poly_t *p, poly_t *r, int d) {

    assert(p->data != NULL && "Input polynomial is not initialized!");
    assert(r->data != NULL && "Output polynomial is not initialized!");
    assert((p->size64 >= r->size64) && "The size of input and output polynomial do not match!");

    // n = number of 64-bit blocks for length/degree = d
    // k = number of remaning bits
    // (d+1) = 64 * n + k
    int n = ( (d+1) >> 6  );
    int k = ( (d+1) & 0x3f);
    
    // if k = 0, just reverse each block, and reverse the block order
    if(k == 0) {
        for(int i = n-1; i >= 0; i--) {
            r->data[i] = rev64(p->data[n-1-i]);
        }        
    } 
    // Construct new blocks by bitshifts, and reverse them in the new block 
    else {
        int shift = 64 - k;     // Number of right shift
        
        uint64_t hi, lo;

        for(int i = n; i > 0; i--) {
            hi = (p->data[i]   << shift);
            lo = (p->data[i-1] >> k);
            r->data[n-i] = rev64(hi | lo);
        }
        r->data[n] = rev64(p->data[0] << shift);
    }

}

// Polynomial division by x^d, which is
// performed by shifting the blocks to right by d bits
static inline void poly_right_shift(poly_t *p, int shift) {
    assert( (shift < 64) && (shift > 0) && "Shift bits (d) should be between 1 and 63 (inclusive)");

    uint64_t mask_shift = ((uint64_t)(0x1) << shift) - 1;

    uint64_t hi, lo;
    for(int i = 0; i < p->size64 - 1; i++) {
        lo = p->data[i] >> shift;
        hi = p->data[i+1] & mask_shift;
        p->data[i] = (hi << (64-shift)) | lo;
    }
    p->data[p->size64-1] >>= shift;
}

/************************************
 * BY FUNCTIONS
 ************************************/

// returns delta
static inline int divstepx_64(
    int n, int delta,
    uint64_t *f, uint64_t *g,  // input
    uint64_t P[4]  // output matrix
) {
    // Copy Values
    uint64_t ff = *(f + 0);
    uint64_t gg = *(g + 0);
    int dd = delta;

    // temp
    uint64_t tmp;

    // Masking for truncating f and g
    uint64_t mask = (0 - 1ULL) >> (64-n);

    // masking for f0 and g0
    // if f0 = 1, mask_f0 = 0xFFFFFFFFFFFFFFFF
    // if f0 = 0, mask_f0 = 0x0000000000000000
    uint64_t mask_f0;
    uint64_t mask_g0;

    // fractional polynomials
    // u, v, q, r
    // [0, 1] = [num, denom]
    uint64_t u[2] = {1, 0};
    uint64_t v[2] = {0, 0};
    uint64_t q[2] = {0, 0};
    uint64_t r[2] = {1, 0};
    int maxdenom;

    // Start
    for(int i = 0; i < n; i ++) {

        // Truncate f
        ff &= mask; 
        
        // Swap
        if ( (dd > 0) && (gg & 0x01) ) {
            dd = -dd;

            tmp = ff; ff = gg; gg = tmp;

            tmp = u[0]; u[0] = q[0]; q[0] = tmp;
            tmp = u[1]; u[1] = q[1]; q[1] = tmp;

            tmp = v[0]; v[0] = r[0]; r[0] = tmp;
            tmp = v[1]; v[1] = r[1]; r[1] = tmp;
        }

        // TODO: g0 is always 1, so we can fix the mask
        mask_f0 = (0 - (ff & 0x01));
        mask_g0 = (0 - (gg & 0x01));

        dd += 1;
        
        // (f0*g-g0*f)/x
        gg = (mask_f0 & gg) ^ (mask_g0 & ff);
        assert( (gg & 0x01) == 0);
        gg = gg >> 1;

        // (g0*u0/x^j + f0*q0/x^k)/x = (g0*u0*x^(m-j) + f0*q0*x^(m-k)) / x^(m+1) 
        // (f0*q-g0*u)/x       
        maxdenom = u[1] > q[1] ? (int) u[1] : (int) q[1];
        q[0] = (
            (mask_f0 & q[0]) << (maxdenom - (int) q[1]) ^
            (mask_g0 & u[0]) << (maxdenom - (int) u[1])
            );
        q[1] = maxdenom + 1;
        
        // (f0*r-g0*v)/x
        maxdenom = r[1] > v[1] ? (int) r[1] : (int) v[1];
        r[0] = (
            (mask_f0 & r[0]) << (maxdenom - (int) r[1]) ^ 
            (mask_g0 & v[0]) << (maxdenom - (int) v[1])
            );
        r[1] = maxdenom + 1;

        mask >>= 1;
        gg &= mask;
    }

    P[0] = u[0] << (n - u[1]); 
    P[1] = v[0] << (n - v[1]);
    P[2] = q[0] << (n - q[1]); 
    P[3] = r[0] << (n - r[1]);

    P[0] <<= (64-n);
    P[1] <<= (64-n);
    P[2] <<= (64-n);
    P[3] <<= (64-n);

    return dd;
}


/*
    Return the new delta. 

    input:
        int n, delta;
        poly_t f, g;
    output:
        int delta;
        polymat_t   P;
*/
static int jumpdivstepx(jnode *parent) { 
    
    if (parent->n <= 64) {        
        // Compute DivStepx64
        uint64_t P[4];
        uint64_t *fp = parent->f.data;
        uint64_t *gp = parent->g.data;
        
        parent->delta = divstepx_64(parent->n, parent->delta, fp, gp, P);

        // Copy P values
        memcpy(parent->P.p0.data, &P[0], sizeof(uint64_t));
        memcpy(parent->P.p1.data, &P[1], sizeof(uint64_t));
        memcpy(parent->P.p2.data, &P[2], sizeof(uint64_t));
        memcpy(parent->P.p3.data, &P[3], sizeof(uint64_t));

        // return new delta
        return parent->delta;
    }

    // Compute j as the max power of 2 so that n < j
    int j = maxpow2(parent->n);
    
    // Construct left child and pass values
    parent->left = malloc(sizeof(jnode));
    parent->left->n = j;
    parent->left->delta = parent->delta;
    parent->left->f.data = malloc(parent->f.size64 * sizeof(uint64_t));
    parent->left->f.size64 = parent->f.size64;
    memcpy(parent->left->f.data, parent->f.data, parent->f.size64 * sizeof(uint64_t));   
    parent->left->g.data = malloc(parent->g.size64 * sizeof(uint64_t));
    parent->left->g.size64 = parent->g.size64;
    memcpy(parent->left->g.data, parent->g.data, parent->g.size64 * sizeof(uint64_t));

    int sizeP1 = (parent->left->n + 63) / 64;
    polymat_t P1= {
        .p0 = { .data = calloc(sizeP1, sizeof(uint64_t)), .size64 = sizeP1 },
        .p1 = { .data = calloc(sizeP1, sizeof(uint64_t)), .size64 = sizeP1 },
        .p2 = { .data = calloc(sizeP1, sizeof(uint64_t)), .size64 = sizeP1 },
        .p3 = { .data = calloc(sizeP1, sizeof(uint64_t)), .size64 = sizeP1 },
        .denom = sizeP1
    };
    parent->left->P = P1;
    
    // delta, P1 <- jumpdivstepsx(j, delta, f, g)
    parent->delta = jumpdivstepx(parent->left);

    // f, g <- P1 * (f, g)
    MatPolyMul(&(parent->left->P), &(parent->f), &(parent->g));

    // f <- kx(f).truncate(n-j)
    // g <- kx(g).truncate(n-j)
    truncate( &(parent->f), (parent->n - j + 63) / 64);
    truncate( &(parent->g), (parent->n - j + 63) / 64);

    // Construct right child and pass values
    parent->right = malloc(sizeof(jnode));
    parent->right->n = parent->n - j;
    parent->right->delta = parent->delta;

    parent->right->f.data = malloc(parent->f.size64 * sizeof(uint64_t));
    parent->right->f.size64 = parent->f.size64;
    memcpy(parent->right->f.data, parent->f.data, parent->f.size64 * sizeof(uint64_t));
    
    parent->right->g.data = malloc(parent->g.size64 * sizeof(uint64_t));
    parent->right->g.size64 = parent->g.size64;
    memcpy(parent->right->g.data, parent->g.data, parent->g.size64 * sizeof(uint64_t));

    int sizeP2 = (parent->right->n + 63) / 64;
    polymat_t P2= {
        .p0 = { .data = calloc(sizeP2, sizeof(uint64_t)), .size64 = sizeP2, },
        .p1 = { .data = calloc(sizeP2, sizeof(uint64_t)), .size64 = sizeP2, },
        .p2 = { .data = calloc(sizeP2, sizeof(uint64_t)), .size64 = sizeP2, },
        .p3 = { .data = calloc(sizeP2, sizeof(uint64_t)), .size64 = sizeP2, },
        .denom = sizeP2
    };
    parent->right->P = P2;
    
    // delta, P2 <- jumpdivstepsx(n-j, delta, f, g)
    parent->delta = jumpdivstepx(parent->right);
    
    // f, g <- P2 * (f, g)
    MatPolyMul(&(parent->right->P), &(parent->f), &(parent->g));

    // P <- P2 * P1
    MatMatMul(&(parent->P), &(parent->left->P), &(parent->right->P));

    // Free left and right nodes
    // free_jnode(parent->left);
    // free_jnode(parent->right);

    return parent->delta;
}

// void gf2x_mod_inv_by(int d, poly_t *f, poly_t *g, poly_t *ginv) {
void gf2x_mod_inv_byi(
    IN ctx_t *ctx, 
    poly_t *g, 
    poly_t *ginv
) {
    
    int d = ctx->p; 

    // Reverse of f (i.e. f_rev = f.reverse(d) = f for f = x^d - 1)    
    poly_t f_rev;
    gf2x_poly_init(&f_rev, d);
    gf2x_poly_zeroize(&f_rev);
    gf2x_poly_setcoef(&f_rev, d, 1);
    gf2x_poly_setcoef(&f_rev, 0, 1);

    // Reverse g (i.e. g_rev = g.reverse(d-1) )
    poly_t g_rev;
    gf2x_poly_init(&g_rev, g->deg);
    reverse(g, &g_rev, d-1);

    // JumpStep
    int Psize64 = g->size64;

    polymat_t P = {
        .p0 = { .data = calloc(Psize64, sizeof(uint64_t)), .size64 = 2 * Psize64 },
        .p1 = { .data = calloc(Psize64, sizeof(uint64_t)), .size64 = 2 * Psize64 },
        .p2 = { .data = calloc(Psize64, sizeof(uint64_t)), .size64 = 2 * Psize64 },
        .p3 = { .data = calloc(Psize64, sizeof(uint64_t)), .size64 = 2 * Psize64 },
        .denom = 0
    };
   
    jnode top = {
        .n          = 2*d-1,
        .delta      = 1,
        .f          = f_rev,
        .g          = g_rev,
        .P          = P,
        .left       = NULL,
        .right      = NULL,
    };

    // Call "jumpdivstepx" function on "top" node
    jumpdivstepx(&top);

    // Multiply P0[1] with x^(2*d-2)
    poly_right_shift(&(top.P.p1), 64*top.P.denom - (2*d-2));

    // ginv = reverse of (P[0][1])  
    reverse(&(top.P.p1), ginv, d-1);

} 