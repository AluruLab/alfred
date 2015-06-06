#include "compute_klcp.hpp"
#include "construct_sa.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>

class LCPOne{
    const std::string& sx;
    const std::string& sy;
    std::string pxy;
    ivec_t gsa, gisa, glcp;
    ivec_t lcp_xy[4];

public:
    LCPOne(const std::string& x, const std::string& y):sx(x), sy(y){
        pxy = sx + "$" + sy + "$";

        // construct SA, ISA and LCP
        construct_sa((const unsigned char*)pxy.c_str(), pxy.size(), gsa);
        construct_isa(gsa, gisa);
        construct_lcp_kasai(pxy.c_str(), gsa, gisa, glcp);
        //construct_lcp_PHI(pxy.c_str(), gsa, glcp);
    }

    void print(std::ostream& ofs){
        for(size_t i = 0; i < gsa.size();i++)
            ofs << std::setw(5) << gsa[i]
                << std::setw(5) << (i < glcp.size() ? glcp[i] : -1)
                << "    " << (pxy.c_str() + gsa[i])
                << std::endl;
        ofs << std::endl;
    }

    void compute(){
        // resize the LCP arrays
        lcp_xy[0].resize(sx.size()); lcp_xy[1].resize(sx.size());
        lcp_xy[2].resize(sy.size()); lcp_xy[2].resize(sy.size());
        // TODO: initialize the list with zero ?

        // TODO:
        // walk through the suffix array gsa
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        //   sort tuples by i'
        //   update_lcp with the sorted tuples
    }

    void update_lcp(){
        // TODO::
        // left -> right pass
        //   initialize src_ptr, tgt_ptr to the first shift
        //   initialize running min.
        //   do until we reach the end of list
        //      - update running min.
        //      - update target's lcp if it is better than current lcp.
        //      - move tgt_ptr
        //      - if tgt_ptr switches string,
        //           update src_ptr and running min.
        // right -> left pass
        //   similar to the above
    }
};

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& param) {
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    LCPOne lxy(sx,sy); // construct suffix array
    lxy.print(std::cout);
    lxy.compute();
}

void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    // TODO
    unsigned nReads = rdb.getReadsCount();

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            process_pair(i, j, rdb, cfg);
            break;
        }
    }
}
