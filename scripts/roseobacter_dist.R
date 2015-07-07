library("phangorn")
library("plyr")
library("stringr")
#
# Rscript roseobacter_dist.R
#   /Users/srirampc/work/phd/research/arakawa/data/roseobacter
#   /Users/srirampc/work/phd/research/arakawa/runs/roseobacter
args <- commandArgs(TRUE)
ref.dir <- args[1]
run.dir <- args[2]


rf.dist <- function(tfx, tfy){
    rx <- read.tree(tfx)
    ry <- read.tree(tfy)
    RF.dist(rx,  collapse.singles(ry))
}

rfiles <- list.files(run.dir, pattern = "^roseo.*tree$", full.names = T)
dist.df <- ldply(rfiles, function(tfx){
    bfn <- basename(tfx)
    spt <- str_split(bfn, "\\.")
    org <- spt[[1]][1]
    cld <- spt[[1]][2]
    mtd <- spt[[1]][3]
    kvx <- spt[[1]][4]
    if(kvx == "tree"){
        kvx = "k0"
    }
    tfy <- paste(ref.dir, paste(org, cld, "tree", sep = "."), sep = "/")
    print(paste(tfy, tfx))
    c(dataset = cld, method = mtd, errs = kvx,
      dist = rf.dist(tfx,tfy))
})

write.table(dist.df, "roseobacter.results.out")
