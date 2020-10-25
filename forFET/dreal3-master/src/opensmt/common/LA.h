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

#ifndef LA_H
#define LA_H

#include <assert.h>

#include <iosfwd>
#include <map>
#include <utility>
#include <vector>

#include "common/Global.h"
#include "egraph/Egraph.h"
#include "egraph/Enode.h"

class Egraph;

class LAExpression
{
public:

  LAExpression  ( )
    : r         ( UNDEF )
  {
    polynome[ 0 ] = 0;
  }

  LAExpression  ( Enode * e )
    : r         ( e->isEq( ) ? EQ : e->isLeq( ) ? LEQ : UNDEF )
  {
    initialize( e );
  }

  inline bool              isTrue     ( ) { return polynome.size( ) == 1 && ( r == EQ ? polynome[ 0 ] == 0 : polynome[ 0 ] <= 0 ); }
  inline bool              isFalse    ( ) { return polynome.size( ) == 1 && ( r == EQ ? polynome[ 0 ] != 0 : polynome[ 0 ] > 0 ); }

  typedef std::map< Enode *, Real >    polynome_t;

  void                     initialize   ( Enode * );   // Initialize
  Enode *                  solve        ( );           // Solve w.r.t. some variable
  void                     canonize     ( );           // Canonize (different from solve!)
  void                     canonizeReal ( );           // Canonize (different from solve!)
  void                     canonizeInt  ( );           // Canonize (different from solve!)
  Enode *                  toEnode      ( Egraph & );  // Output as enode
  void                     print        ( std::ostream & ); // Output as enode
  std::pair< Enode *, Enode * > getSubst     ( Egraph & );  // Get a valid substitution
  std::pair< Enode *, Enode * > getSubstReal ( Egraph & );  // Get a valid substitution for reals
  std::pair< Enode *, Enode * > getSubstInt  ( Egraph & );  // Get a valid substitution for integers
  //
  // Export iterator in order to allow external procedures to read the polynomes
  //
  typedef polynome_t::iterator iterator;
  inline iterator begin   ( ) { return polynome.begin( ); }
  inline iterator end     ( ) { return polynome.end( ); }

  bool                     checkIntCoefficients ( );

  //
  // For Reals we assume that the first coeffient is 1 or -1
  // For Integers we don't care
  //
  inline bool isCanonized ( )
  {
    if ( polynome.size( ) == 1 ) return true;
    polynome_t::iterator it = polynome.begin( );
    if ( it->first == 0 ) it ++;
    Enode * var = it->first;
    return integers || polynome[ var ] == 1 || polynome[ var ] == -1;
  }

private:

  typedef enum { UNDEF, EQ, LEQ } la_relation_t;

  polynome_t          polynome;            // Enode --> coefficient (NULL for constant)
  const la_relation_t r;                   // Arithmetic relation
  bool                integers;            // If we need reasoning on the integers
  //
  // Friend methods
  //
  //
  // Print overloading
  //
  inline friend std::ostream & operator<<( std::ostream & os, LAExpression & p ) { p.print( os ); return os; }
  //
  // Substitute p in q, using the first variable in p
  //
  friend void combine( LAExpression & p, LAExpression & q )
  {
    LAExpression::polynome_t & poly_p = p.polynome;
    LAExpression::polynome_t & poly_q = q.polynome;
    assert( p.polynome.find( 0 ) != p.polynome.end( ) );
    assert( q.polynome.find( 0 ) != q.polynome.end( ) );
    //
    // Get substitution variable
    //
    LAExpression::polynome_t::iterator pit = poly_p.begin( );
    if ( pit->first == 0 ) pit ++;
    Enode * var = pit->first;
    assert( pit->second == 1 );
    //
    // Variable not present in q, just return
    //
    LAExpression::polynome_t::iterator qit = poly_q.find( var );
    if ( qit == poly_q.end( ) )
      return;
    Real icoeff = (-1)/(qit->second);
    //
    // Compute p + q*icoeff
    //
    std::vector< LAExpression::polynome_t::iterator > to_remove;
    std::vector< LAExpression::polynome_t::iterator > to_add;
    qit = poly_q.begin( );
    pit = poly_p.begin( );
    bool done = false;
    while ( !done )
    {
      //
      // Case 1, same variable. Sum them
      //
      bool incp = false, incq = false;
      if ( qit->first == pit->first )
      {
        qit->second = qit->second * icoeff + pit->second;
        assert( qit->first != var || qit->second == 0 );
        incp = true;
        incq = true;
      }
      //
      // Case 2, not present in p
      //
      else if ( qit->first < pit->first )
      {
        qit->second *= icoeff;
        incq = true;
      }
      //
      // Case 3, not present in q
      //
      else
      {
        to_add.push_back( pit );
        incp = true;
      }

      if ( qit->first != 0 && qit->second == 0 )
        to_remove.push_back( qit );

      if ( incp ) pit ++;
      if ( incq ) qit ++;
      //
      // Add remaining p if q is done
      //
      if ( qit == poly_q.end( ) )
      {
        for ( ; pit != poly_p.end( ) ; pit ++ )
          to_add.push_back( pit );
        done = true;
      }
      if ( pit == poly_p.end( ) )
      {
        for ( ; qit != poly_q.end( ) ; qit ++ )
          qit->second *= icoeff;

        done = true;
      }
    }
    //
    // Remove
    //
    while ( !to_remove.empty( ) )
    {
      poly_q.erase( to_remove.back( ) );
      to_remove.pop_back( );
    }
    //
    // Add
    //
    while ( !to_add.empty( ) )
    {
      poly_q.insert( *to_add.back( ) );
      to_add.pop_back( );
    }
    q.canonize( );
  }
};

#endif
