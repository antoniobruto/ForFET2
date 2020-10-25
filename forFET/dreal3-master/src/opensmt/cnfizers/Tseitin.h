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

#ifndef TSEITIN_H
#define TSEITIN_H

#include <map>

#include "cnfizers/Cnfizer.h"
#include "common/Global.h"
#include "common/Otl.h"
#include "egraph/Egraph.h"
#include "smtsolvers/SMTSolver.h"

class Egraph;
class Enode;
class SMTSolver;
class SStore;
struct SMTConfig;

class Tseitin : public Cnfizer
{
public:

  Tseitin( Egraph & egraph_, SMTSolver & solver_, SMTConfig & config_, SStore & sstore_ )
    : Cnfizer( egraph_, solver_, config_, sstore_ )
  { }

  ~Tseitin( ) { }

private:

  bool cnfize           ( Enode *, std::map< int, Enode * > & );       // Do the actual cnfization
#ifdef PRODUCE_PROOF
  void cnfizeAnd        ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize conjunctions
  void cnfizeOr         ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize disjunctions
  void cnfizeIff        ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize iffs
  void cnfizeImplies    ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize implies
  void cnfizeXor        ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize xors
  void cnfizeIfthenelse ( Enode *, Enode *, const uint64_t = 0 ); // Cnfize if then elses
#else
  void cnfizeAnd        ( Enode *, Enode * ); // Cnfize conjunctions
  void cnfizeOr         ( Enode *, Enode * ); // Cnfize disjunctions
  void cnfizeIff        ( Enode *, Enode * ); // Cnfize iffs
  void cnfizeXor        ( Enode *, Enode * ); // Cnfize xors
  void cnfizeIfthenelse ( Enode *, Enode * ); // Cnfize if then elses
#endif
};

#endif
