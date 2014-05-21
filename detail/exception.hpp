///////////////////////////////////////////////////////////////////
//                                                               //
//  Copyright (c) 2006, Universidad de Alcala                    //
//                                                               //
//  See accompanying LICENSE.TXT                                 //
//                                                               //
///////////////////////////////////////////////////////////////////

/*
  detail/exception.hpp
  --------------------

  Exceptions thrown by avl_array:

    allocator_returned_null  (constructors, insert, resize ...)
    index_out_of_bounds      (op.[], op.(), move ...)
    invalid_op_with_end      (move, erase, dereference ...)
    lesser_and_greater       (sort, insert_sorted ...)
*/

#ifndef _AVL_ARRAY_EXCEPTION_HPP_
#define _AVL_ARRAY_EXCEPTION_HPP_

#ifndef _AVL_ARRAY_HPP_
#error "Don't include this file. Include avl_array.hpp instead."
#endif

namespace mkr  // Public namespace
{

  namespace detail  // Private nested namespace mkr::detail
  {

//////////////////////////////////////////////////////////////////

class allocator_returned_null : public virtual std::exception
{
  public:
    virtual ~allocator_returned_null() throw() { }
    virtual const char* what() const throw()
    { return "Memory allocator returned NULL"; }
};

//////////////////////////////////////////////////////////////////

class index_out_of_bounds : public virtual std::exception
{
  public:
    virtual ~index_out_of_bounds() throw() { }
    virtual const char* what() const throw()
    { return "Index out of bounds"; }
};

//////////////////////////////////////////////////////////////////

class invalid_op_with_end : public virtual std::exception
{
  public:
    virtual ~invalid_op_with_end() throw() { }
    virtual const char* what() const throw()
    { return "Invalid operation with end element"; }
};

//////////////////////////////////////////////////////////////////

class lesser_and_greater : public virtual std::exception
{
  public:
    virtual ~lesser_and_greater() throw() { }
    virtual const char* what() const throw()
    { return "Lesser and greater at the same time"; }
};

//////////////////////////////////////////////////////////////////

  }  // namespace detail

}  // namespace mkr

#endif

