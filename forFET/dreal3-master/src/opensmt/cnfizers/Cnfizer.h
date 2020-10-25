/*********************************************************************
Author: Roberto Bruttomesso <roberto.bruttomesso@gmail.com>

OpenSMT -- Copyright (C) 2009, Roberto Bruttomesso

OpenSMT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenSMT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenSMT. If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/

#ifndef CNFIZER_H
#define CNFIZER_H

#include <map>
#include <set>
#include <vector>

#include "common/Global.h"
#include "common/Otl.h"
#include "egraph/Egraph.h"
#include "minisat/core/SolverTypes.h"
#include "smtsolvers/SMTSolver.h"

class Egraph;
class Enode;
class SMTSolver;
class SStore;
struct SMTConfig;

//
// Generic class for conversion into CNF
//
class Cnfizer
{
public:

  Cnfizer( Egraph &    egraph_
         , SMTSolver & solver_
         , SMTConfig & config_
         , SStore &    sstore_ )
   : egraph  ( egraph_ )
   , solver  ( solver_ )
   , config  ( config_ )
   , sstore  ( sstore_ )
  { }

  virtual ~Cnfizer( ) { }

  lbool cnfizeAndGiveToSolver
    ( Enode *
#ifdef PRODUCE_PROOF
    ,  const uint64_t partitions = 0
#endif
    );                                   // Main routine

protected:

  virtual bool cnfize          ( Enode *, std::map< enodeid_t, Enode * > & ) = 0; // Actual cnfization. To be implemented in derived classes
  bool         deMorganize     ( Enode * );                                  // Apply deMorgan rules whenever feasible
  Enode *      rewriteMaxArity ( Enode *, std::map< enodeid_t, int > & );         // Rewrite terms using maximum arity

  bool  checkCnf                   ( Enode * );                              // Check if formula is in CNF
  bool  checkDeMorgan              ( Enode * );                              // Check if formula can be deMorganized
  bool  giveToSolver               ( Enode * );                              // Gives formula to the SAT solver

  void  retrieveTopLevelFormulae   ( Enode *, std::vector< Enode * > & );         // Retrieves the list of top-level formulae
  void  retrieveClause             ( Enode *, std::vector< Enode * > & );         // Retrieve a clause from a formula
  void  retrieveConjuncts          ( Enode *, std::vector< Enode * > & );         // Retrieve the list of conjuncts

  Enode * toggleLit                ( Enode * );                              // Handy function for toggling literals

  Egraph &    egraph;                                                        // Reference to Egraph
  SMTSolver & solver;                                                        // Reference to Solver
  SMTConfig & config;                                                        // Reference to Config
  SStore &    sstore;
#ifdef PRODUCE_PROOF
  uint64_t    current_partitions;
#endif

private:

  void    computeIncomingEdges ( Enode *, std::map< enodeid_t, int > & );         // Computes the list of incoming edges for a node
  Enode * mergeEnodeArgs       ( Enode *
                               , std::map< enodeid_t, Enode * > &
                               , std::map< enodeid_t, int > & );                  // Subroutine for rewriteMaxArity

  bool    checkConj            ( Enode *, std::set< enodeid_t > & );              // Check if a formula is a conjunction
  bool    checkClause          ( Enode *, std::set< enodeid_t > & );              // Check if a formula is a clause
  bool    checkPureConj        ( Enode *, std::set< enodeid_t > & );              // Check if a formula is purely a conjuntion
};

#endif
