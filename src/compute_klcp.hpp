#ifndef COMPUTE_KLCP_H
#define COMPUTE_KLCP_H

#include "defs.hpp"
#include "AppConfig.hpp"
#include "ReadsDB.hpp"
#include "rmq_support_sparse_table.hpp"

struct InternalNode{
    int32_t m_leftEnd;
    int32_t m_rightEnd;
    int32_t m_stringDepth;

    bool operator< (const InternalNode& other) const {
        return
            (m_leftEnd < other.m_leftEnd) ? true :
            ((m_leftEnd == other.m_leftEnd) ? (m_rightEnd < other.m_rightEnd) :
             false);
    }

    bool operator== (const InternalNode& other) const{
        return
            (m_leftEnd == other.m_leftEnd) &&
            (m_rightEnd == other.m_rightEnd) &&
            (m_stringDepth == other.m_stringDepth);
    }

    InternalNode(){
        m_leftEnd = m_rightEnd = -1;
    }

    void write(std::ostream& ots, const char *sepStr = "\t") const{
        ots << m_leftEnd << sepStr << m_rightEnd << sepStr
            << m_stringDepth;
    };

    void writeln(std::ostream& ots) const{
        write(ots);
        ots << std::endl;
    }

    void dwriteln(std::ostream& ots) const{
        ots << " [";
        write(ots, ",\t");
        ots << "]," << std::endl;
    }

};

struct CandidateMatch{
    int32_t m_startPos; // starting position
    int32_t m_errSAPos; // position after one error's SA loc.
    int32_t m_srcStr;   // source string

    bool operator< (const CandidateMatch& other) const {
        return( m_errSAPos < other.m_errSAPos );
    }

    CandidateMatch(int32_t spos, int32_t epos, int32_t src):
        m_startPos(spos), m_errSAPos(epos), m_srcStr(src) { }

    CandidateMatch(){}

    void write(std::ostream& ots, const char *sepStr = "\t") const{
        ots << m_startPos << sepStr << m_errSAPos << sepStr
            << m_srcStr;
    }

    void emit(std::ostream& ots) const{
        ots << m_startPos << "\t" << m_srcStr;
    }

    void writeln(std::ostream& ots) const{
        write(ots);
        ots << std::endl;
    }

    void dwriteln(std::ostream& ots) const{
        ots << "\t[";
        write(ots, ",\t");
        ots << "]," << std::endl;
    }

};

class UpperBoundCheck{
public:
    bool operator()(const int32_t& value, const int32_t& bound) const{
        return (value < bound);
    }
};

class LowerBoundCheck{
public:
    bool operator()(const int32_t& value, const int32_t& bound) const{
        return (value > bound);
    }
};

class IncrPointer{
public:
    void operator()(int32_t& value){
        value += 1;
    }
};

class DecrPointer{
public:
    void operator()(int32_t& value){
        value -= 1;
    }
};

class LCPOne{
private:
    AppConfig& m_aCfg;
    int32_t m_strLength[2];
    std::string m_strXY;
    ivec_t m_gsa, m_gisa, m_glcp;
    ivec_t m_lcpOneXY[2][2];
    rmq_support_sparse_table<ivec_t, true, ivec_t> m_rangeMinQuery;

    int32_t getLeftEnd(const int32_t& curPos);
    int32_t getRightEnd(const int32_t& curPos);
    void getCandidateMatches(InternalNode& iNode,
                             std::vector<CandidateMatch>& candies);
    void getInternalNodes(std::vector<InternalNode>& iNodes);
    void updateLCPOne(InternalNode& iNode, std::vector<CandidateMatch>& candies);
    void updateLtoR(InternalNode& iNode, std::vector<CandidateMatch>& candies);
    void updateRtoL(InternalNode& iNode, std::vector<CandidateMatch>& candies);
    int32_t rangeMinLCP(const CandidateMatch& m1, const CandidateMatch& m2);
    int32_t strPos(const int32_t& rid, const int32_t& gPos);

    template<typename BoundChecker, typename NextPointer>
    void updatePass(int32_t src_ptr, int32_t tgt_ptr,
                    const int32_t& tgt_bound,
                    const InternalNode& iNode,
                    const std::vector<CandidateMatch>& candies
#ifdef DEBUG
                    ,
                    const std::string& dbgStr
#endif
                    ) {
        BoundChecker bound_check;
        NextPointer next_ptr;
        int32_t rmin = 0;
        // move the pointer until we reach the first src, target
        while(bound_check(tgt_ptr, tgt_bound)){
            if(candies[tgt_ptr].m_srcStr != candies[src_ptr].m_srcStr)
                break;
            src_ptr = tgt_ptr;
            next_ptr(tgt_ptr);
        }
        // if not within the bounds leave
        if(!bound_check(tgt_ptr, tgt_bound) ||
           candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
            return;
        while(true){
            int32_t tgt = candies[tgt_ptr].m_srcStr,
                tpos = strPos(tgt, candies[tgt_ptr].m_startPos);
            // - update running min.
            rmin = rangeMinLCP(candies[src_ptr], candies[tgt_ptr]);
            int32_t score = iNode.m_stringDepth + rmin;
#ifdef DEBUG
            m_aCfg.lfs << "\t[ \"" << dbgStr << "\",\t"
                       << iNode.m_stringDepth << ",\t"
                       << rmin << ",\t";
            candies[src_ptr].write(m_aCfg.lfs, ",\t");
            m_aCfg.lfs << ",\t";
            candies[tgt_ptr].write(m_aCfg.lfs, ",\t");
            m_aCfg.lfs << ",\t" << tpos << ",\t" << m_strLength[tgt]
                       << ",\t" << score << "]," << std::endl;
#endif
            // - update target's lcp if it is better than current lcp.
            if(score > m_lcpOneXY[tgt][1][tpos]){
                m_lcpOneXY[tgt][0][tpos] = candies[src_ptr].m_startPos;
                m_lcpOneXY[tgt][1][tpos] = score;
            }
            int32_t prev_tgt = tgt_ptr;
            next_ptr(tgt_ptr);
            if(!bound_check(tgt_ptr, tgt_bound))
                break;
            // - if tgt_ptr switches string,
            if(candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
                src_ptr = prev_tgt; // update src_ptr
        }
    }

public:
    LCPOne(const std::string& x, const std::string& y, AppConfig& cfg);
    void print(std::ostream& ofs);
    void compute();
};

void compute_klcp(ReadsDB& rdb, AppConfig& cfg);

#endif /* COMPUTE_KLCP_H */
