library("phangorn")
library("plyr")
library("stringr")
#
# run as Rscript balibase_kacs_dist.R
#   /Users/srirampc/work/phd/research/arakawa/data/balibase/tree
#
args <- commandArgs(TRUE)
in.dir <- args[1]


rf.dist <- function(tfx, tfy){
    rx <- read.tree(tfx)
    ry <- read.tree(tfy)
    RF.dist(rx, ry)
}

rfiles <- list.files(in.dir, pattern = "RV*", full.names = T)
print(rfiles)
dist.df <- ldply(c(0,1,2,3,4,5,6), function(kval){
    ksuf <- paste(".acs.k", kval, ".tree", sep = "")
    kpat <- paste("*", ksuf, sep = "")
    kfdx <- ldply(rfiles, function(din){
        k.files <- list.files(din, pattern = kpat, full.names = T)
        t.files <- str_replace(k.files, ksuf, ".tree")
        data.frame(kfile = k.files, tfile = t.files)
    })
    print(paste(kval, ksuf, nrow(kfdx)))
    ldply(1:nrow(kfdx), function(ri){
        tfx <- as.character(kfdx[ri, 1])
        tfy <- as.character(kfdx[ri, 2])
        rst <- str_replace(tfx, in.dir, "")
        rst <- dirname(str_replace(rst, "/", ""))
        bfn <- str_replace(tfx, ksuf, "")
        bfn <- str_replace(bfn, in.dir, "")
        bfn <- str_replace(bfn, rst, "")
        bfn <- str_replace(bfn, "/", "")
        bfn <- str_replace(bfn, "/", "")
        c(class = rst, dataset = bfn,
          errs = kval, dist = rf.dist(tfx, tfy))
    })
})
write.table(dist.df, "kacs.results.out")
avg.df <- ddply(dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(avg = avg.val)
})
write.table(avg.df, "kacs.summary.out")

# rf.dist(args[1], args[2])
