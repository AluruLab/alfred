#include "compute_klcp.hpp"
#include "construct_sa.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

LCPOne::LCPOne(const std::string& sx, const std::string& sy,
               AppConfig& cfg) : m_aCfg(cfg){
    m_strXY = sx + "#" + sy + "$";
    m_strLength[0] = sx.size(); m_strLength[1] = sy.size();
    // construct SA, ISA, LCP and RMQ
    construct_sa((const unsigned char*)m_strXY.c_str(), m_strXY.size(), m_gsa);
    construct_isa(m_gsa, m_gisa);
    construct_lcp_kasai(m_strXY.c_str(), m_gsa, m_gisa, m_glcp);
    //construct_lcp_PHI(pxy.c_str(), gsa, glcp);
    m_rangeMinQuery =
        std::move(rmq_support_sparse_table<ivec_t, true, ivec_t>(&m_glcp));
}

void LCPOne::print(std::ostream& ofs){
    for(size_t i = 0; i < m_gsa.size();i++)
        ofs << " [" << std::setw(5) << i << ","
            << std::setw(5) << m_gsa[i] << ","
            << std::setw(5) << (i < m_glcp.size() ? m_glcp[i] : -1) << ","
            << "    \"" << (m_strXY.c_str() + m_gsa[i]) << "\"],"
            << std::endl;
}

int32_t LCPOne::getLeftEnd(const int32_t& curPos){
    if(curPos + 1 >= (int32_t)m_gsa.size()) // reached the end
        return -1;
    if(curPos == 2)
        return curPos;
    int32_t lpos;
    int32_t idx = curPos, idxLCP = m_glcp[idx + 1], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        lpos = idx;
        idx -= 1;
        if(idx < 2)
            break;
        idxLCP = m_glcp[idx + 1];
    }
    return lpos;
}

int32_t LCPOne::getRightEnd(const int32_t& curPos){
    if(curPos + 1 >= (int32_t)m_gsa.size()) // reached the end
        return -1;
    // compute rpos
    int32_t rpos;
    int32_t idx = curPos + 1, idxLCP = m_glcp[idx], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        rpos = idx;
        idx += 1;
        if(idx >= (int32_t)m_gsa.size())
            break;
        idxLCP = m_glcp[idx];
    }
    return rpos;
}

void LCPOne::getCandidateMatches(InternalNode& iNode,
                           std::vector<CandidateMatch>& candies){

    if(iNode.m_leftEnd == -1 || iNode.m_rightEnd == -1)
        return;

    assert(iNode.m_rightEnd > iNode.m_leftEnd);
    candies.clear();
    candies.resize(iNode.m_rightEnd - iNode.m_leftEnd + 1);

    //   collect tuples for each position (going left and right)
    //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
    for(int32_t idx = iNode.m_leftEnd, i = 0;
            idx <= iNode.m_rightEnd; idx++, i++){
        int32_t spos = m_gsa[idx];
        int32_t rs = (spos < m_strLength[0]) ? 0 : 1;
        int32_t epos = (spos + iNode.m_stringDepth + 1);
        // crossing boundary
        int32_t esa = (rs == 0) ?
            (epos <= (int32_t)m_strLength[0] ? m_gisa[epos] : -1) :
            (epos < (int32_t)m_strXY.size() ? m_gisa[epos] : -1);
        CandidateMatch cm(spos, esa, rs);
        candies[i] = cm;
    }
    //   sort tuples by i'
    std::sort(candies.begin(), candies.end());
    for(auto cm : candies)
        cm.dwriteln(m_aCfg.lfs);
}

void LCPOne::getInternalNodes(std::vector<InternalNode>& iNodes){
    iNodes.resize(m_gsa.size() - 2);
    // Get all the internal nodes
    for(int32_t i = 2; i < (int32_t)m_gsa.size(); i++){
        iNodes[i-2].m_leftEnd = getLeftEnd(i);
        iNodes[i-2].m_rightEnd = getRightEnd(i);
        iNodes[i-2].m_stringDepth = ((i + 1) < (int32_t)m_glcp.size()) ?
            m_glcp[(i + 1)] : -1;
    }

    // Eliminate duplicates
    std::sort(iNodes.begin(), iNodes.end());

    unsigned j = 0;
    for(unsigned i = 1; i < iNodes.size(); i++){
        if(iNodes[i] == iNodes[j])
            continue;
        j += 1;
        iNodes[j] = iNodes[i];
    }
    iNodes.resize(j+1);
}

void LCPOne::compute(){
    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++)
        for(unsigned j = 0; j  < 2;j++){
            m_lcpOneXY[i][j].resize(m_strLength[i], 0);
        }

    // get all the internal nodes
    std::vector<InternalNode> iNodes;
    getInternalNodes(iNodes);
    // for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++)
    //    (*nit).dwriteln(m_aCfg.lfs);
    //std::cout << m_rangeMinQuery(4, 6) << std::endl;
    //pstd::cout << m_rangeMinQuery(5, 8) << std::endl;

    // for each internal node
    for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++){
        (*nit).dwriteln(m_aCfg.lfs);
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        std::vector<CandidateMatch> candies;
        getCandidateMatches(*nit, candies);
        // update lcp using sorted tuples using a double pass
        updateLCPOne(*nit, candies);
    }
}

int32_t LCPOne::rangeMinLCP(const CandidateMatch& m1, const CandidateMatch& m2){
    if(m1.m_errSAPos == -1 || m2.m_errSAPos == -1)
        return 0;
    if(m1.m_errSAPos >= m2.m_errSAPos)
        return 0;
    if(m2.m_errSAPos > (int32_t)m_gsa.size())
        return 0;
    // TODO:: handle 0 and 1 case seperately ?
    int32_t rpos = m_rangeMinQuery(m1.m_errSAPos + 1,
                                   m2.m_errSAPos);
    return 1 + m_glcp[rpos];
}

int32_t LCPOne::strPos(int32_t& rid, int32_t& gPos){
    return (rid == 0) ? gPos : (gPos - m_strLength[0] - 1);
}

void LCPOne::updateLtoR(InternalNode& iNode,
                        std::vector<CandidateMatch>& candies){
    // left -> right pass
    //   initialize src_ptr, tgt_ptr to the first shift
    int32_t src_ptr = 0, tgt_ptr = 1, rmin = 0;
    while(tgt_ptr < (int32_t)candies.size()){
        if(candies[tgt_ptr].m_srcStr != candies[src_ptr].m_srcStr)
            break;
        tgt_ptr += 1;
    }
    // nothing to do
    if(tgt_ptr >= (int32_t)candies.size() ||
       candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
        return;
    //   do until we reach the end of list
    while(true){
        //candies[tgt_ptr].write(std::cout);
        int32_t tgt = candies[tgt_ptr].m_srcStr,
            tpos = strPos(tgt, candies[tgt_ptr].m_startPos);
        // - update running min.
        rmin = rangeMinLCP(candies[src_ptr], candies[tgt_ptr]);
        int32_t score = iNode.m_stringDepth + rmin;

        m_aCfg.lfs << "[" << iNode.m_stringDepth << ",\t" << rmin << ",\t";
        candies[src_ptr].write(m_aCfg.lfs, ",\t");
        m_aCfg.lfs << ",\t";
        candies[tgt_ptr].write(m_aCfg.lfs, ",\t");
        m_aCfg.lfs << ",\t" << tpos << ",\t" << m_strLength[tgt]
                   << ",\t" << score << "]," << std::endl;

        // - update target's lcp if it is better than current lcp.
        if(score > m_lcpOneXY[tgt][1][tpos]){
            m_lcpOneXY[tgt][0][tpos] = candies[src_ptr].m_startPos;
            m_lcpOneXY[tgt][1][tpos] = score;
        }
        // - move tgt_ptr
        tgt_ptr += 1;
        if(tgt_ptr >= (int32_t)candies.size())
            break;
        // - if tgt_ptr switches string,
        if(candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
            src_ptr = tgt_ptr - 1; // update src_ptr
    }
    std::cout << std::endl;
}

void LCPOne::updateRtoL(InternalNode& iNode,
                        std::vector<CandidateMatch>& candies){
    //   initialize src_ptr, tgt_ptr to the first shift
    int32_t src_ptr = candies.size() - 1,
        tgt_ptr = src_ptr - 1, rmin = 0;
    while(tgt_ptr > 0){
        if(candies[tgt_ptr].m_srcStr != candies[src_ptr].m_srcStr)
            break;
        tgt_ptr -= 1;
    }
    // nothing to do
    if(tgt_ptr < 0 ||
       candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
        return;
    //   do until we reach the end of list
    while(true){
        int32_t tgt = candies[tgt_ptr].m_srcStr,
            tpos = strPos(tgt, candies[tgt_ptr].m_startPos);
        // - update running min.
        rmin = rangeMinLCP(candies[tgt_ptr], candies[src_ptr]);
        int32_t score = iNode.m_stringDepth + rmin;

        m_aCfg.lfs << "[, \"R->L\", " << iNode.m_stringDepth << ",\t"
                   << rmin << ",\t";
        candies[src_ptr].write(m_aCfg.lfs, ",\t");
        m_aCfg.lfs << ",\t";
        candies[tgt_ptr].write(m_aCfg.lfs, ",\t");
        m_aCfg.lfs << ",\t" << tpos << ",\t" << m_strLength[tgt]
                   << ",\t" << score << "]," << std::endl;

        // - update target's lcp if it is better than current lcp.
        if(score > m_lcpOneXY[tgt][1][tpos]){
            m_lcpOneXY[tgt][0][tpos] = candies[src_ptr].m_startPos;
            m_lcpOneXY[tgt][1][tpos] = score;
        }
        // - move tgt_ptr
        tgt_ptr -= 1;
        if(tgt_ptr < 0)
            break;
        // - if tgt_ptr switches string,
        if(candies[tgt_ptr].m_srcStr == candies[src_ptr].m_srcStr)
            src_ptr = tgt_ptr + 1; // update src_ptr
    }
}

void LCPOne::updateLCPOne(InternalNode& iNode,
                          std::vector<CandidateMatch>& candies){
    m_aCfg.lfs << std::endl;
    for(auto cm: candies){
        cm.dwriteln(m_aCfg.lfs);
    }
    m_aCfg.lfs << std::endl;

    // left -> right pass
    updateLtoR(iNode, candies);
    // right -> left pass
    updateRtoL(iNode, candies);
}

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& cfg) {
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    LCPOne lxy(sx, sy, cfg); // construct suffix array
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
