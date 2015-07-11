import sys
import os
import os.path
import glob
import json

# python balibase_timings.py balibase_dir
# run as
#   python all_timings.py
#     ../runs/balibase/tree ../roseobacter ../primates


def acs_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kacs_pattern = pattern_pfx + "acs.k" + str(k) + ".out.log"
        kacs_list = glob.glob(kacs_pattern)
        total_time = 0.0
        for fname in kacs_list:
            with open(fname) as f:
                jlog = json.load(f)
                total_time += float(jlog['total_time_secs'])
        print "%s,%s,%d,%.2f" % (dataset, "alfred-e", k, total_time)
#        print ",".join(map(str,
#                            ["acsk", k, len(kacs_list), dataset, total_time]))


def kmacs_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kmacs_pattern = pattern_pfx + "kmacs.k" + str(k) + ".out.log"
        kmacs_list = glob.glob(kmacs_pattern)
        total_time = 0.0
        for fname in kmacs_list:
            with open(fname) as f:
                for line in f:
                    if line.startswith("Computation time:"):
                        elts = line.split(" ")
                        assert(len(elts) >= 3)
                        total_time += float(elts[2])
        print "%s,%s,%d,%.2f" % (dataset, "kmacs", k, total_time)


def spaced_timings(in_dir, pattern_pfx, dataset=""):
    spaced_pattern = pattern_pfx + "spaced.out.log"
    spaced_list = glob.glob(spaced_pattern)
    total_time = 0.0
    for fname in spaced_list:
        with open(fname) as f:
            for line in f:
                if line.startswith("Computation time:"):
                    elts = line.split(" ")
                    assert(len(elts) >= 3)
                    total_time += float(elts[2])
    print "%s,%s,%s,%.2f" % (dataset, "spaced", "NA", total_time)


def alfred_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kacs_pattern = pattern_pfx + "alfred.x" + str(k) + ".out.log"
        kacs_list = glob.glob(kacs_pattern)
        total_time = 0.0
        for fname in kacs_list:
            with open(fname) as f:
                jlog = json.load(f)
                total_time += float(jlog['total_time_secs'])
        print "%s,%s,%s,%.2f" % (dataset, "alfred-h", k, total_time)


def balibase_timings(bbdir):
    pattern_pfx = os.path.join(bbdir, "RV*", "BB[0-9]*.")
    # acs_timings(bbdir, pattern_pfx, [1, 2, 3, 4, 5], "balibase")
    alfred_timings(bbdir, pattern_pfx, range(0, 10), "balibase")
    kmacs_timings(bbdir, pattern_pfx, range(0, 10), "balibase")
    # spaced_timings(bbdir, pattern_pfx, "balibase")


def roseobacter_timings(rbdir):
    # pfx_list = ["full", "clade1", "clade2", "clade3", "clade4"]
    pfx_list = ["full"]
    # for px in pfx_list:
    #     pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
    #     acs_timings(rbdir, pattern_pfx, [1, 2, 3, 4, 5],
    #                 "roseobacter." + px)
    for px in pfx_list:
        pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
        alfred_timings(rbdir, pattern_pfx, range(0, 10),
                       "roseobacter." + px)
    for px in pfx_list:
        pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
        kmacs_timings(rbdir, pattern_pfx, range(0, 10),
                      "roseobacter." + px)
    # for px in pfx_list:
    #     pattern_pfx = os.path.join(rbdir, "roseobacter." + px + "*.")
    #     spaced_timings(rb_dir, pattern_pfx, "roseobacter." + px)


def primates_timings(pmdir):
    pattern_pfx = os.path.join(pmdir, "primates.")
    # acs_timings(pmdir, pattern_pfx, [1, 2, 3, 4, 5], "primates")
    alfred_timings(pmdir, pattern_pfx, range(0, 10), "primates")
    kmacs_timings(pmdir, pattern_pfx, range(0, 10), "primates")
    # spaced_timings(pm_dir, pattern_pfx, "primates")

bb_dir = os.path.abspath(sys.argv[1])
rb_dir = os.path.abspath(sys.argv[2])
pm_dir = os.path.abspath(sys.argv[3])
print ",".join(["dataset", "method", "errs", "time"])
balibase_timings(bb_dir)
roseobacter_timings(rb_dir)
primates_timings(pm_dir)
