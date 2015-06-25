import sys

prot_char = "ACDEFGHIKLMNPQRSTVWYBZX"


def is_prot(line):
    for c in line.upper():
        if c not in prot_char:
            return False
    return True


def write_out(orgName, orgSeq, nSeq):
    if len(orgSeq) > 0:
        ofName = orgName + ".fa"
        with open(ofName, 'w') as xf:
            xf.write(">" + orgName + "\n")
            xf.write(orgSeq + "\n")

fname = sys.argv[1]
print fname

with open(fname) as f:
    inOrg = False
    nSeq = 0
    orgSeq = ""
    orgName = ""
    while True:
        try:
            line = f.next()
            line = line.strip()
            line = line.replace(" ", "")
            if len(line) == 0:
                pass
            elif line[0] == ">":
                nSeq += 1
            elif is_prot(line):
                orgSeq = orgSeq + line.upper()
            else:
                #  print "N:", line
                if nSeq > 0:
                    print orgName, nSeq
                    write_out(orgName, orgSeq, nSeq)
                orgName = line
                orgSeq = ""
                nSeq = 0
        except StopIteration:
            break
    print orgName, nSeq
    write_out(orgName, orgSeq, nSeq)
