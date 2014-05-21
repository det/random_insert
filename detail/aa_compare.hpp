///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_compare.hpp
  ---------------------

  Container comparison operators:

  == : size is == and all elements are ==  (O(min{M,N})*)
  != : not ==  (O(min{M,N})*)
  <  : 1st != is <, or all are == and size is <  (O(min{M,N})*)
  >  : 1st != is >, or all are == and size is >  (O(min{M,N})*)
  <= : not >  (O(min{M,N})*)
  >= : not <  (O(min{M,N})*)
  
  (*) M is the # of elements of one container and
      N is the # of elements of the other one
*/

#ifndef _AVL_ARRAY_COMPARE_HPP_
#define _AVL_ARRAY_COMPARE_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// operator==: compare the data contained here with the
// data of another avl_array, and return true iff every
// T element is equal to the corresponding element of the
// other container
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
//not inline  MKR: should I make inline at least the 1st part?
  bool
  avl_array<T,A,W,P>::operator==
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  if (size()!=a.size()) return false; // If they have different
                                      // sizes, they can't be
  const_iterator i, j;                // equal

  for (i=begin(), j=a.begin(); i!=end(); ++i, ++j)
    if (!(*i==*j))
      return false;     // Compare and stop when the first
                        // difference is found
  return true;
}

// operator!=: see operator==
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::operator!=
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  return !(*this==a);
}

// operator<: compare the data contained here with the data
// of another avl_array. Compare elements in sequence
// order until one element differs from the corresponding
// element of the other container. If it is lesser, return
// true; if it is greater, return false; if an end is
// reached the shortest sequence is lesser than the other
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
//not inline  MKR: should I make inline at least the 1st part?
  bool
  avl_array<T,A,W,P>::operator<
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  if (!size()) return a.size()!=0;  // Both empty --> equal
                                    // This empty --> lesser
  const_iterator i, j;

  for (i=begin(), j=a.begin(); i!=end(); ++i, ++j)
    if (j==a.end() || *j<*i)
      return false;           // Greater than a
    else if (*i<*j)
      return true;            // Lesser than a

  return j!=a.end();          // Shorter --> lesser
}

// operator>: see operator<
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::operator>
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  return a<*this;
}

// operator<=: see operator<
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::operator<=
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  return !(a<*this);
}

// operator>=: see operator<
//
// Complexity: O(N) (linear time)
// (where N is the number of T objects in the smaller
// avl_array)

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::operator>=
  (const typename avl_array<T,A,W,P>::my_class & a)   const
{
  return !(*this<a);
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

