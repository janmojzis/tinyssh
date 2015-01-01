b = 256
q = 2**255 - 19
l = 2**252 + 27742317777372353535851937790883648493

def expmod(b,e,m):
  if e == 0: return 1
  t = expmod(b,e/2,m)**2 % m
  if e & 1: t = (t*b) % m
  return t

def inv(x):
  return expmod(x,q-2,q)

d = -121665 * inv(121666)
I = expmod(2,(q-1)/4,q)

def xrecover(y):
  xx = (y*y-1) * inv(d*y*y+1)
  x = expmod(xx,(q+3)/8,q)
  if (x*x - xx) % q != 0: x = (x*I) % q
  if x % 2 != 0: x = q-x
  return x

By = 4 * inv(5)
Bx = xrecover(By)
B = [Bx % q,By % q]

def edwards(P,Q):
  x1 = P[0]
  y1 = P[1]
  x2 = Q[0]
  y2 = Q[1]
  x3 = (x1*y2+x2*y1) * inv(1+d*x1*x2*y1*y2)
  y3 = (y1*y2+x1*x2) * inv(1-d*x1*x2*y1*y2)
  return [x3 % q,y3 % q]

def radix32(r):
  o = ""
  r = r % q
  for i in range(8):
    o += "0x%08x," % (r & 0xffffffff)
    r = r >> 32
  return o[:-1]

Bi = B
for i in range(32):
  print "{"
  Bij = Bi
  for j in range(8):
    print " {"
    print "  {",radix32(Bij[1]+Bij[0]),"},"
    print "  {",radix32(Bij[1]-Bij[0]),"},"
    print "  {",radix32(2*d*Bij[0]*Bij[1]),"},"
    Bij = edwards(Bij,Bi)
    print " },"
  print "},"
  for k in range(8):
    Bi = edwards(Bi,Bi)
