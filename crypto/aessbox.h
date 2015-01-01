/* Author: Peter Schwabe, ported from an assembly implementation by Emilia Käsper
 Date: 2009-03-19
 Public domain 

Modifications by J. Mojzis (20140630):
renamed common.h -> aessbox.h
removed unnecessary macros
*/

#ifndef _AESSBOX_H_____
#define _AESSBOX_H_____

#define aessbox(b0, b1, b2, b3, b4, b5, b6, b7, t0, t1, t2, t3, s0, s1, s2, s3) \
	InBasisChange(b0, b1, b2, b3, b4, b5, b6, b7); \
	Inv_GF256(b6, b5, b0, b3, b7, b1, b4, b2, t0, t1, t2, t3, s0, s1, s2, s3); \
	OutBasisChange(b7, b1, b4, b2, b6, b5, b0, b3); \


#define InBasisChange(b0, b1, b2, b3, b4, b5, b6, b7) \
	xor2(&b5, &b6);\
	xor2(&b2, &b1);\
	xor2(&b5, &b0);\
	xor2(&b6, &b2);\
	xor2(&b3, &b0);\
	;\
	xor2(&b6, &b3);\
	xor2(&b3, &b7);\
	xor2(&b3, &b4);\
	xor2(&b7, &b5);\
	xor2(&b3, &b1);\
	;\
	xor2(&b4, &b5);\
	xor2(&b2, &b7);\
	xor2(&b1, &b5);\
 
#define OutBasisChange(b0, b1, b2, b3, b4, b5, b6, b7) \
	xor2(&b0, &b6);\
	xor2(&b1, &b4);\
	xor2(&b2, &b0);\
	xor2(&b4, &b6);\
	xor2(&b6, &b1);\
	;\
	xor2(&b1, &b5);\
	xor2(&b5, &b3);\
	xor2(&b2, &b5);\
	xor2(&b3, &b7);\
	xor2(&b7, &b5);\
	;\
	xor2(&b4, &b7);\
	
#define Mul_GF4(x0, x1, y0, y1, t0) \
	copy2(&t0, &y0);\
	xor2(&t0, &y1);\
	and2(&t0, &x0);\
	xor2(&x0, &x1);\
	and2(&x0, &y1);\
	and2(&x1, &y0);\
	xor2(&x0, &x1);\
	xor2(&x1, &t0);\
	
#define Mul_GF4_N(x0, x1, y0, y1, t0) \
	copy2(&t0, &y0);\
	xor2(&t0, &y1);\
	and2(&t0, &x0);\
	xor2(&x0, &x1);\
	and2(&x0, &y1);\
	and2(&x1, &y0);\
	xor2(&x1, &x0);\
	xor2(&x0, &t0);\
	
#define Mul_GF4_2(x0, x1, x2, x3, y0, y1, t0, t1) \
	copy2(&t0, = y0);\
	xor2(&t0, &y1);\
	copy2(&t1, &t0);\
	and2(&t0, &x0);\
	and2(&t1, &x2);\
	xor2(&x0, &x1);\
	xor2(&x2, &x3);\
	and2(&x0, &y1);\
	and2(&x2, &y1);\
	and2(&x1, &y0);\
	and2(&x3, &y0);\
	xor2(&x0, &x1);\
	xor2(&x2, &x3);\
	xor2(&x1, &t0);\
	xor2(&x3, &t1);\
	
#define Mul_GF16(x0, x1, x2, x3, y0, y1, y2, y3, t0, t1, t2, t3) \
	copy2(&t0, &x0);\
	copy2(&t1, &x1);\
	Mul_GF4(x0, x1, y0, y1, t2);\
	xor2(&t0, &x2);\
	xor2(&t1, &x3);\
	xor2(&y0, &y2);\
	xor2(&y1, &y3);\
	Mul_GF4_N(t0, t1, y0, y1, t2);\
	Mul_GF4(x2, x3, y2, y3, t3);\
	;\
	xor2(&x0, &t0);\
	xor2(&x2, &t0);\
	xor2(&x1, &t1);\
	xor2(&x3, &t1);\
			
#define Mul_GF16_2(x0, x1, x2, x3, x4, x5, x6, x7, y0, y1, y2, y3, t0, t1, t2, t3) \
	copy2(&t0, &x0);\
	copy2(&t1, &x1);\
	Mul_GF4(x0, x1, y0, y1, t2);\
	xor2(&t0, &x2);\
	xor2(&t1, &x3);\
	xor2(&y0, &y2);\
	xor2(&y1, &y3);\
	Mul_GF4_N(t0, t1, y0, y1, t3);\
	Mul_GF4(x2, x3, y2, y3, t2);\
	;\
	xor2(&x0, &t0);\
	xor2(&x2, &t0);\
	xor2(&x1, &t1);\
	xor2(&x3, &t1);\
	;\
	copy2(&t0, &x4);\
	copy2(&t1, &x5);\
	xor2(&t0, &x6);\
	xor2(&t1, &x7);\
	Mul_GF4_N(t0, t1, y0, y1, t3);\
	Mul_GF4(x6, x7, y2, y3, t2);\
	xor2(&y0, &y2);\
	xor2(&y1, &y3);\
	Mul_GF4(x4, x5, y0, y1, t3);\
	;\
	xor2(&x4, &t0);\
	xor2(&x6, &t0);\
	xor2(&x5, &t1);\
	xor2(&x7, &t1);\
	
#define Inv_GF16(x0, x1, x2, x3, t0, t1, t2, t3) \
	copy2(&t0, &x1);\
	copy2(&t1, &x0);\
	and2(&t0, &x3);\
	or2(&t1, &x2);\
	copy2(&t2, &x1);\
	copy2(&t3, &x0);\
	or2(&t2, &x2);\
	or2(&t3, &x3);\
	xor2(&t2, &t3);\
	;\
	xor2(&t0, &t2);\
	xor2(&t1, &t2);\
	;\
	Mul_GF4_2(x0, x1, x2, x3, t1, t0, t2, t3);\

	
#define Inv_GF256(x0,  x1, x2, x3, x4, x5, x6, x7, t0, t1, t2, t3, s0, s1, s2, s3) \
	copy2(&t3, &x4);\
	copy2(&t2, &x5);\
	copy2(&t1, &x1);\
	copy2(&s1, &x7);\
	copy2(&s0, &x0);\
	;\
	xor2(&t3, &x6);\
	xor2(&t2, &x7);\
	xor2(&t1, &x3);\
	xor2(&s1, &x6);\
	xor2(&s0, &x2);\
	;\
	copy2(&s2, &t3);\
	copy2(&t0, &t2);\
	copy2(&s3, &t3);\
	;\
	or2(&t2, &t1);\
	or2(&t3, &s0);\
	xor2(&s3, &t0);\
	and2(&s2, &s0);\
	and2(&t0, &t1);\
	xor2(&s0, &t1);\
	and2(&s3, &s0);\
	copy2(&s0, &x3);\
	xor2(&s0, &x2);\
	and2(&s1, &s0);\
	xor2(&t3, &s1);\
	xor2(&t2, &s1);\
	copy2(&s1, &x4);\
	xor2(&s1, &x5);\
	copy2(&s0, &x1);\
	copy2(&t1, &s1);\
	xor2(&s0, &x0);\
	or2(&t1, &s0);\
	and2(&s1, &s0);\
	xor2(&t0, &s1);\
	xor2(&t3, &s3);\
	xor2(&t2, &s2);\
	xor2(&t1, &s3);\
	xor2(&t0, &s2);\
	xor2(&t1, &s2);\
	copy2(&s0, &x7);\
	copy2(&s1, &x6);\
	copy2(&s2, &x5);\
	copy2(&s3, &x4);\
	and2(&s0, &x3);\
	and2(&s1, &x2);\
	and2(&s2, &x1);\
	or2(&s3, &x0);\
	xor2(&t3, &s0);\
	xor2(&t2, &s1);\
	xor2(&t1, &s2);\
	xor2(&t0, &s3);\
  ;\
  copy2(&s0, &t3);\
  xor2(&s0, &t2);\
  and2(&t3, &t1);\
  copy2(&s2, &t0);\
  xor2(&s2, &t3);\
  copy2(&s3, &s0);\
  and2(&s3, &s2);\
  xor2(&s3, &t2);\
  copy2(&s1, &t1);\
  xor2(&s1, &t0);\
  xor2(&t3, &t2);\
  and2(&s1, &t3);\
  xor2(&s1, &t0);\
  xor2(&t1, &s1);\
  copy2(&t2, &s2);\
  xor2(&t2, &s1);\
  and2(&t2, &t0);\
  xor2(&t1, &t2);\
  xor2(&s2, &t2);\
  and2(&s2, &s3);\
  xor2(&s2, &s0);\
  ;\
  Mul_GF16_2(x0, x1, x2, x3, x4, x5, x6, x7, s3, s2, s1, t1, s0, t0, t2, t3);\
  	
#endif
