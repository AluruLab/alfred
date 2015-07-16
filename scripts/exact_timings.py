import sys
import os
import os.path
import glob
import json

# run as
#   python exact_timings.py ../runs/primates ../runs/seq


def acs_timings(in_dir, pattern_pfx, kvals, dataset=""):
    for k in kvals:
        kacs_pattern = pattern_pfx + "acs.k" + str(k) + ".out.log"
        kacs_list = glob.glob(kacs_pattern)
        total_time = 0.0
        try:
            for fname in kacs_list:
                with open(fname) as f:
                    jlog = json.load(f)
                    total_time += float(jlog['total_time_secs'])
            print "%s,%s,%d,%.2f" % (dataset, "alfred-e", k, total_time)
        except ValueError:
            pass


def primates_timings(pmdir):
    pattern_pfx = os.path.join(pmdir, "primates.")
    acs_timings(pmdir, pattern_pfx, [0, 1, 2, 3, 4, 5], "primates")


def seq_timings(sqdir):
    pattern_pfx = os.path.join(sqdir, "seq4Mx2.")
    acs_timings(sqdir, pattern_pfx, [0, 1, 2, 3], "seq4Mx2")
    pattern_pfx = os.path.join(sqdir, "seqbig.")
    acs_timings(sqdir, pattern_pfx, [], "seqbig")


def main():
    pm_dir = os.path.abspath(sys.argv[1])
    seq_dir = os.path.abspath(sys.argv[2])
    print ",".join(["dataset", "method", "errs", "time"])
    primates_timings(pm_dir)
    seq_timings(seq_dir)


if __name__ == '__main__':
    main()
