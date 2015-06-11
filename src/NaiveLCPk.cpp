#include "NaiveLCPk.hpp"

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
