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
#    /Users/srirampc/work/phd/research/arakawa/data/balibase/tree
#    /Users/srirampc/work/phd/research/arakawa/data/balibase/tree
#    /Users/srirampc/work/phd/research/arakawa/software/bin/neighbor


in_dir = os.path.abspath(sys.argv[1])
out_dir = os.path.abspath(sys.argv[2])
nbr_exe = sys.argv[3]
# mat_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*k[0-6].out")
# mat_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*k1[1-3].out")
mat_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*spaced.out")
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
