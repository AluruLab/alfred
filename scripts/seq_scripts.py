import sys

script_fmt = """#!/bin/bash

#PBS -l nodes=1:ppn=16
#PBS -l pmem=4gb
#PBS -l walltime=48:00:00
#PBS -q batch
#PBS -j oe
cd %s/runs/seq/
%s/build/arakawa.x -f %s/data/seq/%s,%s/data/seq/%s -o %s.%s.k%d.out -k %d
"""


def gen_4m(seq_4mx2):
    for k in [0, 1, 2, 3, 4, 5, 6]:
        script_name = "seq4Mx2.acs.k" + str(k) + ".pbs"
        with open(script_name, "w") as f:
            script_str = script_fmt % (arakawa_path, arakawa_path,
                                       arakawa_path, seq_4mx2[0],
                                       arakawa_path, seq_4mx2[1],
                                       'seq4Mx2', 'acs', k, k)
            f.write(script_str)
            f.write("\n")


def gen_23m_50m(seq_big):
    for k in [0, 1, 2, 3, 4, 5, 6]:
        script_name = "seqbig.acs.k" + str(k) + ".pbs"
        with open(script_name, "w") as f:
            script_str = script_fmt % (arakawa_path, arakawa_path,
                                       arakawa_path, seq_big[0],
                                       arakawa_path, seq_big[1],
                                       'seqbig', 'acs', k, k)
            f.write(script_str)
            f.write("\n")


if __name__ == '__main__':
    arakawa_path = sys.argv[1]
    seq_4mx2 = ['NC_000962.3.fasta', 'NC_000913.3.fasta']
    seq_big = ['NT_033779.4.fasta', 'NC_019478.1.fasta']
    # gen_4m(seq_4mx2)
    gen_23m_50m(seq_big)
