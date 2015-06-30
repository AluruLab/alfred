import sys
import subprocess
import os
import os.path
import glob
import tempfile

#
# run as
# python balibase_tree.py in_dir out_dir proml_path sre_path
# how I ran :
# python balibase_tree.py
#    /Users/srirampc/work/phd/research/arakawa/data/balibase/bb3_release
#    /Users/srirampc/work/phd/research/arakawa/data/balibase/tree
#    /Users/srirampc/work/phd/research/arakawa/software/bin/proml
#    /Users/srirampc/work/phd/research/arakawa/software/bin/sreformat

#  os.mkdir()
#  subprocess.call
in_dir = os.path.abspath(sys.argv[1])
out_dir = os.path.abspath(sys.argv[2])
proml_exe = sys.argv[3]
sre_exe = sys.argv[4]
msf_pattern = os.path.join(in_dir, "RV*", "BB[0-9]*.msf")
aln_lst = glob.glob(msf_pattern)
tfname = tempfile.mktemp()
tfphyl = tempfile.mktemp()
tfphot = tempfile.mktemp()
print tfname
print tfphyl
print tfphot
for i, msf_name in enumerate(aln_lst):
    msf_dir = os.path.dirname(msf_name)
    msf_base = os.path.basename(msf_dir)
    mo_dir = os.path.join(out_dir, msf_base)
    mo_file = os.path.join(mo_dir, os.path.basename(msf_name))
    mo_out = mo_file.replace("msf", "out")
    mo_tree = mo_file.replace("msf", "tree")
    if not os.path.isfile(mo_file):
        print i, msf_name, mo_dir, mo_file
        if not os.path.isdir(mo_dir):
            os.mkdir(mo_dir)
        with open(tfphyl, "w") as tpf:
            rc = subprocess.call([sre_exe, "phylip", msf_name], stdout=tpf)
            if rc != 0:
                print "ERROR!"
                sys.exit(1)
        with open(tfphyl) as tpf:
            of = open(mo_file, "w")
            for line in tpf:
                line = line.replace(".", "-")
                of.write(line)
            of.close()
        # runnings
        ostr = "\n".join([mo_file, "F", mo_out, "Y", "F", mo_tree])
        #  print ostr
        with open(tfname, "w") as tf:
            tf.write(ostr)
        with open(tfphot, "w") as tfo:
            with open(tfname) as tf:
                rc = subprocess.call([proml_exe], stdin=tf, stdout=tfo)
                if rc != 0:
                    print "ERROR!"
                    sys.exit(1)
os.remove(tfname)
os.remove(tfphyl)
os.remove(tfphot)
