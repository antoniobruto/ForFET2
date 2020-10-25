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

#ifndef TOP_LEVEL_PROP_H
#define TOP_LEVEL_PROP_H

#include <map>
#include <vector>

#include "common/Global.h"
#include "common/LA.h"
#include "common/Otl.h"
#include "egraph/Egraph.h"

class Egraph;
class Enode;
struct SMTConfig;

class TopLevelProp
{
public:

  TopLevelProp ( Egraph & egraph_, SMTConfig & config_ )
    : egraph  ( egraph_ )
    , config  ( config_ )
  { }

  virtual ~TopLevelProp( ) { }

  Enode * doit ( Enode * ); // Main routine

private:

  Enode * learnEqTransitivity             ( Enode * );

  bool    retrieveSubstitutions           ( Enode *, std::map< enodeid_t, Enode * > & );
  bool    arithmeticElimination           ( std::vector< Enode * > &, std::map< enodeid_t, Enode * > & );
  bool    contains                        ( Enode *, Enode * );
  Enode * substitute                      ( Enode *, std::map< enodeid_t, Enode * > &, bool & );
  Enode * canonize                        ( Enode * );
#if NEW_SPLIT
#else
  Enode * splitEqs                        ( Enode * );
#endif
  Enode * propagateUnconstrainedVariables ( Enode *, bool & );
  Enode * replaceUnconstrainedTerms       ( Enode *, std::vector< int > & , bool & );
  void    computeIncomingEdges            ( Enode *, std::vector< int > & );

  Egraph &    egraph; // Reference to Egraph
  SMTConfig & config; // Reference to Config
};

#endif
