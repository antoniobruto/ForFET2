/*********************************************************************
Author: Soonho Kong <soonhok@cs.cmu.edu>
        Sicun Gao <sicung@cs.cmu.edu>

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
#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ibex/ibex.h"
#include "ibex_Interval.h"
#include "minisat/core/SolverTypes.h"
#include "opensmt/egraph/Egraph.h"
#include "opensmt/egraph/Enode.h"

class Enode;
namespace ibex {
class ExprCtr;
class ExprNode;
class ExprSymbol;
}  // namespace ibex

namespace dreal {
ibex::Interval str_to_ibex_interval(std::string const & s);
ibex::ExprNode const * translate_enode_to_exprnode(
    std::map<std::string, ibex::ExprSymbol const *> & var_map, Enode * const e,
    std::unordered_map<Enode *, ibex::Interval> const & subst =
        std::unordered_map<Enode *, ibex::Interval>());
ibex::ExprCtr const * translate_enode_to_exprctr(
    std::map<std::string, ibex::ExprSymbol const *> & var_map, Enode * const e, lbool p = l_Undef,
    std::unordered_map<Enode *, ibex::Interval> const & subst =
        std::unordered_map<Enode *, ibex::Interval>());
std::map<std::string, ibex::ExprSymbol const *> build_var_map(
    std::unordered_set<Enode *> const & vars);
}
