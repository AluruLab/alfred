#ifndef COMPUTE_KLCP_H
#define COMPUTE_KLCP_H

#include "defs.hpp"
#include "AppConfig.hpp"
#include "ReadsDB.hpp"


class CandidateMatch{
private:
    int32_t m_startPos; // starting position
    int32_t m_errPos; // position after one error
    int32_t m_srcStr;   // source string
public:
    bool operator < (const CandidateMatch& other) const {
        return( m_errPos < other.m_errPos );
    };

    CandidateMatch(int32_t spos, int32_t epos, int32_t src):
        m_startPos(spos), m_errPos(epos), m_srcStr(src) { }

    CandidateMatch(){}

    void write(std::ostream& ots, const char *sepStr = "\t") const{
        ots << m_startPos << sepStr << m_errPos << sepStr
            << m_srcStr;
    };

    void emit(std::ostream& ots) const{
        ots << m_startPos << "\t" << m_srcStr;
    }

    void writeln(std::ostream& ots) const{
        write(ots);
        ots << std::endl;
    }

    void dwriteln(std::ostream& ots) const{
        ots << "[";
        write(ots, ",");
        ots << "]," << std::endl;
    }

};

class LCPOne{
private:
    AppConfig& m_aCfg;
    int32_t m_strLength[2];
    std::string m_strXY;
    ivec_t m_gsa, m_gisa, m_glcp;
    ivec_t m_lcpXY[2][2];

    int32_t getLeftBound(const int32_t& curPos);
    int32_t getRightBound(const int32_t& curPos);
    void getCandidates(int32_t curPos, std::vector<CandidateMatch>& candies);
    void makeCandidates(const int32_t& beginPos, const int32_t& endPos,
                        const int32_t& depth,
                        std::vector<CandidateMatch>& candies);
public:
    LCPOne(const std::string& x, const std::string& y, AppConfig& cfg);
    void print(std::ostream& ofs);
    void compute();
    void updateLCP();
};

void compute_klcp(ReadsDB& rdb, AppConfig& cfg);

#endif /* COMPUTE_KLCP_H */
