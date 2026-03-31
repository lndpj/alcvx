#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdalign.h>
#include <sys/param.h>
#include <stdarg.h>
#include <string.h>

/* * * * * * * * * * * * *
 * standard scalar types *
 * * * * * * * * * * * * */
typedef float    f32;
typedef double   f64;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef  int64_t i64;
typedef  int32_t i32;
typedef  int16_t i16;
typedef  int8_t  i8;

typedef  int64_t s64;
typedef  int32_t s32;
typedef  int16_t s16;
typedef  int8_t  s8;

typedef  char     c8;
typedef  bool     b8;

typedef enum be32 : u32
{
	ALCVX_TRUE  = (const u32) true,
	ALCVX_FALSE = (const u32)!true,
} be32;

typedef  uintptr_t uptr;
typedef   intptr_t iptr;
typedef     size_t usz;
typedef    ssize_t ssz;

typedef float          vecf_t;
typedef double         vecd_t;
typedef vecf_t         vec_t;

/* * * * * * * * * * * * * *
 *   expand for permute    *
 * * * * * * * * * * * * * */
#define PARENS ()
#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

/* * * * * * * * * * * * * *
 * bitceil for pow 2 align *
 * * * * * * * * * * * * * */
#define BITOP_RUP01__(x) (             (x) | (             (x) >>  1))
#define BITOP_RUP02__(x) (BITOP_RUP01__(x) | (BITOP_RUP01__(x) >>  2))
#define BITOP_RUP04__(x) (BITOP_RUP02__(x) | (BITOP_RUP02__(x) >>  4))
#define BITOP_RUP08__(x) (BITOP_RUP04__(x) | (BITOP_RUP04__(x) >>  8))
#define BITOP_RUP16__(x) (BITOP_RUP08__(x) | (BITOP_RUP08__(x) >> 16))

#define bitceil(x) (const uint32_t)(BITOP_RUP16__(((uint32_t)(x)) - 1) + 1)

/* define array verification and element count */
#define isarray(a  ) __builtin_choose_expr(__builtin_types_compatible_p(typeof((a)[0]) [], typeof((a))), true, false)
#undef  countof
#define countof(a  ) (sizeof((a))/sizeof((a)[0]))

/* define variable argument macros */
#define countargs(...) (0 __VA_OPT__(+sizeof((typeof(__VA_ARGS__)[]){__VA_ARGS__})/sizeof(__VA_ARGS__)))
#define emptyargs(...) (true __VA_OPT__(-1))

/* add permute */
#define perm(a,...)          { __VA_OPT__(EXPAND(perm_helper(a,__VA_ARGS__))) }
#define perm_helper(a,i,...) (a)[i], __VA_OPT__(perm_again PARENS (a,__VA_ARGS__))
#define perm_again()         perm_helper

#pragma pack(push,1)
/* define different array types */
#define     arr(T,N) typeof(T[N]) /* fixed static array of arbitrary length and type */
#pragma pack(pop)
#pragma pack(push,1)
#define     vla(T  ) typeof(T[ ]) /* variable length array - VLA */
#pragma pack(pop)

#pragma pack(push,1)
/* define vector extension power of 2 sized SIMD vector types for non MSVC */
#ifndef _MSC_VER
#ifdef __clang__
#define vec_ext(T,N) typeof(T __attribute__((ext_vector_type(N))))
#else
#define vec_ext(T,N) typeof(T __attribute__((vector_size(bitceil((alignof(T) * N))))))
#endif
#endif
#pragma pack(pop)

#ifndef vec
#define vec(T,N) arr(T,N)
#endif

#define ALCVX(src,n) (*(vec(typeof(src[0]),n)*)src)

/* * * * * * * * * * * * * * *
 *     sum using OpenMP      *
 * * * * * * * * * * * * * * */
#define sum(a,n,i) \
({ \
typeof((a)[0]) _dst = i; \
_Pragma("omp simd reduction(+:_dst)") \
for(size_t j = 0; j < MIN(countof(a),n); j++) \
        _dst += (a)[j]; \
_dst; \
})

/* * * * * * * * * * * * * * *
 *     dot using OpenMP      *
 * * * * * * * * * * * * * * */
#define dot(a,b,n,i) \
({ \
typeof((a)[0]) _dst = i; \
_Pragma("omp simd reduction(+:_dst)") \
for(size_t j = 0; j < MIN(MIN(countof(a),countof(b)),n); j++) \
        _dst += (a)[j] * (b)[j]; \
_dst; \
})

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *       duplicate/copy array/vector types using       *
 *       the aligned to power of 2 padded vec_ext      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define dup(_dst,src) ({ \
_Pragma("omp simd") \
for(size_t j = 0; j < MIN(countof(src),countof(_dst)); j++) \
(_dst)[j] = (src)[j]; \
(*((vec_ext(typeof((_dst)[0]),countof(_dst))*)&(_dst)[0])); \
})

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TODO: Implement indexed sequence for arbitrary n    *
 *                                                     *
 * https://github.com/HolyBlackCat/macro_sequence_for  *
 *                                                     *
 * duplicate array using initializer list and          * 
 * make indexed sequence unrolling for loop based on   *
 * boilerplates.                                       *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
//#define dup (a) { (a)[0]...(a)[countof(a)-1]                                     }

/* define 2 to 8-dimensional vector operations */
#define dup2(a) { (a)[0], (a)[1]                                                 }
#define dup3(a) { (a)[0], (a)[1], (a)[2]                                         }
#define dup4(a) { (a)[0], (a)[1], (a)[2], (a)[3]                                 }
#define dup5(a) { (a)[0], (a)[1], (a)[2], (a)[3], (a)[4]                         }
#define dup6(a) { (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]                 }
#define dup7(a) { (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6]         }
#define dup8(a) { (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7] }

#define perm2(a,x,y    ) (vec_ext(typeof((a)[0]),2))perm((a),x,y    )
#define perm3(a,x,y,z  ) (vec_ext(typeof((a)[0]),3))perm((a),x,y,z  )
#define perm4(a,x,y,z,w) (vec_ext(typeof((a)[0]),4))perm((a),x,y,z,w)

#define sum2(a)           sum((a),2,0)
#define sum3(a)           sum((a),3,0)
#define sum4(a)           sum((a),4,0)

#define dot2(a,b)         dot((a),(b),2,0)
#define dot3(a,b)         dot((a),(b),3,0)
#define dot4(a,b)         dot((a),(b),4,0)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *              cross/hodge vector products            *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define cross2(a,d) ((d & 0x1) ? -1 : 1) * perm2(a,1,0)

/* cross3 - 3-dimensional vector product for vector extension types only
 * TODO:
 * - support vector/array types without operators
 * - n-dimensional hodge star operator vector product based on
 *   levi-civita, laplace expansion and determinant as a
 *   left contraction grade projection operator. See Clifford, Hodge, 
 */
#define cross3(a,b) \
  perm3(a,1,2,0) * perm3(b,2,0,1) \
- perm3(a,2,1,0) * perm3(b,1,2,0)


/*
#define minor(x,y,...) ({ \
vec(typeof(__VA_ARGS__), countargs(__VA_ARGS__)-1) dst[countargs(__VA_ARGS__)-1]; \
for(size_t i = 0; i < countargs(__VA_ARGS__)-1; i++) \
	for(size_t j = 0; j < countargs(__VA_ARGS__)-1; j++) \
		dst[i][j] = (i <  x && j <  y) ? (vec(typeof(__VA_ARGS__),countargs(__VA_ARGS__))[countargs(__VA_ARGS__)]){__VA_ARGS__}[i  ][j  ] : \
 	                    (i >= x && j <  y) ? (vec(typeof(__VA_ARGS__),countargs(__VA_ARGS__))[countargs(__VA_ARGS__)]){__VA_ARGS__}[i+1][j  ] : \
	                    (i <  x && j >= y) ? (vec(typeof(__VA_ARGS__),countargs(__VA_ARGS__))[countargs(__VA_ARGS__)]){__VA_ARGS__}[i  ][j+1] : \
		                                 COUNTO(vec(typeof(__VA_ARGS__),countargs(__VA_ARGS__))[countargs(__VA_ARGS__)]){__VA_ARGS__}[i+1][j+1]; \
dst; })

#define cross4(a,b,c) (vec(typeof((a)[0]),4))\
{ det(perm3(a,1,2,3), perm3(b,1,2,3), perm3(c,1,2,3)), \
 -det(perm3(a,0,2,3), perm3(b,0,2,3), perm3(c,0,2,3)), \
  det(perm3(a,0,1,3), perm3(b,0,1,3), perm3(c,0,1,3)), \
 -det(perm3(a,0,1,2), perm3(b,0,1,2), perm3(c,0,1,2)) }
*/
