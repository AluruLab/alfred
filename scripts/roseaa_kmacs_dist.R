library("phangorn")
library("plyr")
library("stringr")
#
# run as
#   /Users/srirampc/work/phd/research/arakawa/data/rose-aa/
#
args <- commandArgs(TRUE)
in.dir <- args[1]


rf.dist <- function(tfx, tfy){
    rx <- read.tree(tfx)
    ry <- read.tree(tfy)
    RF.dist(rx, ry)
}

rfiles <- c(in.dir)
print(rfiles)
dist.df <- ldply(c(16,17,18), function(kval){
    ksuf <- paste(".kmacs.k", kval, ".tree", sep = "")
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
        c(dataset = basename(tfx),
          errs = kval, dist = rf.dist(tfx, tfy))
    })
})
write.table(dist.df, "kmacs.results.out")
avg.df <- ddply(dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(avg = avg.val)
})
write.table(avg.df, "kmacs.summary.out")
# warnings()
# rf.dist(args[1], args[2])
