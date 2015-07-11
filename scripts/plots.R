library(stringr)
library(ggplot2)
library(ggplot2)
library(gridExtra)

grid_arrange_shared_legend <- function(...) {
    plots <- list(...)
    g <- ggplotGrob(plots[[1]] + theme(legend.position="bottom"))$grobs
    legend <- g[[which(sapply(g, function(x) x$name) == "guide-box")]]
    lheight <- sum(legend$height)
    arargs <- lapply(plots, function(x)
        x + theme(legend.position="none"))
    arargs$nrow = 1
    grid.arrange(
        do.call(arrangeGrob, arargs),
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

dist_plot <- function(bfname){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("AlFreD (Hueristic) ","Kmacs")
  ggplot(data = bx, aes(x=errs, y=dist, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    xlab("Errors") +
    ylab("Robinson-Foulds Distance") +
    labs(fill="") + theme(legend.position="top") +
    theme(legend.key.size = unit(0.3, "cm")) +
    theme(axis.title=element_text(size=10)) +
    scale_fill_brewer(palette="Set1")
}

time_plot <- function(bfname, dataset){
  bx = read.csv(bfname, header = T, as.is = T)
  bx$errs = factor(bx$errs)
  bx$method = factor(bx$method)
  levels(bx$method) = c("AlFreD (Hueristic) ","Kmacs")
  ggplot(data = bx[bx$dataset == dataset, ],
         aes(x=errs, y=time, fill=method)) +
    geom_bar(stat="identity", position="dodge") +
    xlab("Errors") +
    ylab("Time (seconds)") +
    labs(fill="") + theme(legend.position="top") +
    theme(legend.key.size = unit(0.3, "cm")) +
    theme(axis.title=element_text(size=10)) +
    scale_fill_brewer(palette="Set1")
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

rb_plot()
bl_plot()
pm_plot()


