//-----------------------------------------------------------------------------
//           Name: simd_wrappers.h
//      Developer: Steven Marion
//    Description:
//        License: Read below
//-----------------------------------------------------------------------------
//
//   Copyright 2024 Steven Marion?
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//-----------------------------------------------------------------------------
#pragma once

#ifdef __x86_64__
#include <xmmintrin.h>
#include <emmintrin.h>
#else

typedef float __m128 __attribute__ ((vector_size (16)));

// fill a vector with 4 copies of a float
__attribute__ ((const, always_inline))
inline __m128 _mm_set_ps1(float a) {
    __m128 ret = {a, a, a, a};
    return ret;
}

// fill a vector with 4 zeros
__attribute__ ((const, always_inline))
inline __m128 _mm_setzero_ps() {
    return _mm_set_ps1(0.0F);
}

// same as the above, why does this exist?
__attribute__ ((const, always_inline))
inline __m128 _mm_set1_ps(float a) {
    return _mm_set_ps1(a);
}

// aligned load 4 floats
__attribute__ ((pure))
inline __m128 _mm_load_ps(const float *p) {
    return *(const __m128*)__builtin_assume_aligned(p, sizeof(__m128));
}

// unaligned load 4 floats
__attribute__ ((pure))
inline __m128 _mm_loadu_ps(const float *p) {
    __m128 ret;
    ret[0] = p[0];
    ret[1] = p[1];
    ret[2] = p[2];
    ret[3] = p[3];
    return ret;
}

// unaligned load 4 floats in reverse order
__attribute__ ((pure))
inline __m128 _mm_loadr_ps(const float *p) {
    __m128 ret;
    ret[0] = p[3];
    ret[1] = p[2];
    ret[2] = p[1];
    ret[3] = p[0];
    return ret;
}

// fill vector filled with one float duplicated four times
__attribute__ ((pure))
inline __m128 _mm_load1_ps(const float *p) {
    return _mm_set_ps1(*p);
}

// identical to the above, why does this exist?
__attribute__ ((pure, always_inline))
inline __m128 _mm_load_ps1(const float *p) {
    return _mm_load1_ps(p);
}

// load 1 byte and 3 additional set to 0
__attribute__ ((pure))
inline __m128 _mm_load_ss(const float *p) {
    __m128 ret = _mm_set_ps1(0.0F);
    ret[0] = *p;
    return ret;
}

// aligned store 4 floats
inline void _mm_store_ps(float *p, __m128 v1) {
    *(__m128*)(__builtin_assume_aligned(p, sizeof(__m128))) = v1;
}

// unaligned store 4 floats
inline void _mm_storeu_ps(float *p, __m128 v1) {
    p[0] = v1[0];
    p[1] = v1[1];
    p[2] = v1[2];
    p[3] = v1[3];
}

// unaligned store 4 floats in reverse order
inline void _mm_storer_ps(float *p, __m128 v1) {
    p[0] = v1[3];
    p[1] = v1[2];
    p[2] = v1[1];
    p[3] = v1[0];
}

// store lowest (in order, not value) float from vector to 4 bytes in aligned memory
inline void _mm_store1_ps(float *p, __m128 v1) {
    __m128 temp = _mm_set_ps1(v1[0]);
    *(__m128*)__builtin_assume_aligned(p, sizeof(__m128)) = temp;
}

// identical to the above, why does this exist?
__attribute__ ((always_inline))
inline void _mm_store_ps1(float *p, __m128 v1) {
    _mm_store1_ps(p, v1);
}

// adds two vectors
__attribute__ ((const))
inline __m128 _mm_add_ps(__m128 a, __m128 b) {
    return a+b;
}

// subtracts two vectors
__attribute__ ((const))
inline __m128 _mm_sub_ps(__m128 a, __m128 b) {
    return a-b;
}

// multiplies two vectors
__attribute__ ((const))
inline __m128 _mm_mul_ps(__m128 a, __m128 b) {
    return a*b;
}

// compares two vectors and stores in each slot 0xffffffff if the corresponding lhs element is less than the corresponding rhs element, 0 otherwise.
__attribute__ ((const))
inline __m128 _mm_cmplt_ps(__m128 lhs, __m128 rhs) {
    return lhs < rhs;
}

// compares two vectors and stores in each slot 0xffffffff if the corresponding lhs element is greater than the corresponding rhs element, 0 otherwise.
__attribute__ ((const))
inline __m128 _mm_cmpgt_ps(__m128 lhs, __m128 rhs) {
    return lhs > rhs;
}

// bitwise AND, it's a bit weird that this uses the float registers? but ok
__attribute__ ((const))
inline __m128 _mm_and_ps(__m128 a, __m128 b) {
    typedef uint32_t vec_t __attribute__ ((vector_size (sizeof(__m128))));
    return (__m128)((vec_t)(a) & (vec_t)(b));
}

// macro used in _mm_shuffle_ps. Normally this would expand to an int, but I'm being creative here.
#define _MM_SHUFFLE(z, y, x, w) z, y, x, w

// shuffle. in a weird and roundabout way. this is neat, but I wonder how they thought of this...
// TODO check, is this actually getting turned into a vector on any platform? This is something where I wonder how smart the compiler can be.
// TODO in the only place (I think) in the code that uses this, it has no reason to use a shuffle at all... why does it? maybe it does, I guess
__attribute__ ((const, always_inline))
inline __m128 _mm_shuffle_ps(__m128 a, __m128 b, uint8_t z, uint8_t y, uint8_t x, uint8_t w) {
    __m128 ret;
    int mask __attribute__ ((vector_size (16))) = {w, x, y+4, z+4};
    ret = __builtin_shuffle(a, b, mask);
    return ret;
}

#endif
