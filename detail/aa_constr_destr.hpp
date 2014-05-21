///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_constr_destr.hpp
  --------------------------

  Constructors and destructor:

  Default con.: empty avl_array (O(1))
  Copy con.: copy of other avl_array (O(N))
  Vector con.: avl_array with n elements like t (O(N))
  Vector def. con.: " with n default-constructed elem. (O(N))
  Sequence con.: " with copies of [from,to) (O(N))
  Sequence con.: " with copies of [from,from+n) (O(N))
  Destructor (O(N))

  Private helper method:

  init(): initialize an avl_array with "empty" state (O(1))
*/

#ifndef _AVL_ARRAY_CONSTR_DESTR_HPP_
#define _AVL_ARRAY_CONSTR_DESTR_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ---------------------- PUBLIC INTERFACE -----------------------

// Default constructor: create an empty avl_array
//
// Complexity: O(1)

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array ()
{
  init ();
}

// Copy constructor: create an avl_array copying the
// contents of a. The T objects will be copied one by
// one, in sequence order, using the T copy constructor.
// The resulting tree will be perfectly balanced.
//
// Complexity: O(N)

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array
  (const typename avl_array<T,A,W,P>::my_class & a)
{
  node_t * first, * last;
  iter_data_provider<const_pointer,
                     const_iterator> dp(a.begin());

  construct_nodes_list (first, last, a.size(), dp);
  build_known_size_tree (a.size(), first);
}

// Vector constructor: create an avl_array with n
// elements, all them copies of t. The T objects will be
// created one by one using the T copy constructor. Three
// versions of this constructor are provided to avoid
// conflicts with some template methods provided bellow
//
// Complexity: O(N)

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array
  (typename avl_array<T,A,W,P>::size_type n,
   typename avl_array<T,A,W,P>::const_reference t)
{
  node_t * first, * last;
  copy_data_provider<const_pointer> dp(&t);

  construct_nodes_list (first, last, n, dp);
  build_known_size_tree (n, first);
}

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array
  (int n,
   typename avl_array<T,A,W,P>::const_reference t)
{
  node_t * first, * last;
  copy_data_provider<const_pointer> dp(&t);

  if (n<0) n = 0;
  construct_nodes_list (first, last, n, dp);
  build_known_size_tree (n, first);
}

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array
  (long n,
   typename avl_array<T,A,W,P>::const_reference t)
{
  node_t * first, * last;
  copy_data_provider<const_pointer> dp(&t);

  if (n<0) n = 0;
  construct_nodes_list (first, last, n, dp);
  build_known_size_tree (n, first);
}

// Vector constructor: create an avl_array with n
// elements, all them created with T's default
// constructor
//
// Complexity: O(N)

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::avl_array
  (typename avl_array<T,A,W,P>::size_type n)
{
  node_t * first, * last;
  null_data_provider<const_pointer> dp;

  construct_nodes_list (first, last, n, dp);
  build_known_size_tree (n, first);
}

// Sequence [freom,to) constructor: create an avl_array with an
// undetremined number of elements copied from the sequence
// [from,to). The parameters can be iterators of any type
// (including iterators of other containers).
//
// Complexity: O(N)

template<class T,class A,class W,class P>
template <class IT>
inline
  avl_array<T,A,W,P>::avl_array (IT from, IT to)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires< InputIteratorConcept<IT> >();
#endif

  node_t * first, * last;
  range_data_provider<const_pointer,IT> dp(from,to);
  size_type n;

  n = construct_nodes_list (first, last, 0, dp, false, true);
  build_known_size_tree (n, first);
}

// Sequence [from,from+n) constructor: create an avl_array with
// a known number of elements copied from the sequence that
// starts in [from].
//
// Complexity: O(N)

template<class T,class A,class W,class P>
template <class IT>
inline
  avl_array<T,A,W,P>::avl_array
  (IT from,
   typename avl_array<T,A,W,P>::size_type n)
{
#ifdef BOOST_CLASS_REQUIRE
  function_requires< InputIteratorConcept<IT> >();
#endif

  node_t * first, * last;
  iter_data_provider<const_pointer,IT> dp(from);

  construct_nodes_list (first, last, n, dp);
  build_known_size_tree (n, first);
}

// Destructor: deallocate contents
//
// Complexity: O(N)

template<class T,class A,class W,class P>
inline
  avl_array<T,A,W,P>::~avl_array ()
{
  clear ();  // (See impl. of clear() in erase.hpp)
}


// ------------------- PRIVATE HELPER METHODS --------------------

// init(): initialize an avl_array with default values,
// corresponding to an "empty" state (O(1), regarded that W's
// constructor is O(1) ;)

template<class T,class A,class W,class P>
inline void
  avl_array<T,A,W,P>::init ()
{
  node_t::m_parent =
  node_t::m_children[0] =
  node_t::m_children[1] = NULL;               // Lonely node

  node_t::m_next = node_t::m_prev = dummy (); // List: loop
  node_t::m_count = node_t::m_height = 1;     // Nodes: one (dummy)

  node_t::m_node_width =
  node_t::m_total_width = W(0);            // Zero width

  m_sums_out_of_date = false;      // Sums up to date
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

