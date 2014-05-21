///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_front_back.hpp
  ------------------------

  Methods for double ended queue interface support:

  front(): get reference to first element (O(1))
  front()_const_: idem but const_reference (O(1))
  push_front(): insert before first element (O(log N))
  pop_front(): remove first element (O(log N))
  back(): get reference to last element (O(1))
  back()_const_: idem but const_reference (O(1))
  push_back(): append after last element (O(log N))
  pop_back(): remove last element (O(log N))
*/

#ifndef _AVL_ARRAY_FRONT_BACK_HPP_
#define _AVL_ARRAY_FRONT_BACK_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// front(): return (by reference) the first element
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::front ()
{
  return *begin();
}

// front()_const_: return (by const reference) the first
// element
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reference
  avl_array<T,A,W,P>::front ()                   const
{
  return *begin();
}

// push_front(): insert an element at the beginning
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::push_front
  (typename avl_array<T,A,W,P>::const_reference t)
{
  insert (begin(), t);
}

// pop_front(): extract the first element (and simply
// delete it)
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::pop_front ()
{
  erase (begin());
}

// back(): return (by reference) the last element
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::back ()
{
  return *--end();
}

// back()_const_: return (by const reference) the last
// element
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reference
  avl_array<T,A,W,P>::back ()                    const
{
  return *--end();
}

// push_back(): append an element at the end
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::push_back
  (typename avl_array<T,A,W,P>::const_reference t)
{
  insert (end(), t);
}

// pop_back(): extract the last element (and simply
// delete it)
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  void avl_array<T,A,W,P>::pop_back ()
{
  erase (--end());
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

