# alcvx
Aligned C Vector Extension single-header math library

## Status

### DONE
- integral scalar types i.e. f32, f64, b8, u8, s8, i8, c8
- arbitrary length arr(n,t) and vec(n,t) power of two types and ALCVX load/cast
- bitceil, isarray, countof, countargs, emptyargs, perm
- OpenMP SIMD linear dot product and sum
- vec(n,t) cross3 product using perm3
- bitceil replacement for stdc_bit_ceil

### TODO
- array initializer list duplicate for copy assign for types of arbitrary
  length based on [HolyBlackCat/macro_sequence_for](https://github.com/HolyBlackCat/macro_sequence_for)
- emulated recursion iteration based on `__COUNTER__` for integer sequence
- determinant, laplace expansion and cross4 
- levi-civita symbol
- generic n-dim hodge star operator
- motor/rotor/quat and oct/sed/ten types

```c
vec(3,f32) a = {4,2,0};
vec(3,f32) b = dup(a);
```

## Usage

```c
#include <stdlib.h>
#include <stdio.h>
#include <alcvx.h>

int main(int argc, char** argv)
{
     vec(3,u16) src[2] = { {1,2,3}, {4,5,6} };
     vec(3,u16) dst;
     dup_ext(dst,src[0]);
     printf("%hu %hu %hu %hu\n", dst[0],dst[1],dst[2],dst[3]);

     exit(EXIT_SUCCESS);
}
```

```sh
[1.000000 2.000000 3.000000 0.000000]  4/12
[1.000000 4.000000 9.000000 0.000000] 16/16
[1 2 3 0]  8/ 8
```
