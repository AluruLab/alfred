#ifndef LR_HEURISTICLCPK_H
#define LR_HEURISTICLCPK_H

#include "ExactLCPk.hpp"
#include "AppConfig.hpp"

class LRHeuristicLCPk{
private:
    ExactLCPk m_fwdELCPk;
    ExactLCPk m_revELCPk;
    AppConfig& m_aCfg;
    const std::string& m_sx;
    const std::string& m_sy;
    ivec_t m_klcpXY[2][2];
    int32_t m_strLengths[2];
    int32_t m_shiftPos[2][2];
    int m_kextend;
    int32_t m_totalLength;

    int32_t fwd_rmq(int tidx, int32_t t1, int32_t t2);
    int32_t rev_rmq(int tidx, int32_t t1, int32_t t2);

    void computeBasis();
    void _compute();

    void extendRMQ(const std::string& sa, const std::string& sb,
                   int tidx);

    void update(int tidx, int32_t astart,
                std::vector<int32_t>& fwd_dist,
                std::vector<int32_t>& rev_dist);
    void extendCrawl(const std::string& sa, const std::string& sb,
                     int tidx);
    void extendCrawl();


    inline int32_t rev_pos(int32_t sfx){
        // example :
        //         0123456789
        //  FWD:   ABCD$EFGH#
        //  REV:   HGFE$DCBA#
        if(sfx < (m_totalLength - 1)){
            return (m_totalLength - 2  - sfx); // correct ?
        }
        return m_totalLength;
    }

public:
    LRHeuristicLCPk(const std::string& x, const std::string& y,
                    AppConfig& cfg);
    auto getkLCP() -> const ivec_t (&)[2][2] {
        return m_klcpXY;
    }

    void compute();
    void computeTest(int);
    void print(std::ostream&);
};


#endif /* LR_HEURISTICLCPK_H */
