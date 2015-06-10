#include "compute_klcp.hpp"
#include "construct_sa.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

LCPOne::LCPOne(const std::string& sx, const std::string& sy,
               AppConfig& cfg) : m_aCfg(cfg){
    m_strXY = sx + "#" + sy + "$";
    m_strLengths[0] = sx.size(); m_strLengths[1] = sy.size();
    m_strLenPfx[0] = sx.size(); m_strLenPfx[1] = sx.size() + 1 + sy.size();
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

int32_t LCPOne::leftBound0(int32_t curLeaf){
    if(curLeaf + 1 >= (int32_t)m_gsa.size()) // reached the end
        curLeaf = (int32_t)m_gsa.size() - 2;
    if(curLeaf <= 2) // First two suffixes comes from the separators
        return 2;
    int32_t lpos, idxLCP = m_glcp[curLeaf + 1], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        lpos = curLeaf;
        curLeaf -= 1;
        if(curLeaf < 2)
            break;
        idxLCP = m_glcp[curLeaf + 1];
    }
    return lpos;
}

int32_t LCPOne::rightBound0(int32_t curLeaf){
    if(curLeaf <= 2) // First two suffixes comes from the separators
        curLeaf = 2;
    if(curLeaf + 1 >= (int32_t)m_gsa.size()) // reached the end
        return curLeaf;
    // compute rpos
    curLeaf += 1;
    int32_t rpos;
    int32_t idxLCP = m_glcp[curLeaf], srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        rpos = curLeaf;
        curLeaf += 1;
        if(curLeaf >= (int32_t)m_gsa.size())
            break;
        idxLCP = m_glcp[curLeaf];
    }
    return rpos;
}

void LCPOne::chopSuffixes0(const InternalNode& iNode,
                           std::vector<L1Suffix>& candies){

    assert(iNode.m_rightBound > iNode.m_leftBound);
    assert(iNode.m_leftBound >= 2);
    assert(iNode.m_rightBound < (int32_t)m_gsa.size());

    candies.clear();
    candies.resize(iNode.m_rightBound - iNode.m_leftBound + 1);

    //   collect tuples for each position (going left and right)
    //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
    for(int32_t idx = iNode.m_leftBound, i = 0;
            idx <= iNode.m_rightBound; idx++, i++){
        int32_t spos = m_gsa[idx];
        int32_t rs = spos < m_strLengths[0] ? 0 : 1;
        int32_t epos = spos + iNode.m_stringDepth + 1;
        // crossing boundary
        int32_t esa = epos <= m_strLenPfx[rs] ? m_gisa[epos] : -1;
            // (rs == 0) ?
            // (epos <= (int32_t)m_strLengths[0] ? m_gisa[epos] : -1) :
            // (epos < (int32_t)m_strXY.size() ? m_gisa[epos] : -1);
        L1Suffix cm(spos, esa, rs);
        candies[i] = cm;
    }
    //   sort tuples by i'
    std::sort(candies.begin(), candies.end());
    //for(auto cm : candies)
    //    cm.dwriteln(m_aCfg.lfs);
}

void LCPOne::eliminateDupes(std::vector<InternalNode>& iNodes){
    // sort
    std::sort(iNodes.begin(), iNodes.end());
    // move up
    unsigned j = 0;
    for(unsigned i = 1; i < iNodes.size(); i++){
        if(iNodes[i] == iNodes[j])
            continue;
        j += 1;
        iNodes[j] = iNodes[i];
    }
    iNodes.resize(j+1);
}

void LCPOne::selectInternalNodes0(std::vector<InternalNode>& iNodes){
    iNodes.resize(m_gsa.size() - 2);

    // Get all the internal nodes
    for(int32_t i = 2; i < (int32_t)m_gsa.size(); i++){
        iNodes[i-2].m_leftBound = leftBound0(i);
        iNodes[i-2].m_rightBound = rightBound0(i);
        iNodes[i-2].m_stringDepth = m_glcp[i + 1];
    }

    // Eliminate duplicates
    eliminateDupes(iNodes);
}

int32_t LCPOne::rangeMinLCP(const int32_t& t1, const int32_t& t2){
    if(t1 < 0 || t2 < 0)
        return 0;
    int32_t mxv = std::max(t1, t2);
    if(mxv > (int32_t)m_gsa.size())
        return 0;
    int32_t mnv = std::min(t1, t2);
    int32_t rpos = m_rangeMinQuery(mnv + 1, mxv);
    return 1 + m_glcp[rpos];
}

int32_t LCPOne::rangeMinLCP(const L1Suffix& m1, const L1Suffix& m2){
    return rangeMinLCP(m1.m_errSAPos, m2.m_errSAPos);
}

int32_t LCPOne::strPos(const int32_t& rid, const int32_t& gPos){
    return (rid == 0) ? gPos : (gPos - m_strLengths[0] - 1);
}

void LCPOne::updateLCPOne(InternalNode& iNode,
                          std::vector<L1Suffix>& candies){
#ifdef DEBUG
    m_aCfg.lfs << std::endl;
    for(auto cm: candies){
        cm.dwriteln(m_aCfg.lfs);
    }
    m_aCfg.lfs << std::endl;
#endif

    // left -> right pass
    //updateLtoR(iNode, candies);
    updatePass<UpperBoundCheck, IncrPointer>(0, 1, (int32_t)candies.size(),
                                             iNode, candies
#ifdef DEBUG
                                             , "L->R"
#endif
                                             );
    // right -> left pass
    updatePass<LowerBoundCheck, DecrPointer>((int32_t)candies.size() - 1,
                                             (int32_t)candies.size() - 2, 0,
                                             iNode, candies
#ifdef DEBUG
                                             , "R->L"
#endif

                                             );

#ifdef DEBUG
    m_aCfg.lfs << std::endl;
#endif
}

//
//  Assuming that the trieLeaves are sorted by m_errSAPos
//  In order to maintain consistency,
//    - we start with curLeaf and its next as the tree 'x'
//    - we grow this tree 'x' until we reach its left end
//
int32_t LCPOne::leftBoundK(const std::vector<L1Suffix>& trieLeaves,
                           int32_t curLeaf){
    if(curLeaf + 1 >= (int32_t)trieLeaves.size()) // reached the end
        curLeaf = (int32_t) trieLeaves.size() - 2;
    if(curLeaf <= 0)
        return 0;
    if(trieLeaves[curLeaf].m_errSAPos <= 0 ||
       trieLeaves[curLeaf + 1].m_errSAPos <= 0)
        return curLeaf;
    int32_t lpos,
        idxLCP = rangeMinLCP(trieLeaves[curLeaf].m_errSAPos + 1,
                             trieLeaves[curLeaf + 1].m_errSAPos),
        srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        lpos = curLeaf;
        curLeaf -= 1;
        if(curLeaf < 0 || trieLeaves[curLeaf].m_errSAPos <= 0)
            break;
        idxLCP = rangeMinLCP(trieLeaves[curLeaf].m_errSAPos + 1,
                             trieLeaves[curLeaf + 1].m_errSAPos);
    }
    return lpos;
}

//
//  Assuming that the trieLeaves are sorted by m_errSAPos
//  In order to maintain consistency,
//    - we start with curLeaf and its next as the tree 'x'
//    - we grow this tree 'x' until we reach its right end
//
int32_t LCPOne::rightBoundK(const std::vector<L1Suffix>& trieLeaves,
                            int32_t curLeaf){
    if(curLeaf < 0)
        curLeaf = 1;
    if(curLeaf + 1 >= (int32_t)trieLeaves.size()) // reached the end
        return (int32_t)trieLeaves.size() - 1;
    // compute rpos (TODO:: verify)
    curLeaf += 1;
    if(trieLeaves[curLeaf - 1].m_errSAPos <= 0 ||
       trieLeaves[curLeaf].m_errSAPos <= 0)
        return curLeaf;
    int32_t rpos,
        idxLCP = rangeMinLCP(trieLeaves[curLeaf - 1].m_errSAPos + 1,
                             trieLeaves[curLeaf].m_errSAPos),
        srcLCP = idxLCP;
    while(idxLCP >= srcLCP){
        rpos = curLeaf;
        curLeaf += 1;
        if(curLeaf >= (int32_t)trieLeaves.size() ||
           trieLeaves[curLeaf].m_errSAPos <= 0)
            break;
        idxLCP = rangeMinLCP(trieLeaves[curLeaf - 1].m_errSAPos + 1,
                             trieLeaves[curLeaf].m_errSAPos);
    }
    return rpos;
}

void LCPOne::selectInternalNodesK(const InternalNode& prevNode,
                                  const std::vector<L1Suffix>& candies,
                                  std::vector<InternalNode>& trieNodes){
    // Assume candidates are sorted
    // Each candidate match is a leaf in the trie
    // for each candidate match (TODO:: verify)
    trieNodes.resize(candies.size());
    unsigned j = 0;
    for(int32_t i = 0; i < (int32_t)candies.size(); i++){
        if(candies[i].m_errSAPos <= 0) // skip the one which reach the end
            continue;
        // - make an internal node
        InternalNode iNode;
        iNode.m_leftBound = leftBoundK(candies, i); // get left end
        iNode.m_rightBound = rightBoundK(candies, i); // get right end
        // prevNode.depth + 1 + get range min lcp of left and right ends (?)
        iNode.m_stringDepth =
            rangeMinLCP(candies[iNode.m_leftBound].m_errSAPos + 1,
                        candies[iNode.m_rightBound].m_errSAPos);
        iNode.m_delta = prevNode.m_delta + prevNode.m_stringDepth + 1;
        trieNodes[j] = iNode;
        j++;
    }
    trieNodes.resize(j);
    // remove duplicates of internal nodes
    eliminateDupes(trieNodes);
}

void LCPOne::chopSuffixesK(const InternalNode& iNode,
                           const std::vector<L1Suffix>& inSuffixes,
                           std::vector<L1Suffix>& outSuffixes){
    if(iNode.m_leftBound == -1 || iNode.m_rightBound == -1)
        return;

    assert(iNode.m_rightBound > iNode.m_leftBound);
    outSuffixes.clear();
    outSuffixes.resize(iNode.m_rightBound - iNode.m_leftBound + 1);
    //   collect tuples for each position (going left and right)
    //      (c, c', 0/1) c' = gisa[gsa[c] + d + 1]
    unsigned j = 0;
    for(int32_t idx = iNode.m_leftBound;
            idx <= iNode.m_rightBound; idx++){
        int32_t epx = inSuffixes[idx].m_errSAPos;
        // ignore that has crossed boundary in prev. level
        if(epx < 0 || epx >= (int32_t)m_gsa.size())
            continue;
        int32_t spos = m_gsa[epx];
        int32_t rs = inSuffixes[idx].m_srcStr;
        int32_t epos = spos + iNode.m_stringDepth + 1 ;
        // crossing boundary
        int32_t esa = epos <= m_strLenPfx[rs] ? m_gisa[epos] : -1;

        L1Suffix cm(spos, esa, rs);
        outSuffixes[j] = cm;
        j++;
    }
    outSuffixes.resize(j);
}

void LCPOne::compute(){
    assert(m_gsa.size() > 2);
    assert(m_gsa.size() + 1 == m_glcp.size()); // an assumption of lca

    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++){
        for(unsigned j = 0; j  < 2;j++){
            m_lcpOneXY[i][j].resize(m_strLengths[i], 0);
        }
    }

    // get all the internal nodes
    std::vector<InternalNode> iNodes;
    selectInternalNodes0(iNodes);
    // for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++)
    //     (*nit).dwriteln(m_aCfg.lfs);
    // std::cout << m_rangeMinQuery(4, 6) << std::endl;
    // std::cout << m_rangeMinQuery(5, 8) << std::endl;

    // for each internal node
    for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++){
        (*nit).dwriteln(m_aCfg.lfs);
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        std::vector<L1Suffix> candies;
        chopSuffixes0(*nit, candies);
        // update lcp using sorted tuples using a double pass
        updateLCPOne(*nit, candies);
    }
}

// void LCPOne::computeKHelper(std::vector<InternalNode>& iNodes, int r){
//     if(r == m_k){
//         // for each internal node
//         for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++){
//             (*nit).dwriteln(m_aCfg.lfs);
//             //   collect tuples for each position (going left and right)
//             //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
//             std::vector<L1Suffix> candies;
//             chopSuffixesK(*nit, candies);
//             // update lcp using sorted tuples using a double pass
//             updateLCPOne(*nit, candies);
//         }
//     }
// }

// void LCPOne::computeK(){
//     assert(m_gsa.size() > 2);
//     assert(m_gsa.size() + 1 == m_glcp.size()); // an assumption of lca

//     // resize the LCP arrays
//     for(unsigned i = 0; i < 2; i++){
//         for(unsigned j = 0; j  < 2;j++){
//             m_lcpOneXY[i][j].resize(m_strLengths[i], 0);
//         }
//     }
//     // get all the internal nodes
//     std::vector<InternalNode> iNodes;
//     selectInternalNodes0(iNodes);
// }

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
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            process_pair(i, j, rdb, cfg);
            break;
        }
    }
}
