#include <cmath>
#include "NaiveLCPk.hpp"
#include "ExactLCPk.hpp"

double correct_term(double xLen){
    if(xLen > 0)
        return 2 * (std::log(xLen)) / xLen;
    return 0;
}

void compute_kacs(const ivec_t lcpkXY[2][2],
                  double& acsxy, double& acsyx){
    acsxy = 0;
    for(unsigned i = 0; i < lcpkXY[0][1].size(); i++){
        acsxy += lcpkXY[0][1][i];
    }
    acsxy = acsxy / (double)lcpkXY[0][1].size();

    acsyx = 0;
    for(unsigned i = 0; i < lcpkXY[1][1].size(); i++){
        acsyx += lcpkXY[1][1][i];
    }
    acsyx = acsyx / (double)lcpkXY[1][1].size();
}

void compute_kdist(const ivec_t lcpkXY[2][2],
                   double& dxy){
    double acsxy, acsyx, dpxy, dpyx;
    compute_kacs(lcpkXY, acsxy, acsyx);
    double xlen = (double)lcpkXY[0][1].size();
    double ylen = (double)lcpkXY[1][1].size();
    dpxy = std::log(ylen) / acsxy;
    dpxy -= correct_term(xlen);
    dpyx = std::log(xlen) / acsyx;
    dpyx -= correct_term(ylen);
    dxy = (dpxy + dpyx)/2.0;
}


void compute_kacs(ReadsDB& rdb, AppConfig& cfg){
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            const std::string& sx = rdb.getReadById(i);
            const std::string& sy = rdb.getReadById(j);
            double acsxy, acsyx;
            ExactLCPk lxy(sx, sy, cfg); // construct suffix array
            lxy.compute();
            compute_kacs(lxy.getkLCP(), acsxy, acsyx);
            cfg.ofs << i << "\t" << j << "\t"
                    << sx.size() << "\t" << sy.size() << "\t"
                    << acsxy << "\t" << acsyx << std::endl;
        }
    }

}

void compute_kacs_naive(ReadsDB& rdb, AppConfig& cfg){
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            const std::string& sx = rdb.getReadById(i);
            const std::string& sy = rdb.getReadById(j);
            double acsxy, acsyx;
            NaiveLCPk lxy(sx, sy, cfg); // construct suffix array
            lxy.compute();
            compute_kacs(lxy.getkLCP(), acsxy, acsyx);
            cfg.ofs << i << "\t" << j << "\t"
                    << sx.size() << "\t" << sy.size() << "\t"
                    << acsxy << "\t" << acsyx << std::endl;
        }
    }

}
