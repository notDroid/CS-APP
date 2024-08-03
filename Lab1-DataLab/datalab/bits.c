/* 
 * CS:APP Data Lab 
 * 
 * <Wali Mirza, self-study>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  // Xor is 0 when x and y are the same (0, 0) or (1, 1): !((!x and !y) or (x and y)). 
  // This can also be expressed as !(!x and !y) and !(x and y).
  return (~(~x & ~y) & ~(x & y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  // 1000...0 is the smallest int, bit shift 1 to this position.
  return 1<<31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  // Use the fact that -tmax = tmax + 2, compute x + (x + 2) = 0 only true when x = tmax or -1.
  // Remove the x = 1 case by oring the result with x + 1 producing a 0 value only when x is tmax.
  x = x + 1;
  return !((x + x) | !x);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  // Create odd mask O, mask x with O to get all odd bits x_odd.
  // Use the fact that (O<<1) + 1 = ~O to subtract x_odd from itself giving 0 only when x_odd is O.

  int O = 0xAA;

  O = (O<<8) | O;
  O = (O<<16) | O;
  x = O & x;

  return !(x + (x<<1) + 2);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  // Use the fact that x + ~x = 11...1 therefore x + (~x + 1) = 0 so -x = ~x + 1.
  return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  int c, s;
  // Verify that x = 0x3X where X is 1 hexadecimal digit
  c = (x>>4) + ~0x2;

  // Check if x > '9', if sign bit off then x is in bounds (given c is 0).
  s = 0x3a + ~x; // Compute '9' - x, when x is 10 or greater the result is negative
  s = s>>31;

  // Check if both conditions hold (c and s are 0)
  return !(c | s);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  // Get !x to determine if condition is false or true.
  // Notting and adding 1 convieniently results in a mask of 000... or 111... use this to mask y and z.

  x = ~!x + 1;
  return (~x & y) | (x & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  // Use and compute condition codes for the negative x > y.
  int result, SF, OF;

  // Compute y < x by computing y - x and seeing if its negative, that is SF = y - x < 0.
  result = y + ~x + 1;
  SF = result>>31;

  // Check for signed overflow, x and y different signs and results sign is different than y's sign 
  // Meaning we crossed over + to - boundary from tmax to tmin or vice versa
  y = y>>31;
  OF = (y ^ (x>>31)) & (SF ^ y);

  // If signed overflow occurs not the result.
  return !(SF ^ OF); // Not the result again to get the positive back.
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // Use the fact the pair 0, -0 is the only pair of integers x, -x that both don't have a sign bit.
  x = x | (~x + 1);

  // Duplicate sign bit all the way down to least significant bit, produces -1 for x!=0 and 0 for x= 0.
  x = x>>31;
  
  // For x != 0 we want 0 and for x = 0 we want 1, simply add 1 to result to get this.
  return x + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5         12 = ...01100 5 bits, -12 = ...10100 5 bits, the negative requires the same # of bits unless a power of 2 in which case the positive requires 1 more.
 *            howManyBits(298) = 10       298 = ...0100101010 10 bits
 *            howManyBits(-5) = 4         -5 = ...1011 4 bits, 5 = 0101 4 bits
 *            howManyBits(0)  = 1         0 = ...0 1 bit
 *            howManyBits(-1) = 1         1 = ...1 1 bit
 *            howManyBits(0x80000000) = 32  = 10...0 32 bits, position of last 0 + 1 = how many required for negatives, positions of last 1 + 1 = how many required for positives
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  // Assume x >= 0, look for switch from 00...001##...# 0s to 1, example: 01 ans = 1 + 1
  int mask, ans, next, lz;
  // Negative numbers have the same non negative solution for -x + 1 or ~x.
  mask = x>>31;
  x = (x & ~mask) | (~x & mask);

  // Binary search for last 1 pos, 0 for no 1s, 1 for first lsb, so on.
  next = 15;
  lz = !(x>>(next)); // left side 0s?
  ans = ((!lz)<<4); // If lz = 0 add 16, lz = 1 add 0.
  
  next =  ans + 7;
  lz = !(x>>next);
  ans = ans + ((!lz)<<3);

  next = ans + 3;
  lz = !(x>>next);
  ans = ans + ((!lz)<<2);

  next = ans + 1;
  lz = !(x>>next);
  ans = ans + ((!lz)<<1);

  next = ans;
  lz = !(x>>next);
  ans = ans + !lz;

  return ans + 1;
}

//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned e;

  // Mask e
  e = uf & 0x7f800000;

  // If NaN return NaN
  if (e == 0x7f800000)
    return uf;
  // If denormalized bit shift mantissa. (mantissa shifting into first exponent place intentional)
  if (!e)
     return (uf & 0x80000000) | (uf<<1);
    
  // If normalized increment exponent.
  return ((uf & 0xff800000) + (1<<23)) | (uf & 0x007fffff);
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int E, s, M, i;

  // Mask e then get E = e - 127
  E = ((uf>>23) & 0xff);
  E = E - 127;

  // If E >= 31 return tmin
  if (E >= 31)
    return 0x80000000;
  // If E < 0 return 0
  if (E < 0)
    return 0;
  
  // Get sign
  s = 1 - 2 * (uf>>31);

  // Get mantissa
  M = (1<<23) | (uf & 0x007fffff);

  // Shift floating point
  i = (E > 23)? (M<<(E - 23)):(M>>(23 - E));
  return s * i;
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    int shift;

    // If x > max exponent E = 127, return infinity.
    if (x > 127)
      return 0x7f800000;
    // If x >= min exponent E = -126, compute e = x + 127 and shift into exponent position.
    if (x >= -126)
      return (x + 127)<<23;
    
    shift = -127 - x;

    // Extremely close to 0, neither exponent nor mantissa small enough to measure, return 0.
    if (shift > 22)
      return 0;
    
    // Denormalized value, e = 0 use mantissa f.
    return (1<<22)>>shift;
}
