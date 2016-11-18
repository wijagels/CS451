#! /usr/bin/env python3
import sys
from operator import itemgetter
from collections import Counter

c = Counter()

f = open(sys.argv[1], 'r')
k = f.read(4096)
while k != "":
    for char in k:
        if char.isalpha():
            c[char.upper()] += 1
    k = f.read(4096)
f.close()

out = open('./charcount-out.rst', 'w')
c = sorted(c.items(), key=itemgetter(0))
for el in c:
    out.write("%c %d\n" % el)
out.close()
