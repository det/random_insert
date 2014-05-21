///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_assign.hpp
  --------------------

  Container assignment and swap operations:

  Container assignment (O(M+N): M to delete + N to copy)
  Container swap (O(1))

  Private helper method:

  acquire_tree(): steal the tree of another avl_array (O(1))
*/

#ifndef _AVL_ARRAY_ASSIGN_HPP_
#define _AVL_ARRAY_ASSIGN_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// Assignment operator: replace the current contents of the
// avl_array with a copy of the contents of a. The T
// objects will be copied one by one, in sequence order,
// using the T copy constructor. The resulting tree will be
// perfectly balanced.
//
// Complexity: O(M+N) (linear time)
// (where M is the number of T objects to delete, and N is
// the number of T objects to copy)

template<class T,class A,class W,class P>
inline
  const typename avl_array<T,A,W,P>::my_class &
  avl_array<T,A,W,P>::operator=
  (const typename avl_array<T,A,W,P>::my_class & a)
{
  node_t * first, * last;
  iter_data_provider<const_pointer,
                     const_iterator> dp(a.begin());

  if (&a == this) return *this;
  construct_nodes_list (first, last, a.size(), dp);
  clear ();
  build_known_size_tree (a.size(), first);
  return *this;
}

// swap(): interchange the contents of two avl_array
// containers. This operation only requires changing some
// pointers. T objects are not touched
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::swap
  (typename avl_array<T,A,W,P>::my_class & a)
{
  node_t tmp;

  if (&a == this) return;  // Self-swap is nonsense

  tmp = *dummy ();           // tmp <-- *this
  acquire_tree (*a.dummy()); // *this <-- a
  a.acquire_tree (tmp);      // a <-- tmp
}


// ------------------- PRIVATE HELPER METHODS --------------------

// acquire_tree(): steal the whole tree of another avl_array,
// linking it to *this. Ignore any previous contents of *this.
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::acquire_tree
  (const typename avl_array<T,A,W,P>::node_t & nf)
{
  if (!nf.m_children[L])   // If the tree to acquire is empty,
    init ();            // just initialize
  else
  {
    *dummy () = nf;               // Link dummy to the tree

    node_t::m_children[L]->m_parent =   // Link the tree to dummy
    node_t::m_prev->m_next =
    node_t::m_next->m_prev = dummy ();

    node_t::m_total_width =                         // Copy total
           node_t::m_children[L]->total_width ();   // width into
  }                                                 // dummy node
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

