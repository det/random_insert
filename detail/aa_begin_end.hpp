///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_begin_end.hpp
  -----------------------

  Methods that provide all kinds of iterators referring the
  beginning or the end of the sequence:

  begin(): get normal iterator referring the beginnig (O(1))
  begin()_const_: idem but const_iterator (O(1))
  end(): get normal iterator referring the ending (O(1))
  end()_const_: idem but const_iterator (O(1))
  rbegin(): reverse it. referring rev. beginnig (O(1))
  rbegin()_const_: idem but const_reverse_iterator (O(1))
  rend(): reverse it. referring rev. ending (O(1))
  rend()_const_: idem but const_reverse_iterator (O(1))
*/

#ifndef _AVL_ARRAY_BEGIN_END_HPP_
#define _AVL_ARRAY_BEGIN_END_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// begin(): return an iterator pointing to the beginnig
// of the sequence
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::iterator
  avl_array<T,A,W,P>::begin ()
{
  return iterator(node_t::m_next);
}

// begin()_const_: return an const_iterator (can be moved,
// but can't modify the referenced data) pointing to the
// beginnig of the sequence
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_iterator
  avl_array<T,A,W,P>::begin () const
{
  return const_iterator(node_t::m_next);
}

// end(): return an iterator pointing to the end of the
// sequence (a dummy position after the last element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::iterator
  avl_array<T,A,W,P>::end ()
{
  return iterator(dummy());
}

// end()_const_: return an const_iterator (can be moved,
// but can't modify the referenced data) pointing to the
// end of the sequence (a dummy position after the last
// element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_iterator
  avl_array<T,A,W,P>::end () const
{
  return const_iterator(dummy());
}

// rbegin(): return a reverse iterator pointing to the
// beginnig of the reverse sequence (the last element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reverse_iterator
  avl_array<T,A,W,P>::rbegin ()
{
  return reverse_iterator(node_t::m_prev);
}

// rbegin()_const_: return a const reverse iterator
// pointing to the beginnig of the reverse sequence
// (the last element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reverse_iterator
  avl_array<T,A,W,P>::rbegin () const
{
  return const_reverse_iterator(node_t::m_prev);
}

// rend(): return a reverse iterator pointing to the end of
// the reverse sequence (a dummy position before the first
// element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reverse_iterator
  avl_array<T,A,W,P>::rend ()
{
  return reverse_iterator(dummy());
}

// rend()_const_: return a reverse iterator pointing to the
// end of the reverse sequence (a dummy position before the
// first element)
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reverse_iterator
  avl_array<T,A,W,P>::rend () const
{
  return const_reverse_iterator(dummy());
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

