#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "ExactLCPk.hpp"
#include "NaiveLCPk.hpp"

void print_lcpk(const unsigned& i, const unsigned& j, const ReadsDB& rdb,
                const ivec_t lcpKXY[2][2], const unsigned& k,
                std::ostream& lfs, const char* key){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    lfs << "\"" << key << "\"      : [" << std::endl;
    lfs << "   [" << i << ",\t" << j << ",\t" << sx.size() << ",\t"
        << sy.size() << ",\t" << k << "]," << std::endl << "    [" ;
    for(unsigned i = 0; i < lcpKXY[0][0].size(); i++)
        lfs << "[" << i
            << ", " << lcpKXY[0][0][i]
            << ", " << lcpKXY[0][1][i]
            << ((i == lcpKXY[0][0].size() - 1) ? "]" : "],\t");
    lfs << "]," << std::endl;
    lfs << "    [";
    for(unsigned i = 0; i < lcpKXY[1][0].size(); i++)
        lfs << "[" << i
            << ", " << lcpKXY[1][0][i]
            << ", " << lcpKXY[1][1][i]
            << ((i == lcpKXY[1][0].size() - 1) ? "]" : "],\t");
    lfs << "]" << std::endl << "  ]," << std::endl;

}

void process_pair_naive(unsigned i, unsigned j, ReadsDB& rdb,
                        AppConfig& cfg){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    cfg.kv = cfg.kv == 0 ? 1 : cfg.kv;
    NaiveLCPk lxy(sx, sy, cfg);
    lxy.compute();
#ifdef DEBUG_KLCP
    print_lcpk(i, j, rdb, lxy.getkLCP(), cfg.kv, cfg.lfs);
#endif
}

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& cfg) {
#ifdef DEBUG
    cfg.lfs << "\"klcp_debug\"      : [" << std::endl;
#endif
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    ExactLCPk lxy(sx, sy, cfg); // construct suffix array
#ifdef DEBUG
    lxy.print(cfg.lfs);
#endif
    lxy.compute();
#ifdef DEBUG
    cfg.lfs << " []]," << std::endl;
#endif
#ifdef DEBUG_KLCP
    print_lcpk(i, j, rdb, lxy.getkLCP(), 1, cfg.lfs);
#endif
}


void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    // TODO
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            if(cfg.naive)
                process_pair_naive(i, j, rdb, cfg);
            else
                process_pair(i, j, rdb, cfg);
            break;
        }
    }
}
