#!/usr/bin/env python3

vars = 'k','l','p','q','r','s','x','y','z'

with open('version') as f:
  version = f.read().strip()

# ===== top

csh = f'''/* auto-generated: cd cryptoint; ./autogen */
/* {version} */

#ifndef crypto_intN_h
#define crypto_intN_h

#include <inttypes.h>
#define crypto_intN intN_t
#define crypto_intN_unsigned uintN_t

#define crypto_intN_optblocker namespace_intN_optblocker
extern volatile crypto_intN crypto_intN_optblocker;

'''

cuh = f'''/* auto-generated: cd cryptoint; ./autogen */
/* {version} */

#ifndef crypto_uintN_h
#define crypto_uintN_h

#include <inttypes.h>
#define crypto_uintN uintN_t
#define crypto_uintN_signed intN_t

#define crypto_uintN_signed_optblocker namespace_uintN_signed_optblocker
extern volatile crypto_uintN_signed crypto_uintN_signed_optblocker;

'''

# ===== functions

functions = []

with open('functions') as f:
  which,fun = 'both',None
  for line in f:
    if fun is None:
      if line.startswith('#'): continue
      if line.strip() == '': continue
      if line.strip() == 'SIGNED:':
        which = 'signed'
        continue
      if line.strip() == 'UNSIGNED:':
        which = 'unsigned'
        continue
      fun = ''
    fun += line
    if line.strip() == '}':
      functions += [(which,fun)]
      which,fun = 'both',None

for which,fun in functions:
  if which in ('both','signed'):
    data = fun
    data = data.replace('TYPE','crypto_intN')
    data = data.replace('UNSIGNED','crypto_intN_unsigned')
    data = data.replace('SIGNED','crypto_intN')
    for v in vars:
      data = data.replace(v.upper(),'crypto_intN_'+v)
    csh += '__attribute__((unused))\n'
    csh += 'static inline\n'
    csh += data
    csh += '\n'

  if which in ('both','unsigned'):
    data = fun
    data = data.replace('TYPE','crypto_uintN')
    data = data.replace('UNSIGNED','crypto_uintN')
    data = data.replace('SIGNED','crypto_uintN_signed')
    for v in vars:
      data = data.replace(v.upper(),'crypto_uintN_'+v)
    cuh += '__attribute__((unused))\n'
    cuh += 'static inline\n'
    cuh += data
    cuh += '\n'

# ===== bottom

csh += '''#endif
'''

cuh += '''#endif
'''

# ===== ship it

with open('crypto_intN.h','w') as f:
  f.write(csh)

with open('crypto_uintN.h','w') as f:
  f.write(cuh)

with open('intN_optblocker.c','w') as f:
  f.write(f'''/* auto-generated: cd cryptoint; ./autogen */
/* {version} */
#include "crypto_intN.h"

volatile crypto_intN crypto_intN_optblocker = 0;
''')

with open('uintN_optblocker.c','w') as f:
  f.write(f'''/* auto-generated: cd cryptoint; ./autogen */
/* {version} */
#include "crypto_uintN.h"

volatile crypto_uintN_signed crypto_uintN_signed_optblocker = 0;
''')
