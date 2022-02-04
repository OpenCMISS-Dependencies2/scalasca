/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @ingroup PEARL_base
 *  @brief   Forward declarations of internal helper classes.
 *
 *  This header file provides forward declarations of internal helper
 *  classes from the pearl::detail namespace which are used in public
 *  header files.
 **/
/*-------------------------------------------------------------------------*/


#ifndef PEARL_PEARLFWD_DETAIL_H
#define PEARL_PEARLFWD_DETAIL_H


namespace pearl
{
namespace detail
{
// --- Forward declarations -------------------------------------------------

class CallTree;
template< typename DefClassT >
class DefinitionContainer;
template< typename DefClassT >
class DefinitionTree;
}    // namespace pearl::detail
}    // namespace pearl


#endif    // !PEARL_PEARLFWD_DETAIL_H
