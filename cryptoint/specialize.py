#!/usr/bin/env python3

import os
import readasm

with open('namespace') as f:
  projectnamespace = f.readline().strip()

with open('crypto_intN.h') as f:
  intN = f.read()
with open('crypto_uintN.h') as f:
  uintN = f.read()

for bits in 8,16,32,64:
  for ui in 'int','uint':
    TYPE = f'crypto_{ui}{bits}'
    with open(f'{TYPE}.h','w') as f:
      contents = intN if ui == 'int' else uintN
      contents = contents.replace('N',str(bits))
      contents = contents.replace('namespace',projectnamespace)
      contents = readasm.substitute(contents)
      contents = contents.replace('amd64','defined(__GNUC__) && defined(__x86_64__)')
      contents = contents.replace('arm64','defined(__GNUC__) && defined(__aarch64__)')

      newcontents = ''
      for line in contents.splitlines():
        if ':' in line:
          condbits = line.split(':')[0].strip()
          if condbits.isnumeric():
            if int(condbits) != bits:
              continue
            line = ':'.join(line.split(':')[1:])
        newcontents += line+'\n'
      contents = newcontents

      tounroll = f'  int {TYPE}_k;\n  for ({TYPE}_k = 0;{TYPE}_k < {bits};{TYPE}_k += 8)\n    {TYPE}_z |= (({TYPE}) (*{TYPE}_s++)) << {TYPE}_k;'
      unrolled = '\n'.join(f'  {TYPE}_z |= (({TYPE}) (*{TYPE}_s++)) << {k};' for k in range(0,bits,8))
      contents = contents.replace(tounroll,unrolled)

      tounroll = f'  int {TYPE}_k;\n  for ({TYPE}_k = {bits} - 8;{TYPE}_k >= 0;{TYPE}_k -= 8)\n    {TYPE}_z |= (({TYPE}) (*{TYPE}_s++)) << {TYPE}_k;'
      unrolled = '\n'.join(f'  {TYPE}_z |= (({TYPE}) (*{TYPE}_s++)) << {k};' for k in reversed(range(0,bits,8)))
      contents = contents.replace(tounroll,unrolled)

      tounroll = f'  int {TYPE}_k;\n  for ({TYPE}_k = 0;{TYPE}_k < {bits};{TYPE}_k += 8)\n    *{TYPE}_s++ = {TYPE}_x >> {TYPE}_k;'
      unrolled = '\n'.join(f'  *{TYPE}_s++ = {TYPE}_x >> {k};' for k in range(0,bits,8))
      contents = contents.replace(tounroll,unrolled)

      tounroll = f'  int {TYPE}_k;\n  for ({TYPE}_k = {bits} - 8;{TYPE}_k >= 0;{TYPE}_k -= 8)\n    *{TYPE}_s++ = {TYPE}_x >> {TYPE}_k;'
      unrolled = '\n'.join(f'  *{TYPE}_s++ = {TYPE}_x >> {k};' for k in reversed(range(0,bits,8)))
      contents = contents.replace(tounroll,unrolled)

      f.write(contents)

  for intc in sorted(os.listdir('.')):
    if not intc.endswith('.c'): continue
    if not (intc.startswith('int') or intc.startswith('uint')): continue
    targetintc = intc.replace('N',str(bits))
    with open(intc) as f:
      contents = f.read()
    with open(targetintc,'w') as f:
      f.write(contents.replace('N',str(bits)))
