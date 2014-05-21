///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/empty_number.hpp
  -----------------------

  Class intended to be used as default parameter when features
  like NPSV or stable_sort are not wanted. Compilers should
  optimize away memory and operations of this class.
*/

#ifndef _AVL_ARRAY_EMPTY_NUMBER_HPP_
#define _AVL_ARRAY_EMPTY_NUMBER_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////

class empty_number   // Class supporting some numeric operators
{                    // but containing no number at all
  public:

    empty_number (int n=0) { }
    empty_number (std::size_t n) { }

    empty_number operator+ (const empty_number &) const
    { return empty_number(); }

    empty_number operator- (const empty_number &) const
    { return empty_number(); }

    empty_number operator+= (const empty_number &)
    { return empty_number(); }

    empty_number operator-= (const empty_number &)
    { return empty_number(); }

    empty_number & operator++ () { return *this; }
    empty_number & operator-- () { return *this; }
    empty_number operator++ (int) { return *this; }
    empty_number operator-- (int) { return *this; }
};

inline bool operator== (const empty_number &, const empty_number &)
{ return true; }

inline bool operator!= (const empty_number &, const empty_number &)
{ return false; }

inline bool operator< (const empty_number &, const empty_number &)
{ return false; }

inline bool operator> (const empty_number &, const empty_number &)
{ return false; }

inline bool operator<= (const empty_number &, const empty_number &)
{ return true; }

inline bool operator>= (const empty_number &, const empty_number &)
{ return true; }

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

