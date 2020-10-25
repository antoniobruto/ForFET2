/*********************************************************************
Author: Roberto Bruttomesso <roberto.bruttomesso@gmail.com>

OpenSMT -- Copyright (C) 2008-2010, Roberto Bruttomesso

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

#ifndef THANDLER_H
#define THANDLER_H

#include <stddef.h>
#include <vector>

#include "egraph/Egraph.h"
#include "minisat/core/SolverTypes.h"
#include "smtsolvers/SMTConfig.h"
#include "tsolvers/TSolver.h"

class Enode;
class SMTSolver; // Forward declaration
struct SMTConfig;
template <class T> class vec;

class THandler
{
public:

  THandler ( Egraph &      e
           , SMTConfig &   c
           , SMTSolver &   s
           , vec< Lit > &  t
           , vec< int > &  l
           , vec< char > & a
           , const Var vt
           , const Var vf )
    : core_solver        ( e )
    , config             ( c )
    , solver             ( s )
    , trail              ( t )
    , level              ( l )
    , assigns            ( a )
    , var_True           ( vt )
    , var_False          ( vf )
    , checked_trail_size ( 0 )
    , tatoms             ( 0 )
    , batoms             ( 0 )
    , tatoms_given       ( 0 )
  {
    // Reserve room for true and false
    var_to_enode   .resize( 65536, NULL );
    enode_id_to_var.resize( 65536, var_Undef );
  }

  virtual ~THandler ( ) { }

  void    getConflict          ( vec< Lit > &, int & ); // Returns theory conflict in terms of literals
#ifdef PRODUCE_PROOF
  Enode * getInterpolants      ( );                     // Fill a vector with interpolants
#endif
  Lit     getDeduction         ( );			// Returns a literal that is implied by the current state
  Lit     getSuggestion        ( );			// Returns a literal that is suggested by the current state
  void    getReason            ( Lit, vec< Lit > & );   // Returns the explanation for a deduced literal

  Var     enodeToVar           ( Enode * );             // Converts enode into boolean variable. Create a new variable if needed
  Lit     enodeToLit           ( Enode * );             // Converts enode into boolean literal. Create a new variable if needed
  Lit     enodeToLit           ( Enode *, Var & );      // Converts enode into boolean literal. Create a new variable if needed
  Enode * varToEnode           ( Var );                 // Return the enode corresponding to a variable
  void    clearVar             ( Var );                 // Clear a Var in translation table (used in incremental solving)

  bool    assertLits           ( );                     // Give to the TSolvers the newly added literals on the trail
  bool    check                ( bool );                // Check trail in the theories
  void    backtrack            ( );                     // Remove literals that are not anymore on the trail

  double  getAtomsRatio        ( ) { return (double)batoms/((double)tatoms + 1.0); }

  void    inform               ( );

  lbool   evaluate             ( Enode * e ) { return core_solver.evaluate( e ); }

private:

  // Returns a random float 0 <= x < 1. Seed must never be 0.
  static inline double drand(double& seed)
  {
    seed *= 1389796;
    int q = (int)(seed / 2147483647);
    seed -= (double)q * 2147483647;
    return seed / 2147483647;
  }

  // Returns a random integer 0 <= x < size. Seed must never be 0.
  static inline int irand(double& seed, int size)
  {
    return (int)(drand(seed) * size);
  }

  void verifyCallWithExternalTool        ( bool, size_t );
  void verifyExplanationWithExternalTool ( std::vector< Enode * > & );
  void verifyDeductionWithExternalTool   ( Enode * = NULL );
  bool callCertifyingSolver              ( const char * );
#ifdef PRODUCE_PROOF
  void verifyInterpolantWithExternalTool ( std::vector< Enode * > &, Enode * );
#endif

#ifdef PEDANTIC_DEBUG
  bool  isOnTrail     ( Lit );
#endif

  std::vector< Var >       enode_id_to_var;          // Conversion EnodeID --> Var
  std::vector< Enode * >   var_to_enode;             // Conversion Var --> EnodeID

  Egraph &            core_solver;              // Pointer to Egraph that works as core solver
  SMTConfig &         config;                   // Reference to configuration
  SMTSolver &         solver;                   // Reference to SMT Solver
  vec< Lit > &        trail;                    // Reference to SMT Solver trail
  vec< int > &        level;                    // Reference to SMT Solver level
  vec< char > &       assigns;                  // Reference to SMT Solver assigns
  const Var           var_True;                 // To specify constantly true atoms
  const Var           var_False;                // To specify constantly false atoms
  std::vector< Enode * >   stack;                    // Stacked atoms
  size_t              checked_trail_size;       // Store last size of the trail checked by the solvers

  int                 tatoms;                   // Tracks theory atoms
  int                 batoms;                   // Tracks boolean atoms

  std::vector< bool >      tatoms_seen;              // Atoms seen
  unsigned            tatoms_given;             // Next atom to give
  std::vector< Enode * >   tatoms_list;              // List of tatoms to communicate later
  std::vector< bool >      tatoms_give;              // We might want not to give some atoms
};

#endif
