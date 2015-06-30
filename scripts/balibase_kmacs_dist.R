library("phangorn")
library("plyr")
library("stringr")

# run as
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
dist.df <- ldply(c(11,12,13), function(kval){
    ksuf <- paste(".kmacs.k", kval, ".tree", sep = "")
    kpat <- paste("*", ksuf, sep = "")
    kfdx <- ldply(rfiles, function(din){
        k.files <- list.files(din, pattern = kpat, full.names = T)
        t.files <- str_replace(k.files, ksuf, ".tree")
        data.frame(kfile = k.files, tfile = t.files)
    })
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
write.table(dist.df, "kmacs.results.out")
avg.df <- ddply(dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(avg = avg.val)
})
write.table(avg.df, "kmacs.summary.out")

# rf.dist(args[1], args[2])
