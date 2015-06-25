import sys

fnames = sys.argv[1:]
for sfn in fnames:
    rn = sfn
    rn = rn.replace(".fa", "") 
    print "".join([">", rn])
    with open(sfn) as f:
       for line in f:
           line = line.strip()
           if len(line) > 0 and line[0] != ">":
               print line 
