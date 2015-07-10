library("phangorn")
library("plyr")
library("stringr")
#
# Rscript roseobacter_dist.R
#   /Users/srirampc/work/phd/research/arakawa/data/primates
#   /Users/srirampc/work/phd/research/arakawa/runs/primates
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

rfiles <- list.files(run.dir, pattern = "^primates.*tree$", full.names = T)
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
    c(dataset = org, method = mtd, errs = kvx,
      dist = rf.dist(tfx,tfy))
})

write.csv(dist.df, "primates.results.csv", row.names = F)
