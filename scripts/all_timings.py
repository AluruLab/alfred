import sys
import os
import os.path
import glob
import json

# python balibase_timings.py balibase_dir
# run as
#   python all_timings.py
#          /Users/srirampc/work/phd/research/arakawa/runs/balibase/tree


def acs_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kacs_pattern = pattern_pfx + "acs.k" + str(k) + ".out.log"
        kacs_list = glob.glob(kacs_pattern)
        print "acsk", k, len(kacs_list), dataset,
        total_time = 0.0
        for fname in kacs_list:
            with open(fname) as f:
                jlog = json.load(f)
                total_time += float(jlog['total_time_secs'])
        print total_time


def kmacs_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kmacs_pattern = pattern_pfx + "kmacs.k" + str(k) + ".out.log"
        kmacs_list = glob.glob(kmacs_pattern)
        print "kmacs", k, len(kmacs_list), dataset,
        total_time = 0.0
        for fname in kmacs_list:
            with open(fname) as f:
                for line in f:
                    if line.startswith("Computation time:"):
                        elts = line.split(" ")
                        assert(len(elts) >= 3)
                        total_time += float(elts[2])
        print total_time


def spaced_timings(in_dir, pattern_pfx, dataset=""):
    spaced_pattern = pattern_pfx + "spaced.out.log"
    spaced_list = glob.glob(spaced_pattern)
    print "spaced", "NA", len(spaced_list), dataset,
    total_time = 0.0
    for fname in spaced_list:
        with open(fname) as f:
            for line in f:
                if line.startswith("Computation time:"):
                    elts = line.split(" ")
                    assert(len(elts) >= 3)
                    total_time += float(elts[2])
    print total_time


def balibase_timings(bbdir):
    pattern_pfx = os.path.join(bbdir, "RV*", "BB[0-9]*.")
    #  kacs
    acs_timings(bbdir, pattern_pfx, [0, 1, 2, 3, 4, 5])
    #  kmacs
    kmacs_timings(bbdir, pattern_pfx, [11, 12, 13])
    #  spaced
    spaced_timings(bbdir, pattern_pfx)


def roseobacter_timings(rbdir):
    pfx_list = ["full", "clade1", "clade2", "clade3", "clade4"]
    for px in pfx_list:
        pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
        acs_timings(rbdir, pattern_pfx, [0, 1, 2, 3, 4, 5], px)
    for px in pfx_list:
        pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
        kmacs_timings(rbdir, pattern_pfx, [2, 4, 6], px)
    for px in pfx_list:
        pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
        spaced_timings(rb_dir, pattern_pfx, px)

bb_dir = os.path.abspath(sys.argv[1])
rb_dir = os.path.abspath(sys.argv[2])
balibase_timings(bb_dir)
roseobacter_timings(rb_dir)
