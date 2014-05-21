///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_build_list.hpp
  ------------------------

  Private helper method for massive operations (temp. lists)

  construct_nodes_list(): prepare a list of new nodes (O(N))
*/

#ifndef _AVL_ARRAY_BUILD_LIST_HPP_
#define _AVL_ARRAY_BUILD_LIST_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ------------------- PRIVATE HELPER METHODS --------------------

// construct_nodes_list(): Construct n objects (if
// exhaust_dp==true, then copy objects until data_provider
// returns NULL) copied from those returned by a functor
// called data_provider, or default constructed if the
// functor returns NULL. The functor can be any object with
// an overloaded operator () that returns pointers to
// existing T objects, or NULL. Finally, return the number
// of objects constructed.
// If an exception occurs, roll back (destroy the already
// constructed objects) and re-throw the exception
//
// Complexity: O(n)

template<class T,class A,class W,class P>
template<class DP>
//not inline
  typename avl_array<T,A,W,P>::size_type    // # of nodes created
  avl_array<T,A,W,P>::construct_nodes_list

  (typename avl_array<T,A,W,P>::node_t *& first, // First and last
   typename avl_array<T,A,W,P>::node_t *& last,  // of the list

   typename avl_array<T,A,W,P>::size_type n, // # nodes to create

   DP & data_provider,         // Functor whose operator ()
                               // will provide pointers to
                               // the objects to copy

   bool reverse,               // Direction

   bool exhaust_dp)            // Take objects from
{                              // data_provider until it
  size_type count;             // returns NULL
  const_pointer t;
  node_t * newnode;
  rollback_list_t nodes_list(this);

  first = last = NULL;    // Start with an empty list

  if (!n && !exhaust_dp)  // Zero elements... done
    return 0;

  t = data_provider ();   // Get first object to copy

  if (exhaust_dp &&       // If number was not specified,
      !t && !n)           // stop when data_provider
    return 0;             // returns NULL

  newnode = new_node (t);   // t ? copy : default

  nodes_list.push_back (newnode);

  for (count=1; exhaust_dp || count<n; count ++)
  {
    t = data_provider ();    // Get next object to copy

    if (exhaust_dp && !t &&  // If the number was not
        count>=n)            // specified, stop when
      break;                 // data_prov. returns NULL

    newnode = new_node (t);  // t ? copy : default

    if (reverse)
      nodes_list.push_front (newnode);
    else
      nodes_list.push_back (newnode);
  }

  nodes_list.commit (first, last);

  return count;  // Number of objects actually constructed
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

