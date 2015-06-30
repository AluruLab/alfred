import sys

in_file = sys.argv[1]
out_file = sys.argv[2]
col_map = sys.argv[3]

col_dict = {}
with open(col_map) as f:
    for line in f:
        line = line.strip()
        elts = line.split("|")
        if(len(elts) < 3):
            continue
        col_dict[elts[1].strip()] = elts[2].strip()
