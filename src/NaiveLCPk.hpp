#ifndef NAIVELCPK_H
#define NAIVELCPK_H

#include "defs.hpp"
#include "AppConfig.hpp"
#include <string>

class NaiveLCPk{
    const std::string& sx;
    const std::string& sy;
    AppConfig& m_aCfg;
    int m_kv;
    void runLCPk(const std::string& sx, const std::string& sy,
                 int tidx);
public:
    ivec_t m_klcpXY[2][2];
    ivec_t m_klcpHistoXY[2];
    NaiveLCPk(const std::string& x, const std::string& y, AppConfig& cfg);
    void compute();
    void computeTest(int k);
    auto getkLCP() -> const ivec_t (&)[2][2] {
        return m_klcpXY;
    }
    auto getkLCPHisto() -> const ivec_t (&)[2] {
      return m_klcpHistoXY;
    }

    void print(std::ostream& ){}
};

#endif /* NAIVELCPK_H */
