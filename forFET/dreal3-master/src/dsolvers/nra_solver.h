/*********************************************************************
Author: Soonho Kong <soonhok@cs.cmu.edu>
        Sicun Gao <sicung@mit.edu>

dReal -- Copyright (C) 2013 - 2016, the dReal Team

dReal is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dReal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dReal. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#pragma once
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "./dreal_config.h"
#include "constraint/constraint.h"
#include "contractor/contractor.h"
#include "contractor/contractor_status.h"
#include "ibex/ibex.h"
#include "minisat/core/SolverTypes.h"
#include "opensmt/egraph/Egraph.h"
#include "opensmt/tsolvers/TSolver.h"
#include "util/box.h"
#include "util/ibex_variable_hash.h"
#include "util/logging.h"
#include "util/scoped_vec.h"
#include "util/stat.h"

class Egraph;
class Enode;
class SStore;
struct SMTConfig;

namespace dreal {
class box;
class constraint;

class nra_solver : public OrdinaryTSolver {
public:
    nra_solver(const int, const char *, SMTConfig &, Egraph &, SStore &, std::vector<Enode *> &,
               std::vector<Enode *> &, std::vector<Enode *> &);
    ~nra_solver();
    lbool inform(Enode * e);
    bool assertLit(Enode * e, bool = false);
    void pushBacktrackPoint();
    void popBacktrackPoint();
    bool check(bool c);
    bool belongsToT(Enode * e);
    void computeModel();
    std::ostream & dumpFormulas(std::ostream & out) const;
    void dump_smt_file();
    void dump_dr_file(std::string &);

private:
    bool m_need_init = true;
    std::vector<Enode *> m_lits;
    scoped_vec<std::shared_ptr<constraint>> m_stack;
    scoped_vec<contractor_status> m_cses;
    std::map<std::pair<Enode *, bool>, std::shared_ptr<constraint>> m_ctr_map;
    contractor m_ctc;
    contractor_status m_cs;
    mutable stat m_stat;

    void initialize(std::vector<Enode *> const & lits);
    void initialize_constraints(std::vector<Enode *> const & lits);
    std::vector<Enode *> generate_explanation(
        std::unordered_set<std::shared_ptr<constraint>> const & ctr_set);
    void handle_sat_case(box const & b) const;
    void eval_sat_result(box const & b) const;
    void handle_deduction();
};
}  // namespace dreal
