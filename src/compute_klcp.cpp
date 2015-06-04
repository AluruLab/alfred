#include "compute_klcp.hpp"

void compute_klcp(ReadsDB& rdb, AppConfig& cfg){
    rdb.buildDBS();

    cfg.lfs << "\"db_str_len\"  : " << rdb.getDBS().length()
                 << "," << std::endl;

    t_cst gst;
    construct_im(gst, rdb.getDBS().c_str() , 1);
}
