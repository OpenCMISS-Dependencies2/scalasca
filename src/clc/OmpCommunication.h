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


#ifndef CLC_OMPCOMMUNICATION_H
#define CLC_OMPCOMMUNICATION_H


#include <algorithm>
#include <limits>


template< class T >
inline void
OMP_Allreduce_max(T  lval,
                  T& gval)
{
    {
        #pragma omp barrier
    }
    #pragma omp master
    {
        if (std::numeric_limits< T >::is_integer)
        {
            gval = std::numeric_limits< T >::min();
        }
        else
        {
            gval = -std::numeric_limits< T >::max();
        }
    }
    {
        #pragma omp barrier
    }
    #pragma omp critical
    {
        gval = std::max(lval, gval);
    }
    {
        #pragma omp barrier
    }
}


template< class T >
inline void
OMP_Allreduce_min(T  lval,
                  T& gval)
{
    {
        #pragma omp barrier
    }
    #pragma omp master
    gval = std::numeric_limits< T >::max();
    {
        #pragma omp barrier
    }
    #pragma omp critical
    {
        gval = std::min(lval, gval);
    }
    {
        #pragma omp barrier
    }
}


template< class T >
inline void
OMP_Allreduce_sum(T  lval,
                  T& gval)
{
    {
        #pragma omp barrier
    }
    #pragma omp master
    gval = 0;
    {
        #pragma omp barrier
    }
    #pragma omp critical
    gval += lval;
    {
        #pragma omp barrier
    }
}


template< class T >
inline void
OMP_Bcast(T  lval,
          T& gval)
{
    {
        #pragma omp barrier
    }
    #pragma omp master
    gval = lval;
    {
        #pragma omp barrier
    }
}


#endif    // !CLC_OMPCOMMUNICATION_H
