//============================================================================
//                                  I B E X                                   
// File        : Smear Function bisectors
// Author      : Bertrand Neveu
// Copyright   : Ecole des Mines de Nantes (France)
// License     : See the LICENSE file
// Created     : July 19, 2012
// Last Update : July 19, 2012
//============================================================================

#ifndef __IBEX_SMEAR_FUNCTION_H__
#define __IBEX_SMEAR_FUNCTION_H__

#include "ibex_Bsc.h"
#include "ibex_Backtrackable.h"
#include "ibex_RoundRobin.h"
#include "ibex_System.h"

namespace ibex {

/**
 * \ingroup bisector
 *
 * \brief bisector with Smear function heuristic (abstract class)
 *
 */
class SmearFunction : public RoundRobin {
public:

	/**
	 * \brief Create a bisector with Smear function heuristic
	 *
	 * Create a bisector with Smear function heuristic, i.e, using the impacts based on the Jacobian matrix.
	 * Abstract  class with 4 implementations SmearMax, SmearSum, SmearMaxRelative, SmearSumRelative corresponding to
	 * different variants for computing the variable to be bisected, all variants using the Jacobian matrix.
	 *
	 *  \param sys               - The system of equations.
	 *  \param prec (optional)   - the minimum width (diameter) an interval must have to be bisected.
	 *  \param ratio (optional)  - the ratio between the diameters of the left and the right parts of the
	 *                             bisected interval. Default value is #Bsc::default_ratio().
	 */
	SmearFunction(System& sys, double prec, double ratio=Bsc::default_ratio());

	/**
	 * \brief Create a bisector with Smear function heuristic.
	 *
	 * Variant with a vector of precisions.
	 *
	 * \see #SmearFunction(System&, double, double)
	 */
	SmearFunction(System& sys, const Vector& prec, double ratio=Bsc::default_ratio());

	/**
	 * \brief Bisect the box.
	 *
	 * \param last_var - the last component that has been bisected.
	 * In case the jacobian matrix could not be computed correctly, the box is split
	 * with the \link ibex::RoundRobin::bisect(int) const round-robin strategy \endlink.
	 */
	virtual  std::pair<IntervalVector,IntervalVector> bisect(const IntervalVector& box, int& last_var);

	/**
	 * \brief Returns the variable to bisect.
	 *
	 * Return the index i of the variable with the greatest maximum impact Abs(Dfj/Dxi) * Diam(xi).
	 *
	 * \param J the jacobian matrix J
	 */
	virtual int var_to_bisect(IntervalMatrix& J, const IntervalVector& box) const=0;

protected :
	int nbvars;
	System& sys;
};

/**
 * \ingroup bisector
 *
 * \brief bisector with Smear function heuristic (maximum impact)
 *
 */
class SmearMax : public SmearFunction {

public :
	/**
	 * \brief Create a bisector using Kearfott's variant of the Smear function heuristic.
	 *
	 * Heuristic defined by Kearfott in 1990, where the variable with
	 * the maximum impact is chosen to be bisected.
     *
     * For the parameters, see #SmearFunction::SmearFunction(System&, double, double).
     *
	 */
	SmearMax (System& sys,  double prec, double ratio=Bsc::default_ratio());

	/*
	 * \brief Create a bisector using the Smear function heuristic
	 *
	 * Variant with a vector of precisions.
	 *
	 * \see #SmearMax(System&, double, double)
	 */
	SmearMax (System& sys,  const Vector& prec, double ratio=Bsc::default_ratio());

	/**
	 * \brief Returns the variable to bisect.
	 *
	 * Returns the index i of the variable with the greatest impact over the constraints fj :
	 *
	 *      max(j) Abs(Dfj/Dxi) * Diam (xi)
	 *
	 * \param J the jacobian matrix J
	 */
	int var_to_bisect(IntervalMatrix& J, const IntervalVector& box) const;
};

/**
 * \ingroup bisector
 *
 * \brief bisector with Smear function heuristic (maximum sum of impacts)
 *
 */
class SmearSum : public SmearFunction {

public :

	/**
	 * \brief Create a bisector using Hansen's variant of the Smear function heuristic.
	 *
     * The sum of the impacts is used instead of the maximum
     *
     * For the parameters, see #SmearFunction::SmearFunction(System&, double, double).
	 */
	SmearSum (System& sys, double prec, double ratio=Bsc::default_ratio());

	 /*
	 * \brief Create a bisector using Hansen's variant of the Smear function heuristic.
	 *
	 * Variant with a vector of precisions.
	 *
	 * \see #SmearSum(System&, double, double)
	 */
	SmearSum (System& sys, const Vector& prec, double ratio=Bsc::default_ratio());

	/**
	 * \brief Returns the variable to bisect.
	 *
	 * Returns the index of the variable i with the greatest sum of impacts over the constraints fj :
	 *
	 *      sum(j) Abs(Dfj/Dxi) * Diam (xi)
	 *
	 * \param J the jacobian matrix J
	 */
	int var_to_bisect(IntervalMatrix& J, const IntervalVector& box ) const;
};


/**
 * \ingroup bisector
 *
 * \brief bisector with Smear function heuristic (maximum sum of normalized impacts)
 *
 */

class SmearSumRelative : public SmearFunction {

public :
	/**
	 * \brief Create a normalized Hansen's variant of the Smear function heuristic.
	 *
	 * We use a normalized sum of the impacts instead the maximum used by SmearMax or the sum used by SmearSum.
	 *
     * For the parameters, see #SmearFunction::SmearFunction(System&, double, double).
	 */
	SmearSumRelative (System& sys,  double prec, double ratio=Bsc::default_ratio());

	 /*
	 * Variant with a vector of precisions.
	 *
	 * \see #SmearSumRelative(System&, double, double)
	 */
	SmearSumRelative (System& sys, const Vector& prec, double ratio=Bsc::default_ratio());

	/**
	 * \brief Returns the variable to bisect.
	 *
	 * Returns index of the variable i with the greatest normalized sum of impacts over the constraints fj :
	 *
	 *     sum(j) Dfj/Dxi * Diam (xi) / NC(fj) , where NC(fj) = sum(i) Abs(Dfj/Dxi) * Diam(xi)
	 *
	 * \param J the jacobian matrix J
	 */
	int var_to_bisect(IntervalMatrix & J, const IntervalVector& box ) const;
};


/**
 * \ingroup bisector
 *
 * \brief bisector with Smear function heuristic (maximum  normalized impact)
 *
 */
class SmearMaxRelative : public SmearFunction {

public :
	/**
	 * Create a bisector with a variant of the Smear function heuristic, where we use a normalized maximimum impact instead of the max used by SmearMax or the sum used by SmearSum
	 *
     * For the parameters, see #SmearFunction::SmearFunction(System&, double, double).
	 */
	SmearMaxRelative (System& sys, double prec, double ratio=Bsc::default_ratio());
	 /*
	 * Variant with a vector of precisions.
	 *
	 * \see #SmearMaxRelative(System&, double, double)
	 */
	SmearMaxRelative (System& sys, const Vector& prec, double ratio=Bsc::default_ratio());

	/**
	 * Returns the variable to bisect : the variable i with the greatest normalized  impact over the constraints fj :  Dfj/Dxi * Diam (xi) / NC(fj) , where NC(fj) = sum(i) Abs(Dfj/Dxi) * Diam(xi)
	 * \param J the jacobian matrix J
	 */
	int var_to_bisect(IntervalMatrix & J, const IntervalVector& box ) const;
};



/*============================================ inline implementation ============================================ */

inline SmearFunction::SmearFunction(System& sys, double prec, double ratio) : RoundRobin(prec, ratio), sys(sys) {
	nbvars=sys.nb_var;
}

inline SmearFunction::SmearFunction(System& sys, const Vector& prec, double ratio) : RoundRobin(prec, ratio), sys(sys) {
	nbvars=sys.nb_var;
}

inline SmearMax::SmearMax(System& sys,  double prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearMax::SmearMax(System& sys,  const Vector& prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearSum::SmearSum(System& sys, double prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearSum::SmearSum(System& sys, const Vector& prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearSumRelative::SmearSumRelative(System& sys,  double prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearSumRelative::SmearSumRelative(System& sys, const Vector& prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearMaxRelative::SmearMaxRelative(System& sys, double prec, double ratio) : SmearFunction(sys,prec,ratio) {

}

inline SmearMaxRelative::SmearMaxRelative(System& sys, const Vector& prec, double ratio) : SmearFunction(sys,prec,ratio) {

}


} // end namespace ibex

#endif // __IBEX_SMEAR_FUNCTION_H__
