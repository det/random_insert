///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_sorted_search_tree.hpp
  --------------------------------

  Methods for sorting, searching (once sorted) and other
  related algorithms:

  binary_search(): fast search* (O(log N))
  insert_sorted(): insert keeping order* (O(log N))
  sort(): impose order                          (O(N log N))
  stable_sort(): idem + keep current order between equals "
  merge(): mix two containers, keeping order* (O(M+N))
  unique(): remove duplicates* (O(N))
  (*) Elements must be previously in order

  Private helper method:

  binary_search(): search value in a sorted tree (O(log N))
*/

#ifndef _AVL_ARRAY_SORTED_SEARCH_TREE_HPP_
#define _AVL_ARRAY_SORTED_SEARCH_TREE_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// Binary search: search a given value t (or the position
// where it should be) in a sorted avl_array using a
// 'lesser than' comparison functor (an object that has an
// overloaded operator() which recieves two T& and returns
// a bool indicating whether the first T is lesser than the
// second or not). The bool returned indicates whether the
// element has been found or not. The position of the node
// is stored in an iterator passed by reference (if the
// exact value is not found, the position stored in the
// iterator refers to the element positioned where the
// searched value should be). Several versions of this
// method are provided (const/non-const iterators,
// normal/reverse iterators, explicit/default< comparison
// functor)
//
// IMPORTANT: The whole avl_array must be in order
// according to the comparison used. Otherwise, the result
// will have no sense.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
template<class CMP>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::const_iterator & it,
   CMP cmp)                                          const
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  return binary_search (t, &it.ptr, cmp);
}

template<class T,class A,class W,class P>
template<class CMP>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::iterator & it,
   CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  return binary_search (t, &it.ptr, cmp);
}

template<class T,class A,class W,class P>
template<class CMP>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::const_reverse_iterator & it,
   CMP cmp)                                          const
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * p;
  bool found;

  found = binary_search (t, &p, cmp);
  it.ptr = found ? p : p->m_prev;     // Reverse --> the
                                      // element 'before'
  return found;                       // which t would be
}                                     // is the previous

template<class T,class A,class W,class P>
template<class CMP>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::reverse_iterator & it,
   CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * p;
  bool found;

  found = binary_search (t, &p, cmp);
  it.ptr = found ? p : p->m_prev;     // Reverse --> the
                                      // element 'before'
  return found;                       // which t would be
}                                     // is the previous

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::iterator & it)
{
  return binary_search (t, it, std::less<value_type>());
}

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::reverse_iterator & it)
{
  return binary_search (t, it, std::less<value_type>());
}

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::const_iterator & it)
                                                      const
{
  return binary_search (t, it, std::less<value_type>());
}

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t,
   typename avl_array<T,A,W,P>::const_reverse_iterator & it)
                                                      const
{
  return binary_search (t, it, std::less<value_type>());
}

template<class T,class A,class W,class P>
inline
  bool
  avl_array<T,A,W,P>::binary_search
  (typename avl_array<T,A,W,P>::const_reference t)
                                                      const
{
  node_t * p;
  return binary_search (t, &p, std::less<value_type>());
}

// Sorted insert: insert an element while maintaining the
// _exixsting_ order in the avl_array. Two versions of
// this method are provided. One of them receives a
// 'lesser than' comparison functor (an object that has an
// overloaded operator() which recieves two T& and returns
// a bool indicating whether the first T is lesser than the
// second or not). The other one uses the operator < of the
// class T. The parameter allow_duplicates specifies what
// to do when there is already an element (or more) exactly
// equal to the one being inserted (true: go on with the
// insertion; false: don't insert anything). The iterator
// returned indicates the position of the inserted element
// (or the position of the already existing duplicate, if
// allow_duplicates is false and a duplicate is found)
//
// IMPORTANT: The whole avl_array must be in order
// according to the comparison used. Otherwise, the result
// will have no sense.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
template<class CMP>
inline
  typename avl_array<T,A,W,P>::iterator
  avl_array<T,A,W,P>::insert_sorted
  (typename avl_array<T,A,W,P>::const_reference t,
   bool allow_duplicates,
   CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * pos, * newnode;
  bool found;
                                        // Search for the value
  found = binary_search (t, &pos, cmp); // (or the correct
                                        // insert point)
  if (found && !allow_duplicates)  // If already there and
    return iterator();             // dups. forbidden, abort

  newnode = new_node (&t);         // Create element
  insert_before (newnode, pos);    // and insert

  return iterator(newnode);
}

template<class T,class A,class W,class P>
inline
  typename avl_array<T,A,W,P>::iterator
  avl_array<T,A,W,P>::insert_sorted
  (typename avl_array<T,A,W,P>::const_reference t,
   bool allow_duplicates)
{
  return insert_sorted (t, allow_duplicates,
                        std::less<value_type>());
}

// sort(): arrange the elements of the array so that the
// resulting sequence follows a particular order. Don't
// move the elements in memory; just compare them and
// change the links to achieve the goal. Use a 'lesser
// than' comparison functor (an object that has an
// overloaded operator() which recieves two T& and returns
// a bool indicating whether the first T is lesser than the
// second or not) to compare the elements. An additional
// version of this method uses the T::operator< for
// comparisons. Two additional versions (provided bellow)
// perform a stable sort instead of a normal sort (see
// explanation in their comments)
//
// Complexity: O(N log N)
// (where N is the number of elements in the array)

template<class T,class A,class W,class P>
template<class CMP>
//not inline
  void avl_array<T,A,W,P>::sort (CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * p, * next, * pos;

  if (size()<2)
    return;

  node_t::m_prev->m_next = NULL; // Detach the whole tree and use
  next = node_t::m_next;         // it as an independent list

  init ();                   // Fresh start

  while (next)
  {                          // Take the elements of the list
    p = next;                // one by one and insert them
    next = next->m_next;     // in order

    binary_search (data(p), &pos, cmp);
    p->m_children[L] = p->m_children[R] = NULL;
    insert_before (p, pos);
  }
}

template<class T,class A,class W,class P>
inline
  void avl_array<T,A,W,P>::sort ()    // Same, but with
{                                     // T::operator<
  sort (std::less<value_type>());
}

// stable_sort(): same as sort() (see comments above), but
// performing a _stable_ sort. That is, when duplicates are
// found, place them together in the new array but, unlike
// sort(), respect the order that existed previously among
// them.
//
// Complexity: O(N log N)

template<class T,class A,class W,class P>
template<class CMP>
//not inline
  void avl_array<T,A,W,P>::stable_sort (CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * p, * next, * pos;
  P i;

  AA_ASSERT (P(0)!=P(1));   // No P -> no stable sort
                            // Instantiate the avl_aray
  if (size()<2)             // specifying std::size_t as P
    return;                 // parameter instead of the
                            // default value (empty_number)
  node_t::m_prev->m_next = NULL;
  next = node_t::m_next;

  init ();                  // Same as sort, but mark every
                            // element with its old index and
  for (i=0; next; i++)      // use this index for comparisons
  {                         // between duplicates while
    p = next;               // searching the insertion point
    next = next->m_next;

    binary_search (data(p), &pos, cmp, i, true);
    p->m_oldpos = i;
    p->m_children[L] = p->m_children[R] = NULL;
    insert_before (p, pos);
  }
}

template<class T,class A,class W,class P>
inline
  void avl_array<T,A,W,P>::stable_sort ()  // Same, but with
{                                          // T::operator<
  stable_sort (std::less<value_type>());
}

// merge(): acquire the elements of a donor array
// (already in order), and merge them with the elements
// of this array (already in order too). Use a 'lesser
// than' comparison functor (an object that has an
// overloaded operator() which recieves two T& and returns
// a bool indicating whether the first T is lesser than the
// second or not) to compare the elements. An additional
// version of this method uses the T::operator< for
// comparisons.
//
// IMPORTANT: Both arrays must be in order according to
// the comparison used. Otherwise, the result will have no
// sense.
//
// Complexity: O(M+N)
// (where N is the number of elements in this array,
// and M is the number of elements in the donor array)

template<class T,class A,class W,class P>
template<class CMP>
//not inline
  void avl_array<T,A,W,P>::merge
  (typename avl_array<T,A,W,P>::my_class & donor,
   CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * my_next, * donor_next,
                    * next, * last, * first;
  size_type n;

  if (this==&donor ||
      donor.size()==0)
    return;

  if (size()==0)
  {                                // If *this is empty, just
    acquire_tree (*donor.dummy()); // take the donor's tree,
    donor.init ();                 // leaving the donor empty
    return;
  }

  n = size () + donor.size ();     // Total size

  node_t::m_prev->m_next = NULL;
  my_next = node_t::m_next;        // Detach both trees and
                                   // use them as independent
  donor.m_prev->m_next = NULL;     // lists
  donor_next = donor.m_next;
  donor.init ();                   // Leave the donor empty

  first = last = NULL;             // Start a new list

  while (my_next && donor_next)    // While both lists have
  {                                // elements,
    if (cmp(data(donor_next),      // take the lesser of the
            data(my_next)))        // two first elements,
    {
      next = donor_next;
      donor_next = donor_next->m_next;
    }
    else                           // extract it from its list,
    {
      next = my_next;
      my_next = my_next->m_next;
    }

    if (first)                     // and append it to end of
      last = last->m_next = next;  // the new list (well, the
    else                           // first time it is the
      first = last = next;         // beginnig)
  }
                                       // When one list is
  last->m_next = my_next ? my_next :   // empty, append the
                           donor_next; // rest of the other

  build_known_size_tree (n, first);    // Build the tree with
}                                      // the merged list

template<class T,class A,class W,class P>
inline
  void avl_array<T,A,W,P>::merge
  (typename avl_array<T,A,W,P>::my_class & donor)
{
  merge (donor, std::less<value_type>());    // Same, but with
}                                            // T::operator<

// unique(): erase duplicates in an already sorted array.
// Use a 'lesser than' comparison functor (an object that
// has an overloaded operator() which recieves two T& and
// returns a bool indicating whether the first T is lesser
// than the second or not) to compare the elements. An
// additional version of this method uses the T::operator<
// for comparisons.
//
// IMPORTANT: The whole avl_array must be in order
// according to the comparison used. Otherwise, if the
// duplicates are not contiguous, they will not be
// detected.
//
// Complexity: O(N)

template<class T,class A,class W,class P>
template<class CMP>
//not inline
  void avl_array<T,A,W,P>::unique (CMP cmp)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  node_t * first, * p, * q, * dup;
  size_type n;

  n = size ();

  if (n<2)
    return;

  node_t::m_prev->m_next = NULL;     // Detach the whole tree and use
  first = node_t::m_next;            // it as an independent list
  dup = NULL;

  for (p=first; p && p->m_next; p=p->m_next)
    while (p->m_next &&                       // If two
           !cmp(data(p),data(p->m_next)) &&  // elements
           !cmp(data(p->m_next),data(p)))   // are equal,
    {                                      // remove the second
      q = p->m_next;                      // one and go on with
      p->m_next = q->m_next;             // the list (note that
      q->m_next = dup;                  // more duplicates of
      dup = q;                         // the same value might
      n --;                           // follow)
    }

  build_known_size_tree (n, first);   // Build the tree again

  while (dup)             // The tree is ok now
  {
    p = dup;              // Destroy removed nodes
    dup = dup->m_next;
    delete_node (p);
  }
}

template<class T,class A,class W,class P>
inline
  void avl_array<T,A,W,P>::unique ()
{
  unique (std::less<value_type>());    // Same, but with
}                                      // T::operator<


// ------------------- PRIVATE HELPER METHODS --------------------

// binary_search(): search a given value t (or the position
// where it should be) in a sorted avl_array using a
// 'lesser than' comparison functor (an object that has an
// overloaded operator() which recieves two T& and returns
// a bool indicating whether the first T is lesser than the
// second or not). The bool returned indicates whether the
// element has been found or not. The address of the node
// is stored in *pp (if the exact value is not found, the
// address stored in *pp points to the node positioned
// where the searched value should be). The parameter
// oldpos is used in stable sort operations. When used, it
// serves for diferencing equal values of T in a way that
// leads to placing them together, but in a concrete order
// (the one they had among them before the sort process).
// The binary search takes logarithmic time (time
// proportional to log(N), where N is the number of nodes
// in the tree).
//
// IMPORTANT: The whole avl_array must be in order
// according to the comparison used. Otherwise, the result
// will have no sense.
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
template<class CMP>
//not inline
  bool
  avl_array<T,A,W,P>::binary_search  // Return true iff found
  (typename avl_array<T,A,W,P>::
                 const_reference t,  // What to search
   typename avl_array<T,A,W,P>::
                      node_t ** pp,  // Where it is / should be
   CMP cmp,                         // Functor for '<' comparisons
   P oldpos,                       // Old position (in stable sort)
   bool stable)                   // True for stable sort search
                    const
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires<
      BinaryFunctionConcept<CMP,int,const_reference,
                                    const_reference> >();
#endif

  bool lesser, greater;   // Results of the comparison
  node_t * p;             // Current position in the tree

  if (size()==0)          // If the tree is empty,
  {
    *pp = dummy ();          // Whatever we search/insert, it
    return false;            // would be in the end position
  }

  p = node_t::m_children[L]; // Start with the root

  for (;;) // While we don't find it and we don't fall out...
  {
    lesser = cmp (t, data(p));  // Compare t with what is in
    greater = cmp (data(p), t); // the current position

    AA_ASSERT_EXC (!(lesser && greater),
                   lesser_and_greater());  // Can't be < and >

    if (!lesser && !greater)  // If it is equal,
    {
      if (!stable)              // we found it (unless we are
      {                         // in a stable sort process)
        *pp = p;
        return true;                 // In a stable sort, act
      }                              // as if the values were
                                     // different (greater or
      lesser = oldpos < p->m_oldpos; // lesser, depending on
    }                                // the oldpos values)

    if (lesser)               // If what we search is lesser
    {                         // than the root of the current
      if (!p->m_children[L])  // subtree, we have to go
      {                       // down-left into the left
        *pp = p;              // sub-subtree (unless this takes
        return false;         // us out of the tree, of course)
      }

      p = p->m_children[L];
    }
    else                      // If greater --> down-right
    {
      if (!p->m_children[R])
      {
        *pp = p->m_next;      // Greater: _after_ current node
        return false;         // (_before_ the next one)
      }

      p = p->m_children[R];
    }
  }
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

