#include "compute_klcp.hpp"
#include "construct_sa.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>

LCPOne::LCPOne(const std::string& sx, const std::string& sy,
               AppConfig& cfg) : m_aCfg(cfg){
    m_strXY = sx + "#" + sy + "$";
    m_strLength[0] = sx.size(); m_strLength[1] = sy.size();

    // construct SA, ISA and LCP
    construct_sa((const unsigned char*)m_strXY.c_str(), m_strXY.size(), m_gsa);
    construct_isa(m_gsa, m_gisa);
    construct_lcp_kasai(m_strXY.c_str(), m_gsa, m_gisa, m_glcp);
    //construct_lcp_PHI(pxy.c_str(), gsa, glcp);
}

void LCPOne::print(std::ostream& ofs){
    for(size_t i = 0; i < m_gsa.size();i++)
        ofs << " [" << std::setw(5) << i << ","
            << std::setw(5) << m_gsa[i] << ","
            << std::setw(5) << (i < m_glcp.size() ? m_glcp[i] : -1) << ","
            << "    \"" << (m_strXY.c_str() + m_gsa[i]) << "\"],"
            << std::endl;
}

int32_t LCPOne::getLeftBound(const int32_t& curPos){
    if(curPos + 1 >= (int32_t)m_gsa.size()) // reached the end
        return -1;
    if(curPos == 2)
        return curPos;
    int32_t bpos;
    int32_t idx = curPos, idxLCP = m_glcp[idx + 1], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        bpos = idx;
        idx -= 1;
        if(idx < 2)
            break;
        idxLCP = m_glcp[idx + 1];
    }
    return bpos;
}

int32_t LCPOne::getRightBound(const int32_t& curPos){
    if(curPos + 1 >= (int32_t)m_gsa.size()) // reached the end
        return -1;
    // compute epos
    int32_t epos;
    int32_t idx = curPos + 1, idxLCP = m_glcp[idx], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        epos = idx;
        idx += 1;
        if(idx >= (int32_t)m_gsa.size())
            break;
        idxLCP = m_glcp[idx];
    }
    return epos;
}

void LCPOne::makeCandidates(const int32_t& beginPos, const int32_t& endPos,
                            const int32_t& depth,
                            std::vector<CandidateMatch>& candies){
    if(beginPos == -1 || endPos == -1)
        return;

    candies.resize(endPos - beginPos + 1);

    for(int32_t idx = beginPos, i = 0; idx <= endPos; idx++, i++){
        int32_t sp = m_gsa[idx];
        int32_t rs = (sp < m_strLength[0]) ? 0 : 1;
        int32_t ep = m_gisa[sp + depth + 1];
        //int32_t re = (ep < m_strLength[0]) ? 0 : 1;
        //if(rs != re) // crossing boundary
        //ep = m_gsa[0];
        CandidateMatch cm(sp, ep, rs);
        candies[i] = cm;
    }
}

void LCPOne::getCandidates(int32_t curPos, std::vector<CandidateMatch>& candies){

    int32_t
        beginPos = getLeftBound(curPos),
        endPos = getRightBound(curPos);

    if(beginPos == -1 || endPos == -1)
        return;

    int32_t depth = m_glcp[curPos + 1];
    m_aCfg.lfs << " [\"CDS\",\t" << curPos << ",\t" << depth
               << ",\t" << beginPos << ",\t" << endPos << "]," << std::endl;

    candies.clear();
    makeCandidates(beginPos, endPos, depth, candies);
    for(auto cm : candies)
        cm.dwriteln(m_aCfg.lfs);
}

void LCPOne::compute(){
    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++)
        for(unsigned j = 0; j  < 2;j++){
            m_lcpXY[i][j].resize(m_strLength[i]);
        }

    // TODO: initialize the list with zero ?

    // TODO:
    // walk through the suffix array gsa
    for(int32_t i = 2; i < (int32_t)m_gsa.size(); i++){
        std::vector<CandidateMatch> candies;
        getCandidates(i, candies);
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        //   sort tuples by i'
        //   update_lcp with the sorted tuples
    }
}

void LCPOne::updateLCP(){
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

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& cfg) {
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    LCPOne lxy(sx, sy, cfg); // construct suffix array
    lxy.print(cfg.lfs);
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
