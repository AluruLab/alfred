import sys
import os
import os.path
import glob
import json

# python balibase_timings.py in_dir
# run as
#   python balibase_timings.py
#          /Users/srirampc/work/phd/research/arakawa/runs/balibase/tree


def acs_timings(in_dir, pattern_pfx):
    for k in [0, 1, 2, 3, 4, 5]:
        kacs_pattern = pattern_pfx + "acs.k" + str(k) + ".out.log"
        kacs_list = glob.glob(kacs_pattern)
        print "acsk", k, len(kacs_list),
        total_time = 0.0
        for fname in kacs_list:
            with open(fname) as f:
                jlog = json.load(f)
                total_time += float(jlog['total_time_secs'])
        print total_time


def kmacs_timings(in_dir, pattern_pfx):
    for k in [11, 12, 13]:
        kmacs_pattern = pattern_pfx + "kmacs.k" + str(k) + ".out.log"
        kmacs_list = glob.glob(kmacs_pattern)
        print "kmacs", k, len(kmacs_list),
        total_time = 0.0
        for fname in kmacs_list:
            with open(fname) as f:
                for line in f:
                    if line.startswith("Computation time:"):
                        elts = line.split(" ")
                        assert(len(elts) >= 3)
                        total_time += float(elts[2])
        print total_time


def spaced_timings(in_dir, pattern_pfx):
    spaced_pattern = pattern_pfx + "spaced.out.log"
    spaced_list = glob.glob(spaced_pattern)
    print "spaced", 0, len(spaced_list),
    total_time = 0.0
    for fname in spaced_list:
        with open(fname) as f:
            for line in f:
                if line.startswith("Computation time:"):
                    elts = line.split(" ")
                    assert(len(elts) >= 3)
                    total_time += float(elts[2])
    print total_time


in_dir = os.path.abspath(sys.argv[1])
pattern_pfx = os.path.join(in_dir, "RV*", "BB[0-9]*.")
#  kacs
acs_timings(in_dir, pattern_pfx)
#  kmacs
kmacs_timings(in_dir, pattern_pfx)
#  spaced
spaced_timings(in_dir, pattern_pfx)
