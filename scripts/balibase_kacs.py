import sys
import subprocess
import os
import os.path
import glob
import tempfile

#
# run as
# python balibase_kacs.py in_dir out_dir kacs_path
# python balibase_kacs.py
#         /Users/srirampc/work/phd/research/arakawa/data/balibase/bb3_release
#         /Users/srirampc/work/phd/research/arakawa/data/balibase/tree
#         /Users/srirampc/work/phd/research/arakawa/build/arakawa.x

in_dir = os.path.abspath(sys.argv[1])
out_root = os.path.abspath(sys.argv[2])
kacs_exe = sys.argv[3]
tfa_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*.tfa")
tfa_lst = glob.glob(tfa_pattern)
tfname = tempfile.mktemp()
tfphyl = tempfile.mktemp()
tfphot = tempfile.mktemp()
print tfname
print tfphyl
print tfphot
print len(tfa_lst)
#for k in [0, 1, 2, 3, 4, 5]:
for k in [6, 7, 8]:
    for i, fa_name in enumerate(tfa_lst):
        fa_dir = os.path.dirname(fa_name)
        fa_base = os.path.basename(fa_dir)
        out_dir = os.path.join(out_root, fa_base)
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("tfa", "acs.k" + str(k) + ".out")
        # if os.path.isfile(out_file):
        #    continue
        print i, k, fa_name, out_file
        rc = subprocess.call([kacs_exe, "-f", fa_name, "-k", str(k),
                              "-o", out_file])
        if rc != 0:
            print "ERROR!"
            sys.exit(1)
# os.remove(tfname)
# os.remove(tfphyl)
# os.remove(tfphot)
