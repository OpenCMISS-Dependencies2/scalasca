/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2019-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PEARL_IOMANIP_DETAIL_H
    #error "Do not include this file!  Use 'iomanip_detail.h' instead."
#endif


namespace pearl
{
namespace detail
{
// --- I/O manipulators -----------------------------------------------------

inline IomSetIndent
setIndent(long level)
{
    const IomSetIndent value = { level };

    return value;
}
}    // namespace pearl::detail
}    // namespace pearl
