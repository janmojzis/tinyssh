#!/usr/bin/env python3

import re
import sys

machinedesc = {}

# note for qhasm users:
# the machine descriptions here have, e.g.,
# int8 meaning where the C ABI stores an int8,
# such as a 64-bit integer register on amd64

# ===== amd64

machinedesc['amd64'] = r'''

int8 r:var/r=int8:
int16 r:var/r=int16:
int32 r:var/r=int32:
int64 r:var/r=int64:

int8 r s:var/r=int8:var/s=int8:
int16 r s:var/r=int16:var/s=int16:
int32 r s:var/r=int32:var/s=int32:
int64 r s:var/r=int64:var/s=int64:

int8 r s t:var/r=int8:var/s=int8:var/t=int8:
int16 r s t:var/r=int16:var/s=int16:var/t=int16:
int32 r s t:var/r=int32:var/s=int32:var/t=int32:
int64 r s t:var/r=int64:var/s=int64:var/t=int64:

int8 r s t u:var/r=int8:var/s=int8:var/t=int8:var/u=int8:
int16 r s t u:var/r=int16:var/s=int16:var/t=int16:var/u=int16:
int32 r s t u:var/r=int32:var/s=int32:var/t=int32:var/u=int32:
int64 r s t u:var/r=int64:var/s=int64:var/t=int64:var/u=int64:

r = 0:>r=int8:asm/xorb >r,>r:>?cc:
r = 0:>r=int16:asm/xorw >r,>r:>?cc:
r = 0:>r=int32:asm/xorl >r,>r:>?cc:
r = 0:>r=int64:asm/xorq >r,>r:>?cc:

r = n:>r=int8:#n:asm/movb $#n,>r:
r = n:>r=int16:#n:asm/movw $#n,>r:
r = n:>r=int32:#n:asm/movl $#n,>r:
r = n:>r=int64:#n:asm/movq $#n,>r:

r = -n:>r=int8:#n:asm/movb $-#n,>r:
r = -n:>r=int16:#n:asm/movw $-#n,>r:
r = -n:>r=int32:#n:asm/movl $-#n,>r:
r = -n:>r=int64:#n:asm/movq $-#n,>r:

r signed>>= n:+r=int8:#n:asm/sarb $#n,+r:>?cc:
r signed>>= n:+r=int16:#n:asm/sarw $#n,+r:>?cc:
r signed>>= n:+r=int32:#n:asm/sarl $#n,+r:>?cc:
r signed>>= n:+r=int64:#n:asm/sarq $#n,+r:>?cc:

r signed>>= s:+r=int8:<s=int8#4:asm/sarb %%cl,+r:>?cc:
r signed>>= s:+r=int16:<s=int16#4:asm/sarw %%cl,+r:>?cc:
r signed>>= s:+r=int32:<s=int32#4:asm/sarl %%cl,+r:>?cc:
r signed>>= s:+r=int64:<s=int64#4:asm/sarq %%cl,+r:>?cc:

r unsigned>>= n:+r=int8:#n:asm/shrb $#n,+r:>?cc:
r unsigned>>= n:+r=int16:#n:asm/shrw $#n,+r:>?cc:
r unsigned>>= n:+r=int32:#n:asm/shrl $#n,+r:>?cc:
r unsigned>>= n:+r=int64:#n:asm/shrq $#n,+r:>?cc:

r unsigned>>= s:+r=int8:<s=int8#4:asm/shrb %%cl,+r:>?cc:
r unsigned>>= s:+r=int16:<s=int16#4:asm/shrw %%cl,+r:>?cc:
r unsigned>>= s:+r=int32:<s=int32#4:asm/shrl %%cl,+r:>?cc:
r unsigned>>= s:+r=int64:<s=int64#4:asm/shrq %%cl,+r:>?cc:

r <<= n:+r=int8:#n:asm/shlb $#n,+r:>?cc:
r <<= n:+r=int16:#n:asm/shlw $#n,+r:>?cc:
r <<= n:+r=int32:#n:asm/shll $#n,+r:>?cc:
r <<= n:+r=int64:#n:asm/shlq $#n,+r:>?cc:

r <<= s:+r=int8:<s=int8#4:asm/shlb %%cl,+r:>?cc:
r <<= s:+r=int16:<s=int16#4:asm/shlw %%cl,+r:>?cc:
r <<= s:+r=int32:<s=int32#4:asm/shll %%cl,+r:>?cc:
r <<= s:+r=int64:<s=int64#4:asm/shlq %%cl,+r:>?cc:

r &= n:+r=int8:#n:asm/andb $#n,+r:>?cc:
r &= n:+r=int16:#n:asm/andw $#n,+r:>?cc:
r &= n:+r=int32:#n:asm/andl $#n,+r:>?cc:
r &= n:+r=int64:#n:asm/andq $#n,+r:>?cc:

r & (int8) s:<r=int32:<s=int32:asm/testb <s%8,<r%8:>?cc:
r & s:<r=int16:<s=int16:asm/testw <s,<r:>?cc:
r & s:<r=int32:<s=int32:asm/testl <s,<r:>?cc:
r & s:<r=int64:<s=int64:asm/testq <s,<r:>?cc:

r - (int8) s:<r=int32:<s=int32:asm/cmpb <s%8,<r%8:>?cc:
r - s:<r=int16:<s=int16:asm/cmpw <s,<r:>?cc:
r - s:<r=int32:<s=int32:asm/cmpl <s,<r:>?cc:
r - s:<r=int64:<s=int64:asm/cmpq <s,<r:>?cc:

r = s:>r=int16:<s=int16:asm/movw <s,>r:
r = s:>r=int32:<s=int32:asm/movl <s,>r:
r = s:>r=int64:<s=int64:asm/movq <s,>r:

r = s if signed<:+r=int16:<s=int16:<?cc:asm/cmovlw <s,+r:
r = s if signed<:+r=int32:<s=int32:<?cc:asm/cmovll <s,+r:
r = s if signed<:+r=int64:<s=int64:<?cc:asm/cmovlq <s,+r:

r = s if signed>:+r=int16:<s=int16:<?cc:asm/cmovgw <s,+r:
r = s if signed>:+r=int32:<s=int32:<?cc:asm/cmovgl <s,+r:
r = s if signed>:+r=int64:<s=int64:<?cc:asm/cmovgq <s,+r:

r = s if signed<=:+r=int16:<s=int16:<?cc:asm/cmovlew <s,+r:
r = s if signed<=:+r=int32:<s=int32:<?cc:asm/cmovlel <s,+r:
r = s if signed<=:+r=int64:<s=int64:<?cc:asm/cmovleq <s,+r:

r = s if unsigned<:+r=int16:<s=int16:<?cc:asm/cmovbw <s,+r:
r = s if unsigned<:+r=int32:<s=int32:<?cc:asm/cmovbl <s,+r:
r = s if unsigned<:+r=int64:<s=int64:<?cc:asm/cmovbq <s,+r:

r = s if unsigned>:+r=int16:<s=int16:<?cc:asm/cmovaw <s,+r:
r = s if unsigned>:+r=int32:<s=int32:<?cc:asm/cmoval <s,+r:
r = s if unsigned>:+r=int64:<s=int64:<?cc:asm/cmovaq <s,+r:

r = s if unsigned<=:+r=int16:<s=int16:<?cc:asm/cmovbew <s,+r:
r = s if unsigned<=:+r=int32:<s=int32:<?cc:asm/cmovbel <s,+r:
r = s if unsigned<=:+r=int64:<s=int64:<?cc:asm/cmovbeq <s,+r:

r = s if =:+r=int16:<s=int16:<?cc:asm/cmovew <s,+r:
r = s if =:+r=int32:<s=int32:<?cc:asm/cmovel <s,+r:
r = s if =:+r=int64:<s=int64:<?cc:asm/cmoveq <s,+r:

r = s if !=:+r=int16:<s=int16:<?cc:asm/cmovnew <s,+r:
r = s if !=:+r=int32:<s=int32:<?cc:asm/cmovnel <s,+r:
r = s if !=:+r=int64:<s=int64:<?cc:asm/cmovneq <s,+r:

r = numbottomzeros_tricky s:>r=int16:<s=int16:>?cc:asm/bsfw <s,>r:
r = numbottomzeros_tricky s:>r=int32:<s=int32:>?cc:asm/bsfl <s,>r:
r = numbottomzeros_tricky s:>r=int64:<s=int64:>?cc:asm/bsfq <s,>r:

'''

# ===== arm64

machinedesc['arm64'] = r'''

int8 r:var/r=int8:
int16 r:var/r=int16:
int32 r:var/r=int32:
int64 r:var/r=int64:

int8 r s:var/r=int8:var/s=int8:
int16 r s:var/r=int16:var/s=int16:
int32 r s:var/r=int32:var/s=int32:
int64 r s:var/r=int64:var/s=int64:

int8 r s t:var/r=int8:var/s=int8:var/t=int8:
int16 r s t:var/r=int16:var/s=int16:var/t=int16:
int32 r s t:var/r=int32:var/s=int32:var/t=int32:
int64 r s t:var/r=int64:var/s=int64:var/t=int64:

int8 r s t u:var/r=int8:var/s=int8:var/t=int8:var/u=int8:
int16 r s t u:var/r=int16:var/s=int16:var/t=int16:var/u=int16:
int32 r s t u:var/r=int32:var/s=int32:var/t=int32:var/u=int32:
int64 r s t u:var/r=int64:var/s=int64:var/t=int64:var/u=int64:

r = s signed>> n:>r=int32:<s=int32:#n:asm/asr >r,<s,#n:
r = s signed>> n:>r=int64:<s=int64:#n:asm/asr >r,<s,#n:

r = s unsigned>> n:>r=int32:<s=int32:#n:asm/lsr >r,<s,#n:
r = s unsigned>> n:>r=int64:<s=int64:#n:asm/lsr >r,<s,#n:

r = s signed>> t:>r=int8:<s=int8:<t=int8:asm/asr >r,<s,<t:
r = s signed>> t:>r=int16:<s=int16:<t=int16:asm/asr >r,<s,<t:
r = s signed>> t:>r=int32:<s=int32:<t=int32:asm/asr >r,<s,<t:
r = s signed>> t:>r=int64:<s=int64:<t=int64:asm/asr >r,<s,<t:

r = s unsigned>> t:>r=int8:<s=int8:<t=int8:asm/lsr >r,<s,<t:
r = s unsigned>> t:>r=int16:<s=int16:<t=int16:asm/lsr >r,<s,<t:
r = s unsigned>> t:>r=int32:<s=int32:<t=int32:asm/lsr >r,<s,<t:
r = s unsigned>> t:>r=int64:<s=int64:<t=int64:asm/lsr >r,<s,<t:

r = s << t:>r=int8:<s=int8:<t=int8:asm/lsl >r,<s,<t:
r = s << t:>r=int16:<s=int16:<t=int16:asm/lsl >r,<s,<t:
r = s << t:>r=int32:<s=int32:<t=int32:asm/lsl >r,<s,<t:
r = s << t:>r=int64:<s=int64:<t=int64:asm/lsl >r,<s,<t:

r = s | n:>r=int8:<s=int8:#n:asm/orr >r,<s,#n:
r = s | n:>r=int16:<s=int16:#n:asm/orr >r,<s,#n:
r = s | -n:>r=int8:<s=int8:#n:asm/orr >r,<s,-#n:
r = s | -n:>r=int16:<s=int16:#n:asm/orr >r,<s,-#n:

r = (int8) s:>r=int8:<s=int8:asm/sxtb >r,<s:
r = (uint8) s:>r=int8:<s=int8:asm/and >r,<s,255:
r = (int16) s:>r=int16:<s=int16:asm/sxth >r,<s:
r = (uint16) s:>r=int16:<s=int16:asm/and >r,<s,65535:

r = s & n:>r=int8:<s=int8:#n:asm/and >r,<s,#n:
r = s & n:>r=int16:<s=int16:#n:asm/and >r,<s,#n:

r & n:<r=int8:#n:asm/tst <r,#n:>?cc:
r & n:<r=int16:#n:asm/tst <r,#n:>?cc:

r - n:<r=int8:#n:asm/cmp <r,#n:>?cc:
r - n:<r=int16:#n:asm/cmp <r,#n:>?cc:
r - n:<r=int32:#n:asm/cmp <r,#n:>?cc:
r - n:<r=int64:#n:asm/cmp <r,#n:>?cc:

r - s:<r=int8:<s=int8:asm/cmp <r,<s:>?cc:
r - s:<r=int16:<s=int16:asm/cmp <r,<s:>?cc:
r - s:<r=int32:<s=int32:asm/cmp <r,<s:>?cc:
r - s:<r=int64:<s=int64:asm/cmp <r,<s:>?cc:

r - (int8) s:<r=int8:<s=int8:asm/cmp <r,<s,sxtb:>?cc:
r - (uint8) s:<r=int8:<s=int8:asm/cmp <r,<s,uxtb:>?cc:
r - (int16) s:<r=int16:<s=int16:asm/cmp <r,<s,sxth:>?cc:
r - (uint16) s:<r=int16:<s=int16:asm/cmp <r,<s,uxth:>?cc:

r = -1 if != else 0:>r=int8:<?cc:asm/csetm >r,ne:
r = -1 if != else 0:>r=int16:<?cc:asm/csetm >r,ne:
r = -1 if != else 0:>r=int32:<?cc:asm/csetm >r,ne:
r = -1 if != else 0:>r=int64:<?cc:asm/csetm >r,ne:

r = -1 if = else 0:>r=int8:<?cc:asm/csetm >r,eq:
r = -1 if = else 0:>r=int16:<?cc:asm/csetm >r,eq:
r = -1 if = else 0:>r=int32:<?cc:asm/csetm >r,eq:
r = -1 if = else 0:>r=int64:<?cc:asm/csetm >r,eq:

r = -1 if signed< else 0:>r=int8:<?cc:asm/csetm >r,lt:
r = -1 if signed< else 0:>r=int16:<?cc:asm/csetm >r,lt:
r = -1 if signed< else 0:>r=int32:<?cc:asm/csetm >r,lt:
r = -1 if signed< else 0:>r=int64:<?cc:asm/csetm >r,lt:

r = -1 if signed> else 0:>r=int8:<?cc:asm/csetm >r,gt:
r = -1 if signed> else 0:>r=int16:<?cc:asm/csetm >r,gt:
r = -1 if signed> else 0:>r=int32:<?cc:asm/csetm >r,gt:
r = -1 if signed> else 0:>r=int64:<?cc:asm/csetm >r,gt:

r = -1 if signed<= else 0:>r=int8:<?cc:asm/csetm >r,le:
r = -1 if signed<= else 0:>r=int16:<?cc:asm/csetm >r,le:
r = -1 if signed<= else 0:>r=int32:<?cc:asm/csetm >r,le:
r = -1 if signed<= else 0:>r=int64:<?cc:asm/csetm >r,le:

r = -1 if unsigned< else 0:>r=int8:<?cc:asm/csetm >r,lo:
r = -1 if unsigned< else 0:>r=int16:<?cc:asm/csetm >r,lo:
r = -1 if unsigned< else 0:>r=int32:<?cc:asm/csetm >r,lo:
r = -1 if unsigned< else 0:>r=int64:<?cc:asm/csetm >r,lo:

r = -1 if unsigned<= else 0:>r=int8:<?cc:asm/csetm >r,ls:
r = -1 if unsigned<= else 0:>r=int16:<?cc:asm/csetm >r,ls:
r = -1 if unsigned<= else 0:>r=int32:<?cc:asm/csetm >r,ls:
r = -1 if unsigned<= else 0:>r=int64:<?cc:asm/csetm >r,ls:

r = 1 if != else 0:>r=int8:<?cc:asm/cset >r,ne:
r = 1 if != else 0:>r=int16:<?cc:asm/cset >r,ne:
r = 1 if != else 0:>r=int32:<?cc:asm/cset >r,ne:
r = 1 if != else 0:>r=int64:<?cc:asm/cset >r,ne:

r = 1 if = else 0:>r=int8:<?cc:asm/cset >r,eq:
r = 1 if = else 0:>r=int16:<?cc:asm/cset >r,eq:
r = 1 if = else 0:>r=int32:<?cc:asm/cset >r,eq:
r = 1 if = else 0:>r=int64:<?cc:asm/cset >r,eq:

r = 1 if signed< else 0:>r=int8:<?cc:asm/cset >r,lt:
r = 1 if signed< else 0:>r=int16:<?cc:asm/cset >r,lt:
r = 1 if signed< else 0:>r=int32:<?cc:asm/cset >r,lt:
r = 1 if signed< else 0:>r=int64:<?cc:asm/cset >r,lt:

r = 1 if signed> else 0:>r=int8:<?cc:asm/cset >r,gt:
r = 1 if signed> else 0:>r=int16:<?cc:asm/cset >r,gt:
r = 1 if signed> else 0:>r=int32:<?cc:asm/cset >r,gt:
r = 1 if signed> else 0:>r=int64:<?cc:asm/cset >r,gt:

r = 1 if signed<= else 0:>r=int8:<?cc:asm/cset >r,le:
r = 1 if signed<= else 0:>r=int16:<?cc:asm/cset >r,le:
r = 1 if signed<= else 0:>r=int32:<?cc:asm/cset >r,le:
r = 1 if signed<= else 0:>r=int64:<?cc:asm/cset >r,le:

r = 1 if unsigned< else 0:>r=int8:<?cc:asm/cset >r,lo:
r = 1 if unsigned< else 0:>r=int16:<?cc:asm/cset >r,lo:
r = 1 if unsigned< else 0:>r=int32:<?cc:asm/cset >r,lo:
r = 1 if unsigned< else 0:>r=int64:<?cc:asm/cset >r,lo:

r = 1 if unsigned<= else 0:>r=int8:<?cc:asm/cset >r,ls:
r = 1 if unsigned<= else 0:>r=int16:<?cc:asm/cset >r,ls:
r = 1 if unsigned<= else 0:>r=int32:<?cc:asm/cset >r,ls:
r = 1 if unsigned<= else 0:>r=int64:<?cc:asm/cset >r,ls:

r = s if signed< else t:>r=int8:<s=int8:<t=int8:<?cc:asm/csel >r,<s,<t,lt:
r = s if signed< else t:>r=int16:<s=int16:<t=int16:<?cc:asm/csel >r,<s,<t,lt:
r = s if signed< else t:>r=int32:<s=int32:<t=int32:<?cc:asm/csel >r,<s,<t,lt:
r = s if signed< else t:>r=int64:<s=int64:<t=int64:<?cc:asm/csel >r,<s,<t,lt:

r = s if unsigned< else t:>r=int8:<s=int8:<t=int8:<?cc:asm/csel >r,<s,<t,lo:
r = s if unsigned< else t:>r=int16:<s=int16:<t=int16:<?cc:asm/csel >r,<s,<t,lo:
r = s if unsigned< else t:>r=int32:<s=int32:<t=int32:<?cc:asm/csel >r,<s,<t,lo:
r = s if unsigned< else t:>r=int64:<s=int64:<t=int64:<?cc:asm/csel >r,<s,<t,lo:

r = bitrev32 s:>r=int8:<s=int8:asm/rbit >r,<s:
r = bitrev32 s:>r=int16:<s=int16:asm/rbit >r,<s:
r = bitrev32 s:>r=int32:<s=int32:asm/rbit >r,<s:
r = bitrev64 s:>r=int64:<s=int64:asm/rbit >r,<s:

r = numbottomzeros s:>r=int8:<s=int8:asm/clz >r,<s:
r = numbottomzeros s:>r=int16:<s=int16:asm/clz >r,<s:
r = numbottomzeros s:>r=int32:<s=int32:asm/clz >r,<s:
r = numbottomzeros s:>r=int64:<s=int64:asm/clz >r,<s:

r = -(1 & (s unsigned>> n)):>r=int8:<s=int8:#n:asm/sbfx >r,<s,#n,1:
r = -(1 & (s unsigned>> n)):>r=int16:<s=int16:#n:asm/sbfx >r,<s,#n,1:
r = -(1 & (s unsigned>> n)):>r=int32:<s=int32:#n:asm/sbfx >r,<s,#n,1:
r = -(1 & (s unsigned>> n)):>r=int64:<s=int64:#n:asm/sbfx >r,<s,#n,1:

r = 1 & (s unsigned>> n):>r=int8:<s=int8:#n:asm/ubfx >r,<s,#n,1:
r = 1 & (s unsigned>> n):>r=int16:<s=int16:#n:asm/ubfx >r,<s,#n,1:
r = 1 & (s unsigned>> n):>r=int32:<s=int32:#n:asm/ubfx >r,<s,#n,1:
r = 1 & (s unsigned>> n):>r=int64:<s=int64:#n:asm/ubfx >r,<s,#n,1:

'''

# ===== arm32

machinedesc['arm32'] = r'''

int8 r:var/r=int8:
int16 r:var/r=int16:
int32 r:var/r=int32:
int64 r:var/r=int64:

int8 r s:var/r=int8:var/s=int8:
int16 r s:var/r=int16:var/s=int16:
int32 r s:var/r=int32:var/s=int32:
int64 r s:var/r=int64:var/s=int64:

int8 r s t:var/r=int8:var/s=int8:var/t=int8:
int16 r s t:var/r=int16:var/s=int16:var/t=int16:
int32 r s t:var/r=int32:var/s=int32:var/t=int32:
int64 r s t:var/r=int64:var/s=int64:var/t=int64:

int8 r s t u:var/r=int8:var/s=int8:var/t=int8:var/u=int8:
int16 r s t u:var/r=int16:var/s=int16:var/t=int16:var/u=int16:
int32 r s t u:var/r=int32:var/s=int32:var/t=int32:var/u=int32:
int64 r s t u:var/r=int64:var/s=int64:var/t=int64:var/u=int64:

r - n:<r=int8:#n:asm/cmp <r,###n:>?cc:
r - n:<r=int16:#n:asm/cmp <r,###n:>?cc:
r - n:<r=int32:#n:asm/cmp <r,###n:>?cc:

r = n if !=:>r=int8:#n:<?cc:asm/movne >r,###n:
r = n if !=:>r=int16:#n:<?cc:asm/movne >r,###n:
r = n if !=:>r=int32:#n:<?cc:asm/movne >r,###n:
r.lo = n if !=:>r=int64:#n:<?cc:asm/movne >r%L,###n:
r.hi = n if !=:>r=int64:#n:<?cc:asm/movne >r%H,###n:

r = -n if !=:>r=int8:#n:<?cc:asm/movne >r,##-#n:
r = -n if !=:>r=int16:#n:<?cc:asm/movne >r,##-#n:
r = -n if !=:>r=int32:#n:<?cc:asm/movne >r,##-#n:
r.lo = -n if !=:>r=int64:#n:<?cc:asm/movne >r%L,##-#n:
r.hi = -n if !=:>r=int64:#n:<?cc:asm/movne >r%H,##-#n:

r = (int8) s:>r=int8:<s=int8:asm/sxtb >r,<s:
r = (uint8) s:>r=int8:<s=int8:asm/uxtb >r,<s:
r = (int16) s:>r=int16:<s=int16:asm/sxth >r,<s:
r = (uint16) s:>r=int16:<s=int16:asm/uxth >r,<s:

r.lo = n:>r=int64:#n:asm/mov >r%H,###n:
r.hi = n:>r=int64:#n:asm/mov >r%H,###n:

r.lo = s.lo:>r=int64:<s=int64:asm/mov >r%L,<s%L:
r.lo = s.hi:>r=int64:<s=int64:asm/mov >r%L,<s%H:
r.hi = s.lo:>r=int64:<s=int64:asm/mov >r%H,<s%L:
r.hi = s.hi:>r=int64:<s=int64:asm/mov >r%H,<s%H:

r = -s:>r=int8:<s=int8:asm/neg >r,<s:
r = -s:>r=int16:<s=int16:asm/neg >r,<s:
r = -s:>r=int32:<s=int32:asm/neg >r,<s:
r.lo = -s.lo:>r=int64:<s=int64:asm/neg >r%L,<s%L:
r.lo = -s.hi:>r=int64:<s=int64:asm/neg >r%L,<s%H:
r.hi = -s.lo:>r=int64:<s=int64:asm/neg >r%H,<s%L:
r.hi = -s.hi:>r=int64:<s=int64:asm/neg >r%H,<s%H:

r = s - n:>r=int8:<s=int8:#n:asm/sub >r,<s,###n:
r = s - n:>r=int16:<s=int16:#n:asm/sub >r,<s,###n:
r = s - n:>r=int32:<s=int32:#n:asm/sub >r,<s,###n:
r.lo = s.lo - n:>r=int64:<s=int64:#n:asm/sub >r%L,<s%L,###n:
r.lo = s.hi - n:>r=int64:<s=int64:#n:asm/sub >r%L,<s%H,###n:
r.hi = s.lo - n:>r=int64:<s=int64:#n:asm/sub >r%H,<s%L,###n:
r.hi = s.hi - n:>r=int64:<s=int64:#n:asm/sub >r%H,<s%H,###n:

flags, r = s - n:>r=int8:<s=int8:#n:asm/subs >r,<s,###n:>?cc:
flags, r = s - n:>r=int16:<s=int16:#n:asm/subs >r,<s,###n:>?cc:
flags, r = s - n:>r=int32:<s=int32:#n:asm/subs >r,<s,###n:>?cc:
flags, r.lo = s.lo - n:>r=int64:<s=int64:#n:asm/subs >r%L,<s%L,###n:>?cc:
flags, r.lo = s.hi - n:>r=int64:<s=int64:#n:asm/subs >r%L,<s%H,###n:>?cc:
flags, r.hi = s.lo - n:>r=int64:<s=int64:#n:asm/subs >r%H,<s%L,###n:>?cc:
flags, r.hi = s.hi - n:>r=int64:<s=int64:#n:asm/subs >r%H,<s%H,###n:>?cc:

r = n - s:>r=int8:<s=int8:#n:asm/rsb >r,<s,###n:
r = n - s:>r=int16:<s=int16:#n:asm/rsb >r,<s,###n:
r = n - s:>r=int32:<s=int32:#n:asm/rsb >r,<s,###n:
r.lo = n - s.lo:>r=int64:<s=int64:#n:asm/rsb >r%L,<s%L,###n:
r.lo = n - s.hi:>r=int64:<s=int64:#n:asm/rsb >r%L,<s%H,###n:
r.hi = n - s.lo:>r=int64:<s=int64:#n:asm/rsb >r%H,<s%L,###n:
r.hi = n - s.hi:>r=int64:<s=int64:#n:asm/rsb >r%H,<s%H,###n:

r = s & n:>r=int8:<s=int8:#n:asm/and >r,<s,###n:
r = s & n:>r=int16:<s=int16:#n:asm/and >r,<s,###n:
r = s & n:>r=int32:<s=int32:#n:asm/and >r,<s,###n:
r.lo = s.lo & n:>r=int64:<s=int64:#n:asm/and >r%L,<s%L,###n:
r.lo = s.hi & n:>r=int64:<s=int64:#n:asm/and >r%L,<s%H,###n:
r.hi = s.lo & n:>r=int64:<s=int64:#n:asm/and >r%H,<s%L,###n:
r.hi = s.hi & n:>r=int64:<s=int64:#n:asm/and >r%H,<s%H,###n:

r = s << n:>r=int8:<s=int8:#n:asm/lsl >r,<s,###n:
r = s << n:>r=int16:<s=int16:#n:asm/lsl >r,<s,###n:
r = s << n:>r=int32:<s=int32:#n:asm/lsl >r,<s,###n:
r.lo = s.lo << n:>r=int64:<s=int64:#n:asm/lsl >r%L,<s%L,###n:
r.lo = s.hi << n:>r=int64:<s=int64:#n:asm/lsl >r%L,<s%H,###n:
r.hi = s.lo << n:>r=int64:<s=int64:#n:asm/lsl >r%H,<s%L,###n:
r.hi = s.hi << n:>r=int64:<s=int64:#n:asm/lsl >r%H,<s%H,###n:

r = s signed>> n:>r=int8:<s=int8:#n:asm/asr >r,<s,###n:
r = s signed>> n:>r=int16:<s=int16:#n:asm/asr >r,<s,###n:
r = s signed>> n:>r=int32:<s=int32:#n:asm/asr >r,<s,###n:
r.lo = s.lo signed>> n:>r=int64:<s=int64:#n:asm/asr >r%L,<s%L,###n:
r.lo = s.hi signed>> n:>r=int64:<s=int64:#n:asm/asr >r%L,<s%H,###n:
r.hi = s.lo signed>> n:>r=int64:<s=int64:#n:asm/asr >r%H,<s%L,###n:
r.hi = s.hi signed>> n:>r=int64:<s=int64:#n:asm/asr >r%H,<s%H,###n:

r = s unsigned>> n:>r=int8:<s=int8:#n:asm/lsr >r,<s,###n:
r = s unsigned>> n:>r=int16:<s=int16:#n:asm/lsr >r,<s,###n:
r = s unsigned>> n:>r=int32:<s=int32:#n:asm/lsr >r,<s,###n:
r.lo = s.lo unsigned>> n:>r=int64:<s=int64:#n:asm/lsr >r%L,<s%L,###n:
r.lo = s.hi unsigned>> n:>r=int64:<s=int64:#n:asm/lsr >r%L,<s%H,###n:
r.hi = s.lo unsigned>> n:>r=int64:<s=int64:#n:asm/lsr >r%H,<s%L,###n:
r.hi = s.hi unsigned>> n:>r=int64:<s=int64:#n:asm/lsr >r%H,<s%H,###n:

flags, r = s | t:>r=int8:<s=int8:<t=int8:asm/orrs >r,<s,<t:>?cc:
flags, r = s | t:>r=int16:<s=int16:<t=int16:asm/orrs >r,<s,<t:>?cc:
flags, r = s | t:>r=int32:<s=int32:<t=int32:asm/orrs >r,<s,<t:>?cc:
flags, r.lo = s.lo | t.lo:>r=int64:<s=int64:<t=int64:asm/orrs >r%L,<s%L,<t%L:>?cc:
flags, r.lo = s.lo | t.hi:>r=int64:<s=int64:<t=int64:asm/orrs >r%L,<s%L,<t%H:>?cc:
flags, r.lo = s.hi | t.lo:>r=int64:<s=int64:<t=int64:asm/orrs >r%L,<s%H,<t%L:>?cc:
flags, r.lo = s.hi | t.hi:>r=int64:<s=int64:<t=int64:asm/orrs >r%L,<s%H,<t%H:>?cc:
flags, r.hi = s.lo | t.lo:>r=int64:<s=int64:<t=int64:asm/orrs >r%H,<s%L,<t%L:>?cc:
flags, r.hi = s.lo | t.hi:>r=int64:<s=int64:<t=int64:asm/orrs >r%H,<s%L,<t%H:>?cc:
flags, r.hi = s.hi | t.lo:>r=int64:<s=int64:<t=int64:asm/orrs >r%H,<s%H,<t%L:>?cc:
flags, r.hi = s.hi | t.hi:>r=int64:<s=int64:<t=int64:asm/orrs >r%H,<s%H,<t%H:>?cc:

r = s << t:>r=int8:<s=int8:<t=int8:asm/lsl >r,<s,<t:
r = s << t:>r=int16:<s=int16:<t=int16:asm/lsl >r,<s,<t:
r = s << t:>r=int32:<s=int32:<t=int32:asm/lsl >r,<s,<t:
r.lo = s.lo << t.lo:>r=int64:<s=int64:<t=int64:asm/lsl >r%L,<s%L,<t%L:
r.lo = s.lo << t.hi:>r=int64:<s=int64:<t=int64:asm/lsl >r%L,<s%L,<t%H:
r.lo = s.hi << t.lo:>r=int64:<s=int64:<t=int64:asm/lsl >r%L,<s%H,<t%L:
r.lo = s.hi << t.hi:>r=int64:<s=int64:<t=int64:asm/lsl >r%L,<s%H,<t%H:
r.hi = s.lo << t.lo:>r=int64:<s=int64:<t=int64:asm/lsl >r%H,<s%L,<t%L:
r.hi = s.lo << t.hi:>r=int64:<s=int64:<t=int64:asm/lsl >r%H,<s%L,<t%H:
r.hi = s.hi << t.lo:>r=int64:<s=int64:<t=int64:asm/lsl >r%H,<s%H,<t%L:
r.hi = s.hi << t.hi:>r=int64:<s=int64:<t=int64:asm/lsl >r%H,<s%H,<t%H:

r = s signed>> t:>r=int8:<s=int8:<t=int8:asm/asr >r,<s,<t:
r = s signed>> t:>r=int16:<s=int16:<t=int16:asm/asr >r,<s,<t:
r = s signed>> t:>r=int32:<s=int32:<t=int32:asm/asr >r,<s,<t:
r.lo = s.lo signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/asr >r%L,<s%L,<t%L:
r.lo = s.lo signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/asr >r%L,<s%L,<t%H:
r.lo = s.hi signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/asr >r%L,<s%H,<t%L:
r.lo = s.hi signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/asr >r%L,<s%H,<t%H:
r.hi = s.lo signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/asr >r%H,<s%L,<t%L:
r.hi = s.lo signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/asr >r%H,<s%L,<t%H:
r.hi = s.hi signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/asr >r%H,<s%H,<t%L:
r.hi = s.hi signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/asr >r%H,<s%H,<t%H:

r = s unsigned>> t:>r=int8:<s=int8:<t=int8:asm/lsr >r,<s,<t:
r = s unsigned>> t:>r=int16:<s=int16:<t=int16:asm/lsr >r,<s,<t:
r = s unsigned>> t:>r=int32:<s=int32:<t=int32:asm/lsr >r,<s,<t:
r.lo = s.lo unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/lsr >r%L,<s%L,<t%L:
r.lo = s.lo unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/lsr >r%L,<s%L,<t%H:
r.lo = s.hi unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/lsr >r%L,<s%H,<t%L:
r.lo = s.hi unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/lsr >r%L,<s%H,<t%H:
r.hi = s.lo unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/lsr >r%H,<s%L,<t%L:
r.hi = s.lo unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/lsr >r%H,<s%L,<t%H:
r.hi = s.hi unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/lsr >r%H,<s%H,<t%L:
r.hi = s.hi unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/lsr >r%H,<s%H,<t%H:

r.lo = s.lo | (t.lo << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%L,lsl <u%L:
r.lo = s.lo | (t.lo << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%L,lsl <u%H:
r.lo = s.lo | (t.hi << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%H,lsl <u%L:
r.lo = s.lo | (t.hi << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%H,lsl <u%H:
r.lo = s.hi | (t.lo << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%L,lsl <u%L:
r.lo = s.hi | (t.lo << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%L,lsl <u%H:
r.lo = s.hi | (t.hi << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%H,lsl <u%L:
r.lo = s.hi | (t.hi << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%H,lsl <u%H:
r.hi = s.lo | (t.lo << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%L,lsl <u%L:
r.hi = s.lo | (t.lo << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%L,lsl <u%H:
r.hi = s.lo | (t.hi << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%H,lsl <u%L:
r.hi = s.lo | (t.hi << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%H,lsl <u%H:
r.hi = s.hi | (t.lo << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%L,lsl <u%L:
r.hi = s.hi | (t.lo << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%L,lsl <u%H:
r.hi = s.hi | (t.hi << u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%H,lsl <u%L:
r.hi = s.hi | (t.hi << u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%H,lsl <u%H:

r.lo = s.lo | (t.lo signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%L,<t%L,asr <u%L:
r.lo = s.lo | (t.lo signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%L,<t%L,asr <u%H:
r.lo = s.lo | (t.hi signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%L,<t%H,asr <u%L:
r.lo = s.lo | (t.hi signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%L,<t%H,asr <u%H:
r.lo = s.hi | (t.lo signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%H,<t%L,asr <u%L:
r.lo = s.hi | (t.lo signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%H,<t%L,asr <u%H:
r.lo = s.hi | (t.hi signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%H,<t%H,asr <u%L:
r.lo = s.hi | (t.hi signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%L,<s%H,<t%H,asr <u%H:
r.hi = s.lo | (t.lo signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%L,<t%L,asr <u%L:
r.hi = s.lo | (t.lo signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%L,<t%L,asr <u%H:
r.hi = s.lo | (t.hi signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%L,<t%H,asr <u%L:
r.hi = s.lo | (t.hi signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%L,<t%H,asr <u%H:
r.hi = s.hi | (t.lo signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%H,<t%L,asr <u%L:
r.hi = s.hi | (t.lo signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%H,<t%L,asr <u%H:
r.hi = s.hi | (t.hi signed>> u.lo) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%H,<t%H,asr <u%L:
r.hi = s.hi | (t.hi signed>> u.hi) if unsigned>=:>r=int64:<s=int64:<t=int64:<u=int64:asm/orrhs >r%H,<s%H,<t%H,asr <u%H:

r.lo = s.lo | (t.lo unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%L,lsr <u%L:
r.lo = s.lo | (t.lo unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%L,lsr <u%H:
r.lo = s.lo | (t.hi unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%H,lsr <u%L:
r.lo = s.lo | (t.hi unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%L,<t%H,lsr <u%H:
r.lo = s.hi | (t.lo unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%L,lsr <u%L:
r.lo = s.hi | (t.lo unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%L,lsr <u%H:
r.lo = s.hi | (t.hi unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%H,lsr <u%L:
r.lo = s.hi | (t.hi unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%L,<s%H,<t%H,lsr <u%H:
r.hi = s.lo | (t.lo unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%L,lsr <u%L:
r.hi = s.lo | (t.lo unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%L,lsr <u%H:
r.hi = s.lo | (t.hi unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%H,lsr <u%L:
r.hi = s.lo | (t.hi unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%L,<t%H,lsr <u%H:
r.hi = s.hi | (t.lo unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%L,lsr <u%L:
r.hi = s.hi | (t.lo unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%L,lsr <u%H:
r.hi = s.hi | (t.hi unsigned>> u.lo):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%H,lsr <u%L:
r.hi = s.hi | (t.hi unsigned>> u.hi):>r=int64:<s=int64:<t=int64:<u=int64:asm/orr >r%H,<s%H,<t%H,lsr <u%H:

'''

# ===== sparc32

machinedesc['sparc32'] = r'''

int8 r:var/r=int8:
int16 r:var/r=int16:
int32 r:var/r=int32:
int64 r:var/r=int64:

int8 r s:var/r=int8:var/s=int8:
int16 r s:var/r=int16:var/s=int16:
int32 r s:var/r=int32:var/s=int32:
int64 r s:var/r=int64:var/s=int64:

int8 r s t:var/r=int8:var/s=int8:var/t=int8:
int16 r s t:var/r=int16:var/s=int16:var/t=int16:
int32 r s t:var/r=int32:var/s=int32:var/t=int32:
int64 r s t:var/r=int64:var/s=int64:var/t=int64:

int8 r s t u:var/r=int8:var/s=int8:var/t=int8:var/u=int8:
int16 r s t u:var/r=int16:var/s=int16:var/t=int16:var/u=int16:
int32 r s t u:var/r=int32:var/s=int32:var/t=int32:var/u=int32:
int64 r s t u:var/r=int64:var/s=int64:var/t=int64:var/u=int64:

r = carry:>r=int8:asm/addx %%g0,0,>r:
r = carry:>r=int16:asm/addx %%g0,0,>r:
r = carry:>r=int32:asm/addx %%g0,0,>r:
r.lo = carry:>r=int64:asm/addx %%g0,0,>r%L:
r.hi = carry:>r=int64:asm/addx %%g0,0,>r%H:

r = carry - n:>r=int8:#n:asm/addx %%g0,-#n,>r:
r = carry - n:>r=int16:#n:asm/addx %%g0,-#n,>r:
r = carry - n:>r=int32:#n:asm/addx %%g0,-#n,>r:
r.lo = carry - n:>r=int64:#n:asm/addx %%g0,-#n,>r%L:
r.hi = carry - n:>r=int64:#n:asm/addx %%g0,-#n,>r%H:

r = -carry:>r=int8:asm/subx %%g0,0,>r:
r = -carry:>r=int16:asm/subx %%g0,0,>r:
r = -carry:>r=int32:asm/subx %%g0,0,>r:
r.lo = -carry:>r=int64:asm/subx %%g0,0,>r%L:
r.hi = -carry:>r=int64:asm/subx %%g0,0,>r%H:

r = n - carry:>r=int8:#n:asm/subx %%g0,-#n,>r:
r = n - carry:>r=int16:#n:asm/subx %%g0,-#n,>r:
r = n - carry:>r=int32:#n:asm/subx %%g0,-#n,>r:
r.lo = n - carry:>r=int64:#n:asm/subx %%g0,-#n,>r%L:
r.hi = n - carry:>r=int64:#n:asm/subx %%g0,-#n,>r%H:

r = s - t:>r=int8:<s=int8:<t=int8:asm/sub <s,<t,>r:
r = s - t:>r=int16:<s=int16:<t=int16:asm/sub <s,<t,>r:
r = s - t:>r=int32:<s=int32:<t=int32:asm/sub <s,<t,>r:

carry, r.lo = s.lo - t.lo:>r=int64:<s=int64:<t=int64:asm/subcc <s%L,<t%L,>r%L:>?cc:
carry, r.lo = s.lo - t.hi:>r=int64:<s=int64:<t=int64:asm/subcc <s%L,<t%H,>r%L:>?cc:
carry, r.lo = s.hi - t.lo:>r=int64:<s=int64:<t=int64:asm/subcc <s%H,<t%L,>r%L:>?cc:
carry, r.lo = s.hi - t.hi:>r=int64:<s=int64:<t=int64:asm/subcc <s%H,<t%H,>r%L:>?cc:
carry, r.hi = s.lo - t.lo:>r=int64:<s=int64:<t=int64:asm/subcc <s%L,<t%L,>r%H:>?cc:
carry, r.hi = s.lo - t.hi:>r=int64:<s=int64:<t=int64:asm/subcc <s%L,<t%H,>r%H:>?cc:
carry, r.hi = s.hi - t.lo:>r=int64:<s=int64:<t=int64:asm/subcc <s%H,<t%L,>r%H:>?cc:
carry, r.hi = s.hi - t.hi:>r=int64:<s=int64:<t=int64:asm/subcc <s%H,<t%H,>r%H:>?cc:

r.lo = s.lo - t.lo - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%L,<t%L,>r%L:
r.lo = s.lo - t.hi - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%L,<t%H,>r%L:
r.lo = s.hi - t.lo - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%H,<t%L,>r%L:
r.lo = s.hi - t.hi - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%H,<t%H,>r%L:
r.hi = s.lo - t.lo - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%L,<t%L,>r%H:
r.hi = s.lo - t.hi - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%L,<t%H,>r%H:
r.hi = s.hi - t.lo - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%H,<t%L,>r%H:
r.hi = s.hi - t.hi - carry:>r=int64:<s=int64:<t=int64:asm/subx <s%H,<t%H,>r%H:

0 - r:<r=int8:asm/cmp %%g0,<r:>?cc:
0 - r:<r=int16:asm/cmp %%g0,<r:>?cc:
0 - r:<r=int32:asm/cmp %%g0,<r:>?cc:
0 - r.lo:<r=int64:asm/cmp %%g0,<r%L:>?cc:
0 - r.hi:<r=int64:asm/cmp %%g0,<r%H:>?cc:

r - 0:<r=int8:asm/cmp <r,%%g0:>?cc:
r - 0:<r=int16:asm/cmp <r,%%g0:>?cc:
r - 0:<r=int32:asm/cmp <r,%%g0:>?cc:
r.lo - 0:<r=int64:asm/cmp <r%L,%%g0:>?cc:
r.hi - 0:<r=int64:asm/cmp <r%H,%%g0:>?cc:

r.lo = n:>r=int64:#n:asm/mov #n,>r%H:
r.hi = n:>r=int64:#n:asm/mov #n,>r%H:

r.lo = s.lo:>r=int64:<s=int64:asm/mov <s%L,>r%L:
r.lo = s.hi:>r=int64:<s=int64:asm/mov <s%H,>r%L:
r.hi = s.lo:>r=int64:<s=int64:asm/mov <s%L,>r%H:
r.hi = s.hi:>r=int64:<s=int64:asm/mov <s%H,>r%H:

r = -s:>r=int8:<s=int8:asm/neg <s,>r:
r = -s:>r=int16:<s=int16:asm/neg <s,>r:
r = -s:>r=int32:<s=int32:asm/neg <s,>r:
r.lo = -s.lo:>r=int64:<s=int64:asm/neg <s%L,>r%L:
r.lo = -s.hi:>r=int64:<s=int64:asm/neg <s%H,>r%L:
r.hi = -s.lo:>r=int64:<s=int64:asm/neg <s%L,>r%H:
r.hi = -s.hi:>r=int64:<s=int64:asm/neg <s%H,>r%H:

r = ~s:>r=int8:<s=int8:asm/not <s,>r:
r = ~s:>r=int16:<s=int16:asm/not <s,>r:
r = ~s:>r=int32:<s=int32:asm/not <s,>r:
r.lo = ~s.lo:>r=int64:<s=int64:asm/not <s%L,>r%L:
r.lo = ~s.hi:>r=int64:<s=int64:asm/not <s%H,>r%L:
r.hi = ~s.lo:>r=int64:<s=int64:asm/not <s%L,>r%H:
r.hi = ~s.hi:>r=int64:<s=int64:asm/not <s%H,>r%H:

r = s + n:>r=int8:<s=int8:#n:asm/add <s,#n,>r:
r = s + n:>r=int16:<s=int16:#n:asm/add <s,#n,>r:
r = s + n:>r=int32:<s=int32:#n:asm/add <s,#n,>r:
r.lo = s.lo + n:>r=int64:<s=int64:#n:asm/add <s%L,#n,>r%L:
r.lo = s.hi + n:>r=int64:<s=int64:#n:asm/add <s%H,#n,>r%L:
r.hi = s.lo + n:>r=int64:<s=int64:#n:asm/add <s%L,#n,>r%H:
r.hi = s.hi + n:>r=int64:<s=int64:#n:asm/add <s%H,#n,>r%H:

r = s & n:>r=int8:<s=int8:#n:asm/and <s,#n,>r:
r = s & n:>r=int16:<s=int16:#n:asm/and <s,#n,>r:
r = s & n:>r=int32:<s=int32:#n:asm/and <s,#n,>r:
r.lo = s.lo & n:>r=int64:<s=int64:#n:asm/and <s%L,#n,>r%L:
r.lo = s.hi & n:>r=int64:<s=int64:#n:asm/and <s%H,#n,>r%L:
r.hi = s.lo & n:>r=int64:<s=int64:#n:asm/and <s%L,#n,>r%H:
r.hi = s.hi & n:>r=int64:<s=int64:#n:asm/and <s%H,#n,>r%H:

r = s << n:>r=int8:<s=int8:#n:asm/sll <s,#n,>r:
r = s << n:>r=int16:<s=int16:#n:asm/sll <s,#n,>r:
r = s << n:>r=int32:<s=int32:#n:asm/sll <s,#n,>r:
r.lo = s.lo << n:>r=int64:<s=int64:#n:asm/sll <s%L,#n,>r%L:
r.lo = s.hi << n:>r=int64:<s=int64:#n:asm/sll <s%H,#n,>r%L:
r.hi = s.lo << n:>r=int64:<s=int64:#n:asm/sll <s%L,#n,>r%H:
r.hi = s.hi << n:>r=int64:<s=int64:#n:asm/sll <s%H,#n,>r%H:

r = s signed>> n:>r=int8:<s=int8:#n:asm/sra <s,#n,>r:
r = s signed>> n:>r=int16:<s=int16:#n:asm/sra <s,#n,>r:
r = s signed>> n:>r=int32:<s=int32:#n:asm/sra <s,#n,>r:
r.lo = s.lo signed>> n:>r=int64:<s=int64:#n:asm/sra <s%L,#n,>r%L:
r.lo = s.hi signed>> n:>r=int64:<s=int64:#n:asm/sra <s%H,#n,>r%L:
r.hi = s.lo signed>> n:>r=int64:<s=int64:#n:asm/sra <s%L,#n,>r%H:
r.hi = s.hi signed>> n:>r=int64:<s=int64:#n:asm/sra <s%H,#n,>r%H:

r = s unsigned>> n:>r=int8:<s=int8:#n:asm/srl <s,#n,>r:
r = s unsigned>> n:>r=int16:<s=int16:#n:asm/srl <s,#n,>r:
r = s unsigned>> n:>r=int32:<s=int32:#n:asm/srl <s,#n,>r:
r.lo = s.lo unsigned>> n:>r=int64:<s=int64:#n:asm/srl <s%L,#n,>r%L:
r.lo = s.hi unsigned>> n:>r=int64:<s=int64:#n:asm/srl <s%H,#n,>r%L:
r.hi = s.lo unsigned>> n:>r=int64:<s=int64:#n:asm/srl <s%L,#n,>r%H:
r.hi = s.hi unsigned>> n:>r=int64:<s=int64:#n:asm/srl <s%H,#n,>r%H:

r = s ^ t:>r=int8:<s=int8:<t=int8:asm/xor <s,<t,>r:
r = s ^ t:>r=int16:<s=int16:<t=int16:asm/xor <s,<t,>r:
r = s ^ t:>r=int32:<s=int32:<t=int32:asm/xor <s,<t,>r:
r.lo = s.lo ^ t.lo:>r=int64:<s=int64:<t=int64:asm/xor <s%L,<t%L,>r%L:
r.lo = s.lo ^ t.hi:>r=int64:<s=int64:<t=int64:asm/xor <s%L,<t%H,>r%L:
r.lo = s.hi ^ t.lo:>r=int64:<s=int64:<t=int64:asm/xor <s%H,<t%L,>r%L:
r.lo = s.hi ^ t.hi:>r=int64:<s=int64:<t=int64:asm/xor <s%H,<t%H,>r%L:
r.hi = s.lo ^ t.lo:>r=int64:<s=int64:<t=int64:asm/xor <s%L,<t%L,>r%H:
r.hi = s.lo ^ t.hi:>r=int64:<s=int64:<t=int64:asm/xor <s%L,<t%H,>r%H:
r.hi = s.hi ^ t.lo:>r=int64:<s=int64:<t=int64:asm/xor <s%H,<t%L,>r%H:
r.hi = s.hi ^ t.hi:>r=int64:<s=int64:<t=int64:asm/xor <s%H,<t%H,>r%H:

r = s & t:>r=int8:<s=int8:<t=int8:asm/and <s,<t,>r:
r = s & t:>r=int16:<s=int16:<t=int16:asm/and <s,<t,>r:
r = s & t:>r=int32:<s=int32:<t=int32:asm/and <s,<t,>r:
r.lo = s.lo & t.lo:>r=int64:<s=int64:<t=int64:asm/and <s%L,<t%L,>r%L:
r.lo = s.lo & t.hi:>r=int64:<s=int64:<t=int64:asm/and <s%L,<t%H,>r%L:
r.lo = s.hi & t.lo:>r=int64:<s=int64:<t=int64:asm/and <s%H,<t%L,>r%L:
r.lo = s.hi & t.hi:>r=int64:<s=int64:<t=int64:asm/and <s%H,<t%H,>r%L:
r.hi = s.lo & t.lo:>r=int64:<s=int64:<t=int64:asm/and <s%L,<t%L,>r%H:
r.hi = s.lo & t.hi:>r=int64:<s=int64:<t=int64:asm/and <s%L,<t%H,>r%H:
r.hi = s.hi & t.lo:>r=int64:<s=int64:<t=int64:asm/and <s%H,<t%L,>r%H:
r.hi = s.hi & t.hi:>r=int64:<s=int64:<t=int64:asm/and <s%H,<t%H,>r%H:

r = s | t:>r=int8:<s=int8:<t=int8:asm/or <s,<t,>r:
r = s | t:>r=int16:<s=int16:<t=int16:asm/or <s,<t,>r:
r = s | t:>r=int32:<s=int32:<t=int32:asm/or <s,<t,>r:
r.lo = s.lo | t.lo:>r=int64:<s=int64:<t=int64:asm/or <s%L,<t%L,>r%L:
r.lo = s.lo | t.hi:>r=int64:<s=int64:<t=int64:asm/or <s%L,<t%H,>r%L:
r.lo = s.hi | t.lo:>r=int64:<s=int64:<t=int64:asm/or <s%H,<t%L,>r%L:
r.lo = s.hi | t.hi:>r=int64:<s=int64:<t=int64:asm/or <s%H,<t%H,>r%L:
r.hi = s.lo | t.lo:>r=int64:<s=int64:<t=int64:asm/or <s%L,<t%L,>r%H:
r.hi = s.lo | t.hi:>r=int64:<s=int64:<t=int64:asm/or <s%L,<t%H,>r%H:
r.hi = s.hi | t.lo:>r=int64:<s=int64:<t=int64:asm/or <s%H,<t%L,>r%H:
r.hi = s.hi | t.hi:>r=int64:<s=int64:<t=int64:asm/or <s%H,<t%H,>r%H:

r = s << t:>r=int8:<s=int8:<t=int8:asm/sll <s,<t,>r:
r = s << t:>r=int16:<s=int16:<t=int16:asm/sll <s,<t,>r:
r = s << t:>r=int32:<s=int32:<t=int32:asm/sll <s,<t,>r:
r.lo = s.lo << t.lo:>r=int64:<s=int64:<t=int64:asm/sll <s%L,<t%L,>r%L:
r.lo = s.lo << t.hi:>r=int64:<s=int64:<t=int64:asm/sll <s%L,<t%H,>r%L:
r.lo = s.hi << t.lo:>r=int64:<s=int64:<t=int64:asm/sll <s%H,<t%L,>r%L:
r.lo = s.hi << t.hi:>r=int64:<s=int64:<t=int64:asm/sll <s%H,<t%H,>r%L:
r.hi = s.lo << t.lo:>r=int64:<s=int64:<t=int64:asm/sll <s%L,<t%L,>r%H:
r.hi = s.lo << t.hi:>r=int64:<s=int64:<t=int64:asm/sll <s%L,<t%H,>r%H:
r.hi = s.hi << t.lo:>r=int64:<s=int64:<t=int64:asm/sll <s%H,<t%L,>r%H:
r.hi = s.hi << t.hi:>r=int64:<s=int64:<t=int64:asm/sll <s%H,<t%H,>r%H:

r = s signed>> t:>r=int8:<s=int8:<t=int8:asm/sra <s,<t,>r:
r = s signed>> t:>r=int16:<s=int16:<t=int16:asm/sra <s,<t,>r:
r = s signed>> t:>r=int32:<s=int32:<t=int32:asm/sra <s,<t,>r:
r.lo = s.lo signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/sra <s%L,<t%L,>r%L:
r.lo = s.lo signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/sra <s%L,<t%H,>r%L:
r.lo = s.hi signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/sra <s%H,<t%L,>r%L:
r.lo = s.hi signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/sra <s%H,<t%H,>r%L:
r.hi = s.lo signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/sra <s%L,<t%L,>r%H:
r.hi = s.lo signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/sra <s%L,<t%H,>r%H:
r.hi = s.hi signed>> t.lo:>r=int64:<s=int64:<t=int64:asm/sra <s%H,<t%L,>r%H:
r.hi = s.hi signed>> t.hi:>r=int64:<s=int64:<t=int64:asm/sra <s%H,<t%H,>r%H:

r = s unsigned>> t:>r=int8:<s=int8:<t=int8:asm/srl <s,<t,>r:
r = s unsigned>> t:>r=int16:<s=int16:<t=int16:asm/srl <s,<t,>r:
r = s unsigned>> t:>r=int32:<s=int32:<t=int32:asm/srl <s,<t,>r:
r.lo = s.lo unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/srl <s%L,<t%L,>r%L:
r.lo = s.lo unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/srl <s%L,<t%H,>r%L:
r.lo = s.hi unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/srl <s%H,<t%L,>r%L:
r.lo = s.hi unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/srl <s%H,<t%H,>r%L:
r.hi = s.lo unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/srl <s%L,<t%L,>r%H:
r.hi = s.lo unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/srl <s%L,<t%H,>r%H:
r.hi = s.hi unsigned>> t.lo:>r=int64:<s=int64:<t=int64:asm/srl <s%H,<t%L,>r%H:
r.hi = s.hi unsigned>> t.hi:>r=int64:<s=int64:<t=int64:asm/srl <s%H,<t%H,>r%H:

r = s & n:>r=int8:<s=int8:#n:asm/and <s,#n,>r:
r = s & n:>r=int16:<s=int16:#n:asm/and <s,#n,>r:
r = s & n:>r=int32:<s=int32:#n:asm/and <s,#n,>r:
r.lo = s.lo & n:>r=int64:<s=int64:#n:asm/and <s%L,#n,>r%L:
r.lo = s.hi & n:>r=int64:<s=int64:#n:asm/and <s%H,#n,>r%L:
r.hi = s.lo & n:>r=int64:<s=int64:#n:asm/and <s%L,#n,>r%H:
r.hi = s.hi & n:>r=int64:<s=int64:#n:asm/and <s%H,#n,>r%H:

r = (uint8) s:>r=int8:<s=int8:asm/and <s,255,>r:
r = (uint16) s:>r=int16:<s=int16:asm/sll <s,16,>r:asm/srl <r,16,>r:

'''

# ===== general processing

def isvarname(line):
  return re.fullmatch(r'\w+',line)

def tokens(line):
  return re.findall(r'\w+|[^\w\s]',line)

def process(script):
  todo = []

  machine = None
  vartype = {}
  ccinit = False

  inputvars = []
  outputvars = []
  earlyclobber = set()
  rcxvars = set()

  for line in script.split(';'):
    line = tokens(line.strip())
    if machine is None:
      assert len(line) == 1
      machine = line[0]
      assert machine in machinedesc
      continue

    foundinsn = False
    for insndesc in machinedesc[machine].splitlines():
      insndesc = insndesc.split(':')
      if len(insndesc) == 0: continue

      insnvars = []
      insnvartype = {}
      insnvarpos = {}
      insnvar2line = {}
      insnmakevars = []
      insnasm = []
      insnread = []
      insnwrite = []

      for x in insndesc[1:]:
        if x == '': continue
        if x[0] == '#':
          assert len(x) == 2
          insnvars += [x[1]]
          insnvartype[x[1]] = '#'
          continue
        if x[0] in ('<','+','>'):
          if x[1:] == '?cc':
            if x[0] != '<': insnwrite += ['?cc']
            if x[0] != '>': insnread += ['?cc']
            continue
          if x[0] != '<': insnwrite += [x[1]]
          if x[0] != '>': insnread += [x[1]]
          insnvars += [x[1]]
          assert x[2] == '='
          insnvartype[x[1]] = x[3:].split('#')[0]
          if len(x[3:].split('#')) > 1:
            insnvarpos[x[1]] = int(x[3:].split('#')[1])
          continue
        if x.startswith('var/'):
          insnvars += [x[4]]
          assert x[5] == '='
          insnvartype[x[4]] = x[6:]
          insnmakevars += [x[4]]
          continue
        if x.startswith('asm/'):
          insnasm += [x[4:]]
          continue
        raise Exception(x)

      insn = tokens(insndesc[0])

      insnpos = 0
      linepos = 0

      while True:
        if insnpos == len(insn) and linepos == len(line):
          foundinsn = True
        if insnpos == len(insn) or linepos == len(line):
          break

        x = insn[insnpos]
        y = line[linepos]

        if x in insnvars:
          if y in vartype:
            if vartype[y] != insnvartype[x]:
              break # line and insn both have variables, but different types
            if x in insnvar2line:
              if insnvar2line[x] != y:
                break # insn variable already assigned to something different in line
            else:
              insnvar2line[x] = y
          else:
            if not isvarname(y):
              break # not an acceptable variable name
            insnvar2line[x] = y
        else:
          if y in vartype:
            break # XXX: but maybe sometimes want to allow variable in line to match literal in insn
          if y != x:
            break # non-matching literals

        insnpos += 1
        linepos += 1

      if foundinsn: break

    if not foundinsn: print(line)
    assert foundinsn

    insnvar2line['?cc'] = '?cc'

    for v in insnmakevars:
      w = insnvar2line[v]
      vartype[w] = insnvartype[v]

    for v in insnvar2line:
      if v == '?cc': continue
      if insnvartype[v] == '#': continue
      w = insnvar2line[v]
      if w not in vartype:
        raise Exception(f'undefined variable {w} from {v} in {line}')

    for v in insnread:
      w = insnvar2line[v]
      if v in insnvarpos:
        assert insnvarpos[v] == 4
        assert machine == 'amd64'
        rcxvars.add(w)
      if w not in outputvars:
        if w not in inputvars:
          inputvars += [w]
        for z in outputvars:
          earlyclobber.add(z)

    for v in insnwrite:
      w = insnvar2line[v]
      if w not in outputvars:
        outputvars += [w]

    for asm in insnasm:
      todo += [(asm,insnvar2line)]

  need8 = set()

  todo2 = []
  for asm,insnvar2line in todo:
    todo2 += [('asm',)]
    while len(asm) > 0:
      if asm[0] in ('<','+','>'):
        if asm[2:].startswith('%8'):
          todo2 += [('var8',insnvar2line[asm[1]])]
          need8.add(insnvar2line[asm[1]])
          asm = asm[4:]
        elif asm[2:].startswith('%L'):
          todo2 += [('var0',insnvar2line[asm[1]])]
          asm = asm[4:]
        elif asm[2:].startswith('%H'):
          todo2 += [('var1',insnvar2line[asm[1]])]
          asm = asm[4:]
        else:
          todo2 += [('var',insnvar2line[asm[1]])]
          asm = asm[2:]
      elif asm[0] == '#':
        if asm[1:].startswith('#'):
          todo2 += [('str',asm[1])]
          asm = asm[2:]
        else:
          todo2 += [('str',insnvar2line[asm[1]])]
          asm = asm[2:]
      else:
        todo2 += [('str',asm[0])]
        asm = asm[1:]

  varnextpos = 0
  var2name = {}

  outputs = []
  for w in outputvars:
    if w == '?cc': continue
    if machine == 'arm64' and vartype[w] != 'int64': # XXX: abstract this
      var2name[w] = f'%w{varnextpos}'
    else:
      var2name[w] = f'%{varnextpos}'
    varnextpos += 1
    regtype = 'r'
    if w in need8: regtype = 'q'
    if w in rcxvars: regtype = 'c'
    if w in earlyclobber: regtype = '&'+regtype
    if w in inputvars:
      outputs += [f'"+{regtype}"({w})']
    else:
      outputs += [f'"={regtype}"({w})']

  inputs = []
  for w in inputvars:
    if w in outputvars: continue
    assert w != '?cc'
    if machine == 'arm64' and vartype[w] != 'int64': # XXX: abstract this
      var2name[w] = f'%w{varnextpos}'
    else:
      var2name[w] = f'%{varnextpos}'
    varnextpos += 1
    regtype = 'r'
    if w in need8: regtype = 'q'
    if w in rcxvars: regtype = 'c'
    inputs += [f'"{regtype}"({w})']

  clobbers = []
  for w in outputvars:
    if w != '?cc': continue
    clobbers += ['"cc"']

  lohalf64 = '%Q' if machine == 'arm32' else '%L'
  hihalf64 = '%R' if machine == 'arm32' else '%H'

  result = ''
  for T in todo2:
    if T[0] == 'asm':
      if result != '': result += '\\n '
    elif T[0] == 'var8':
      result += var2name[T[1]].replace('%','%b')
    elif T[0] == 'var0':
      result += var2name[T[1]].replace('%',lohalf64)
    elif T[0] == 'var1':
      result += var2name[T[1]].replace('%',hihalf64)
    elif T[0] == 'var':
      result += var2name[T[1]]
    else:
      assert T[0] == 'str'
      result += T[1]

  outputs = ', '.join(outputs)
  if len(outputs) > 0: outputs += ' '
  inputs = ', '.join(inputs)
  if len(inputs) > 0: inputs += ' '
  clobbers = ', '.join(clobbers)
  result = f'__asm__ ("{result}" : {outputs}: {inputs}: {clobbers})'

  return result

def substitute(program):
  result = ''
  for line in program.splitlines():
    m = re.fullmatch(r'(.*)\breadasm\("([^"]*)"\)(.*)',line)
    if m:
      result += m.group(1)+process(m.group(2))+m.group(3)+'\n'
    else:
      result += line+'\n'
  return result

def test():
  print(process('amd64; int64 X; X signed>>= 63'))
  print(process('arm64; int8 X; int8 Y; X = (int8) X; Y = (int8) Y; X - Y; X = Y if signed< else X'))
  print(process('arm64; int8 X; int8 Y; int8 Z; Z = (uint8) X; Z - (uint8) Y; Z = -1 if unsigned<= else 0'))
  print(process('arm64; int64 X; int64 Y; int64 R; int64 S; X - Y; R = X if unsigned< else Y; S = Y if unsigned< else X'))

if __name__ == '__main__':
  test()
