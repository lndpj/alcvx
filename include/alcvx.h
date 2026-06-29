#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdalign.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * standard scalar types                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Compiler dependent common defines                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if defined(_MSC_VER)
#if !defined(__cplusplus)
#define inline __inline
#endif
#define ALIGN(x) __declspec(align(x))
#define INLINE inline __force_inline __flatten __declspec(nothrow)
#define CONST __declspec(noalias)
#else
#if !defined(inline)
#define inline __inline__
#endif
#define ALIGN(x) __attribute__((aligned(x)))
#define INLINE inline __attribute__((always_inline,flatten,nothrow))
#define CONST __attribute__((const))
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * bitceil replacement for stdc_bit_ceil               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define sh0(x) (uintmax_t)(   (x) | (   (x) >> (1 << 0)))
#define sh1(x) (uintmax_t)(sh0(x) | (sh0(x) >> (1 << 1)))
#define sh2(x) (uintmax_t)(sh1(x) | (sh1(x) >> (1 << 2)))
#define sh3(x) (uintmax_t)(sh2(x) | (sh2(x) >> (1 << 3)))
#define sh4(x) (uintmax_t)(sh3(x) | (sh3(x) >> (1 << 4)))
#define sh5(x) (uintmax_t)(sh4(x) | (sh4(x) >> (1 << 5)))
#define bitceil(x) (const __typeof__(x))(sh5(((uintmax_t)(x)) - 1) + 1)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * preprocessor macro variable argument count          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define countargs(...) (0 __VA_OPT__(+sizeof((__typeof__(__VA_ARGS__)[]){__VA_ARGS__})/sizeof(__VA_ARGS__)))
#define emptyargs(...) (true __VA_OPT__(-1))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * array verification and element count                *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define isarray(a  ) __builtin_choose_expr(__builtin_types_compatible_p(typeof((a)[0]) [], typeof((a))), true, false)
#undef  countof
#define countof(a  ) (sizeof((a))/sizeof((a)[0]))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * cast conversions                                    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define ALCVX(src,n) (*(vec(typeof(src[0]),n)*)src)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * n-dim static array type (aligned if n == bitceil(n) *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define arr(n,t) ALIGN((n == bitceil(n) ? n : 1) * __alignof__(t)) __typeof__(__typeof__(t)[n])

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * n-dim simd extension vector type (n == bitceil(n))  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#if defined(__GNUC__) && !defined(__clang__) && !defined(_MSC_VER)
#define vec(n,t) __typeof__(__attribute__((vector_size(bitceil(n) * __alignof__(__typeof__(t))))) __typeof__(t))
#else
#if defined(__clang__)
#define vec(n,t) __typeof__(__attribute__((ext_vector_type(bitceil(n)))) __typeof__(t))
#else
#define vec(n,t) arr(n,t)
#endif
#endif

typedef vec(2,vec_t) vec2_t;
typedef vec(3,vec_t) vec3_t;
typedef vec(4,vec_t) vec4_t;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * array/vector terminal output function               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define eout() puts("")
#define sout(_pre,_s,_post) printf("%s%+e%s",(_pre),(double)(_s),(_post))
#define vout(_pre, _v, _post, _n, _align) \
({ \
 	printf("%s[",_pre); \
	 _Pragma("omp simd") \
	 for(size_t i = 0; i < _n; i++) \
	printf("%+e%s", (double)(_v)[i], i == (_n - 1) ? "]" : " "); \
	printf("%s",_post); \
	if((bool)_align) \
	printf(": %4zu/%4zu", __alignof__((_v)), sizeof((_v))); \
})

/* vector duplicate */
#define vdup(_n,_dst,_src) \
({ \
	_Pragma("omp simd") \
	for(size_t i = 0; i < _n; i++) \
	(_dst)[i] = (__typeof__((_dst)[0]))(_src)[i]; \
})

/* vector set */
#define vset(_v,...) \
({ \
	static_assert(!emptyargs(__VA_ARGS__)); \
	constexpr size_t _n = countargs(__VA_ARGS__); \
	__typeof__(__VA_ARGS__)* args = (__typeof__(__VA_ARGS__)[]){__VA_ARGS__}; \
	vdup(_n,_v,args); \
	(_v); \
})

/* vector permute */
#define vperm(_v,...) \
({ \
	static_assert(!emptyargs(__VA_ARGS__)); \
	constexpr size_t _n = countargs(__VA_ARGS__); \
	__typeof__(__VA_ARGS__)* args = (__typeof__(__VA_ARGS__)[]){__VA_ARGS__}; \
	vec(_n,__typeof__((_v)[0])) _dst; \
	_Pragma("omp simd") \
	for(size_t i = 0; i < _n; i++) \
		(_dst)[i] = (_v)[(size_t)(args)[i]]; \
	(_dst); \
})

/* permute v and pad fill bit ceiled rest with w */
#define vwfillperm(_w,_v,...) \
({ \
	static_assert(!emptyargs(__VA_ARGS__)); \
	constexpr size_t _n = countargs(__VA_ARGS__); \
	__typeof__(__VA_ARGS__)* args = (__typeof__(__VA_ARGS__)[]){__VA_ARGS__}; \
	vec(_n + 1,__typeof__((_v)[0])) _dst; \
	_Pragma("omp simd") \
	for(size_t i = 0; i < _n; i++) \
		(_dst)[i] = (_v)[(size_t)(args)[i]]; \
	_Pragma("omp simd") \
	for(size_t i = _n; i < bitceil(_n+1); i++) \
		(_dst)[i] = (__typeof__((_dst)[0]))_w; \
	(_dst); \
})

/* vector accumulate sum */
#define  vsum(_n,_src,_t,_i) \
({ \
	_t _dst = (_t)_i; \
	_Pragma("omp simd reduction(+:_dst)") \
	for(size_t _j = 0; _j < _n; _j++) \
		_dst += (_t)(_src)[_j]; \
	_dst; \
})

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *            reduction based vector products          *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define vred(_n,_a,_b,_op,_t,_i) \
({ \
	_t _dst = (_t)_i; \
	_Pragma("omp simd reduction(+:_dst)") \
	for(size_t i = 0; i < _n; i++) \
		_dst += (_t)(_a)[i] _op (_b)[i]; \
	_dst; \
})

/* vector dot product */
#define vdot(_n,_a,_b,_t,_i) vred(_n,_a,_b,*,_t,_i)
#define vdot3(_a,_b) vdot(3,_a,_b,__typeof__((_a)[0] * (_b)[0]),0)
#define vdot4(_a,_b) vdot(4,_a,_b,__typeof__((_a)[0] * (_b)[0]),0)

/* vector length */
#define vlen(_n,_v,_squared) _squared ? (double)vred(_n,_v,_v,*,double,0) : (double)sqrt(vred(_n,_v,_v,*,double,0))
#define vlen3(_v,_squared) vlen(3,_v,_squared)
#define vlen4(_v,_squared) vlen(3,_v,_squared)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *              cross vector products                  *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#define vcross3(_a,_b) (vwfillperm(0,_a,1,2,0) * vwfillperm(0,_b,2,0,1)) - (vwfillperm(0,_a,2,0,1) * vwfillperm(0,_b,1,2,0))
#define vcross2(_a,_d) ((_d & 0x1) ? -1 : 1) * vperm(_a,1,0)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                     *
 *               hat/vee/hodge operators               *
 *                                                     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/*
#define cross4(a,b,c) (vec(typeof((a)[0]),4))\
{ det(perm3(a,1,2,3), perm3(b,1,2,3), perm3(c,1,2,3)), \
 -det(perm3(a,0,2,3), perm3(b,0,2,3), perm3(c,0,2,3)), \
  det(perm3(a,0,1,3), perm3(b,0,1,3), perm3(c,0,1,3)), \
 -det(perm3(a,0,1,2), perm3(b,0,1,2), perm3(c,0,1,2)) }
*/
