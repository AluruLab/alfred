library(stringr)
library(ggplot2)
library(ggplot2)
library(gridExtra)
library(grid)
source("../scripts/plot_helper.R")

grid_arrange_shared_legend <- function(...) {
    plots <- list(...)
    g <- ggplotGrob(plots[[1]] + theme(legend.position="bottom"))$grobs
    legend <- g[[which(sapply(g, function(x) x$name) == "guide-box")]]
    lheight <- sum(legend$height)
    arargs <- lapply(plots, function(x)
        x + theme(legend.position="none"))
    arargs$nrow = 1
    grid.arrange(
        do.call(gridExtra::arrangeGrob, arargs),
        legend,
        ncol = 1,
        heights = unit.c(unit(1, "npc") - lheight, lheight))
}


grid_arrange_shared_legend2 <- function(...) {
    plots <- list(...)
    g <- ggplotGrob(plots[[1]] + theme(legend.position="bottom"))$grobs
    legend <- g[[which(sapply(g, function(x) x$name) == "guide-box")]]
    lheight <- sum(legend$height)
    grid.arrange(
        do.call(arrangeGrob, lapply(plots, function(x)
            x + theme(legend.position="none"))),
        legend,
        nrow = 2,
        heights = unit.c(unit(1, "npc") - lheight, lheight))
}

roseobacter_plot <- function(bfname){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$errs = str_replace(bx$errs,pattern = "k",replacement = "")
  bx$label = str_c("k = ", as.character(bx$errs))
  bx$label[13] = ""
  bx = bx[2:nrow(bx),]

  bx[bx$method == "acs", "method"] = "ALFRED"
  bx[bx$method == "kmacs" & bx$errs == "0", "method" ] = "ACS"
  bx[bx$method == "kmacs", "method" ] = "Kmacs"
  bx[bx$method == "spaced", "method" ] = "Spaced seed"
  bx$label = str_c(" - ", bx$label)
  bx$label = str_c(bx$method, bx$label)
  bx$label[12] = "Spaced seed"
  ggplot(data = bx, aes(x = label, y = dist, fill=method)) +
    geom_bar(stat ="identity") +
    xlab("Method") + ylab("RF distance") +
    labs(fill = "Method") +
    theme(axis.text.x = element_text(angle=45, hjust=1))

}

primates_plot <- function(bfname){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$errs = str_replace(bx$errs,pattern = "k",replacement = "")
  bx$label = str_c("k = ", as.character(bx$errs))
  bx$label[13] = ""
  bx = bx[2:nrow(bx),]

  bx[bx$method == "acs", "method"] = "ALFRED"
  bx[bx$method == "kmacs" & bx$errs == "0", "method" ] = "ACS"
  bx[bx$method == "kmacs", "method" ] = "Kmacs"
  bx[bx$method == "spaced", "method" ] = "Spaced seed"
  bx$label = str_c(" - ", bx$label)
  bx$label = str_c(bx$method, bx$label)
  bx$label[12] = "Spaced seed"
  ggplot(data = bx, aes(x = label, y = dist, fill=method)) +
    geom_bar(stat ="identity") +
    xlab("Method") + ylab("RF distance") +
    labs(fill = "Method") +
    theme(axis.text.x = element_text(angle=45, hjust=1))
}



balibase_plot <- function(bfname){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$label = str_c("k = ", as.character(bx$errs))
  bx$label[14] = ""
  bx = bx[2:nrow(bx),]
  bx[bx$method == "acs", "method"] = "ALFRED"
  bx[bx$method == "kmacs" & bx$errs == 0, "method" ] = "ACS"
  bx[bx$method == "kmacs", "method" ] = "Kmacs"
  bx[bx$method == "spaced", "method" ] = "Spaced seed"
  bx$label = str_c(" - ", bx$label)
  bx$label = str_c(bx$method, bx$label)
  bx$label[13] = "Spaced seed"
  ggplot(data = bx, aes(x = label, y = dist, fill=method)) +
    geom_bar(stat ="identity") +
    xlab("Method") + ylab("RF distance") +
    labs(fill = "Method") +
    theme(axis.text.x = element_text(angle=45, hjust=1))
}

cbPalette <- c("#FC9272", "#08519C")

dist_plot <- function(bfname){
  bx = read.csv(bfname, header = T, as.is = T)
  bx = bx[(bx$method == "alfred-h") | (bx$method == "kmacs"),]
  bx = bx[(bx$errs %in% c(0,1,2,3,4,5,6,7,8,9)), ]
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  print(levels(bx$method))
  levels(bx$method) = c("ALFRED-G","Kmacs")
  ggplot(data = bx, aes(x=errs, y=dist, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    xlab("Hamming Distance, k") +
    ylab("Robinson-Foulds Distance") +
    labs(fill="") + theme(legend.position="top") +
    theme(legend.key.size = unit(0.3, "cm")) +
    theme(axis.title=element_text(size=10)) +
    scale_fill_manual(values=cbPalette)
}

time_plot <- function(bfname, dataset){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("ALFRED-G","Kmacs")
  ggplot(data = bx[bx$dataset == dataset, ],
         aes(x=errs, y=time, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    xlab("Hamming Distance, k") +
    ylab("Time (seconds)") +
    labs(fill="") + theme(legend.position="top") +
    theme(legend.key.size = unit(0.3, "cm")) +
    theme(axis.title=element_text(size=10)) +
    scale_fill_manual(values=cbPalette)
}

rb_plot <- function(){
  dpr = dist_plot("../scripts/roseobacter.results.csv")
  tpr = time_plot("../scripts/timings.csv", "roseobacter.full")
  
  pdf("roseobacter_plot.pdf", width=6.94, height=3.88, onefile=FALSE)
  grid_arrange_shared_legend(dpr,tpr)
  dev.off()  
}

bl_plot <- function(){
  dpb = dist_plot("../scripts/balibase.results.csv")
  tpb = time_plot("../scripts/timings.csv", "balibase")
  pdf("balibase_plot.pdf", width=6.94, height=3.88, onefile=FALSE)
  grid_arrange_shared_legend(dpb,tpb)
  dev.off()  
}

pm_plot <- function(){
  dpp = dist_plot("../scripts/primates.results.csv")
  tpp = time_plot("../scripts/timings.csv", "primates")
  pdf("primates_plot.pdf", width=6.94, height=3.88, onefile=FALSE)
  grid_arrange_shared_legend(dpp,tpp)
  dev.off()  
  
}


exact_dist_plot <- function(csv.name, dataset){
  bx = read.csv(csv.name, header = T, as.is = T)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("AlFreD (Exact)")
  ggplot(data = bx, aes(x=errs, y=dist, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    geom_text(aes(label = dist, y = dist+0.125), size = 3) +
    xlab("Hamming Distance, k") +
    ylab("Robinson-Foulds Distance") +
    theme(legend.position="none") +
    #labs(fill="") + 
    #theme(legend.position="top") +
    #theme(legend.key.size = unit(0.3, "cm")) +
    scale_fill_brewer(palette="Set2") +
    theme(axis.title=element_text(size=10)) 
}

analysis_time_plot <- function(csv.name, dataset, tshift=70){
  bx = read.csv(csv.name, header = T, as.is = T)
  bx$tshift=tshift + (bx$time)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("Analysis")
  ggplot(data = bx[bx$dataset == dataset, ],
         aes(x=errs, y=time, fill=method)) +
    geom_point() +
    geom_text(aes(label = time, y = time+tshift), size = 3) +
    xlab("Hamming Distance, k") +
    ylab("Complexity Analysis Time") +
    theme(legend.position="none") +
    scale_y_log10()+
    #labs(fill="") + 
    #theme(legend.position="top") +
    #theme(legend.key.size = unit(0.3, "cm")) +
    #scale_color_brewer(palette="Set2") +
    theme(axis.title=element_text(size=10)) 
}

exact_time_plot <- function(csv.name, dataset, tshift=70){
  bx = read.csv(csv.name, header = T, as.is = T)
  bx$tshift=tshift
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("AlFreD (Exact)")
  ggplot(data = bx[bx$dataset == dataset, ],
         aes(x=errs, y=time, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    geom_text(aes(label = time, y = time+tshift), size = 3) +
    xlab("Hamming Distance, k") +
    ylab("Run Time (seconds)") +
    theme(legend.position="none") +
    #labs(fill="") + 
    #theme(legend.position="top") +
    #theme(legend.key.size = unit(0.3, "cm")) +
    scale_fill_brewer(palette="Set2") +
    theme(axis.title=element_text(size=10)) 
}

etm_plot <- function(){
  etp = exact_time_plot("../scripts/exact.timings.csv", "primates", 70)
  edp = exact_dist_plot("../scripts/exact.results.csv", "primates")
  pdf("exact_primates_plot.pdf", width=6.94, height=3.88, onefile=FALSE)
  multiplot(etp,edp, cols=2)
  dev.off()  
  
  droso_plot = exact_time_plot("../scripts/exact.timings.csv",
                               "droso", 120)
  #pdf("exact_droso_plot.pdf", width=3.47, height=3.88, onefile=FALSE)
  #dev.off()  
  ggsave(filename = "exact_droso_plot.pdf", plot=droso_plot,
         width=3.47,height=3.88,units="in")

  ecoli_plot = exact_time_plot("../scripts/exact.timings.csv",
                               "seq4Mx2", 280)
  ggsave(filename = "exact_ecoli_plot.pdf", plot=ecoli_plot,
         width=3.55,height=3.88,units="in")
}

exact_ecoli_plot2 <- function(){
  p1 = analysis_time_plot("analysis_time.csv", "seq4Mx2", 0)
  p2 = exact_time_plot("exact.timings.csv", "seq4Mx2", 190)
  pdf("exact_ecoli_plot2.pdf", width=6.94, height=3.88, onefile=FALSE)
  multiplot(p1,p2, cols=3)
  dev.off()  
}

exact_primates_plot2 <- function(){
  p1 = analysis_time_plot("analysis_time.csv", "primates", 0)
  etp = exact_time_plot("../scripts/exact.timings.csv", "primates", 70)
  edp = exact_dist_plot("../scripts/exact.results.csv", "primates")
  pdf("exact_primates_plot2.pdf", width=9.94, height=3.88, onefile=FALSE)
  multiplot(p1, etp,edp, cols=3)
  dev.off()  
}
exact_dist_plot_grant <- function(csv.name, dataset){
  bx = read.csv(csv.name, header = T, as.is = T)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("AlFreD (Exact)")
  ggplot(data = bx, aes(x=errs, y=dist, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    # geom_text(aes(label = dist, y = dist+0.125), size = 3) +
    xlab("Hamming Distance, k") +
    ylab("RF Distance") +
    theme(legend.position="none") +
    #labs(fill="") + 
    #theme(legend.position="top") +
    #theme(legend.key.size = unit(0.3, "cm")) +
    scale_fill_brewer(palette="Set2") +
    theme(axis.title=element_text(size=22),
          axis.text=element_text(size=20))
}


exact_time_plot_grant <- function(csv.name, dataset, tshift=70){
  bx = read.csv(csv.name, header = T, as.is = T)
  bx$tshift=tshift
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  bx$time = bx$time / 60.0
  levels(bx$method) = c("AlFreD (Exact)")
  ggplot(data = bx[bx$dataset == dataset, ],
         aes(x=errs, y=time, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    # geom_text(aes(label = time, y = time+tshift), size = 3) +
    xlab("Hamming Distance, k") +
    ylab("Run Time (min)") +
    theme(legend.position="none") +
    #labs(fill="") + 
    #theme(legend.position="top") +
    #theme(legend.key.size = unit(0.3, "cm")) +
    scale_fill_brewer(palette="Set2") +
    theme(axis.title=element_text(size=22),
          axis.text=element_text(size=20))
}


ecoli_plot_grant <- function(){
  etp = exact_time_plot_grant("../scripts/exact.timings.csv", "primates", 70)
  edp = exact_dist_plot_grant("../scripts/exact.results.csv", "primates")
  pdf("exact_primates_plot_grant.pdf", width=6.94, height=3.88, onefile=FALSE)
  multiplot(etp,edp, cols=2)
  dev.off()  
}

#rb_plot()
#bl_plot()
#pm_plot()
#ecoli_plot_grant()

