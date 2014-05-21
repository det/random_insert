///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_random_access.hpp
  ---------------------------

  Methods for random access:

  operator[]: get reference of n'th element O(log(N))
  operator(): idem O(log(N))
  at():       idem O(log(N))
  operator[]_const_: idem, but const O(log(N))
  operator()_const_: idem, but const O(log(N))
  at()_const_:       idem, but const O(log(N))

  Private helper methods:

  position_of_node(): get the position of a node (O(log N))
  node_at_pos(): get the node of a position (O(log N))
  jump(): get the node of a relative position (O(log N))
*/

#ifndef _AVL_ARRAY_RANDOM_ACCESS_HPP_
#define _AVL_ARRAY_RANDOM_ACCESS_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// Indexation operator: return the n'th element (by
// reference). Both operator[] and operator() check
// bounds and throw an exception if necessary. And
// at() too, of course. Const versions are provided
// too.
//
// Complexity: O(log(N))

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::operator[]
  (typename avl_array<T,A,W,P>::size_type n)
{
  AA_ASSERT_EXC (n>=0 && n<size(),
                 index_out_of_bounds());  // Index out of range

  return data (node_at_pos(n));
}

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::operator()
  (typename avl_array<T,A,W,P>::size_type n)
{
  return operator[](n);     // Operator() does exactly
}                           // the same as operator[]

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::reference
  avl_array<T,A,W,P>::at
  (typename avl_array<T,A,W,P>::size_type n)
{
  return operator[](n);     // And at() too
}

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reference
  avl_array<T,A,W,P>::operator[]
  (typename avl_array<T,A,W,P>::size_type n)    const
{
  return (*const_cast<my_class*>(this))[n];
}

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reference
  avl_array<T,A,W,P>::operator()
  (typename avl_array<T,A,W,P>::size_type n)    const
{
  return operator[](n);
}

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::const_reference
  avl_array<T,A,W,P>::at
  (typename avl_array<T,A,W,P>::size_type n)    const
{
  return operator[](n);
}


// ------------------- PRIVATE HELPER METHODS --------------------

// position_of_node(): given a node, it calculates its
// index in the array. In the way, it also returns a
// pointer (passed by ref.) to the container where the
// node belongs. The parameter reverse is only used in
// the case of the node being the dummy node. In this
// case, the position is size() for normal iterators,
// and -1 for reverse iterators.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
//not inline
  typename avl_array<T,A,W,P>::size_type
  avl_array<T,A,W,P>::position_of_node
  (const typename avl_array<T,A,W,P>::node_t * p,
   typename avl_array<T,A,W,P>::my_class * & a,
   bool reverse)
{
  size_type pos;
  const node_t * parent;

  AA_ASSERT (p);  // NULL pointer dereference

  if (!p->m_parent)        // Already in the dummy node?
  {
    a = dummy_owner (p);
    return reverse ?
           -1 :            // rend()
           p->m_count-1;   // end()
  }
                             // Otherwise, start with the
  for (pos=p->left_count();  // left conunt of the node and
       p->m_parent;          // climb up to the root counting
       p=parent)             // all nodes that stay in the
  {                          // left of the path
    parent = p->m_parent;
                                        // That is, when we
    if (parent->m_children[R]==p)       // step up-left, the
      pos += parent->left_count () + 1; // parent node and its
  }                                     // left count

  a = dummy_owner (p);
  return pos;
}

// node_at_pos(): finds a node, given its index.
// It is used by the operator []. It travels down from the
// root to the searched node. This takes logarithmic time
// both on average and in the worst case.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
//not inline
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::node_at_pos
  (typename avl_array<T,A,W,P>::size_type pos) const
{
  node_t * p;

  //                                  // Out of bounds: end()
  //if (pos<0 ||                      // currently impossible,
  //    pos>=size())                  // to the fact that op.,
  //    return dummy ();              // [], is the only caller

  if (pos==0)                // The easiest cases are the first
    return node_t::m_next;   // and last element. Covering them
                             // this way, we add an extra little
  if (pos==size()-1)         // overhead to the average case.
    return node_t::m_prev;   // Is the benefit worth it? Well,
                             // the user might index [0] and
                             // [size()-1] very often...

  p = node_t::m_children[L]; // Start with the element at the
                             // root (remember that the dummy
                             // node has no T element)

      // For every subtree, the index (in it) of its root node
      // is equal to the left count of this root node...

  for (;;)
  {
    if (size_type(pos)<            // Travel down updating pos
            p->left_count())       // to make it the index in
      p = p->m_children[L];        // the visited subtree,
    else if (size_type(pos)==      // until it fits the index
                  p->left_count()) // of the subtree's root
      return p;                    // node.
    else                           // A step down-left doesn't
    {                              // touch pos.
      pos -= p->left_count() + 1;  // A step down-right
      p = p->m_children[R];        // decreases pos by the
    }                              // number of nodes we leave
  }                                // at the left side
}

// jump(): given a starting position p and an offset n,
// return a pointer to the node at position p+n. If p+n is
// out of bounds, use the parameter reverse to decide
// whether to return end, rend, or NULL. This will take
// logarithmic time. The complexity will be between
// log(|n|) and log(N), where |n| is the magnitude of the
// jump, and N is the number of nodes in the tree.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
//not inline
  typename avl_array<T,A,W,P>::node_t *
  avl_array<T,A,W,P>::jump
  (typename avl_array<T,A,W,P>::node_t * p,
   typename avl_array<T,A,W,P>::difference_type n,
   bool reverse)
{
  difference_type i;

  AA_ASSERT (p);  // NULL pointer dereference

  if (!n) return p;                               // Stupid
  if (!p->m_parent && p->m_count==1) return NULL; // cases

  if (reverse &&          // Special case: jump _from_ rend
      !p->m_parent)
  {                       // Go to the first element, and
    p = p->m_next;        // jump from there instead
    n --;                 // (adjust n, of course)
  }
                          // The offset n will be adjusted all
  n += p->left_count ();  // the time to make it relative to
                          // the leftmost node of the current
                          // subtree

                                  // While n falls out of the
  while (p->m_parent &&           // current subtree, we need
         (n<0 ||                               // to climb up
          size_type(n)>=p->m_count))
  {
    if (p == p->m_parent->m_children[R])    // If we move up-
      n += p->m_parent->left_count () + 1;  // left, adjust n

    p = p->m_parent;                   // Step up
  }

  if (!p->m_parent)
  {
    if (n<0)                                // rend or
      return reverse && n==-1 ? p : NULL;   // out of range

    if (size_type(n)>=p->m_count-1)
      return !reverse &&
             size_type(n)==                 // end or
                p->m_count-1 ? p : NULL;    // out of range
  }
            // Now n falls inside the current subtree
  for (;;)  // We just have to go down until we find it
  {
    i = difference_type(p->left_count());

    if (n==i)       // If the root of the current subtree
      return p;     // is the position n ... done!

    if (n<i)
      p = p->m_children[L];    // Step down-left
    else
    {
      n -= i+1;                // Adjust n
      p = p->m_children[R];    // Step down-right
    }
  }
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

