import math
p27len = [
  16883 ,
  17248 ,
  16855 ,
  16868 ,
  16873 ,
  16700 ,
  16874 ,
  15769 ,
  16864 ,
  16835 ,
  16916 ,
  16809 ,
  16887 ,
  17085 ,
  16901 ,
  16878 ,
  17354 ,
  16883 ,
  16783 ,
  16675 ,
  17073 ,
  16686 ,
  16870 ,
  16664 ,
  16964 ,
  16869 ,
  16707
]

np = len(p27len)

print "E.Coli"
efactor = math.log(9053184, 2.0)
for k in [1,2,3,4,5]:
    print k, math.pow(efactor, k)

print "27 primates"
z27p = sum([p27len[x] + p27len[y] for y in range(27) for x in range(y)])
print 0, z27p
for k in [1,2,3,4,5]:
    print k, sum([(p27len[x] + p27len[y]) * math.pow(math.log(p27len[x] + p27len[y], 2), k) for y in range(27) for x in range(y)])/float(z27p)
