///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/node_with_data.hpp
  -------------------------

  The class avl_array_node is where the payload value_type is
  aggregated. This class inherits all the tree stuff from
  avl_array_node_tree_fields (see detail/node.hpp). A special
  "copy" constructor is provided for copying from value_type.
  Therefore, construction from scratch and normal copy
  construction need to be defined too (they work just like the
  default constructors would).
*/

#ifndef _AVL_ARRAY_NODE_WITH_DATA_HPP_
#define _AVL_ARRAY_NODE_WITH_DATA_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////

template<class T, class A,
         class W, class P>       // Tree node (with payload T)
class avl_array_node
  : private avl_array_node_tree_fields<T,A,W,P>
{
  friend class mkr::avl_array<T,A,W,P>;

  typedef avl_array_node_tree_fields<T,A,W,P>    node_t;
  typedef avl_array_node<T,A,W,P>                payload_node_t;

  typedef typename
          avl_array<T,A,W,P>::value_type         value_type;
  typedef typename
          avl_array<T,A,W,P>::const_reference    const_reference;

  private:  // Only avl_array<T,A,W,P> has access to this class

    // [ Node links and counters: inherited from base class ]

    value_type m_data;   // Payload

    avl_array_node () {}                      // Default ctor.

    avl_array_node (const payload_node_t & n) // Copy ctor.
      : node_t(n),
        m_data(n.m_data) {}

    avl_array_node (const_reference t)        // "Copy" ctor.
      : m_data(t) {}                          // (from T)
};

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

