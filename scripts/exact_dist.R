library("phangorn")
library("plyr")
library("stringr")
#
# Rscript exact_dist.R ../data/primates ../runs/primates
args <- commandArgs(TRUE)
ref.dir <- args[1]
run.dir <- args[2]


rf.dist <- function(tfx, tfy){
    rx <- read.tree(tfx)
    ry <- read.tree(tfy)
    rx$tip.label[ rx$tip.label == "H.lar____0" ] = "H.lar"
    ry$tip.label[ ry$tip.label == "H.lar____0" ] = "H.lar"
    RF.dist(rx,  ry)
}

method.name <- function(fname, kvx){
    if(fname == "acs"){
        "alfred-e"
    } else if(fname == "alfred"){
        "alfred-h"
    } else if(fname == "kmacs" && kvx == "k0"){
        "kmacs"
    } else {
        fname
    }
}

err.value <- function(fname, kvx){
    if (fname == "spaced"){
        "X"
    } else {
        str_replace(str_replace(kvx, pattern = "x", replacement = ""),
                    pattern = "k", replacement = "")
    }
}

rfiles <- list.files(run.dir, pattern = "^primates\\.acs.*\\.[kx][0-9].tree$", full.names = T)
dist.df <- ldply(rfiles, function(tfx){
    bfn <- basename(tfx)
    spt <- str_split(bfn, "\\.")
    org <- spt[[1]][1]
    mtd <- spt[[1]][2]
    kvx <- spt[[1]][3]
    if(kvx == "tree"){
        kvx = "k0"
    }
    tfy <- paste(ref.dir, paste(org, "tree", sep = "."), sep = "/")
    print(paste(tfy, tfx))
    if(mtd == "acs" && kvx == "k0"){
        NULL
    }
    else {
        c(dataset = org, method = method.name(mtd, kvx),
          errs = err.value(mtd, kvx),
          dist = rf.dist(tfx,tfy))
    }
})

write.csv(dist.df, "exact.results.csv", row.names = F)
