///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/node.hpp
  ---------------

  The class avl_array_node_tree_fields, defined here, contains all
  the links required by tree nodes. It does _not_ contain the
  payload value_type (see detail/node_with_data.hpp).

  Two classes inherit from avl_array_node_tree_fields:

    avl_array_node   (see detail/node_with_data.hpp)
    avl_array        (see avl_array.hpp)
*/

#ifndef _AVL_ARRAY_NODE_HPP_
#define _AVL_ARRAY_NODE_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////
/*
  The type enum_left_right declares L and R.
  Every tree node stores the pointers to its children in an array
  of two pointers. L and R (constant values) are often used as
  index in the array, but in some cases the index is a variable.
  We can take advantage of symmetry: if children[x] is one side,
  children[1-x] is the other one.
*/

typedef enum { L=0, R=1 } enum_left_right;

//////////////////////////////////////////////////////////////////

template<class T, class A,        // Data of a tree node (payload
         class W, class P>        // not included)
class avl_array_node_tree_fields
{                                 // Note that the dummy has no T

  friend class mkr::avl_array<T,A,W,P>;
  friend class rollback_list<T,A,W,P>;

  typedef avl_array_node_tree_fields<T,A,W,P>    node_t;

  protected:

    // Tree links (parent of root and children of leafs are NULL)

    node_t * m_parent;      // parent node
    node_t * m_children[2]; // [0]:left [1]:right

    // Circular doubly linked list (equiv. to in-order travel)

    node_t * m_next;        // (last_node.next==dummy)
    node_t * m_prev;        // (first_node.prev==dummy)

    // Data for balancing, indexing, and stable-sort

    std::size_t m_height;   // levels in subtree, including self
    std::size_t m_count;    // nodes in subtree, including self
    P m_oldpos;             // position (used only in stable_sort)

                      // Alternative sequence view:
    W m_node_width;   // Width of this node
    W m_total_width;  // Width of this subtree

    // Constructor and initializer, both O(1)

    avl_array_node_tree_fields ();     // Default constructor
    void init ();                      // Write default values

    // Helper functions, all O(1) (no loop, no recursion)

    std::size_t left_count () const;   // Nodes in left subtree
    std::size_t right_count () const;  // Nodes in right subtree

    std::size_t left_height () const;  // Height of left subtree
    std::size_t right_height () const; // Height of right subtree

    W left_width () const;             // Width of left subtree
    W right_width () const;            // Width of right subtree
};

//////////////////////////////////////////////////////////////////

// Initializer, or "reset" method: write default values

template<class T,class A,class W,class P>
inline void
  avl_array_node_tree_fields<T,A,W,P>::
  init ()                                // Write default values
{
  m_parent =
  m_children[L] = m_children[R] = NULL; // No relatives

  m_next = m_prev = this;     // Loop list
  m_height = m_count = 1;     // Single element, single level

  m_node_width = m_total_width = W(1);  // Default
}                                       // width

// Constructor: just call init()

template<class T,class A,class W,class P>
inline
  avl_array_node_tree_fields<T,A,W,P>::
  avl_array_node_tree_fields ()
{ init (); }

// Helper functions: return count/height/width of left/right
// subtree. This doesn't require loops or recursion. If the
// left/right subtree is empty, return 0; otherwise, return
// the count/height/width of its root. Time required is O(1)

template<class T,class A,class W,class P>
inline std::size_t
  avl_array_node_tree_fields<T,A,W,P>::
  left_count ()                         const
{
  return m_children[L] ?
         m_children[L]->m_count : 0;
}

template<class T,class A,class W,class P>
inline std::size_t
  avl_array_node_tree_fields<T,A,W,P>::
  right_count ()                        const
{
  return m_children[R] ?
         m_children[R]->m_count : 0;
}

template<class T,class A,class W,class P>
inline std::size_t
  avl_array_node_tree_fields<T,A,W,P>::
  left_height ()                        const
{
  return m_children[L] ?
         m_children[L]->m_height : 0;
}

template<class T,class A,class W,class P>
inline std::size_t
  avl_array_node_tree_fields<T,A,W,P>::
  right_height ()                       const
{
  return m_children[R] ?
         m_children[R]->m_height : 0;
}

template<class T,class A,class W,class P>
inline W
  avl_array_node_tree_fields<T,A,W,P>::
  left_width ()                         const
{
  return m_children[L] ?
         m_children[L]->m_total_width : W(0);
}

template<class T,class A,class W,class P>
inline W
  avl_array_node_tree_fields<T,A,W,P>::
  right_width ()                        const
{
  return m_children[R] ?
         m_children[R]->m_total_width : W(0);
}

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

