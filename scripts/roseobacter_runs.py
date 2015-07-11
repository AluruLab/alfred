import sys
import subprocess
import os
import os.path
import glob

#
# run as
# python roseobacter_runs.py
#  ../data/roseobacter/  ../runs/roseobacter
#  ../build/arakawa.x ../software/kmacs/kmacs ../software/spaced-prdna/spaced

in_dir = os.path.abspath(sys.argv[1])
out_dir = os.path.abspath(sys.argv[2])
kacs_exe = os.path.abspath(sys.argv[3])
kmacs_exe = os.path.abspath(sys.argv[4])
spaced_exe = os.path.abspath(sys.argv[5])
fas_pattern = os.path.join(in_dir, "roseobacter.full.fas")
fas_lst = glob.glob(fas_pattern)
for k in [0, 1, 2, 3, 4, 5]:
    for i, fa_name in enumerate(fas_lst):
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("fas", "acs.k" + str(k) + ".out")
        if os.path.isfile(out_file):
            continue
        print i, k, fa_name, out_file
        rc = 0
        rc = subprocess.call([kacs_exe, "-f", fa_name, "-k", str(k),
                              "-o", out_file])
        if rc != 0:
            print "ERROR!"
            sys.exit(1)

for k in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]:
    for i, fa_name in enumerate(fas_lst):
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("fas", "kmacs.k" + str(k) + ".out")
        log_file = out_file + ".log"
        if os.path.isfile(out_file):
            continue
        print i, k, fa_name, out_file
        rc = 0
        with open(log_file, "w") as tfo:
            rc = subprocess.call([kmacs_exe, fa_name, str(k), out_file],
                                 stdout=tfo)
        if rc != 0:
            print "ERROR!"
            sys.exit(1)

for i, fa_name in enumerate(fas_lst):
    out_file = os.path.join(out_dir, os.path.basename(fa_name))
    out_file = out_file.replace("fas", "spaced.out")
    log_file = out_file + ".log"
    if os.path.isfile(out_file):
        continue
    print i, 0, fa_name, out_file
    rc = 0
    with open(log_file, "w") as tfo:
        rc = subprocess.call([spaced_exe, "-o", out_file, "-k", "4", fa_name],
                             stdout=tfo, stderr=tfo)
    if rc != 0:
        print "ERROR!"
        with open(log_file) as tf:
            print "".join(tf.readlines())
        sys.exit(1)

for k in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]:
    for i, fa_name in enumerate(fas_lst):
        out_file = os.path.join(out_dir, os.path.basename(fa_name))
        out_file = out_file.replace("fas", "alfred.x" + str(k) + ".out")
        if os.path.isfile(out_file):
            continue
        print i, k, fa_name, out_file
        rc = 0
        if k == 0:
            rcmd = [kacs_exe, "-f", fa_name, "-k", str(k), "-o", out_file]
            rc = subprocess.call(rcmd)
        else:
            rc = subprocess.call([kacs_exe, "-f", fa_name, "-k", "1", "-x",
                                  str(k), "-o", out_file])
        if rc != 0:
            print "ERROR!"
            sys.exit(1)
