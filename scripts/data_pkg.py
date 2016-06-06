import sys
import shutil
import os
import os.path
import errno
import glob


data_src = ['primates/primates.full.fas',
            'primates/primates.full.tree',
            'roseobacter/32Genome_Alignemnt_phylip_25May10.txt',
            'roseobacter/roseobacter.full.fas',
            'roseobacter/roseobacter.full.tree']

data_dest = ['primates/primates.fa',
             'primates/primates.tree',
             'roseobacter/roseobacter_25May10.msf',
             'roseobacter/roseobacter.fa',
             'roseobacter/roseobacter.tree']

runs_src = ['primates/primates.alfred.x4.tree',
            'primates/primates.alfred.x4.out',
            'roseobacter/roseobacter.full.alfred.x7.tree',
            'roseobacter/roseobacter.full.alfred.x7.out']

runs_dest = ['primates/primates.alfred.x4.tree',
             'primates/primates.alfred.x4.mat',
             'roseobacter/roseobacter.alfred.x7.tree',
             'roseobacter/roseobacter.alfred.x7.mat']


def make_dir(dest_dir):
    try:
        os.makedirs(dest_dir)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(dest_dir):
            pass


def copy_file(src_dir, src_file, dest_root, dest_file):
    src_fpath = os.path.join(src_dir, src_file)
    dst_fpath = os.path.join(dest_root, dest_file)
    dest_dir = os.path.dirname(dst_fpath)
    make_dir(dest_dir)
    shutil.copy2(src_fpath, dst_fpath)


def main():
    arakawa_dir = sys.argv[1]
    pkg_dir = sys.argv[2]
    src_data = os.path.join(arakawa_dir, "data")
    src_runs = os.path.join(arakawa_dir, "runs")
    copy_file(src_data, "README", pkg_dir, "README")
    copy_file(src_data, "aliases.xlsx", pkg_dir, "aliases.xlsx")
    # copy data files
    dest_data = os.path.join(pkg_dir, "data")
    for x, y in zip(data_src, data_dest):
        copy_file(src_data, x, dest_data, y)
    bb_data = os.path.join(src_data, "balibase/bb3_release")
    tfa_pattern = os.path.join(bb_data, "RV*", "BB[0-9]*.tfa")
    for x in glob.glob(tfa_pattern):
        bdir = os.path.dirname(x)
        bfn = os.path.basename(x)
        sdir = bdir.replace(src_data, dest_data)
        bfa = bfn.replace("tfa", "fa")
        copy_file(bdir, bfn, sdir, bfa)
        bdir = bdir.replace("bb3_release", "tree")
        bdir = bdir.replace(src_data, src_runs)
        sdir = bdir.replace(src_runs, dest_data)
        bfn = bfn.replace(".tfa", ".tree")
        copy_file(bdir, bfn, sdir, bfn)
    # copy output files
    dest_runs = os.path.join(pkg_dir, "runs")
    for x, y in zip(runs_src, runs_dest):
        copy_file(src_runs, x, dest_runs, y)
    bb_runs = os.path.join(src_runs, "balibase/tree")
    out_pattern = os.path.join(bb_runs, "RV*", "BB[0-9]*.alfred.x9.out")
    print(len(glob.glob(out_pattern)))
    for x in glob.glob(out_pattern):
        bdir = os.path.dirname(x)
        bfn = os.path.basename(x)
        sdir = bdir.replace(src_runs, dest_runs)
        sdir = sdir.replace("tree", "")
        bfa = bfn.replace("out", "mat")
        copy_file(bdir, bfn, sdir, bfa)
        bfn = bfn.replace("out", "tree")
        bfa = bfa.replace("mat", "tree")
        copy_file(bdir, bfn, sdir, bfa)


if __name__ == '__main__':
    main()
