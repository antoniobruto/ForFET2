
/////////////////////////////////////////////////////////////////////////////
/// @file IntervalError.h
///
///  IntervalError class and checkInterval funtion definitions
///  
/// @author Tomasz Kapela   @date 11-01-2006
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) Tomasz Kapela 2006
//
// This file constitutes a part of the CAPD library, 
// distributed under the terms of the GNU General Public License.
// Consult  http://capd.wsb-nlu.edu.pl/ for details. 


#ifndef _CAPD_INTERVAL_INTERVALERROR_H_ 
#define _CAPD_INTERVAL_INTERVALERROR_H_ 

#include <stdexcept>
#include <sstream>
#include "capd/intervals/IntervalSettings.h"

namespace capd{
namespace intervals{

//////////////////////////////////////////////////////////////////////////////
//   IntervalError
///
///  Instation of the IntervalError class is throwed by the Interval class on error. 
///
///  IntervalError is derived from std::runtime_error class. <br>
///  It contains information about an interval which caused error.
///
///   @author Tomasz Kapela   @date 11-01-2006
//////////////////////////////////////////////////////////////////////////////

template <typename T_Bound>
class IntervalError : public std::runtime_error
{      
public:         
  IntervalError( const char * A_msg, 
                 T_Bound A_left,
                 T_Bound A_right)
    : std::runtime_error(A_msg), m_left(A_left), m_right(A_right)
  {}

  virtual ~IntervalError() throw()
  {}

  const char * what() const throw()
  {
    std::ostringstream str;
    str << "Interval error: " << std::runtime_error::what() 
        << "\n   left=" << m_left << "  right=" << m_right;
    m_temp_buf = str.str();
    return m_temp_buf.c_str();
  }

protected:
  mutable std::string m_temp_buf;
  T_Bound  m_left, 
           m_right;
};  // IntervalError

//////////////////////////////////////////////////////////////////////////////
//   checkInterval
///  
///  Function for a debugging purposes. 
///  Function checkInterval helps to get rid of many "ifdef's".
///  If __DEBUGGING__ options is set it checks if ends of the interval 
///  are in the right order.
///  Otherwise it does nothing.
///  
///   @author Tomasz Kapela   @date 11-01-2006
//////////////////////////////////////////////////////////////////////////////

#ifdef  __DEBUGGING__
template < typename T_Bound > 
inline void checkInterval( const char * A_msg,  
                           const T_Bound & A_left, 
                           const T_Bound & A_right)
{
    if( A_left > A_right)
      throw IntervalError<T_Bound>(A_msg, A_left, A_right);
}
#else
template < typename T_Bound >
inline void checkInterval( const char *,
                           const T_Bound &,
                           const T_Bound &) {
}
#endif
}} // namespace capd::intervals

#endif // _CAPD_INTERVAL_INTERVALERROR_H_ 
