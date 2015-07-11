import sys
import subprocess
import os
import os.path
import glob
import tempfile


#
# run as
# python balibase_nbr.py in_dir out_dir nbr_exe_path
# how I ran :

# python balibase_nbr.py
#  ../runs/balibase/tree ../runs/balibase/tree ../software/bin/neighbor

def run_nbr(out_dir, nbr_exe, mat_pattern):
    mat_lst = glob.glob(mat_pattern)
    tfname = tempfile.mktemp()
    tfphot = tempfile.mktemp()
    print len(mat_lst)
    for i, mat_name in enumerate(mat_lst):
        mplp_name = mat_name.replace("out", "plp")
        mtree_name = mat_name.replace("out", "tree")
        ostr = "\n".join([mat_name, "F", mplp_name, "Y", "F", mtree_name])
        with open(tfname, "w") as tf:
            tf.write(ostr)
        if os.path.isfile(mplp_name):
            os.remove(mplp_name)
        if os.path.isfile(mtree_name):
            os.remove(mtree_name)
        print i, mat_name, mplp_name
        rc = 0
        with open(tfphot, "w") as tfo:
            with open(tfname) as tf:
                rc = subprocess.call([nbr_exe], stdin=tf, stdout=tfo)
        if rc != 0:
            print "ERROR!"
            tfo = open(tfphot)
            print "".join(tfo.readlines())
            sys.exit(1)
    os.remove(tfname)
    os.remove(tfphot)


indir = os.path.abspath(sys.argv[1])
outdir = os.path.abspath(sys.argv[2])
nbrexe = sys.argv[3]
# mpattern = os.path.join(in_dir, "RV*", "BB[0-9]*k[0-6].out")
# run_nbr(outdir, nbrexe, mpattern)
# mpattern = os.path.join(indir, "RV*", "BB[0-9]*kmacs.*.out")
# run_nbr(outdir, nbrexe, mpattern)
# mpattern = os.path.join(in_dir, "RV*", "BB[0-9]*spaced.out")
# run_nbr(outdir, nbrexe, mpattern)
mpattern = os.path.join(indir, "RV*", "BB[0-9]*alfred.*.out")
run_nbr(outdir, nbrexe, mpattern)
