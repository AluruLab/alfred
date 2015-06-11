#include "compute_klcp.hpp"
#include "construct_sa.hpp"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

void print_lcpk(const unsigned& i, const unsigned& j, const ReadsDB& rdb,
                const ivec_t lcpKXY[2][2], const unsigned& k,
                std::ostream& lfs){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    lfs << " [[" << i << ",\t" << j << ",\t" << sx.size() << ",\t"
        << sy.size() << ",\t" << k << "]," << std::endl << "  [" ;
    for(unsigned i = 0; i < lcpKXY[0][0].size(); i++)
        lfs << "[" << i
            << ", " << lcpKXY[0][0][i]
            << ", " << lcpKXY[0][1][i]
            << ((i == lcpKXY[0][0].size() - 1) ? "]" : "],\t");
    lfs << "]," << std::endl;
    lfs << "  [";
    for(unsigned i = 0; i < lcpKXY[1][0].size(); i++)
        lfs << "[" << i
            << ", " << lcpKXY[1][0][i]
            << ", " << lcpKXY[1][1][i]
            << ((i == lcpKXY[1][0].size() - 1) ? "]" : "],\t");
    lfs << "]]," << std::endl;

}

NaiveLCPk::NaiveLCPk(const std::string& x, const std::string& y,
                     AppConfig& cfg): sx(x), sy(y), m_aCfg(cfg){
    m_kv = m_aCfg.kv;
}

void NaiveLCPk::runLCPk(const std::string& sx, const std::string& sy,
                        int tidx){
    for(unsigned xstart = 0; xstart < sx.size(); xstart++){
        for(unsigned ystart = 0; ystart < sy.size(); ystart++){
            unsigned howfar = 0, kdx = 0;
            while((ystart + howfar < sy.size()) &&
                  (xstart + howfar < sx.size())){
                if(sx[xstart + howfar] != sy[ystart + howfar])
                    kdx += 1;
                if(m_kv < (int)kdx)
                    break;
                howfar++;
            }
            if(m_klcpXY[tidx][1][xstart] < (int32_t)howfar){
                m_klcpXY[tidx][0][xstart] = (int32_t)ystart;
                m_klcpXY[tidx][1][xstart] = (int32_t)howfar;
            }
        }
    }
}

void NaiveLCPk::compute(){
    int32_t strLengths[2];
    strLengths[0] = sx.size(); strLengths[1] = sy.size();

    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++){
        for(unsigned j = 0; j  < 2;j++){
            m_klcpXY[i][j].resize(strLengths[i], 0);
        }
    }
    runLCPk(sx, sy, 0);
    runLCPk(sy, sx, 1);
}

void NaiveLCPk::computeTest(int k){
    m_kv = k;
    compute();
}


void process_pair_naive(unsigned i, unsigned j, ReadsDB& rdb,
                        AppConfig& cfg){
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    cfg.kv = cfg.kv == 0 ? 1 : cfg.kv;
    NaiveLCPk lxy(sx, sy, cfg);
    lxy.compute();
    print_lcpk(i, j, rdb, lxy.getkLCP(), cfg.kv, cfg.lfs);
}

ExactLCPk::ExactLCPk(const std::string& sx, const std::string& sy,
               AppConfig& cfg) : m_aCfg(cfg){
    m_kv = m_aCfg.kv > 0 ? m_aCfg.kv : 1;
    m_strXY = sx + "#" + sy + "$";
    m_strLengths[0] = sx.size(); m_strLengths[1] = sy.size();
    m_strLenPfx[0] = sx.size(); m_strLenPfx[1] = sx.size() + 1 + sy.size();
    m_shiftPos[0] = 0; m_shiftPos[1] = m_strLengths[0] + 1;
    // construct SA, ISA, LCP and RMQ
    construct_sa((const unsigned char*)m_strXY.c_str(), m_strXY.size(), m_gsa);
    construct_isa(m_gsa, m_gisa);
    construct_lcp_kasai(m_strXY.c_str(), m_gsa, m_gisa, m_glcp);
    //construct_lcp_PHI(pxy.c_str(), gsa, glcp);
    m_rangeMinQuery =
        std::move(rmq_support_sparse_table<ivec_t, true, ivec_t>(&m_glcp));
}

void ExactLCPk::print(std::ostream& ofs){
    for(size_t i = 0; i < m_gsa.size();i++)
        ofs << " [" << std::setw(5) << i << ","
            << std::setw(5) << m_gsa[i] << ","
            << std::setw(5) << (i < m_glcp.size() ? m_glcp[i] : -1) << ","
            << "    \"" << (m_strXY.c_str() + m_gsa[i]) << "\"],"
            << std::endl;
}

int32_t ExactLCPk::leftBound0(int32_t curLeaf){
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

int32_t ExactLCPk::rightBound0(int32_t curLeaf){
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

void ExactLCPk::chopSuffixes0(const InternalNode& iNode,
                           std::vector<L1Suffix>& leaves){

    assert(iNode.m_rightBound > iNode.m_leftBound);
    assert(iNode.m_leftBound >= 2);
    assert(iNode.m_rightBound < (int32_t)m_gsa.size());

    leaves.clear();
    leaves.resize(iNode.m_rightBound - iNode.m_leftBound + 1);

    //   collect tuples for each position (going left and right)
    //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
    for(int32_t idx = iNode.m_leftBound, i = 0;
            idx <= iNode.m_rightBound; idx++, i++){
        int32_t spos = m_gsa[idx];
        int32_t rs = spos < m_strLengths[0] ? 0 : 1;
        int32_t epos = spos + iNode.m_stringDepth + 1;
        // crossing boundary
        int32_t esa = epos <= m_strLenPfx[rs] ? m_gisa[epos] : -1;

        L1Suffix cm(spos, esa, rs);
        leaves[i] = cm;
    }
    //   sort tuples by i'
    std::sort(leaves.begin(), leaves.end());
}

void ExactLCPk::eliminateDupes(std::vector<InternalNode>& iNodes){
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

void ExactLCPk::selectInternalNodes0(std::vector<InternalNode>& uNodes){
    uNodes.resize(m_gsa.size() - 2);

    // Get all the internal nodes
    for(int32_t i = 2; i < (int32_t)m_gsa.size(); i++){
        uNodes[i-2].m_leftBound = leftBound0(i);
        uNodes[i-2].m_rightBound = rightBound0(i);
        uNodes[i-2].m_stringDepth = m_glcp[i + 1];
        uNodes[i-2].m_delta = 0;
    }

    // Eliminate duplicates
    eliminateDupes(uNodes);
}

void ExactLCPk::updateExactLCPk(InternalNode& uNode,
                          std::vector<L1Suffix>& leaves){
#ifdef DEBUG
    m_aCfg.lfs << std::endl;
    for(auto cm: leaves){
        cm.dwriteln(m_aCfg.lfs);
    }
    m_aCfg.lfs << std::endl;
#endif

    // left -> right pass
    //updateLtoR(uNode, leaves);
    updatePass<UpperBoundCheck, IncrPointer>(0, 1, (int32_t)leaves.size(),
                                             uNode, leaves
#ifdef DEBUG
                                             , "L->R"
#endif
                                             );
    // right -> left pass
    updatePass<LowerBoundCheck, DecrPointer>((int32_t)leaves.size() - 1,
                                             (int32_t)leaves.size() - 2, -1,
                                             uNode, leaves
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
int32_t ExactLCPk::leftBoundK(const std::vector<L1Suffix>& trieLeaves,
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
int32_t ExactLCPk::rightBoundK(const std::vector<L1Suffix>& trieLeaves,
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

void ExactLCPk::selectInternalNodesK(const InternalNode& prevNode,
                                  const std::vector<L1Suffix>& leaves,
                                  std::vector<InternalNode>& trieNodes){
    // Assume candidates are sorted
    // Each candidate match is a leaf in the trie
    // for each candidate match (TODO:: verify)
    trieNodes.resize(leaves.size());
    unsigned j = 0;
    for(int32_t i = 0; i < (int32_t)leaves.size(); i++){
        if(leaves[i].m_errSAPos <= 0) // skip the one which reach the end
            continue;
        // - make an internal node
        InternalNode iNode;
        iNode.m_leftBound = leftBoundK(leaves, i); // get left end
        iNode.m_rightBound = rightBoundK(leaves, i); // get right end
        // prevNode.depth + 1 + get range min lcp of left and right ends (?)
        iNode.m_stringDepth =
            rangeMinLCP(leaves[iNode.m_leftBound].m_errSAPos + 1,
                        leaves[iNode.m_rightBound].m_errSAPos);
        iNode.m_delta = prevNode.m_delta + prevNode.m_stringDepth + 1;
        trieNodes[j] = iNode;
        j++;
    }
    trieNodes.resize(j);
    // remove duplicates of internal nodes
    eliminateDupes(trieNodes);
}

void ExactLCPk::chopSuffixesK(const InternalNode& iNode,
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

void ExactLCPk::compute0(){
    assert(m_gsa.size() > 2);
    assert(m_gsa.size() + 1 == m_glcp.size()); // an assumption of lca

    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++){
        m_klcpXY[i][0].resize(m_strLengths[i], 0);
        m_klcpXY[i][1].resize(m_strLengths[i], 1);
    }

    // get all the internal nodes
    std::vector<InternalNode> iNodes;
    selectInternalNodes0(iNodes);

    // for each internal node
    for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++){
#ifdef DEBUG
        (*nit).dwriteln(m_aCfg.lfs);
#endif
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        std::vector<L1Suffix> choppedSfxs;
        chopSuffixes0(*nit, choppedSfxs);
        // update lcp using sorted tuples using a double pass
        updateExactLCPk(*nit, choppedSfxs);
    }
}

void ExactLCPk::computeK(InternalNode& uNode, std::vector<L1Suffix>& uLeaves,
                      int searchLevel){
    if(searchLevel == 0){
        // update LCP array
        updateExactLCPk(uNode, uLeaves);
        return;
    }
    std::vector<InternalNode> trieNodes;
    selectInternalNodesK(uNode, uLeaves, trieNodes);
    for(auto nit = trieNodes.begin(); nit != trieNodes.end(); nit++){
        std::vector<L1Suffix> trieLeaves;
        chopSuffixesK(*nit, uLeaves, trieLeaves);
        computeK(*nit, trieLeaves, searchLevel - 1);
    }
}

void ExactLCPk::computeK(){
    assert(m_gsa.size() > 2);
    assert(m_gsa.size() + 1 == m_glcp.size()); // an assumption of lca

    // resize the LCP arrays
    for(unsigned i = 0; i < 2; i++){
        m_klcpXY[i][0].resize(m_strLengths[i], 0);
        m_klcpXY[i][1].resize(m_strLengths[i], m_kv);
        // last end
        for(int j = 1; j < m_kv; j++){
            auto xit = m_strLengths[i] - j;
            if(xit >= 0)
                m_klcpXY[i][1][xit] = j;
        }
    }
    // get all the internal nodes
    std::vector<InternalNode> iNodes;
    selectInternalNodes0(iNodes);
    // for each internal node
    for(auto nit = iNodes.begin(); nit != iNodes.end(); nit++){
#ifdef DEBUG
        (*nit).dwriteln(m_aCfg.lfs);
#endif
        //   collect tuples for each position (going left and right)
        //      (i, i', 0/1) i' = gisa[gsa[i] + d + 1]
        std::vector<L1Suffix> choppedSfxs;
        chopSuffixes0(*nit, choppedSfxs);
        // update lcp using sorted tuples using a double pass
        computeK(*nit, choppedSfxs, m_kv - 1);
    }
}

void ExactLCPk::compute(){
    assert(m_kv >= 0);
    if (m_kv == 0){
        compute0();
    } else {
        computeK();
    }
}

void ExactLCPk::computeTest(int k){
    m_kv= k;
    computeK();
}

void process_pair(unsigned i, unsigned j, ReadsDB& rdb, AppConfig& cfg) {
    const std::string& sx = rdb.getReadById(i);
    const std::string& sy = rdb.getReadById(j);

    ExactLCPk lxy(sx, sy, cfg); // construct suffix array
#ifdef DEBUG
    lxy.print(cfg.lfs);
#endif
    lxy.compute();
    print_lcpk(i, j, rdb, lxy.getkLCP(), 1, cfg.lfs);
}


void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    // TODO
    unsigned nReads = rdb.getReadsCount();
    assert(nReads >= 2);

    for(unsigned i  = 0; i < nReads; i++){
        for(unsigned j = i + 1; j < nReads; j++){
            if(cfg.naive)
                process_pair_naive(i, j, rdb, cfg);
            else
                process_pair(i, j, rdb, cfg);
            //process_pair_naive(i, j, rdb, cfg, 1);
            //process_pair_naive(i, j, rdb, cfg, 2);
            break;
        }
    }
}
