#include "compute_klcp.hpp"

void process_ancestor(t_cst& pst, t_node& leafx,
                      t_node& vancestor, t_node& srcx){
    // process ancestor
    unsigned ncount = 0;
    std::cout << "X" << " " << srcx << " ";
    for(auto vchild : pst.children(vancestor)){
        //  if child is not an ancestor or leafx
        if(vchild == srcx)
            continue;
        ncount += 1;
        // TODO:
        // get left end and right ends
        unsigned vlb = pst.lb(vchild), vrb = pst.rb(vchild);

        // get string depth of vparent -> d
        unsigned d = pst.depth(srcx);

        // get position of vnd's suffix + d + 1's from ISA
        // binary search between vlb, vrb
    }
    std::cout << pst.id(vancestor) << " " << ncount << " "
              << std::endl;
}

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& param) {
    const std::string& s1 = rdb.getReadById(i);
    const std::string& s2 = rdb.getReadById(j);

    std::string strpx = s1 + "$" + s2 + "$";

    param.lfs << "\"db_str_len\"  : " << strpx.length() << "," << std::endl;
    t_cst pst;
    construct_im(pst, strpx.c_str(), 1);

    // Suffix Tree STATS
    param.lfs << "\"suffix_tree\" : {" << std::endl
              << " \t\"INNER_NODES\" : " << pst.nodes() - pst.csa.size()
              << "," << std::endl
              << " \t\"LEAVES\"      : " << pst.csa.size() << "," << std::endl
              << " \t\"ALL_NODES\"   : " << pst.nodes() << std::endl
              << " }," << std::endl;

    unsigned nans = 0;

    // walk through the leaves
    for(unsigned i = 1;i <= pst.csa.size(); i++) {
        t_node leafx = pst.select_leaf(i);
        t_node vans = pst.parent(leafx), vsrc = leafx;

        while(vans != pst.root()){
            // process ancestor
            process_ancestor(pst, leafx, vans, vsrc);
            vsrc = vans; vans = pst.parent(vsrc); nans++;
        }
        // TODO: process root ?
    }
    //
    std::cout << nans << std::endl;

}

void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    // TODO
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >=2);
    process_pair(0, 1, rdb, cfg);
}
