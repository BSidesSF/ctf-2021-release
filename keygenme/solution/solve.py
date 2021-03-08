import hashlib
import struct
import sys

words = open('english.txt').read().splitlines()


sys_idx = words.index('system')

kid = "ed2591b6660b2b05a0e5b22152b120cd59458eb2504c529927ff9a4890b6912e"

def doublehash(rk):
    return hashlib.sha256(
            hashlib.sha256(rk).digest()).hexdigest()

ct = 0

exp = ["system", "over", "nuclear", "acid", "tag"]
exp_i = list(words.index(e) for e in exp)
exp_b = struct.pack('<5I', *exp_i)
assert doublehash(exp_b) == kid

for i in range(0, len(words), 3):
    for j in range(0, len(words), 22):
        if i == j:
            continue
        for k in range(0, len(words), 15):
            if i==k or j==k:
                continue
            accum = 0x55
            opts = [sys_idx, i, j, k]
            opts.append(accum ^ sys_idx ^ i ^ j ^ k)
            raw_key = struct.pack('<5I', *opts)
            if doublehash(raw_key) == kid:
                print('Solution: {}'.format(opts))
                print(' '.join(words[i] for i in opts))
                sys.exit(0)
            ct += 1
            if ct % 1000 == 0:
                print(ct)
