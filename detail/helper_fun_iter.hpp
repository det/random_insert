///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/helper_fun_iter.hpp
  --------------------------

  Private helper methods (of avl_array) for iterators.

  Iterators can't access the nodes' private members. They must
  use the helper methods implemented here. They also need
  avl_array as a friendship gateway between different iterators,
  because the 2-in-1 trick (used for declaring const and var
  iterators together) avoids declaring them friends of each other.
  The method data() is used in avl_array too (index operators)

  next(): get the next node of a given node (O(1))
  prev(): get the previous node of a given node (O(1))
  data(): get (by ref) the data of a node (with data!) (O(1))
  iterator_pointer(): get the node refered by an it. (O(1))
  make_const_iterator(): get const it. referring a node (O(1))
  make_const_rev_iter(): get const reverse it... (O(1))
*/

#ifndef _AVL_ARRAY_HELPER_FUN_ITER_HPP_
#define _AVL_ARRAY_HELPER_FUN_ITER_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// next(): Get the next node of a given node
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline //static
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::next
  (typename avl_array<T,A,W,P>::node_t * p)
{
  AA_ASSERT (p);       // NULL pointer dereference
  return p->m_next;
}

// prev(): Get the previous node of a given node
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline //static
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::prev
  (typename avl_array<T,A,W,P>::node_t * p)
{
  AA_ASSERT (p);       // NULL pointer dereference
  return p->m_prev;
}

// data(): Get a reference to the payload data (value_type)
// of a given node
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline //static
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::data
  (typename avl_array<T,A,W,P>::node_t * p)
{
  AA_ASSERT (p);            // NULL pointer dereference

  AA_ASSERT_EXC (p->m_parent,
                 invalid_op_with_end());   // end() has no data

  return (static_cast<payload_node_t*>(p))->m_data;
}

// iterator_pointer(): return the node pointer of an
// iterator. This is required by iterator classes
// for reading the pointer of other iterator classes
//
// Complexity: O(1)

template<class T,class A,class W,class P>
template<class IT>
inline //static
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::iterator_pointer
  (const IT & it)
{
#ifdef BOOST_CLASS_REQUIRE
#ifdef AA_USE_RANDOM_ACCESS_TAG
  function_requires<
      RandomAccessIteratorConcept<IT> >();
#else
  function_requires<
      BidirectionalIteratorConcept<IT> >();
#endif
#endif

  return it.ptr;
}

// make_const_iterator(): call the private constructor
// of the class const_iterator. This is required by class
// iterator for converting iterators to const_iterators
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline //static
  typename avl_array<T,A,W,P>::const_iterator
  avl_array<T,A,W,P>::make_const_iterator
  (typename avl_array<T,A,W,P>::node_t * p)
{
  return const_iterator(p);
}

// make_const_reverse_iterator(): call the private
// constructor of the class const_iterator. This is
// required by class reverse_iterator for converting
// reverse iterators to const_reverse_iterators
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline //static
  typename avl_array<T,A,W,P>::const_reverse_iterator
  avl_array<T,A,W,P>::make_const_rev_iter
  (typename avl_array<T,A,W,P>::node_t * p)
{
  return const_reverse_iterator(p);
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

