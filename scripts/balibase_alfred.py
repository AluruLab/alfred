import sys
import subprocess
import os
import os.path
import glob

#
# run as
# python balibase_alfred.py in_dir out_dir kacs_path
# python balibase_alfred.py
#         ../data/balibase/bb3_release ../runs/balibase/tree ../build/arakawa.x

in_dir = os.path.abspath(sys.argv[1])
out_root = os.path.abspath(sys.argv[2])
kacs_exe = sys.argv[3]
tfa_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*.tfa")
tfa_lst = glob.glob(tfa_pattern)
print len(tfa_lst)
for k in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]:
    for i, fa_name in enumerate(tfa_lst):
        fa_dir = os.path.dirname(fa_name)
        fa_base = os.path.basename(fa_dir)
        out_dir = os.path.join(out_root, fa_base)
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("tfa", "alfred.x" + str(k) + ".out")
        if os.path.isfile(out_file):
            continue
        print i, k, fa_name, out_file
        if k == 0:
            rc = subprocess.call([kacs_exe, "-f", fa_name, "-k", "0",
                                  "-o", out_file])
        else:
            rc = subprocess.call([kacs_exe, "-f", fa_name, "-k", "1", "-x",
                                  str(k), "-o", out_file])
        if rc != 0:
            print "ERROR!"
            sys.exit(1)
