import sys
import subprocess
import os
import os.path
import glob
import tempfile

#
# run as
# python balibase_kmacs.py in_dir out_dir kmacs_path
# python balibase_kmacs.py
#         /Users/srirampc/work/phd/research/arakawa/data/balibase/bb3_release
#         /Users/srirampc/work/phd/research/arakawa/runs/balibase/tree
#         /Users/srirampc/work/phd/research/arakawa/software/kmacs/kmacs


in_dir = os.path.abspath(sys.argv[1])
out_root = os.path.abspath(sys.argv[2])
kmacs_exe = sys.argv[3]
tfa_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*.tfa")
tfa_lst = glob.glob(tfa_pattern)
tfname = tempfile.mktemp()
tfphyl = tempfile.mktemp()
tfphot = tempfile.mktemp()
print tfname
print tfphyl
print tfphot
print len(tfa_lst)
#  for k in [0, 1, 2, 3, 4, 5]:
for k in [11, 12, 13]:
    for i, fa_name in enumerate(tfa_lst):
        fa_dir = os.path.dirname(fa_name)
        fa_base = os.path.basename(fa_dir)
        out_dir = os.path.join(out_root, fa_base)
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("tfa", "kmacs.k" + str(k) + ".out")
        if os.path.isfile(out_file):
            continue
        print i, k, fa_name, out_file
        log_file = out_file + ".log"
        with open(log_file, "w") as tfo:
            rc = subprocess.call([kmacs_exe, fa_name, str(k), out_file],
                                 stdout=tfo)
            if rc != 0:
                print "ERROR!"
                sys.exit(1)
# os.remove(tfname)
