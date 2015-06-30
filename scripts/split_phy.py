import sys

# runs as
#    python split_phy.py fname obase
# splits input phy into obase files
# output file format is obase-0X.tree

fname = sys.argv[1]
obase = sys.argv[2]


with open(fname) as f:
    fidx = 0
    cf = None
    for line in f:
        line = line
        if line.startswith("--- run"):
            f.next()
            fidx += 1
            if cf:
                cf.close()
            cf = open(obase + format(fidx, "04") + ".phy", "w")
            continue
        if cf:
            cf.write(line)
    if cf:
        cf.close()
