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

rf.dist.dataset.k <- function(kval, ksuf, kpat) {
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
        dval <- tryCatch({rf.dist(tfx, tfy)}, error = function(err){
            print(paste(tfx, tfy, err))
            return(Inf)
        })
        c(class = rst, dataset = bfn,
          errs = kval, dist = dval)
        #c(class = rst, dataset = bfn,
        #  errs = kval, dist = rf.dist(tfx, tfy))
    })
}

rf.dist.kacs <- function(kvals){
    kdf <- ldply(kvals, function(kval){
        ksuf <- paste("\\.acs.k", kval, ".tree", sep = "")
        kpat <- paste("*", ksuf, sep = "")
        rf.dist.dataset.k(kval, ksuf, kpat)
    })
    write.table(kdf, "balibase.kacs.dist")
    kdf
}

rf.dist.kmacs <- function(kvals){
    kdf <- ldply(kvals, function(kval){
        ksuf <- paste(".kmacs.k", kval, ".tree", sep = "")
        kpat <- paste("*", ksuf, sep = "")
        rf.dist.dataset.k(kval, ksuf, kpat)
    })
    write.table(kdf, "balibase.kmacs.dist")
    kdf
}

rf.dist.spaced <- function(){
    kdf <- ldply(c(0), function(kval){
        ksuf <- paste(".spaced", ".tree", sep = "")
        kpat <- paste("*", ksuf, sep = "")
        rf.dist.dataset.k(kval, ksuf, kpat)
    })
    write.table(kdf, "balibase.spaced.dist")
    kdf
}

rf.dist.alfred <- function(kvals){
    kdf <- ldply(kvals, function(kval){
        ksuf <- paste("\\.alfred.x", kval, ".tree", sep = "")
        kpat <- paste("*", ksuf, sep = "")
        rf.dist.dataset.k(kval, ksuf, kpat)
    })
    write.table(kdf, "alfred.kacs.dist")
    kdf
}


rfiles <- list.files(run.dir, pattern = "RV*", full.names = T)
print(rfiles)

kacs.dist.df <- rf.dist.kacs(c(0,1,2,3,4,5))
kmacs.dist.df <- rf.dist.kmacs(c(0,1,2,3,4,5,6,7,8,9))
spaced.dist.df <- rf.dist.spaced()
alfred.dist.df <- rf.dist.alfred(c(0,1,2,3,4,5,6,7,8,9))

kacs.avg.df <- ddply(kacs.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "alfred-e", dist = avg.val)
})

kmacs.avg.df <- ddply(kmacs.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "kmacs", dist = avg.val)
})

spaced.avg.df <- ddply(spaced.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "spaced",dist = avg.val)
})

alfred.avg.df <- ddply(alfred.dist.df, .(errs), function(kdx){
    avg.val = mean(as.numeric(kdx$dist))
    c(dataset = "balibase", method = "alfred-h", dist = avg.val)
})

final.df = rbind(alfred.avg.df, kmacs.avg.df)
write.csv(final.df[,c(2,3,1,4)], "balibase.results.csv", row.names = FALSE)


# rf.dist(args[1], args[2])
