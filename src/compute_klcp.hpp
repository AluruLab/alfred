#ifndef COMPUTE_KLCP_H
#define COMPUTE_KLCP_H

#include "defs.hpp"
#include "AppConfig.hpp"
#include "ReadsDB.hpp"
#include <sdsl/suffix_trees.hpp>


// TYPEDEFS ----------------------------------------------------------
typedef sdsl::cst_sct3<>         t_cst;
typedef t_cst::node_type         t_node;
typedef t_cst::string_type       t_string;
typedef t_string::const_iterator t_pa_iter;

void compute_klcp(ReadsDB& rdb, AppConfig& cfg);

#endif /* COMPUTE_KLCP_H */
