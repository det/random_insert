///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/iterator.hpp
  -------------------

  'Straight' avl_array iterators (both mutable and const
  implemented with a single template).

  See the accompanying documentation for more details.
*/

#ifndef _AVL_ARRAY_ITERATOR_HPP_
#define _AVL_ARRAY_ITERATOR_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////

template<class T, class A,
         class W, class P,             // 2-in-1 trick: Ref and
         class Ref, class Ptr>         // Ptr are re-defined for
class avl_array_iterator               // const_iterator
{
  friend class mkr::avl_array<T,A,W,P>;

  typedef avl_array_node_tree_fields<T,A,W,P>    node_t;
  typedef avl_array_iterator<T,A,W,P,Ref,Ptr>    my_class;
  typedef avl_array_rev_iter<T,A,W,P,Ref,Ptr>    my_reverse;
  typedef mkr::avl_array<T,A,W,P>                my_array;

  public: // -------------- PUBLIC INTERFACE ----------------

#ifdef AA_USE_RANDOM_ACCESS_TAG
    typedef std::random_access_iterator_tag      iterator_category;
#else
    typedef std::bidirectional_iterator_tag      iterator_category;
#endif

    typedef typename
            avl_array<T,A,W,P>::value_type       value_type;
    typedef Ref                                  reference;
    typedef Ptr                                  pointer;
    typedef typename my_array::size_type         size_type;
    typedef typename my_array::difference_type   difference_type;
    typedef typename my_array::const_iterator    const_iterator;

    // Constructors: all O(1)

    avl_array_iterator ();                    // Singular iterator
    avl_array_iterator (const my_class & it); // Copy constructor
    explicit
    avl_array_iterator (const my_reverse & it); // From reverse

    operator const_iterator ();     // Conversion to const

    // Dereference: O(1)

    reference operator* () const;
    pointer operator->() const;

    // Indexing: O(log N)

    reference operator[] (difference_type n) const;
    reference operator() (difference_type n) const;

    // Iterating through the list: O(1) just like std::list

    my_class & operator++ ();   // (pre++)
    my_class & operator-- ();   // (pre--)
    my_class operator++ (int);  // (post++)
    my_class operator-- (int);  // (post--)

    // Iterating through the tree: O(log N)

    my_class operator+ (difference_type n) const;
    my_class operator- (difference_type n) const;
    my_class & operator+= (difference_type n);
    my_class & operator-= (difference_type n);

    // Iterators difference: O(log N)

    template<class X,class Y> difference_type operator-
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    // Equality comparisons: O(1)

    template<class X,class Y> bool operator==
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    template<class X,class Y> bool operator!=
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    // Lesser/greater comparisons: O(log N)

    template<class X,class Y> bool operator<
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    template<class X,class Y> bool operator>
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    template<class X,class Y> bool operator<=
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

    template<class X,class Y> bool operator>=
      (const avl_array_iterator<T,A,W,P,X,Y> & it) const;

  private: // ----- PRIVATE DATA MEMBER AND HELPER FUN. ------

    node_t * ptr;  // Poiter to referenced node

    avl_array_iterator (node_t * node)  // Only avl_array can
      : ptr(node) {}                    // access this constructor

    template<class IT>                          // Access the ptr
    static node_t * it_ptr (IT & it)            // member of other
    {                                           // iterators
      return my_array::iterator_pointer (it);   // through the
    }                                           // container class

};

//////////////////////////////////////////////////////////////////

// Default constructor: create a singular iterator

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>::
  avl_array_iterator () : ptr(NULL) {}

// Copy constructor: just copy the embedded pointer

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>::
  avl_array_iterator (const my_class & it) { ptr = it.ptr; }

// Conversion from reverse iterator: copy the pointer (yes, the
// same pointer; reverse iterators point to the refered element,
// not to its neighbor). The helper method it_ptr() calls a
// method of the avl_array class, which has access to the pointer

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>::
  avl_array_iterator (const my_reverse & it) { ptr = it_ptr(it); }

// Conversion to const iterator. Again through the avl_array class

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>::
  operator typename avl_array_iterator<T,A,W,P,Ref,Ptr>::
  const_iterator ()
{ return my_array::make_const_iterator(ptr); }

// Dereference. data() asserts that this is neither a singular
// iterator nor an end node.

template<class T,class A,class W,class P,class Ref,class Ptr>
inline typename avl_array_iterator<T,A,W,P,Ref,Ptr>::reference
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator* () const
{ return my_array::data (ptr); }

// The arrow can be used when T is a struct or class

template<class T,class A,class W,class P,class Ref,class Ptr>
inline typename avl_array_iterator<T,A,W,P,Ref,Ptr>::pointer
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator->() const
{ return &**this; }

// Index operator [] indirectly calls avl_array::jump(), which
// takes O(log N) time. NOTE: avl_array::jump() does check the
// range

template<class T,class A,class W,class P,class Ref,class Ptr>
inline typename avl_array_iterator<T,A,W,P,Ref,Ptr>::reference
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator[]
  (typename avl_array_iterator<T,A,W,P,Ref,Ptr>::difference_type n)
                                                              const
{ return *(*this+n); }

// Index operator () does exactly the same as operator []

template<class T,class A,class W,class P,class Ref,class Ptr>
inline typename avl_array_iterator<T,A,W,P,Ref,Ptr>::reference
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator()
  (typename avl_array_iterator<T,A,W,P,Ref,Ptr>::difference_type n)
                                                              const
{ return *(*this+n); }

// Operators ++ and -- iterate through the list. They require a
// single step through the next pointer or the previous pointer.
// They need to call helper methods of avl_array because the
// iterator class is not friend of the node class

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr> &
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator++ ()  // (pre++)
{
  ptr = my_array::next (ptr);    // Step forward
  return *this;
}

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr> &
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator-- ()  // (pre--)
{
  ptr = my_array::prev (ptr);    // Step back
  return *this;
}

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator++ (int) // (post++)
{
  my_class tmp(*this);
  ptr = my_array::next (ptr);    // Step forward
  return tmp;                    // Return unmodified copy
}

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator-- (int) // (post--)
{
  my_class tmp(*this);
  ptr = my_array::prev (ptr);    // Step back
  return tmp;                    // Return unmodified copy
}

// Operators +(it,n), +(n,it), -(it,n), +=(it,n) and -=(it,n)
// iterate through the tree. They call jump() on the avl_array,
// which takes between O(log n) and O(log N) time (n is the size
// of the jump, and N is the size of the avl_array)

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator+
  (difference_type n)                             const
{
  my_class tmp(*this);
  tmp.ptr = my_array::jump (tmp.ptr, n, false);
  AA_ASSERT_EXC (tmp.ptr, index_out_of_bounds());
  return tmp;
}                           // jump() takes logarithmic time

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr> operator+
  (typename avl_array<T,A,W,P>::difference_type n,
   const avl_array_iterator<T,A,W,P,Ref,Ptr> & it)
{ return it + n; }

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr>
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator-
  (difference_type n)                             const
{ return *this + -n; }

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr> &
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator+=
  (difference_type n)
{
  *this = *this + n;
  return *this;
}

template<class T,class A,class W,class P,class Ref,class Ptr>
inline avl_array_iterator<T,A,W,P,Ref,Ptr> &
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator-=
  (difference_type n)
{ return *this += -n; }

// Operator -(it,it') can mix const and var iterators. It takes
// O(log N) time. It checks the consistency of operands regarding
// the container they refer (should be the same for both)

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline
  typename avl_array_iterator<T,A,W,P,Ref,Ptr>::difference_type
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator-
  (const avl_array_iterator<T,A,W,P,X,Y> & it)    const
{
  my_array * a, * b;
  size_type m, n;

  if (!ptr && !it_ptr(it)) return 0;  // Both singular

  m = my_array::position_of_node (ptr, a, false);
  n = my_array::position_of_node (it_ptr(it), b, false);

  AA_ASSERT (a==b); // Inter-array distance has no sense

  return difference_type(m) - difference_type(n);
}

// Equality and inequality operators take O(1) time. They can
// also mix const and var iterators

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator==
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr==it_ptr(it); }

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator!=
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr!=it_ptr(it); }

// Greater and lesser operators take O(log N) time in general.
// They will take just O(1) in the cases where the result can
// be decided with a simple equality/inequality comparison.
// The compared iterators must refer the same container

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator<
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr==it_ptr(it) ? false : *this-it<0; }

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator>
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr==it_ptr(it) ? false : *this-it>0; }

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator<=
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr==it_ptr(it) ? true : *this-it<0; }

template<class T,class A,class W,class P,class Ref,class Ptr>
template                                <class X,  class Y>
inline bool
  avl_array_iterator<T,A,W,P,Ref,Ptr>::operator>=
  (const avl_array_iterator<T,A,W,P,X,Y> & it)     const
{ return ptr==it_ptr(it) ? true : *this-it>0; }

//////////////////////////////////////////////////////////////////

// Iterator tag function iterator_category()

template<class T, class A, class W, class P,
         class Ref, class Ptr>
inline
  typename avl_array_iterator<T,A,W,P,Ref,Ptr>::iterator_category
  iterator_category (const avl_array_iterator<T,A,W,P,Ref,Ptr>&)
{
  return typename avl_array_iterator<T,A,W,P,Ref,Ptr>::
                                          iterator_category();
}

// Iterator tag function value_type()

template<class T, class A, class W, class P,
         class Ref, class Ptr>
inline
  typename avl_array_iterator<T,A,W,P,Ref,Ptr>::value_type *
  value_type (const avl_array_iterator<T,A,W,P,Ref,Ptr>&)
{
  return reinterpret_cast<
          typename avl_array_iterator<T,A,W,P,Ref,Ptr>::
                                             value_type *>(0);
}

// Iterator tag function distance_type()

template<class T, class A, class W, class P,
         class Ref, class Ptr>
inline
  typename avl_array_iterator<T,A,W,P,Ref,Ptr>::difference_type *
  distance_type (const avl_array_iterator<T,A,W,P,Ref,Ptr>&)
{
  return reinterpret_cast<
          typename avl_array_iterator<T,A,W,P,Ref,Ptr>::
                                        difference_type *>(0);
}

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

