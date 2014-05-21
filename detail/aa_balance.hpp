///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/aa_balance.hpp
  ---------------------

  Private helper methods for maintaining the tree balance:
 
  update_counters():
    climb from a node to the root updating all height and
    count fields in the way                    (O(log N))
 
  update_counters_and_rebalance():
    climb from a node to the root updating all height and
    count fields in the way _and_ rebalancing with AVL
    rotations where required                   (O(log N))
*/

#ifndef _AVL_ARRAY_BALANCE_HPP_
#define _AVL_ARRAY_BALANCE_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

//////////////////////////////////////////////////////////////////

// ------------------- PRIVATE HELPER METHODS --------------------

// update_counters(): climb from a node to the root updating all
// height and count fields in the way
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
  void
  avl_array<T,A,W,P>::update_counters
  (typename avl_array<T,A,W,P>::node_t * p)
{
  size_type i, j;

  while (p)                      // Climb until the root is
  {                              // reached
    i = p->left_height ();
    j = p->right_height ();

    p->m_height = (i>j?i:j) + 1; // The height is that of the
                                 // largest branch, plus one
    i = p->left_count ();
    j = p->right_count ();
                                 // The count is the sum of the
    p->m_count = i + j + 1;      // subtrees' counts plus one

                                           // The width works
    p->m_total_width = p->left_width () +  // like count, but
                       p->right_width () + // adding the node's
                       p->m_node_width;    // width instead of
                                           // just 1

    p = p->m_parent;             // Step up
  }
}

// update_counters_and_rebalance(): climb from a node to the root
// updating all height and count fields in the way _and_
// rebalancing with AVL rotations if necessary (this is what makes
// the whole thing possible, so thanks go to G.M. Adelson-Velsky
// and E.M. Landis)
//
// Complexity: O(log N)

template<class T,class A,class W,class P>
// not inline
  void
  avl_array<T,A,W,P>::update_counters_and_rebalance
  (typename avl_array<T,A,W,P>::node_t * p)
{
  size_type i, j;
  int s;
  node_t * q, * r;

  while (p)                      // Climb until the root is
  {                              // reached
    i = p->left_height ();
    j = p->right_height ();

    p->m_height = (i>j?i:j) + 1; // The height is that of the
                                 // largest branch, plus one

                                           // The width works
    p->m_total_width = p->left_width () +  // like count, but
                       p->right_width () + // adding the node's
                       p->m_node_width;    // width instead of
                                           // just 1

    s = -1; // -1 means balanced

    if (p->m_parent)  // (don't re-balance dummy node)
    {
      if (i > j+1)       // Unbalanced to the left..
        s = R;           // .. rotate to the right
      else if (j > i+1)  // Unbalanced to the right..
        s = L;           // .. rotate to the left
    }

    if (s==-1)               // If we are in the dummy node or
    {                        // the current node is balanced
      i = p->left_count ();
      j = p->right_count ();
                                 // The count is the sum of the
      p->m_count = i + j + 1;    // subtrees' counts plus one

      p = p->m_parent;           // Step up
      continue;                  // This node is done
    }
                             // Otherwise... re-balance!
                             // Go down to child
    p = p->m_children[1-s];  // (side of the long branch)

    i = p->left_height ();   // Calculate heights of this
    j = p->right_height ();  // new position

         // Consider the sub-subtrees of the long subtree.
         // If the sub-subtree in the middle (between the
         // other sub-subtree and the short subtree) is shorter
         // or equal to the side sub-subtree, we just need
         // a simple rotation; otherwise, we need a double
         // rotation

    if ( (s==R && i>=j) ||      // If a simple rotation
         (s==L && i<=j)    )    // is enough...
    {
      /*
        Before:       C
                      |              Y shorter or eq. to X
                      B
                   /     \                   p -> A
                A         Z
              /   \      ***
             X     Y    *****_____
            ***   ***              |
           ***** *****_ _          | 2 <-- unbalanced!
           ***** ·····    | 0/1    |
           *****_·····____|_______ |
                                    
                                        Simple rotation
        After:          C               (with s==R in
                        |               the figure)
                        A
                     /     \
                   X         B
                  ***      /   \
                 *****    Y     Z
                 *****   ***   ***
              ___*****  ***** *****___
        0/1 |           ·····          | 0/1
            | __________·····_________ |


        Notation for comments in the code bellow:

          F
          |   F's downling points to G
          G   G's uplink points to F

          F
          v   F's downling points to G
          G   G's uplink points to somthing else (not to F)

          F
          ^   F's downling points to somthing else (not to G)
          G   G's uplink points to F
      */

      p->m_parent->m_children[1-s] =
                         p->m_children[s]; //   B       B
                                           //  /  =>>  /^
      if (p->m_children[s])                // A       Y A
        p->m_children[s]->m_parent =
                              p->m_parent;
                                           //  C       C
      p->m_children[s] = p->m_parent;      //  |       ^
      p->m_parent =                        //  B  =>>  A
               p->m_children[s]->m_parent; //  ^        \.
      p->m_children[s]->m_parent = p;      //  A         B

      if (p->m_parent->m_children[L] ==
                         p->m_children[s]) //  C       C
        p->m_parent->m_children[L] = p;    //  v  =>>  |
      else                                 //  B       A
        p->m_parent->m_children[R] = p;

                             // Step down to B
      p = p->m_children[s];  // (it is balanced but it needs
    }                        // count and height updates)
    else
    {                           // If a double rotation
      q = p->m_children[s];     // is required...
      r = p->m_parent;

      /*
        Before:              C
                             |          Y larger than X
                             B
                          /     \            p -> A
                       A           Z         q -> Y
                     /   \        ***        r -> B
                   X       Y     *****___
                  ***     / \             |
              ___*****   U   V            | 2 <-- unbalanced!
          1 |           *** ***           |
            | _________**** ****_________ |


                                        Double rotation
        After:             C            (with s==R in
                           |            the figure)
                           Y
                        /     \
                     A           B
                   /   \       /   \
                  X     U     V     Z
                 ***   ***   ***   ***
                ***** ***** ***** *****

        (See notation for comments in simple rotation)
      */
                                             //   C
      q->m_parent = r->m_parent;             //   |
                                             //   B      C
      if (q->m_parent->m_children[L] == r)   //  /  =>>  |
        q->m_parent->m_children[L] = q;      // A        Y
      else                                   //  \.
        q->m_parent->m_children[R] = q;      //   Y

      r->m_children[1-s] = q->m_children[s]; //   B       B
                                             //  /  =>>  /
      if (r->m_children[1-s])                // A       V
        r->m_children[1-s]->m_parent = r;

      p->m_children[s] = q->m_children[1-s]; // A       A
                                             //  \  =>>  \.
      if (p->m_children[s])                  //   Y       U
        p->m_children[s]->m_parent = p;
                                             // B  Y        Y
      q->m_children[1-s] = p;                // ^  v  =>>  /
      p->m_parent = q;                       // A  U      A

      q->m_children[s] = r;                  // C  Y      Y
      r->m_parent = q;                       // ^  v  =>>  \.
                                             // B  V        B
      i = r->left_height ();
      j = r->right_height ();       // A, B, Y, C and upper
                                    // nodes need height and
      r->m_height = (i>j?i:j) + 1;  // count updates

      i = r->left_count ();         // Update B here and go on
      j = r->right_count ();        // from A (and up) in the
                                    // next iteration
      r->m_count = i + j + 1;

      r->m_total_width = r->left_width () +  // Update B's
                         r->right_width () + // width too
                         r->m_node_width;
    }
  }
}

//////////////////////////////////////////////////////////////////

}  // namespace mkr

#endif

