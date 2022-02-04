/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SILAS_BUFFER_H
#define SILAS_BUFFER_H


#include <cstddef>


namespace silas
{
class Buffer
{
    public:
        Buffer(std::size_t capacity = 1024);

        ~Buffer();

        void*
        get_buffer();

        void*
        get_buffer(std::size_t required_capacity);

        std::size_t
        get_size();


    private:
        char*       m_buffer;
        std::size_t m_size;
};
}    // namespace silas


#endif    // !SILAS_BUFFER_H
