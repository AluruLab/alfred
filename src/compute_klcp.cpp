#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "ExactLCPk.hpp"
#include "NaiveLCPk.hpp"
#include "HeuristicLCPk.hpp"

void print_histo(const unsigned& i, const unsigned& j, const ReadsDB& rdb,
                 const ivec_t histoKXY[2], const unsigned& k,
                 std::ostream& hfs, const char* key){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    hfs << "\"" << key << "\"      : {" << std::endl;
#ifdef DEBUG
    hfs << "   \"meta\" : [" << i << ",\t" << j << ",\t" << sx.size() << ",\t"
        << sy.size() << ",\t" << k << "]," << std::endl;
#endif
    hfs << "   \"" << rdb.getReadNameById(i)<< "\": [" << std::endl;
    for(unsigned i = 0; i < histoKXY[0].size(); i++)
        hfs << "        "
            << histoKXY[0][i]
            << ((i == histoKXY[0].size() - 1) ? "\n" : ",\n");
    hfs << "   ]," << std::endl;
    hfs << "   \"" << rdb.getReadNameById(j)<< "\": [" << std::endl;
    for(unsigned i = 0; i < histoKXY[1].size(); i++)
        hfs << "        "
            << histoKXY[1][i]
            << ((i == histoKXY[1].size() - 1) ? "\n" : ",\n");
    hfs << "   ]" << std::endl;
    hfs << "  }," << std::endl;

}



void print_lcpk(const unsigned& i, const unsigned& j, const ReadsDB& rdb,
                const ivec_t lcpKXY[2][2], const unsigned& k,
                std::ostream& lfs, const char* key){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    lfs << "\"" << key << "\"      : {" << std::endl;
#ifdef DEBUG
    lfs << "   \"meta\" : [" << i << ",\t" << j << ",\t" << sx.size() << ",\t"
        << sy.size() << ",\t" << k << "]," << std::endl;
#endif
    lfs << "   \"" << rdb.getReadNameById(i)<< "\": [" << std::endl;
    for(unsigned i = 0; i < lcpKXY[0][0].size(); i++)
        lfs << "        "
            << "[" << i
            << ", " << lcpKXY[0][0][i]
            << ", " << lcpKXY[0][1][i]
            << ((i == lcpKXY[0][0].size() - 1) ? "]" : "],\n");
    lfs << "]," << std::endl;
    lfs << "   \"" << rdb.getReadNameById(j)<< "\": [" << std::endl;
    for(unsigned i = 0; i < lcpKXY[1][0].size(); i++)
        lfs << "        "
            << "[" << i
            << ", " << lcpKXY[1][0][i]
            << ", " << lcpKXY[1][1][i]
            << ((i == lcpKXY[1][0].size() - 1) ? "]" : "],\n");
    lfs << "]" << std::endl;
    lfs << "  }," << std::endl;

}

template<typename LCPk>
void klcp_pair_factory(unsigned i, unsigned j, ReadsDB& rdb,
                      AppConfig& cfg){
#ifdef DEBUG
    cfg.lfs << "\"klcp_debug\"      : [" << std::endl;
#endif
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    LCPk lxy(sx, sy, cfg); // construct suffix array
#ifdef DEBUG
    lxy.print(cfg.lfs);
#endif
    lxy.compute();
#ifdef DEBUG
    cfg.lfs << " []]," << std::endl;
#endif
#ifndef DEBUG_KLCP
    print_lcpk(i, j, rdb, lxy.m_klcpXY, cfg.kv, cfg.lfs, "lcpk");
#endif
    cfg.ofs << "{" << std::endl;
    print_lcpk(i, j, rdb, lxy.m_klcpXY, cfg.kv, cfg.ofs, "lcpk");
    cfg.ofs << "  \"end\": []" << std::endl
            << "}" << std::endl;

    if(cfg.histogram){
      cfg.histfs << "{" << std::endl;
      print_histo(i, j, rdb, lxy.getkLCPHisto(), cfg.kv,
                  cfg.histfs, "lcpkhisto");
      cfg.histfs << "  \"end\": []" << std::endl
              << "}" << std::endl;
    }
}

void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    // TODO
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            if(cfg.method == 1)
                klcp_pair_factory<NaiveLCPk>(i, j, rdb, cfg);
            else if(cfg.method == 2)
                klcp_pair_factory<HeuristicLCPk>(i, j, rdb, cfg);
            else
                klcp_pair_factory<ExactLCPk>(i, j, rdb, cfg);
            break;
        }
        break;
    }
}
