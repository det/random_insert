///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  avl_array.hpp
  -------------

  AVL Array is a C++ STL-style container with fast random access
  _and_ fast insert/erase at any point (all are O(log N) worst
  case)

  Free Software Project hosted at:
  http://avl-array.sourceforge.net

  The source code is organized in 28 different header files, of
  which this is the main one. All them have been profusely
  commented. The #include sections at the beginnig and the end of
  this file might serve as an index to the different files.

  See the accompanying documentation for more details.

                        November, 2006
                        Martin Knoblauch Revuelta
                        Universidad de Alcala (Spain)
                        comocomocomo at users.sourceforge.net
*/

#ifndef _AVL_ARRAY_HPP_
#define _AVL_ARRAY_HPP_

//////////////////////////////////////////////////////////////////

#include <iterator>
#include <utility>
#include <memory>
#include <functional>
#include <cassert>

//////////////////////////////////////////////////////////////////

#ifdef AA_DEREF_NULL_ON_ASSERTION_FAILURE
#define AA_ASSERT(C) do { if (!(C)) (*(int*)0)++; } while (0)
#else
#ifdef BOOST_ASSERT
#define AA_ASSERT(C) do { BOOST_ASSERT(C);        } while (0)
#else
#define AA_ASSERT(C) do { assert(C);              } while (0)
#endif
#endif

#ifdef AA_NO_SLOW_ASSERTS
#define AA_ASSERT_HO(C) do {                      } while (0)
#else
#define AA_ASSERT_HO(C) do { AA_ASSERT(C);        } while (0)
#endif

#ifdef AA_ASSERT_BEFORE_THROW
#define AA_ASSERT_EXC(C,E) do { AA_ASSERT (C); \
                                if (!(C)) throw E; } while (0)
#else
#define AA_ASSERT_EXC(C,E) do { if (!(C)) throw E; } while (0)
#endif

//////////////////////////////////////////////////////////////////

#include "detail/forward_decl.hpp"      // Forward declarations
                                        // (for internal use only)

#include "detail/data_provider.hpp"     // Helper functors
                                        // (for internal use only)

#include "detail/empty_number.hpp"      // Default W (no NPSV)
                                        // and P (no stable sort)

#include "detail/exception.hpp"         // Exceptions

#include "detail/iterator.hpp"          // Normal iterators
#include "detail/iterator_reverse.hpp"  // Reverse iterators

#include "detail/node.hpp"              // Generic node of the tree
#include "detail/node_with_data.hpp"    // Node+data (all but end)
                                        // (for internal use only)

                                        // Temporary container of
                                        // nodes in massive
#include "detail/rollback_list.hpp"     // operations, used for
                                        // consistency in case of
                                        // guest (T) exceptions
                                        // (for internal use only)

// (Other headers, containing avl_array methods implementations
// are included from the end of this file)

//////////////////////////////////////////////////////////////////

namespace mkr  // Public namespace
{

  using namespace detail;

//////////////////////////////////////////////////////////////////

template<class T,                       // The container class
         class A=std::allocator<T>,     // (at last)
         class W=empty_number,
         class P=empty_number>
class avl_array
  : private avl_array_node_tree_fields<T,A,W,P>
{

  // -------------------------- TYPES ----------------------------

  public:

    typedef avl_array_node_tree_fields<T,A,W,P>  node_t;
    typedef avl_array_node<T,A,W,P>              payload_node_t;
    typedef avl_array<T,A,W,P>                   my_class;
    typedef rollback_list<T,A,W,P>               rollback_list_t;

    typedef typename A::value_type               value_type;
    typedef typename A::reference                reference;
    typedef typename A::const_reference          const_reference;
    typedef typename A::pointer                  pointer;
    typedef typename A::const_pointer            const_pointer;

    typedef std::ptrdiff_t                       difference_type;
    typedef std::size_t                          size_type;

    typedef avl_array_iterator<T,A,W,P,
                           reference,pointer>    iterator;
    typedef avl_array_iterator<T,A,W,P,
               const_reference,const_pointer>    const_iterator;
    typedef avl_array_rev_iter<T,A,W,P,
                           reference,pointer>    reverse_iterator;
    typedef avl_array_rev_iter<T,A,W,P,
            const_reference,const_pointer> const_reverse_iterator;

    typedef typename A::template
            rebind<payload_node_t>::other        allocator_t;


  // ---------------------- CONCEPT CHECKS -----------------------
#ifdef BOOST_CLASS_REQUIRE
  private:

  BOOST_CLASS_REQUIRE (value_type, boost, AssignableConcept);
  BOOST_CLASS_REQUIRE (value_type, boost,
                                        LessThanComparableConcept);
  BOOST_CLASS_REQUIRE (value_type, boost,
                                        EqualityComparableConcept);
  BOOST_CLASS_REQUIRE (value_type, boost,
                                         CopyConstructibleConcept);
  BOOST_CLASS_REQUIRE (value_type, boost,
                                      DefaultConstructibleConcept);

  BOOST_CLASS_REQUIRE (allocator_t, boost,
                                      DefaultConstructibleConcept);

  BOOST_CLASS_REQUIRE (W, boost, AssignableConcept);
  BOOST_CLASS_REQUIRE (W, boost, LessThanComparableConcept);
  BOOST_CLASS_REQUIRE (W, boost, EqualityComparableConcept);
  BOOST_CLASS_REQUIRE (W, boost, CopyConstructibleConcept);
  BOOST_CLASS_REQUIRE (W, boost, DefaultConstructibleConcept);
  BOOST_CLASS_REQUIRE2 (int, W, boost, ConvertibleConcept);

  BOOST_CLASS_REQUIRE (P, boost, AssignableConcept);
  BOOST_CLASS_REQUIRE (P, boost, LessThanComparableConcept);
  BOOST_CLASS_REQUIRE (P, boost, EqualityComparableConcept);
  BOOST_CLASS_REQUIRE (P, boost, CopyConstructibleConcept);
  BOOST_CLASS_REQUIRE (P, boost, DefaultConstructibleConcept);
  BOOST_CLASS_REQUIRE2 (unsigned, P, boost, ConvertibleConcept);

#endif

  // --------------------- PUBLIC INTERFACE ----------------------

  public:


    // Constructors and destructor
    // See constr_destr.hpp
    //
    // Default con.: empty avl_array (O(1))
    // Copy con.: copy of other avl_array (O(N))
    // Vector con.: avl_array with n elements like t (O(N))
    // Vector def. con.: " with n default-constructed elem. (O(N))
    // Sequence con.: " with copies of [from,to) (O(N))
    // Sequence con.: " with copies of [from,from+n) (O(N))
    // Destructor (O(N))

    avl_array ();
    avl_array (const my_class & a);
    avl_array (size_type n, const_reference t);
    avl_array (int       n, const_reference t);
    avl_array (long      n, const_reference t);
    avl_array (size_type n);

    template <class IT>
    avl_array (IT from, IT to);

    template <class IT>
    avl_array (IT from, size_type n);

    ~avl_array ();


    // Container assignment
    // See assign.hpp
    //
    // Assignment operator (O(M+N), M to delete + N to copy)
    // swap(): interchange contents (O(1))

    const my_class & operator= (const my_class & a);
    void swap (my_class & a);


    // Size methods
    // See size.hpp
    //
    // size(): retrieve current size (O(1))
    // empty(): true if empty; false otherwise (O(1))
    // max_size(): estimated maximum size in theory (O(1))
    // resize(n): change size (O(min{N, n log N}))
    // resize(n,t): idem, but add copies of t  "

    size_type size () const;
    bool empty () const;
    static size_type max_size ();
    void resize (size_type n);
    void resize (size_type n, const_reference t);


    // Iterators referring begin and end
    // See begin_end.hpp
    //
    // begin(): get normal iterator referring the beginnig (O(1))
    // begin()_const_: idem but const_iterator (O(1))
    // end(): get normal iterator referring the ending (O(1))
    // end()_const_: idem but const_iterator (O(1))
    // rbegin(): reverse it. referring rev. beginnig (O(1))
    // rbegin()_const_: idem but const_reverse_iterator (O(1))
    // rend(): reverse it. referring rev. ending (O(1))
    // rend()_const_: idem but const_reverse_iterator (O(1))

    iterator begin ();
    const_iterator begin () const;
    iterator end ();
    const_iterator end () const;
    reverse_iterator rbegin ();
    const_reverse_iterator rbegin () const;
    reverse_iterator rend ();
    const_reverse_iterator rend () const;


    // Container comparison operators
    // See compare.hpp
    //
    // ==: size is == and all elements are ==  (O(min{M,N})*)
    // !=: not ==  (O(min{M,N})*)
    // <: 1st != is <, or all are == and size is <  (O(min{M,N})*)
    // >: 1st != is >, or all are == and size is >  (O(min{M,N})*)
    // <=: not >  (O(min{M,N})*)
    // >=: not <  (O(min{M,N})*)
    //
    // (*) M is the # of elements of one container and
    //     N is the # of elements of the other one

    bool operator== (const my_class & a) const;
    bool operator!= (const my_class & a) const;
    bool operator< (const my_class & a) const;
    bool operator> (const my_class & a) const;
    bool operator<= (const my_class & a) const;
    bool operator>= (const my_class & a) const;


    // Indexation operators
    // See random_access.hpp
    //
    // operator[]: get reference of n'th element O(log(N))
    // operator(): idem O(log(N))
    // at():       idem O(log(N))
    // operator[]_const_: idem, but const O(log(N))
    // operator()_const_: idem, but const O(log(N))
    // at()_const_:       idem, but const O(log(N))

    reference operator[] (size_type n);
    reference operator() (size_type n);
    reference at (size_type n);
    const_reference operator[] (size_type n) const;
    const_reference operator() (size_type n) const;
    const_reference at (size_type n) const;


    // Insertion
    // See insert.hpp
    //
    // it insert(t): insert anywhere (O(log N) with no rotations)
    // it insert(it,t): insert before (O(log N))
    // rit insert(rit,t): insert before* (O(log N))
    // insert(it,n,t): vector-insert before (O(min{N, n log N}))
    // insert(rit,n,t): vector-insert before**        "
    // insert(it,from,to): sequence-insert before     "
    // insert(rit,from,to): sequence-insert before**  "
    //
    // (*) "after" from a 'straight' point of view
    // (**) "after and in reverse order" from a 'straight' POV

    iterator insert (const_reference t);

    iterator insert (const iterator & it, const_reference t);
    reverse_iterator insert (const reverse_iterator & it,
                             const_reference t);

    void insert (const iterator it,
                 size_type n, const_reference t);
    void insert (const iterator & it,
                 int n,       const_reference t);
    void insert (const iterator & it,
                 long n,      const_reference t);
    void insert (const reverse_iterator it,
                 size_type n, const_reference t);
    void insert (const reverse_iterator & it,
                 int n,       const_reference t);
    void insert (const reverse_iterator & it,
                 long n,      const_reference t);

    template <class IT>
    void insert (iterator it, IT from, IT to);

    template <class IT>
    void insert (reverse_iterator it, IT from, IT to);


    // Erasing
    // See erase.hpp
    //
    // it/rit erase(it/rit): (O(log N))
    // it erase(it,n): vector-erase (O(min{N, n log N}))
    // rit erase(rit,n): vector-erase (reverse)    "
    // it erase(from,to): range-erase              "
    // rit erase(rfrom,rto): range-erase (reverse) "
    // clear(): delete all (O(N))

    iterator         erase (iterator it);
    reverse_iterator erase (reverse_iterator it);

    iterator         erase (iterator from, size_type n);
    reverse_iterator erase (reverse_iterator from, size_type n);

    iterator         erase (iterator from, iterator to);
    reverse_iterator erase (reverse_iterator from,
                            reverse_iterator to);
    void clear ();


    // Double ended queue interface support
    // See front_back.hpp
    //
    // front(): get reference to first element (O(1))
    // front()_const_: idem but const_reference (O(1))
    // push_front(): insert before first element (O(log N))
    // pop_front(): remove first element (O(log N))
    // back(): get reference to last element (O(1))
    // back()_const_: idem but const_reference (O(1))
    // pop_back(): remove last element (O(log N))
    // push_back(): append after last element (O(log N))

    reference front ();
    const_reference front () const;
    void push_front (const_reference t);
    void pop_front ();
    reference back ();
    const_reference back () const;
    void push_back (const_reference t);
    void pop_back ();


    // Move operations (don't touch value_type objects, just
    // change the links of tree nodes)
    // See move.hpp
    //
    // swap(it/rit,it/rit): interchange two elements
    //                                (O(1), O(log N) with NPSV)
    // move(it/rit,n): offset move (O(log N))
    // move(it/rit,it/rit): individual move O(log(M)+log(N))
    // move(it/rit,n,it/rit): group move O(min{N, n log N} +
    //                                     min{M, n log M})
    // move(it/rit,it/rit,it/rit): range move    "
    // splice(it/rit,cont): group move (see above)
    // splice(it/rit,cont,it/rit): individual move (see above)
    // splice(it/rit,cont,it/rit,it/rit): range move (see above)
    // reverse(): invert the sequence (O(N))

    static void swap (iterator it1, iterator it2);
    static void swap (iterator it1, reverse_iterator it2);
    static void swap (reverse_iterator it1, iterator it2);
    static void swap (reverse_iterator it1, reverse_iterator it2);

    static void move (iterator it, difference_type n);
    static void move (reverse_iterator it, difference_type n);

    static void move (iterator src, iterator dst);
    static void move (iterator src, reverse_iterator dst);
    static void move (reverse_iterator src, iterator dst);
    static void move (reverse_iterator src, reverse_iterator dst);

    static void move (iterator src_from, size_type n,
                      iterator dst);
    static void move (reverse_iterator src_from, size_type n,
                      iterator dst);
    static void move (iterator src_from, size_type n,
                      reverse_iterator dst);
    static void move (reverse_iterator src_from, size_type n,
                      reverse_iterator dst);

    static void move (iterator src_from,
                      iterator src_to,
                      iterator dst);
    static void move (reverse_iterator src_from,
                      reverse_iterator src_to,
                      iterator dst);
    static void move (iterator src_from,
                      iterator src_to,
                      reverse_iterator dst);
    static void move (reverse_iterator src_from,
                      reverse_iterator src_to,
                      reverse_iterator dst);

    void splice (iterator dst,         my_class & src);
    void splice (reverse_iterator dst, my_class & src);

    void splice (iterator dst,
                 my_class & src, iterator src_from);
    void splice (reverse_iterator dst,
                 my_class & src, iterator src_from);
    void splice (iterator dst,
                 my_class & src, reverse_iterator src_from);
    void splice (reverse_iterator dst,
                 my_class & src, reverse_iterator src_from);

    void splice (iterator dst, 
                 my_class & src,
                 iterator src_from, iterator src_to);
    void splice (reverse_iterator dst, 
                 my_class & src,
                 iterator src_from, iterator src_to);
    void splice (iterator dst, 
                 my_class & src,
                 reverse_iterator src_from,
                 reverse_iterator src_to);
    void splice (reverse_iterator dst, 
                 my_class & src,
                 reverse_iterator src_from,
                 reverse_iterator src_to);

    void reverse ();


    // Sorting methods and related algorithms
    // See sorted_search_tree.hpp
    //
    // binary_search(): fast search* (O(log N))
    // insert_sorted(): insert keeping order* (O(log N))
    // sort(): impose order                          (O(N log N))
    // stable_sort(): idem + keep current order between equals "
    // merge(): mix two containers, keeping order* (O(M+N))
    // unique(): remove duplicates* (O(N))
    // (*) Elements must be previously in order

    template<class CMP>
    bool binary_search (const_reference t,
                        const_iterator & it,
                        CMP cmp) const;
    template<class CMP>
    bool binary_search (const_reference t,
                        iterator & it,
                        CMP cmp);
    template<class CMP>
    bool binary_search (const_reference t,
                        const_reverse_iterator & it,
                        CMP cmp) const;
    template<class CMP>
    bool binary_search (const_reference t,
                        reverse_iterator & it,
                        CMP cmp);

    bool binary_search (const_reference t, iterator & it);
    bool binary_search (const_reference t, reverse_iterator & it);
    bool binary_search (const_reference t,
                        const_iterator & it) const;
    bool binary_search (const_reference t,
                        const_reverse_iterator & it) const;

    bool binary_search (const_reference t) const;

    template<class CMP>
    iterator insert_sorted (const_reference t,
                            bool allow_duplicates,
                            CMP cmp);

    iterator insert_sorted (const_reference t,
                            bool allow_duplicates=true);

    template<class CMP>
    void sort (CMP cmp);

    void sort ();

    template<class CMP>
    void stable_sort (CMP cmp);

    void stable_sort ();

    template<class CMP>
    void merge (my_class & donor, CMP cmp);

    void merge (my_class & donor);

    template<class CMP>
    void unique (CMP cmp);

    void unique ();


    // Non-Proportional Sequence View
    // See npsv.hpp
    //
    // npsv_update_sums(): udate width sums O(1) or O(N)*
    // npsv_width(): get total width O(1) or O(N)*
    // npsv_width(it): get an element's width O(1)
    // npsv_set_width(): set an element's width O(log N) or O(1)**
    // npsv_pos_of(): get an element's position O(log N) or O(N)*
    // npsv_at_pos(): get elem. of a position O(log N) or O(N)*
    // (*) width sums need to be updated
    // (**) don't update width sums (lazy mode)

    void npsv_update_sums () const;

    W npsv_width () const;
    W npsv_width (const_iterator it) const;

    void npsv_set_width (const iterator & it, W w,
                         bool update_sums=true);

    W npsv_pos_of (const_iterator it) const;

    iterator npsv_at_pos (W pos);
    const_iterator npsv_at_pos (W pos) const;

    template<class CMP>
    iterator npsv_at_pos (W pos, CMP cmp);

    template<class CMP>
    const_iterator npsv_at_pos (W pos, CMP cmp) const;


  // ------------------------- FRIENDS ---------------------------

  private:

  friend class avl_array_iterator<T,A,W,P,reference,pointer>;
  friend class avl_array_iterator<T,A,W,P,const_reference,
                                            const_pointer>;

  friend class avl_array_rev_iter<T,A,W,P,reference,pointer>;
  friend class avl_array_rev_iter<T,A,W,P,const_reference,
                                          const_pointer>;

  friend class rollback_list<T,A,W,P>;


  // ----------------------- PRIVATE DATA ------------------------

  private:

    allocator_t allocator;   // Nodes allocator for this object

                                      // NPSV dirty bit
    mutable bool m_sums_out_of_date;  // If true: NPSV sums must
                                      // be recalculated


  // ------------------ PRIVATE HELPER METHODS -------------------

  private:

    // Private helper methods for the relationship container/nodes
    // See detail/helper_fun_node.hpp
    //
    // dummy(): get the sentinel/end node of this container (O(1))
    // dummy_ownwer(): get the container of a dummy node (O(1))
    // owner(): get the container of a node (O(log N))

    node_t * dummy () const;
    static my_class * dummy_owner (const node_t * pdummy);
    static my_class * owner (const node_t * node);


    // Private helper methods for iterators
    // See detail/helper_fun_iter.hpp
    //
    // next(): get the next node of a given node (O(1))
    // prev(): get the previous node of a given node (O(1))
    // data(): get (by ref) the data of a node (with data!) (O(1))
    // iterator_pointer(): get the node refered by an it. (O(1))
    // make_const_iterator(): get const it. referring a node (O(1))
    // make_const_rev_iter(): get const reverse it...       (O(1))

    static node_t * next (node_t * p);
    static node_t * prev (node_t * p);
    static reference data (node_t * p);

    template<class IT>
    static node_t * iterator_pointer (const IT & it);

    static const_iterator make_const_iterator (node_t * p);
    static const_reverse_iterator make_const_rev_iter (node_t * p);


    // Methods for nodes allocation/deallocation
    // See alloc.hpp
    //
    // new_node(): Allocate and construct a new node (O(1))
    // delete_node(): Destruct and deallocate a node (O(1))

    node_t * new_node (const_pointer t=NULL);
    void delete_node (node_t * p);


    // Tree balance methods
    // See balance.hpp
    //
    // update_counters():
    //   climb from a node to the root updating all height and
    //   count fields in the way                    (O(log N))
    //
    // update_counters_and_rebalance():
    //   climb from a node to the root updating all height and
    //   count fields in the way _and_ rebalancing with AVL
    //   rotations where required                   (O(log N))

    static void update_counters (node_t * p);
    static void update_counters_and_rebalance (node_t * p);


    // Helper methods for erasing nodes (or just moving...)
    // See erase.hpp
    //
    // extract_node(): remove a node (O(1)) [+ later rebalance]
    // erase_it(IT): delete one element (O(log N))
    // extract_nodes(): remove n nodes (O(min{N, n log N})
    // erase_it(IT,n): delete n elements (O(min{N, n log N})

    static node_t * extract_node (node_t * p);

    template<class IT>
    IT erase_it (IT it);

    template <class IT>  // Return true iff the destination (dst)
    bool extract_nodes   // belongs to the extracted range

      (IT & from,                   // Source (might be reverse!)
       size_type n,                 // # of nodes to extract

       node_t *& first,             // List with extracted nodes
       node_t *& last,              // (pointers passed by ref.!)

       node_t * dst=NULL,           // Destination to check

       bool * delayed_rebuild=NULL, // In: non-NULL means "delay
                                    // tree reconstruction, cause
                                    // the same tree is used for
                                    // destination"
                                    // Out: true means "reconstr.
                                    // was delayed and the caller
                                    // should do it after
                                    // reinsertion"

       bool reverse=false);         // true means: "destination
                                    // has been secified with a
                                    // reverse iterator, so build
                                    // a reverse list"

    template <class IT>
    IT erase_it (IT from,      // Where to start erasing
                 size_type n); // Number of elements to erase


    // Helper methods for random access
    // See random_access.hpp
    //
    // position_of_node(): get the position of a node (O(log N))
    // node_at_pos(): get the node of a position (O(log N))
    // jump(): get the node of a relative position (O(log N))

    static size_type position_of_node (const node_t * p,
                                       my_class * & a,
                                       bool reverse);

    node_t * node_at_pos (size_type pos) const;

    static node_t * jump (node_t * p,
                          difference_type n,
                          bool reverse);


    // Helper method for assignments and swaps
    // See assign.hpp
    //
    // acquire_tree(): steal the tree of another avl_array (O(1))

    void acquire_tree (const node_t & nf);


    // Helper methods for insertion
    // See insert.hpp
    //
    // insert_before(): insert a node in a given pos. (O(log N))
    // insert_anywhere(): add a node to the tree (O(log N))

    static void insert_before (node_t * newnode, node_t * p);
    void insert_anywhere (node_t * newnode);


    // Initializer method
    // See constr_destr.hpp
    //
    // init(): initialize an avl_array with "empty" state (O(1))

    void init ();


    // Helper methods for massive operations (trees building)
    // See build_tree.hpp
    //
    // worth_rebuild(): decide how to perform a massive op. (O(1))
    // build_known_size_tree(): make a tree of a given size (O(N))

    static bool worth_rebuild (         // Return: true=rebuild
                     size_type n,       // Elements to insert/erase
                     size_type N,       // Current size
                     bool erase=false); // true=erase, false=insert

    node_t * build_known_size_tree      // Return first unused node
                       (size_type n,    // Total number of nodes
                        node_t * next); // List with nodes to link


    // Helper method for massive operations (temp. lists)
    // See build_list.hpp
    //
    // construct_nodes_list(): prepare a list of new nodes (O(N))

    template<class DP>               // Return number of nodes
    size_type                        // created
      construct_nodes_list
        (node_t *& first,            // First and last node of
         node_t *& last,             // the list created
         size_type n,                // Number of nodes to create
         DP & data_provider,         // Functor: objects to copy
         bool reverse=false,         // Direction
         bool exhaust_dp=false);     // Use DP until NULL


    // Helper methods for moving and swapping
    // See move.hpp
    //
    // swap_nodes(): swap two nodes (O(1), O(log N) with NPSV)
    // move_node(): move n places along the sequence (O(log N))
    // move_node(): extract and insert in other pos. (O(log N))
    // move_nodes(): move n nodes to another pos.
    //                                        (O(min{N, n log N})

    static void swap_nodes (node_t * p, node_t * q);
    static void move_node (node_t * p, difference_type n);
    static void move_node (node_t * p, node_t * q);

    template<class IT>
    static void move_nodes (IT src_from,
                            size_type n,
                            node_t * dst,
                            bool reverse=false);


    // Helper method for sorting and searching
    //
    // binary_search(): search value in a sorted tree (O(log N))

    template<class CMP>
    bool binary_search             // Return true iff it is found
        (const_reference t,        // What to search
         node_t ** pp,             // Pos. where it is / should be
         CMP cmp,                  // Functor for '<' comparisons
         P oldpos=0,               // Old position (in stable sort)
         bool stable=false)
                        const;


    // Helper method for massive resize operations
    // See size.hpp
    //
    // resize(n,DP): change size (rebuild)
    //                                 O(max{old_size,new_size})

    template<class DP>
    void resize (size_type n, DP & dp);

};

//////////////////////////////////////////////////////////////////

} // End of namespace mkr

//////////////////////////////////////////////////////////////////

#include "detail/helper_fun_iter.hpp" // Helpers for iterators
#include "detail/helper_fun_node.hpp" //  " for managing nodes

#include "detail/aa_alloc.hpp"  // Nodes' allocation/deallocation

#include "detail/aa_build_list.hpp"   // Massive alloc.[+rollback]
#include "detail/aa_build_tree.hpp"   // Build perfectly balanced
#include "detail/aa_constr_destr.hpp" // Constr., destr. and init.
#include "detail/aa_assign.hpp"       // Assignments and cont. swap
#include "detail/aa_compare.hpp"      // Containers comparison

#include "detail/aa_random_access.hpp" // op.[] and iterators +...
#include "detail/aa_begin_end.hpp"     // begin(), end(), r...
#include "detail/aa_front_back.hpp"    // front(), back(), push...

#include "detail/aa_balance.hpp" // Counters update and AVL balance
#include "detail/aa_insert.hpp"  // insert()
#include "detail/aa_erase.hpp"   // erase(), clear()
#include "detail/aa_move.hpp"   // move/splice(), swap(), reverse()
#include "detail/aa_size.hpp"   // size(), max_size(), resize()...

#include "detail/aa_sorted_search_tree.hpp" // sort(),
                                           // [stable_sort()],
                                          // binary_search(),
                                         // insert_sorted(),
                                        // merge(), unique()

#include "detail/aa_npsv.hpp"   // Non Proportional Sequence View

// (Other headers, containing detail classes
// are included from the beginning of this file)

//////////////////////////////////////////////////////////////////

#undef AA_ASSERT
#undef AA_ASSERT_HO
#undef AA_ASSERT_EXC

//////////////////////////////////////////////////////////////////

#endif

