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


#ifndef SCOUT_LOCKEPOCHQUEUE_H
#define SCOUT_LOCKEPOCHQUEUE_H


#include <cassert>
#include <queue>

#include <pearl/CountedPtr.h>
#include <pearl/RemoteEvent.h>
#include <pearl/RemoteRmaEpoch.h>


namespace scout
{
typedef pearl::CountedPtr< pearl::RemoteRmaEpoch > RemoteRmaEpochPtr;
class LockEpochCompare
{
    public:
        LockEpochCompare(const bool& reverse = false)
        {
            mReverse = reverse;
        }

        bool
        operator()(const RemoteRmaEpochPtr& lhs,
                   const RemoteRmaEpochPtr& rhs) const
        {
            // Retrieve Unlock event from first epoch
            pearl::RemoteRmaEpoch::const_reverse_iterator lhsIt  = lhs->rbegin();
            pearl::RemoteRmaEpoch::const_reverse_iterator lhsEnd = lhs->rend();
            while (  (lhsIt != lhsEnd)
                  && !(*lhsIt)->isOfType(pearl::MPI_RMA_UNLOCK))
            {
                ++lhsIt;
            }
            assert(lhsIt != lhsEnd);

            // Retrieve Unlock event from second epoch
            pearl::RemoteRmaEpoch::const_reverse_iterator rhsIt  = rhs->rbegin();
            pearl::RemoteRmaEpoch::const_reverse_iterator rhsEnd = rhs->rend();
            while (  (rhsIt != rhsEnd)
                  && !(*rhsIt)->isOfType(pearl::MPI_RMA_UNLOCK))
            {
                ++rhsIt;
            }
            assert(rhsIt != rhsEnd);

            return ((*lhsIt)->getTimestamp() < (*rhsIt)->getTimestamp());
        }


    private:
        bool mReverse;
};


typedef std::priority_queue< RemoteRmaEpochPtr,
                             std::vector< RemoteRmaEpochPtr >,
                             LockEpochCompare > LockEpochQueue;
}    // namespace scout


#endif    // !SCOUT_LOCKEPOCHQUEUE_H
