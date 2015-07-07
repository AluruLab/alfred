import sys
import subprocess
import os
import os.path
import glob

#
# run as
# python balibase_spaced.py in_dir out_dir spaced_path
# python balibase_spaced.py
#         /Users/srirampc/work/phd/research/arakawa/data/balibase/bb3_release
#         /Users/srirampc/work/phd/research/arakawa/runs/balibase/tree
#         /Users/srirampc/work/phd/research/arakawa/software/spaced-prdna/spaced


in_dir = os.path.abspath(sys.argv[1])
out_root = os.path.abspath(sys.argv[2])
spaced_exe = sys.argv[3]
tfa_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*.tfa")
tfa_lst = glob.glob(tfa_pattern)
print len(tfa_lst)
#  for k in [0, 1, 2, 3, 4, 5]:
for i, fa_name in enumerate(tfa_lst):
    fa_dir = os.path.dirname(fa_name)
    fa_base = os.path.basename(fa_dir)
    out_dir = os.path.join(out_root, fa_base)
    out_file = os.path.join(out_dir, os.path.basename(fa_name))
    out_file = out_file.replace("tfa", "spaced.out")
    # if os.path.isfile(out_file):
    #    continue
    log_file = out_file + ".log"
    print i, fa_name, out_file
    with open(log_file, "w") as tfo:
        rc = subprocess.call([spaced_exe, "-o", out_file, "-k", "4",
                              "-l", "34", "-d", "MS", fa_name],
                             stdout=tfo, stderr=tfo)
    if rc != 0:
        print "ERROR!"
        with open(log_file) as tf:
            print "".join(tf.readlines())
        sys.exit(1)
