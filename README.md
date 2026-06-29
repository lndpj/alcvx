# alcvx
Aligned C Vector Extension single-header math library

## Status

### Types
- [x] scalar types
- [x] vec(n,t)
- [x] arr(n,t)

### Macros
- [x] countargs
- [x] emptyargs
- [x] isrray
- [ ] isvector
- [x] bitceil
- [x] vout
- [x] vperm
- [x] vwfillperm
- [x] vred
- [x] vdot
- [x] vlen
- [x] vcross3
- [x] vcross2
- [x] vset
- [x] vdup
- [ ] vhat
- [ ] vvee
- [ ] vhodge
- [ ] mdet
- [ ] vlaplace
- [ ] levicivita

### TODO
- array initializer list duplicate for copy assign for types of arbitrary
  length based on [HolyBlackCat/macro_sequence_for](https://github.com/HolyBlackCat/macro_sequence_for)
- emulated recursion iteration based on `__COUNTER__` for integer sequence
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
