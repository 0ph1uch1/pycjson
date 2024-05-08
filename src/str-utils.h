#ifndef STR_UTILS_H
#define STR_UTILS_H

#ifndef __aarch64__
#include <immintrin.h> // AVX
#define mm256_greater8u_mask(a, b) _mm256_movemask_epi8(_mm256_and_si256(_mm256_cmpgt_epi8(a, b), _mm256_cmpgt_epi8(m256_zero, a)))

#else
#include <arm_neon.h>

uint32_t movemask(uint8x16x2_t vector);

#define _mm256_loadu_si256(c) vld2q_u8((const uint8_t *) c)
#define _mm256_set1_epi8(v) vtrnq_u8(vdupq_n_u8(v), vdupq_n_u8(v))
#define _mm256_set1_epi16(v) vtrnq_u8(vreinterpretq_u8_u16(vdupq_n_u16(v)), vreinterpretq_u8_u16(vdupq_n_u16(v)))
#define _mm256_cmpeq_epi8(v, v2) vtrnq_u8(vceqq_u8(v.val[0], v2.val[0]), vceqq_u8(v.val[1], v2.val[1]))
#define _mm256_movemask_epi8 movemask
#define __m256i uint8x16x2_t
#define __mmask32 uint32_t
#define mm256_greater8u_mask(a, b) movemask(vtrnq_u8(vcgtq_u8(a.val[0], b.val[0]), vcgtq_u8(a.val[1], b.val[1])))

#endif

#endif //STR_UTILS_H
