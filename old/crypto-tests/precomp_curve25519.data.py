import binascii

P = 2 ** 255 - 19
A = 486662

def expmod(b, e, m):
  if e == 0: return 1
  t = expmod(b, e / 2, m) ** 2 % m
  if e & 1: t = (t * b) % m
  return t

def inv(x):
  return expmod(x, P - 2, P)

# Addition and doubling formulas taken from Appendix D of "Curve25519:
# new Diffie-Hellman speed records".

def add((xn,zn), (xm,zm), (xd,zd)):
  x = 4 * (xm * xn - zm * zn) ** 2 * zd
  z = 4 * (xm * zn - zm * xn) ** 2 * xd
  return (x % P, z % P)

def double((xn,zn)):
  x = (xn ** 2 - zn ** 2) ** 2
  z = 4 * xn * zn * (xn ** 2 + A * xn * zn + zn ** 2)
  return (x % P, z % P)

def curve25519(n, base):
  one = (base,1)
  two = double(one)
  # f(m) evaluates to a tuple containing the mth multiple and the
  # (m+1)th multiple of base.
  def f(m):
    if m == 1: return (one, two)
    (pm, pm1) = f(m / 2)
    if (m & 1):
      return (add(pm, pm1, one), double(pm1))
    return (double(pm), add(pm, pm1, one))
  ((x,z), _) = f(n)
  return (x * inv(z)) % P

def unpack(s):
  if len(s) != 32: raise ValueError('Invalid Curve25519 argument')
  return sum(ord(s[i]) << (8 * i) for i in range(32))

def pack(n):
  return ''.join([chr((n >> (8 * i)) & 255) for i in range(32)])

def clamp(n):
  n &= ~7
  n &= ~(128 << 8 * 31)
  n |= 64 << 8 * 31
  return n

def crypto_scalarmult_curve25519(n, p):
  n = clamp(unpack(n))
  p = unpack(p)
  return pack(curve25519(n, p))

def crypto_scalarmult_curve25519_base(n):
  n = clamp(unpack(n))
  return pack(curve25519(n, 9))


f = open("precomp.txt")

def l(line):

        l = ""
        for i in range(32):
                if i == 31:
                        l += "0x%s%s" % (line[2*i], line[2*i+1])
                else:
                        l += "0x%s%s," % (line[2*i], line[2*i+1])
        return l


while True:
        line=f.readline()
        if len(line) == 0:
                f.close()
                break
        print "{", l(binascii.hexlify(crypto_scalarmult_curve25519_base(binascii.unhexlify(line[0:-1])))), "},"

