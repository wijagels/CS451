from operator import itemgetter
from collections import Counter

c = Counter()

f = open('./input-moon10.txt', 'r')
k = f.read(4096)
while k != "":
    for char in k:
        if char.isalpha():
            c[char.upper()] += 1
    k = f.read(4096)

print(sorted(c.items(), key=itemgetter(0)))
