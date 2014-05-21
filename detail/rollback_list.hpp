///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/rollback_list.hpp
  ------------------------

  Temporary container (list form) for groups of nodes under
  creation. If an exception is thrown during the creation of a
  node, the rollback_list destructor will deallocate all
  previously created nodes. The method commit() takes the nodes
  out of the rollback_list, cancelling the rollback.
*/

#ifndef _AVL_ARRAY_ROLLBACK_LIST_HPP_
#define _AVL_ARRAY_ROLLBACK_LIST_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////

template<class T, class A,
         class W, class P>            // A list of nodes to
class rollback_list                   // complete or delete
{
  friend class mkr::avl_array<T,A,W,P>;

  typedef avl_array_node_tree_fields<T,A,W,P> node_t;
  typedef mkr::avl_array<T,A,W,P> my_array;

  private:
                          // avl_array that will deallocate
    my_array * m_owner;   // the nodes in case of rollback

    node_t * m_first;     // Ends of the doubly linked list
    node_t * m_last;      // with the nodes to commit (detach)
                          // or rollback (deallocate)

    rollback_list (my_array * owner); // The one and only constr.

    rollback_list<T,A,W,P> & operator=   // Assignment is a no-op
      (const rollback_list<T,A,W,P> &);  // (just prevents messing
                                         // lists)

    ~rollback_list ();  // Rollback on destruction (if nodes are
                        // still here) O(1) or O(n)

    void push_front (node_t * newnode);  // Add a new node to the
    void push_back (node_t * newnode);   // rollback list: O(1)

    void commit (node_t *& first,        // Detach the nodes from
                 node_t *& last);        // the rollback list,
};                                       // (cancel dealloc.) O(1)

//////////////////////////////////////////////////////////////////

// Constructor: initialize the list with null pointers and store
// the address of the avl_array. This address will be required for
// deallocating the nodes in the destructor (in case of rollback)

template<class T,class A,class W,class P>
inline rollback_list<T,A,W,P>::rollback_list (my_array * owner)
   : m_owner(owner),
     m_first(NULL),
     m_last(NULL)
{}

// The assignment operator is just a no-op. It should never be
// called. The no-op implementation avoids the posibility of a
// default assignment that would leave memory leaks and/or
// provoke double destructions

template<class T,class A,class W,class P>
inline rollback_list<T,A,W,P> &
  rollback_list<T,A,W,P>::operator=
  (const rollback_list<T,A,W,P> &)
{}

// Destructor: iff there are nodes in the list (as a result of the
// destructor being called before commit() due to an exception)
// deallocate them. If the list is empty (the usual case), this
// will take O(1). Otherwise, it will take O(n) time

template<class T,class A,class W,class P>
inline rollback_list<T,A,W,P>::~rollback_list ()
{
  node_t * p;

  while (m_first)
  {
    p = m_first;
    m_first = m_first->m_next;
    m_owner->delete_node (p);
  }
}

// push_front(): insert a node at the beginning of the list. Time
// required: O(1)

template<class T,class A,class W,class P>
inline void
  rollback_list<T,A,W,P>::
  push_front (node_t * newnode)
{
  newnode->m_next = m_first;
  newnode->m_prev = NULL;

  if (m_first)
    m_first->m_prev = newnode;
  else
    m_last = newnode;

  m_first = newnode;
}

// push_back(): append a node at the end of the list. Time
// required: O(1)

template<class T,class A,class W,class P>
inline void
  rollback_list<T,A,W,P>::
  push_back (node_t * newnode)
{
  newnode->m_prev = m_last;
  newnode->m_next = NULL;

  if (m_last)
    m_last->m_next = newnode;
  else
    m_first = newnode;

  m_last = newnode;
}

// commit(): detach the list and return it in (the pointers first
// and last are received by reference). Reset the list (cancel
// rollback)

template<class T,class A,class W,class P>
inline void
  rollback_list<T,A,W,P>::
  commit (node_t *& first,
          node_t *& last)
{
  first = m_first;
  last = m_last;

  m_first = m_last = NULL;
}

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

