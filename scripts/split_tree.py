import sys

# runs as
#    python split_tree.py fname obase
# splits input tree into obase files
# output file format is obase-0X.tree


fname = sys.argv[1]
obase = sys.argv[2]


with open(fname) as f:
    fidx = 1
    cf = None
    for line in f:
        if not cf:
            cf = open(obase + format(fidx, "04") + ".tree", "w")
        cf.write(line)
        if ";" in line:
            fidx += 1
            cf.close()
            cf = None
    if cf:
        cf.close()
