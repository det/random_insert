///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_move.hpp
  ------------------

  Methods for moving or swapping nodes:

  swap (it/rit,it/rit): interchange two elements
                                 (O(1)), O(log N) with NPSV)
  move (it/rit,n): offset move (O(log N))
  move (it/rit,it/rit): individual move O(log(M)+log(N))
  move (it/rit,n,it/rit): group move *
  move (it/rit,it/rit,it/rit): range move *
  splice (it/rit,cont): group move (see above)
  splice (it/rit,cont,it/rit): individual move (see above)
  splice (it/rit,cont,it/rit,it/rit): range move (see above)
  reverse(): invert the sequence (O(N))

  Private helper methods:

  swap_nodes(): swap two nodes (O(1), O(log N) with NPSV)
  move_node(): move n places along the sequence (O(log N))
  move_node(): extract and insert in other pos. (O(log N))
  move_nodes(): move n nodes to another pos. *

  (*) Complexity of group moves:
      If the source and destination containers are the same, then
      the complexity is O(min{N, n log N})
      Otherwise, it is O(min{N, n log N} + min{M, n log M}), where
      M and N are the containers' sizes
*/

#ifndef _AVL_ARRAY_MOVE_HPP_
#define _AVL_ARRAY_MOVE_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// swap(): change the links of two given nodes so that
// they interchange their positions in the tree(s), but
// don't move them in memory and don't touch their T
// objects. The nodes can be from the same tree, or from
// a different tree each one
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::swap
  (typename avl_array<T,A,W,P>::iterator it1,
   typename avl_array<T,A,W,P>::iterator it2)
{
  swap_nodes (it1.ptr, it2.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::swap
  (typename avl_array<T,A,W,P>::iterator it1,
   typename avl_array<T,A,W,P>::reverse_iterator it2)
{
  swap_nodes (it1.ptr, it2.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::swap
  (typename avl_array<T,A,W,P>::reverse_iterator it1,
   typename avl_array<T,A,W,P>::iterator it2)
{
  swap_nodes (it1.ptr, it2.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::swap
  (typename avl_array<T,A,W,P>::reverse_iterator it1,
   typename avl_array<T,A,W,P>::reverse_iterator it2)
{
  swap_nodes (it1.ptr, it2.ptr);
}

// Offset move: change the links of a given node p so that
// its position in the sequence of the array moves by n
// places, but don't move it in memory and don't touch its
// T object. Two versions of this method are provided. One
// receives a normal iterator. The other one receives a
// reverse iterator, which inverts the sign of the move.
// IF |n|==1 AND [ NPSV is not used, OR
//                 both NPSV widths are the same, OR
//                 m_sums_out_of_date==true ]
// THEN the oparetion takes just O(1) time
// ELSE it takes O(log n)
//
// Complexity: O(log(N)), or O(1) in special cases

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator it,
   typename avl_array<T,A,W,P>::difference_type n)
{
  move_node (it.ptr, n);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator it,
   typename avl_array<T,A,W,P>::difference_type n)
{
  move_node (it.ptr, -n);     // Reverse ---> -n
}

// Individual move: extract a given node from its position,
// and insert it somewhere else (the destination can be
// a different avl_array)
//
// Complexity: O(log(M)+log(N))
// (where M and N are the numbers of elements in source
// and destination arrays respectively)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src,
   typename avl_array<T,A,W,P>::iterator dst)
{
  move_node (src.ptr, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  move_node (src.ptr, dst.ptr->m_next);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src,
   typename avl_array<T,A,W,P>::iterator dst)
{
  move_node (src.ptr, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  move_node (src.ptr, dst.ptr->m_next);
}

// Group move: extract n nodes starting with src_from, and
// insert them again before dst.
//
// Complexity: see note at the beginnig of this file

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::size_type n,
   typename avl_array<T,A,W,P>::iterator dst)
{
  move_nodes (src_from, n, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::size_type n,
   typename avl_array<T,A,W,P>::iterator dst)
{
  move_nodes (src_from, n, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::size_type n,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  move_nodes (src_from, n, dst.ptr->m_next, true);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::size_type n,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  move_nodes (src_from, n, dst.ptr->m_next, true);
}

// Range move: extract nodes [src_from, src_to) and
// insert them again before dst.
//
// Complexity: see note at the beginnig of this file

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::iterator src_to,
   typename avl_array<T,A,W,P>::iterator dst)
{
  difference_type n;
  AA_ASSERT_HO (owner(src_from.ptr)==owner(src_to.ptr));
  n = src_to - src_from;
  if (n>0) move_nodes (src_from, size_type(n), dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::reverse_iterator src_to,
   typename avl_array<T,A,W,P>::iterator dst)
{
  difference_type n;
  AA_ASSERT_HO (owner(src_from.ptr)==owner(src_to.ptr));
  n = src_to - src_from;
  if (n>0) move_nodes (src_from, size_type(n), dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::iterator src_to,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  difference_type n;
  AA_ASSERT_HO (owner(src_from.ptr)==owner(src_to.ptr));
  n = src_to - src_from;
  if (n>0) move_nodes (src_from, size_type(n),
                       dst.ptr->m_next, true);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move
  (typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::reverse_iterator src_to,
   typename avl_array<T,A,W,P>::reverse_iterator dst)
{
  difference_type n;
  AA_ASSERT_HO (owner(src_from.ptr)==owner(src_to.ptr));
  n = src_to - src_from;
  if (n>0) move_nodes (src_from, size_type(n),
                       dst.ptr->m_next, true);
}

// splice (it/rit,cont): move all contents of another avl_array
// to a given position of this avl_array (group move)
//
// Complexity: see note at the beginnig of this file

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::iterator dst,
   typename avl_array<T,A,W,P>::my_class & src)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT (&src!=this);

  move_nodes (src.begin(), src.size(),
              dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::reverse_iterator dst,
   typename avl_array<T,A,W,P>::my_class & src)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT (&src!=this);

  move_nodes (src.begin(), src.size(),
              dst.ptr->m_next, true);    // Reverse
}

// splice (it/rit,cont,it/rit): move an elemnt of another
// avl_array to a given position of this avl_array (individual
// move)
//
// Complexity: O(log(M)+log(N))
// (where M and N are the numbers of elements in source
// and destination arrays respectively)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::iterator src_from)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);

  move_node (src_from.ptr, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::reverse_iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::iterator src_from)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);

  move_node (src_from.ptr, dst.ptr->m_next);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::reverse_iterator src_from)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);

  move_node (src_from.ptr, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::reverse_iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::reverse_iterator src_from)
{
  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);

  move_node (src_from.ptr, dst.ptr->m_next);
}

// splice (it/rit,cont): move a range [from,to) of another
// avl_array to a given position of this avl_array (range move)
//
// Complexity: see note at the beginnig of this file

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::iterator src_to)
{
  difference_type n;

  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);
  AA_ASSERT_HO (owner(src_to.ptr)==&src);

  n = src_to - src_from;

  if (n>0) move_nodes (src_from, n, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::reverse_iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::iterator src_from,
   typename avl_array<T,A,W,P>::iterator src_to)
{
  difference_type n;

  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);
  AA_ASSERT_HO (owner(src_to.ptr)==&src);

  n = src_to - src_from;

  if (n>0) move_nodes (src_from, n,
                       dst.ptr->m_next, true);  // Reverse
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::reverse_iterator src_to)
{
  difference_type n;

  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);
  AA_ASSERT_HO (owner(src_to.ptr)==&src);

  n = src_to - src_from;

  if (n>0) move_nodes (src_from, n, dst.ptr);
}

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::splice
  (typename avl_array<T,A,W,P>::reverse_iterator dst,
   typename avl_array<T,A,W,P>::my_class & src,
   typename avl_array<T,A,W,P>::reverse_iterator src_from,
   typename avl_array<T,A,W,P>::reverse_iterator src_to)
{
  difference_type n;

  AA_ASSERT_HO (owner(dst.ptr)==this);
  AA_ASSERT_HO (owner(src_from.ptr)==&src);
  AA_ASSERT_HO (owner(src_to.ptr)==&src);

  n = src_to - src_from;

  if (n>0) move_nodes (src_from, n,
                       dst.ptr->m_next, true);  // Reverse
}

// reverse(): invert the sequence of the array without
// moving the elements in memory and without touching
// the T objects
//
// Complexity: O(N)

template<class T,class A,class W,class P>
//not inline
  void avl_array<T,A,W,P>::reverse ()
{
  node_t * p, * next, * tmp;

  next = node_t::m_next;

  while (next!=dummy())   // For every node (excepting the
  {                       // dummy node)
    p = next;
    next = p->m_next;

    p->m_next = p->m_prev;    // Swap prev and next links
    p->m_prev = next;         // (circular doubly linked list)

    tmp = p->m_children[L];              // Swap left and right
    p->m_children[L] = p->m_children[R]; // children links
    p->m_children[R] = tmp;              // (tree)
  }

  tmp = node_t::m_next;             // For the dummy node, swap
  node_t::m_next = node_t::m_prev;  // prev (last) and next (first)
  node_t::m_prev = tmp;             // links, but don't touch
}                                   // children links


// ------------------- PRIVATE HELPER METHODS --------------------

// swap_nodes(): change the links of two given nodes so
// that they interchange their positions in the tree(s),
// but don't move them in memory and don't touch their T
// objects. The nodes can be from the same tree, or from
// a different tree each one. This takes constant time.
// The dificulty here is that a lot of corner cases have
// to be taken into account when the nodes are directly
// related (previous-next or parent-child)
// If NPSV is not used, or both nodes have the same NPSV
// width, or the NPSV dirty bit (m_sums_out_of_date) is
// set, then the operation is O(1) (constant time).
// Otherwise, the width sums must be updated, taking
// O(log N) time.
//
// Complexity: O(1) without NPSV or with NPSV but eq. widths
//             O(log N) with NPSV and different widths

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::swap_nodes
  (typename avl_array<T,A,W,P>::node_t * p,
   typename avl_array<T,A,W,P>::node_t * q)
{
  node_t * tmp, tmpnode;

  AA_ASSERT (p);            // NULL pointer dereference
  AA_ASSERT (q);            // NULL pointer dereference

  AA_ASSERT_EXC (p->m_parent &&
                 q->m_parent,
                 invalid_op_with_end());   // Can't move end()

  if (p==q)   // Self swap is nosense
    return;

  // 1st: doubly linked list swap

  if (q->m_next==p)   // If they are contiguous, force
  {                   // them to be in a concrete order
    tmp = p;          // (swap the parameters p and q
    p = q;            // if necessary). This helps
    q = tmp;          // simplifying what comes next
  }
                          // With the previous trick,
  p->m_prev->m_next = q;  // this applies for every case:
  q->m_next->m_prev = p;  // fix outer side links to p and q

  if (p->m_next==q)       // If they are contiguous,
  {
    q->m_prev = p->m_prev;   // Fix side links from p and q
    p->m_next = q->m_next;
    p->m_prev = q;           // Link p and q with each other
    q->m_next = p;
  }
  else                    // Otherwise,
  {
    p->m_next->m_prev = q;   // Inner side links to p and q
    q->m_prev->m_next = p;   // are just like outer links...

    tmp = p->m_next;
    p->m_next = q->m_next;   // Swap next links from p and q
    q->m_next = tmp;

    tmp = p->m_prev;
    p->m_prev = q->m_prev;   // Swap prev links from p and q
    q->m_prev = tmp;
  }

  // 2nd: binary tree swap

  if (q==p->m_parent)  // If they are parent-child of each
  {                    // other, force a concrete position
    tmp = p;           // (swap parameters p an q if
    p = q;             // necessary). This helps simplifying
    q = tmp;           // what comes next
  }
                    // Swap all but data, prev, and next
  tmpnode = *p;
                                         // tmpnode = p->links
  p->m_parent = q->m_parent;
  p->m_children[L] = q->m_children[L];
  p->m_children[R] = q->m_children[R];
  p->m_height = q->m_height;             // p->links = q->links
  p->m_count = q->m_count;               // (excepting prev
  p->m_oldpos = q->m_oldpos;             // and next)

  p->m_node_width = q->m_node_width;

  q->m_parent = tmpnode.m_parent;
  q->m_children[L] = tmpnode.m_children[L];
  q->m_children[R] = tmpnode.m_children[R];
  q->m_height = tmpnode.m_height;         // q->links = tmpnode
  q->m_count = tmpnode.m_count;           // (excepting prev
  q->m_oldpos = tmpnode.m_oldpos;         // and next)

  q->m_node_width = tmpnode.m_node_width;

  if (p==p->m_parent)  // Very special case: parent-child
  {                    // (p its own parent?? That's because
    p->m_parent = q;   // of the previous pointers dance)

    if (q->m_children[L]==q)  // (same mess with q)
      q->m_children[L] = p;   // Choose the correct side
    else                      // for the child pointer
      q->m_children[R] = p;

    if (q->m_parent->m_children[L]==p)  // q->m_parent already
      q->m_parent->m_children[L] = q;   // points to the right
    else                                // place. Now make this
      q->m_parent->m_children[R] = q;   // reciprocal
  }
  else if (p->m_parent==q->m_parent)  // Special case: children
  {                                   // of the same parent
    tmp = p->m_parent->m_children[L];

    p->m_parent->m_children[L] =        // Swap the children
        p->m_parent->m_children[R];     // pointers of the
                                        // parent
    p->m_parent->m_children[R] = tmp;
  }
  else                 // Normal case: not directly related
  {
    if (p->m_parent->m_children[L]==q)  // Make the correct
      p->m_parent->m_children[L] = p;   // child pointers of
    else                                // the parents, point
      p->m_parent->m_children[R] = p;   // to their new
                                        // children
    if (q->m_parent->m_children[L]==p)
      q->m_parent->m_children[L] = q;
    else
      q->m_parent->m_children[R] = q;
  }
                                      // For all cases,
  if (p->m_children[L])               // if p and q have
    p->m_children[L]->m_parent = p;   // children, make them
                                      // point to their
  if (p->m_children[R])               // new parents
    p->m_children[R]->m_parent = p;

  if (q->m_children[L])
    q->m_children[L]->m_parent = q;

  if (q->m_children[R])
    q->m_children[R]->m_parent = q;

  if (p->m_node_width!=q->m_node_width)
  {
    do
    {
      p->m_total_width = p->left_width () +
                         p->right_width () +
                         p->m_node_width;
      p = p->m_parent;
    }                        // If necesary, update
    while (p);               // width sums from p
                             // to root, and from q
    do                       // to root
    {
      q->m_total_width = q->left_width () +
                         q->right_width () +
                         q->m_node_width;
      q = q->m_parent;
    }
    while (q);
  }
}

// move_node(): change the links of a given node p so that
// its position in the sequence of the array moves by n
// places, but don't move it in memory and don't touch
// its T object.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
//not inline
  void
  avl_array<T,A,W,P>::move_node
  (typename avl_array<T,A,W,P>::node_t * p,
   typename avl_array<T,A,W,P>::difference_type n)
{
  int side;
  node_t * q, * r;

  AA_ASSERT (p);            // NULL pointer dereference

  AA_ASSERT_EXC (p->m_parent,
                 invalid_op_with_end());  // Can't move end()

  if (!n ||                      // If n==0 or
      (p->m_count==1 &&          // the node to move is
       !p->m_parent->m_parent))  // alone in the array,
    return;                      // there's nothing to do

  if (n==1)                      // Just one pos. right?
  {
    AA_ASSERT_EXC (p->m_next->m_parent,
                   index_out_of_bounds());  // (don't swap end!)

    swap_nodes (p, p->m_next);   // Swap with the next one
    return;                      // Done
  }

  if (n==-1)                     // Just one pos. left?
  {
    AA_ASSERT_EXC (p->m_prev->m_parent,
                   index_out_of_bounds());  // (don't swap end!)

    swap_nodes (p->m_prev, p);   // Swap with the previous one
    return;                      // Done
  }
                                      // Find the node that is
  r = jump (p, n>0 ? n+1 : n, false); // now at the new pos.

  AA_ASSERT_EXC (r, index_out_of_bounds());

  q = extract_node (p);  // Extract p, (but don't free it)
                         // q: potentially unbalanced branch

  if (r->m_children[L])  // If r has a left subtree, then the
  {                      // previous node (the rightmost node
    r = r->m_prev;       // in this left subtree) has no right
    side = R;            // child. Point r there and insert
  }                      // after the new r
  else
    side = L;            // Otherwise, insert before r

  if (side==L)           // Insert p as r's left child
  {
    p->m_prev = r->m_prev;    // Link p with r's previous
    p->m_prev->m_next = p;    // node and link p with r
                              // in the circular doubly
    p->m_next = r;            // linked list
    r->m_prev = p;

    r->m_children[L] = p;     // Make p the left child of r
  }
  else                   // Insert p as r's right child
  {
    p->m_next = r->m_next;    // Link p with r's next
    p->m_next->m_prev = p;    // node and link p with r
                              // in the circular doubly
    p->m_prev = r;            // linked list
    r->m_next = p;

    r->m_children[R] = p;     // Make p the right child of r
  }

  p->m_parent = r;       // Make r the parent of p

  p->m_children[L] = NULL;
  p->m_children[R] = NULL;   // Reset other links and
  p->m_count = 1;            // counters of p
  p->m_height = 1;
                                     // Two branches might be
  update_counters_and_rebalance (q); // unbalanced now: source
  update_counters_and_rebalance (r); // and destination of p
}

// move_node(): extract a node p from where it is, and
// insert it before another node q, which might belong to
// the same container, or to another one.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
inline
  void
  avl_array<T,A,W,P>::move_node
  (typename avl_array<T,A,W,P>::node_t * p,
   typename avl_array<T,A,W,P>::node_t * q)
{
  AA_ASSERT (p);            // NULL pointer dereference
  AA_ASSERT (q);            // NULL pointer dereference

  if (q==p || q==p->m_next)
    return;

  AA_ASSERT_EXC (p->m_parent,
                 invalid_op_with_end());  // Can't move end node

  update_counters_and_rebalance (extract_node (p));
  insert_before (p, q);
}

// move_nodes(): move n nodes, starting with src_from, to
// the position dst. The parameter reverse indicates the
// direction of the destination iterator (true: insert in
// inverse order and _after_ dst)
//
// Complexity: O(min{N, n log N})

template<class T,class A,class W,class P>
template<class IT>
//not inline
  void
  avl_array<T,A,W,P>::move_nodes
  (IT src_from,                                // Source
   typename avl_array<T,A,W,P>::size_type n,   // # nodes to move
   typename avl_array<T,A,W,P>::node_t * dst,  // Destination
   bool reverse)                               // Dest. direction
{
#ifdef BOOST_CLASS_REQUIRE
#ifdef AA_USE_RANDOM_ACCESS_TAG
  function_requires<
      Mutable_RandomAccessIteratorConcept<IT> >();
#else
  function_requires<
      Mutable_BidirectionalIteratorConcept<IT> >();
#endif
#endif

  my_class * s, * d;
  node_t * next, * first, * last;
  bool rebuild_tree, dest_extracted;

  AA_ASSERT (src_from.ptr);
  AA_ASSERT (dst);

  s = owner (src_from.ptr);
  d = owner (dst);

  rebuild_tree = false;

  dest_extracted =         // Detect source-destination overlap
      s->extract_nodes
        (src_from, n,      // Source
         first, last,      // Pointers passed by reference!
         dst,              // Destination (just for checking)
         s==d ?            // If src and dest are in the same tree,
           &rebuild_tree : // rebuild only once, after reinsertion
           NULL,
         reverse);         // If the destination was specified with
                           // a reverse iter., make a reverse list

  if (dest_extracted)      // If dest belongs to the extracted
    dst = src_from.ptr;    // range, fix it (the nodes will be
                           // reinserted in the same place, maybe
                           // in revers order...)

  if (reverse)             // If dest is reverse, "before" means
    dst = dst->m_next;     // "after" from a 'straight' POV

  if (!rebuild_tree &&                // No delayed rebuild _and_
      !worth_rebuild(n,d->size()))    // 'few' elements
  {
    while (first)                     // Repeat n times:
    {
      next = first;                   // Insert before dst
      first = first->m_next;          // (after previously
      insert_before (next, dst);      // inserted copies)
    }
  }
  else                                // 'many' elements
  {
    last->m_next = dst;               // Splice list and
    dst->m_prev->m_next = first;      // rebuild tree

    d->build_known_size_tree (n+d->size(), d->m_next);
  }
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

