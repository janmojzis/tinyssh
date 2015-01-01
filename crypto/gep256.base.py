p = 2**256 - 2**224 + 2**192 + 2**96 - 1

basepx = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
basepy = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5

B = (basepx, basepy)


def expmod(b, e, m):
        if e == 0: return 1
        t = expmod(b, e/2, m)**2 % p
        if e & 1: t = (t * b) % p
        return t

def inv(x):
        return expmod(x, p - 2, p)

def add(P, Q):
        """
        """

        x1 = P[0]
        y1 = P[1]
        x2 = Q[0]
        y2 = Q[1]

        x3 = ((y2-y1)**2)*inv((x2-x1)**2)-x1-x2
        y3 = (2*x1+x2)*(y2-y1)*inv(x2-x1)-((y2-y1)**3)*inv((x2-x1)**3)-y1

        return [x3 % p, y3 % p]

def dbl(P):
        """
        """

        x1 = P[0]
        y1 = P[1]

        x3 = (3*x1**2-3)**2 * inv((2*y1)**2)-x1-x1
        y3 = (2*x1+x1)*(3*x1**2-3)*inv((2*y1))-(3*x1**2-3)**3*inv((2*y1)**3)-y1
        return [x3 % p, y3 % p]

b = []

b.append(B)
tmp = dbl(B)
b.append(tmp)

for i in range(6):
        tmp = add(tmp, B)
        b.append(tmp)


def radix32(r):
        o = ""
        r = r % p
        for i in range(8):
                o += "0x%08x," % (r & 0xffffffff)
                r = r >> 32
        return o[:-1]


for i in range(33):

        print "{"
        for j in range(0, 8):
                 print " {"
                 print "  {",radix32(b[j][0]),"},"
                 print "  {",radix32(b[j][1]),"},"
                 print " },"
        print "},"

        for j in range(0, 8):
                for k in range(8):
                        b[j] = dbl(b[j])
