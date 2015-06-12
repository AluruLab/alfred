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

void compute_kdist(const double& xlen, const double& ylen,
                   const double& acsxy, const double& acsyx,
                   double& dxy){
    double dpxy, dpyx;
    dpxy = std::log(ylen) / acsxy;
    dpxy -= correct_term(xlen);
    dpyx = std::log(xlen) / acsyx;
    dpyx -= correct_term(ylen);
    dxy = (dpxy + dpyx)/2.0;
}


void compute_kdist(const ivec_t lcpkXY[2][2], double& dxy){
    double acsxy, acsyx;
    compute_kacs(lcpkXY, acsxy, acsyx);
    double xlen = (double)lcpkXY[0][1].size();
    double ylen = (double)lcpkXY[1][1].size();
    compute_kdist(xlen, ylen, acsxy, acsyx, dxy);
}

void compute_kacs(ReadsDB& rdb, AppConfig& cfg){
    unsigned nReads = rdb.getReadsCount();
    unsigned nFiles = rdb.getFilesCount();
    assert(nReads >= 2);
    assert(nReads == nFiles);

    cfg.ofs << nReads << std::endl;
    for(int i  = nReads - 1; i >= 0; i--){
        cfg.ofs << rdb.getFileName(i) << "\t";
        for(int j = i + 1; j < (int)nReads; j++){
            const std::string& sx = rdb.getReadById(i);
            const std::string& sy = rdb.getReadById(j);
            double xlen = sx.size(), ylen = sy.size();
            double acsxy, acsyx, kdxy;
            ExactLCPk lxy(sx, sy, cfg); // construct suffix array
            lxy.compute();
            compute_kacs(lxy.getkLCP(), acsxy, acsyx);
            compute_kdist(xlen, ylen, acsxy, acsyx, kdxy);
            // cfg.ofs << i << "\t" << j << "\t"
            //         << sx.size() << "\t" << sy.size() << "\t"
            //         << acsxy << "\t" << acsyx << "\t"
            //         << kdxy << std::endl;
            cfg.ofs << kdxy << "\t";
        }
        cfg.ofs << std::endl;
    }

}

void compute_kacs_naive(ReadsDB& rdb, AppConfig& cfg){
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            const std::string& sx = rdb.getReadById(i);
            const std::string& sy = rdb.getReadById(j);
            double xlen = sx.size(), ylen = sy.size();
            double acsxy, acsyx, kdxy;
            NaiveLCPk lxy(sx, sy, cfg); // construct suffix array
            lxy.compute();
            compute_kacs(lxy.getkLCP(), acsxy, acsyx);
            compute_kdist(xlen, ylen, acsxy, acsyx, kdxy);
            cfg.ofs << i << "\t" << j << "\t"
                    << sx.size() << "\t" << sy.size() << "\t"
                    << acsxy << "\t" << acsyx << "\t"
                    << kdxy << std::endl;
        }
    }

}
