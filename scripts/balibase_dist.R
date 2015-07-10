library("phangorn")
library("plyr")
library("stringr")
#
# run as
#   Rscript balibase_kacs_dist.R ../runs/balibase/tree
#
args <- commandArgs(TRUE)
run.dir <- args[1]


rf.dist <- function(tfx, tfy){
    rx <- read.tree(tfx)
    ry <- read.tree(tfy)
    RF.dist(rx, ry)
}

rfiles <- list.files(run.dir, pattern = "RV*", full.names = T)
print(rfiles)
kacs.dist.df <- ldply(c(0,1,2,3,4,5,6), function(kval){
    ksuf <- paste("\\.acs.k", kval, ".tree", sep = "")
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
        # print(paste(tfx, tfy))
        rst <- str_replace(tfx, run.dir, "")
        rst <- dirname(str_replace(rst, "/", ""))
        bfn <- str_replace(tfx, ksuf, "")
        bfn <- str_replace(bfn, run.dir, "")
        bfn <- str_replace(bfn, rst, "")
        bfn <- str_replace(bfn, "/", "")
        bfn <- str_replace(bfn, "/", "")
        c(class = rst, dataset = bfn,
          errs = kval, dist = rf.dist(tfx, tfy))
    })
})
write.table(kacs.dist.df, "balibase.kacs.dist")

kmacs.dist.df <- ldply(c(0,11,12,13,14,15), function(kval){
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
        rst <- str_replace(tfx, run.dir, "")
        rst <- dirname(str_replace(rst, "/", ""))
        bfn <- str_replace(tfx, ksuf, "")
        bfn <- str_replace(bfn, run.dir, "")
        bfn <- str_replace(bfn, rst, "")
        bfn <- str_replace(bfn, "/", "")
        bfn <- str_replace(bfn, "/", "")
        c(class = rst, dataset = bfn,
          errs = kval, dist = rf.dist(tfx, tfy))
    })
})
write.table(kmacs.dist.df, "balibase.kmacs.dist")

spaced.dist.df <- ldply(c(0), function(kval){
    ksuf <- paste(".spaced", ".tree", sep = "")
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
        rst <- str_replace(tfx, run.dir, "")
        rst <- dirname(str_replace(rst, "/", ""))
        bfn <- str_replace(tfx, ksuf, "")
        bfn <- str_replace(bfn, run.dir, "")
        bfn <- str_replace(bfn, rst, "")
        bfn <- str_replace(bfn, "/", "")
        bfn <- str_replace(bfn, "/", "")
        dval <- tryCatch({rf.dist(tfx, tfy)}, error = function(err){
            print(paste(tfx, tfy, err))
            return(Inf)
        })
        c(class = rst, dataset = bfn,
          errs = kval, dist = dval)
    })
})
write.table(spaced.dist.df, "balibase.spaced.dist")

kacs.avg.df <- ddply(kacs.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "acs", dist = avg.val)
})

kmacs.avg.df <- ddply(kmacs.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "kmacs", dist = avg.val)
})

spaced.avg.df <- ddply(spaced.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "spaced",dist = avg.val)
})

final.df = rbind(kacs.avg.df, kmacs.avg.df, spaced.avg.df)
write.table(final.df[,c(2,3,1,4)], "balibase.results.out")


# rf.dist(args[1], args[2])
