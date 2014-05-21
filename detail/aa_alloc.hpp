///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_alloc.hpp
  -------------------

  Private helper methods for nodes allocation/deallocation

  new_node(): Allocate and construct a new node (O(1))
  delete_node(): Destruct and deallocate a node (O(1))
*/

#ifndef _AVL_ARRAY_ALLOC_HPP_
#define _AVL_ARRAY_ALLOC_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ------------------- PRIVATE HELPER METHODS --------------------

// new_node(): Allocate and construct a new node. If the parameter
// t is null, use T's default constructor. If it is not NULL (it
// points to a valid T object), use T's copy constructor.
// Note: T's constructors are not called directly, but through the
// node constructors
//
// Complexity: O(1) (regarded that T's constructor is O(1) ;)

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::new_node
  (typename avl_array<T,A,W,P>::const_pointer t)
{
  payload_node_t * p;

  p = allocator.allocate (1); // Just one

  if (p==NULL)                       // If the allocator didn't
    throw allocator_returned_null(); // throw an exception, but
                                     // it returned NULL, throw
  if (t)
    new (p) payload_node_t(*t);      // Call the constructor
  else                               // through the placement
    new (p) payload_node_t;          // new operator

  return static_cast<node_t*>(p);    // Return allocated node
}

// delete_node(): Destruct and deallocate an existing node
//
// Complexity: O(1) (regarded that T's destructor is O(1) ;)

template<class T,class A,class W,class P>
inline void
  avl_array<T,A,W,P>::delete_node
  (typename avl_array<T,A,W,P>::node_t * p)
{
  AA_ASSERT (p);
  payload_node_t * q = static_cast<payload_node_t*>(p);
  q->~payload_node_t ();
  allocator.deallocate (q, 1);
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

