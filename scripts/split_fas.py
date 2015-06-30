import sys

# runs as
#    python split_fas.py fname nseqs obase
# splits input fasta into files with nseqs
# output file format is obase-0X.fa


fname = sys.argv[1]
nseqs = int(sys.argv[2])
obase = sys.argv[3]

with open(fname) as f:
    fidx = 0
    ncount = 0
    cf = None
    for line in f:
        line = line.strip()
        if len(line) == 0:
            continue
        if line[0] == ">":
            if ncount == nseqs:
                ncount = 0
                cf.close()
            if ncount == 0:
                fidx += 1
                cf = open(obase + format(fidx, "04") + ".fa", "w")
            ncount += 1
        cf.write(line)
        cf.write("\n")
    if cf:
        cf.close()
