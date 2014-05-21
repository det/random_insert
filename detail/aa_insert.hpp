///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_insert.hpp
  --------------------

  Methods for insertion:

  it insert(t): insert anywhere (O(log N) with no rotations)
  it insert(it,t): insert before (O(log N))
  rit insert(rit,t): insert before* (O(log N))
  insert(it,n,t): vector-insert before (O(min{N, n log N}))
  insert(rit,n,t): vector-insert before**        "
  insert(it,from,to): sequence-insert before     "
  insert(rit,from,to): sequence-insert before**  "
  (*) "after" from a 'straight' point of view
  (**) "after and in reverse order" from a 'straight' POV

  Private helper methods:

  insert_before(): insert a node in a given pos. (O(log N))
  insert_anywhere(): add a node to the tree (O(log N))
*/

#ifndef _AVL_ARRAY_INSERT_HPP_
#define _AVL_ARRAY_INSERT_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// Insert anywhere: add a node to the tree. The position
// doesn't matter, so choose the best place to keep the
// tree in balance. A travel to the root will be required
// to update counts and heights, but no rotation will be
// required. Therefore, this will be slightly faster than
// other insert operations. Furthermore, trees populated
// with insert anywhere will be perfectly balanced, so that
// other operations will be slightly faster too. That said,
// it must be clarified that constructing a new array with
// N elements from scratch (see available constructors)
// makes it perfectly balanced too, and is much faster than
// constructing an empty one, and then calling insert
// anywhere N times.
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::iterator     // Insert anywhere
  avl_array<T,A,W,P>::insert
  (typename avl_array<T,A,W,P>::const_reference t) // Original
{
  node_t * newnode;

  newnode = new_node (&t);
  insert_anywhere (newnode);
  return iterator(newnode);
}

// Insert Before: insert a new T copy constructed element
// before a given position. Return iterator pointing to the
// new element
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::iterator
  avl_array<T,A,W,P>::insert
  (const typename avl_array<T,A,W,P>::iterator & it, // Where
   typename avl_array<T,A,W,P>::const_reference t)   // Original
{
  node_t * newnode;

  newnode = new_node (&t);
  insert_before (newnode, it.ptr);
  return iterator(newnode);
}

// Insert Before _reverse_: insert a new T copy constructed
// element before a given position. Return iterator
// pointing to the new element. Note that, with a reverse
// iterator, 'before' really means 'after' in the original
// sequence order
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reverse_iterator
  avl_array<T,A,W,P>::insert
  (const typename                                   // Where and
     avl_array<T,A,W,P>::reverse_iterator & it,     // how (REV.)
   typename avl_array<T,A,W,P>::const_reference t)  // Original
{
  node_t * newnode;

  newnode = new_node (&t);
  insert_before (newnode, it.ptr->m_next);
  return reverse_iterator(newnode);
}

// Vector Insert: insert n copies of t before it. As with
// previous insert operations, several versions are
// provided (signed/unsigned, normal/reverse)
//
// Complexity: O(min{N, n log N})

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::insert
  (const typename avl_array<T,A,W,P>::iterator it,  // Where
   typename avl_array<T,A,W,P>::size_type n,        // How many
   typename avl_array<T,A,W,P>::const_reference t)  // Original
{
  node_t * p, * next, * first, * last;
  copy_data_provider<const_pointer> dp(&t);

  p = it.ptr;

  AA_ASSERT_HO (owner(p)==this);  // it must point into this arr.

  if (n==0) return;

  construct_nodes_list (first, last, n, dp);

  if (!worth_rebuild(n,size()))   // If there are 'few'
  {                               // elements to insert
    do
    {                                 // Repeat n times:
      next = first;                   // Insert before it
      first = first->m_next;          // (after previously
      insert_before (next, p);        // inserted copies)
    }
    while (first);
  }
  else            // If there are 'many' elements to insert
  {
    last->m_next = p;
    p->m_prev->m_next = first;

    build_known_size_tree (n+size(), node_t::m_next);
  }
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::insert
  (const typename avl_array<T,A,W,P>::iterator & it, // Where
   int n,                                            // How many
   typename avl_array<T,A,W,P>::const_reference t)   // Original
{
  AA_ASSERT (n>=0);  // Can't insert a negative amount

  insert (it, size_type(n>0?n:0), t);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::insert
  (const typename avl_array<T,A,W,P>::iterator & it, // Where
   long n,                                           // How many
   typename avl_array<T,A,W,P>::const_reference t)   // Original
{
  AA_ASSERT (n>=0);  // Can't insert a negative amount

  insert (it, size_type(n>0?n:0), t);
}

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::insert
  (const typename                                   // Where and
     avl_array<T,A,W,P>::reverse_iterator it,       // how (REV.)
   typename avl_array<T,A,W,P>::size_type n,        // How many
   typename avl_array<T,A,W,P>::const_reference t)  // Original
{
  node_t * first, * last, * p;
  copy_data_provider<const_pointer> dp(&t);

  AA_ASSERT_HO (owner(it.ptr)==this); // it must point
                                      // into this arr.
  if (n==0) return;

  construct_nodes_list (first, last, n, dp, true);

  if (!worth_rebuild(n,size()))   // 'few' elements to insert
  {
    do                         // Insert them one by one after
    {                          // it (every node goes before
      p = first;               // the previously inserted one)
      first = first->m_next;
      insert_before (p, it.ptr->m_next);
    }
    while (first);
  }
  else            // If there are 'many' elements to insert
  {
                                     // Insert them only in the
    last->m_next = it.ptr->m_next;   // circular doubly linked
    it.ptr->m_next = first;          // list, and then
                                     // rebuild the tree

    build_known_size_tree (n+size(), node_t::m_next);
  }
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::insert
  (const typename                                   // Where and
     avl_array<T,A,W,P>::reverse_iterator & it,     // how (REV.)
   int n,                                           // How many
   typename avl_array<T,A,W,P>::const_reference t)  // Original
{
  AA_ASSERT (n>=0);  // Can't insert a negative amount

  insert (it, size_type(n>0?n:0), t);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::insert
  (const typename                                   // Where and
     avl_array<T,A,W,P>::reverse_iterator & it,     // how (REV.)
   long n,                                          // How many
   typename avl_array<T,A,W,P>::const_reference t)  // Original
{
  AA_ASSERT (n>=0);  // Can't insert a negative amount

  insert (it, size_type(n>0?n:0), t);
}

// Sequence insert: insert a copy of the given sequence
// [from,to) before a given position it
//
// Complexity: O(min{N, n log N})

template<class T,class A,class W,class P>
template <class IT>
//not inline
  void
  avl_array<T,A,W,P>::insert
  (typename avl_array<T,A,W,P>::iterator it,    // Where
   IT from,
   IT to)                // Originals (*to not included)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires< InputIteratorConcept<IT> >();
#endif

  node_t * first, * last, * p;
  range_data_provider<const_pointer,IT> dp(from,to);
  size_type n;
                                      // it must point
  AA_ASSERT_HO (owner(it.ptr)==this); // into this arr.

  if (from==to)              // Make a list with copies of the
    return;                  // range [from,to) and count them

  n = construct_nodes_list (first, last, 0, dp, false, true);

  if (!worth_rebuild(n,size()))   // 'few' elements to insert
  {
    do
    {
      p = first;                  // Insert them one by one
      first = first->m_next;      // before it (every one
      insert_before (p, it.ptr);  // goes after the previously
    }                             // inserted)
    while (first);
  }
  else            // If there are 'many' elements to insert
  {
                                     // Insert them only in the
    last->m_next = it.ptr;           // circular doubly linked
    it.ptr->m_prev->m_next = first;  // list, and then
                                     // rebuild the tree

    build_known_size_tree (n+size(), node_t::m_next);
  }
}

template<class T,class A,class W,class P>
template <class IT>
//not inline
  void
  avl_array<T,A,W,P>::insert
  (typename avl_array<T,A,W,P>::reverse_iterator it, // Where
   IT from,
   IT to)                     // Originals (*to not included)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires< InputIteratorConcept<IT> >();
#endif

  node_t * first, * last, * p;
  range_data_provider<const_pointer,IT> dp(from,to);
  size_type n;
                                      // it must point
  AA_ASSERT_HO (owner(it.ptr)==this); //  into this arr.

  if (from==to)              // Make a list with copies of the
    return;                  // range [from,to) and count them

  n = construct_nodes_list (first, last, 0, dp, true, true);

  if (!worth_rebuild(n,size()))   // 'few' elements to insert
  {
    do                         // Insert them one by one after
    {                          // it (every node goes before
      p = first;               // the previously inserted one)
      first = first->m_next;
      insert_before (p, it.ptr->m_next);
    }
    while (first);
  }
  else            // If there are 'many' elements to insert
  {
                                     // Insert them only in the
    last->m_next = it.ptr->m_next;   // circular doubly linked
    it.ptr->m_next = first;          // list, and then
                                     // rebuild the tree

    build_known_size_tree (n+size(), node_t::m_next);
  }
}


// ------------------- PRIVATE HELPER METHODS --------------------

// insert_before(): insert a given node in a given position
// of the tree. The node is inserted as a leaf node and
// then, if necessary, the tree is rebalanced. Anyway, a
// travel up to the root is required for count and height
// updates. Therefore, this operation takes log(N) time,
// where N is the number of nodes in the tree. When the
// given position is the dummy node, the new node is
// appended at the end of the sequence.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::insert_before
  (typename avl_array<T,A,W,P>::node_t * newnode,
   typename avl_array<T,A,W,P>::node_t * p)
{
  node_t * parent;       // Future parent of the new node
  int side;              // Side (of the parent) where the
                         // new node will be inserted

  AA_ASSERT (p);         // NULL pointer dereference
  AA_ASSERT (newnode);   // Can't insert NULL

  if (p->m_children[L])  // If p has a left subtree, then the
  {                      // previous node (the rightmost node
    parent = p->m_prev;  // in this left subtree) has no right
    side = R;            // child. Put the new node there, as
  }                      // the right child of the previous
  else                   // node
  {
    parent = p;          // If p has no left child... it can't
    side = L;            // be easier! Put the new node there
  }

  newnode->m_prev = p->m_prev;
  newnode->m_prev->m_next = newnode;  // Insert the new node in
                                      // the circular doubly
  newnode->m_next = p;                // linked list
  p->m_prev = newnode;
                                      // Link the new node and
  parent->m_children[side] = newnode; // its new parent with
  newnode->m_parent = parent;         // each other

  update_counters_and_rebalance (parent); // Travel to the root
}                                         // assuring balance

// insert_anywhere(): add a node to the tree. The position
// doesn't matter, so choose the best place to keep the
// tree in balance. A travel to the root will be required
// to update counts and heights, but no rotation will be
// required. Therefore, insert_anywhere() will be slightly
// faster than insert_before(). Furthermore, trees
// populated with insert_anywhere() will be perfectly
// balanced, so that other operations will be slightly
// faster too. That said, it must be clarified that
// constructing a new array with N elements from scratch
// (see available constructors) makes it perfectly balanced
// too, and is much faster than constructing an empty one,
// and then calling insert_anywhere() N times.
//
// Complexity: O(log N)  (no rotations!)

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::insert_anywhere
  (typename avl_array<T,A,W,P>::node_t * newnode)
{
  node_t * p;   // Future parent of the new node

  AA_ASSERT (newnode);     // Can't insert NULL

  if (!node_t::m_children[L])   // If the tree is empty, use the
    p = dummy ();               // dummy node as parent
  else                          // Otherwise, go down through the
  {                             // tree chosing allways the less
    p = node_t::m_children[L];  // populated path

    while (p->m_children[L] && p->m_children[R])
      p = p->m_children[L]->m_count <=
          p->m_children[R]->m_count   ? p->m_children[L] :
                                        p->m_children[R];
  }
                                   // Insert in the empty side
  if (!p->m_children[L])           // (try left first, just in
  {                                // case of p==dummy)
    newnode->m_prev = p->m_prev;
    newnode->m_next = p;                // Insert the new node
                                        // _before_ p in the
    newnode->m_prev->m_next = newnode;  // circular doubly
    p->m_prev = newnode;                // linked list

    p->m_children[L] = newnode;         // Link the parent
  }
  else
  {
    newnode->m_next = p->m_next;
    newnode->m_prev = p;                // Insert the new node
                                        // _after_ p in the
    newnode->m_next->m_prev = newnode;  // circular doubly
    p->m_next = newnode;                // linked list

    p->m_children[R] = newnode;         // Link the parent
  }

  newnode->m_parent = p;           // Link to the parent

  update_counters (p);     // Travel to the root updating
}                          // counts and heights

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

