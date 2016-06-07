#include "LRHeuristicLCPk.hpp"
#include "util.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

LRHeuristicLCPk::LRHeuristicLCPk(const std::string& x, const std::string& y,
                                 AppConfig& cfg):
    m_fwdELCPk(x, y, cfg), m_revELCPk(str_reverse(y), str_reverse(x), cfg),
    m_aCfg(cfg), m_sx(x), m_sy(y) {
    m_strLengths[0] = m_sx.size(); m_strLengths[1] = m_sy.size();
    m_totalLength = m_sx.size() + 1 + m_sy.size() + 1;
    m_shiftPos[0][0] = 0; m_shiftPos[0][1] = m_strLengths[0] + 1;
    m_shiftPos[1][0] = m_strLengths[0] + 1; m_shiftPos[1][1] = 0;
}

void LRHeuristicLCPk::update(int tidx, int32_t astart,
                             std::vector<int32_t>& fwd_dist,
                             std::vector<int32_t>& rev_dist){
    int32_t nparts = (int32_t) fwd_dist.size();
    assert(nparts > 0);
    assert(fwd_dist.size() == rev_dist.size());
    for(int32_t i = 0; i < nparts; i++){
        int32_t howfar = fwd_dist[i] + rev_dist[nparts - 1 - i];
        int32_t aback = (i == 0) ? astart : (astart - rev_dist[nparts - 1 - i]);
        if(aback > 0 && m_klcpXY[tidx][1][aback] < (int32_t)howfar){
            m_klcpXY[tidx][1][aback] = howfar;
        }
    }
}

void LRHeuristicLCPk::extendCrawl(const std::string& sa, const std::string& sb,
                                  int tidx){
    if(m_kextend <= 0)
        return;
    for(int32_t astart = 0; astart < (int32_t) sa.size(); astart++){
        std::vector<int32_t> fwd_dist(m_kextend + 1, 0),
            rev_dist(m_kextend + 1, 0);
        int32_t bstart = (int32_t)m_klcpXY[tidx][0][astart];
        int32_t howfar = (int32_t)m_klcpXY[tidx][1][astart];
        int kdx = 0;
        while((bstart + howfar < (int32_t)sb.size()) &&
              (astart + howfar < (int32_t)sa.size())){
            if(sa[astart + howfar] != sb[bstart + howfar]){
                fwd_dist[kdx] = howfar;
                kdx++;
            }
            if(m_kextend < kdx) break;
            howfar++;
        }

        howfar = 1; kdx = 0;
        while((bstart >= howfar) && (astart >= howfar)){
            if(sa[astart - howfar] != sb[bstart - howfar]){
                rev_dist[kdx] = howfar - 1;
                kdx++;
            }
            if(m_kextend < kdx) break;
            howfar++;
        }
        update(tidx, astart, fwd_dist, rev_dist);
    }
}


void LRHeuristicLCPk::extendRMQ(const std::string& sa, const std::string& sb,
                                int tidx){
    if(m_kextend <= 0)
        return;
    //std::cout << sa << std::endl;
    //std::cout << sb << std::endl;
    for(int32_t astart = 0; astart < (int32_t)sa.size(); astart++){
        std::vector<int32_t> fwd_dist(m_kextend + 1, 0),
            rev_dist(m_kextend + 1, 0);
        int32_t bstart = m_klcpXY[tidx][0][astart];
        int32_t howfar = m_klcpXY[tidx][1][astart] - 1; // start back a bit
#ifdef DEBUG
        m_aCfg.lfs << " [" << astart << ",\t" << bstart << ",\t"
                   << howfar << ",\t";
#endif
        fwd_dist[0] = howfar + 1;
        for(int kdx = 1; kdx <= m_kextend ; kdx++){
            howfar += 1;
            if((bstart + howfar >= (int32_t)sb.size()) ||
               (astart + howfar >= (int32_t)sa.size()))
                break;
            int32_t hx = fwd_rmq(tidx, astart + howfar, bstart + howfar);
            howfar += hx;
            fwd_dist[kdx] = howfar;
#ifdef DEBUG
            m_aCfg.lfs << howfar << ",\t" << hx << ",\t";
#endif
        }
#ifdef DEBUG
        m_aCfg.lfs << howfar << "]," << std::endl;
#endif
        howfar = 0;
        for(int kdx = 0; kdx <= m_kextend ; kdx++){
            howfar += 1;
            if((bstart < howfar) || (astart < howfar))
                break;
            int32_t hx = rev_rmq(tidx, astart - howfar, bstart - howfar);
            howfar += hx;
            rev_dist[kdx] = howfar - 1;
        }
        update(tidx, astart, fwd_dist, rev_dist);
    }
#ifdef DEBUG
    m_aCfg.lfs << std::endl << std::endl;
#endif

}


void LRHeuristicLCPk::computeBasis(){
    // compute
    m_fwdELCPk.compute0();
    // sawp
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            std::swap(m_klcpXY[i][j], m_fwdELCPk.m_klcpXY[i][j]);
}

void LRHeuristicLCPk::extendCrawl(){
    extendCrawl(m_sx, m_sy, 0);
    extendCrawl(m_sy, m_sx, 1);
}

void LRHeuristicLCPk::_compute(){
    computeBasis();
    extendCrawl();
}

void LRHeuristicLCPk::compute(){
    m_kextend = m_aCfg.kv > 0 ? m_aCfg.kv : 0;
    _compute();
}

void LRHeuristicLCPk::computeTest(int k){
    m_kextend = k;
    _compute();
}


void LRHeuristicLCPk::print(std::ostream& ofs){
    m_fwdELCPk.print(ofs);
}

int32_t LRHeuristicLCPk::fwd_rmq(int tidx, int32_t t1, int32_t t2){
    return m_fwdELCPk.sufRangeMinLCP(t1 + m_shiftPos[tidx][0],
                                     t2 + m_shiftPos[tidx][1]);
}


int32_t LRHeuristicLCPk::rev_rmq(int tidx, int32_t t1, int32_t t2){
    // TODO:: modify this.
    return m_fwdELCPk.sufRangeMinLCP(rev_pos(t1 + m_shiftPos[tidx][0]),
                                     rev_pos(t2 + m_shiftPos[tidx][1]));
}
