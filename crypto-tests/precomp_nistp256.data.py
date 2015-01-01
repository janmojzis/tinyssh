import binascii

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


def scalarmult(P, e):
        if e == 0: return [0,0]
        if e == 1: return P
        Q = scalarmult(P, e/2)
        Q = dbl(Q)
        if e & 1: Q = add(Q, P)
        return Q

def pack(n):
        n = n % p
        r = ""
        for i in range(32):
                r = "%s%s" % (chr(n & 255), r)
                n = n >> 8
        return r


def unpack(s):
        r = 0

        for i in range(32):
                r <<= 8
                r += ord(s[i])
        return r



def publickey(sk):
        a = unpack(sk)
        A = scalarmult(B, a)
        return "%s%s" % (pack(A[0]), pack(A[1]))



f = open("precomp.txt")

def l(line):
        l = ""
        for i in range(64):
                if i == 63:
                        l += "0x%s%s" % (line[2*i], line[2*i+1])
                else:
                        l += "0x%s%s," % (line[2*i], line[2*i+1])
        return l


while True:
        line=f.readline()
        if len(line) == 0:
                f.close()
                break
        print "{", l(binascii.hexlify(publickey(binascii.unhexlify(line[0:-1])))), "},"


