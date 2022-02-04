/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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


#ifndef SCOUT_MPICOMMUNICATIONHANDLER_H
#define SCOUT_MPICOMMUNICATIONHANDLER_H


#include <pearl/ScopedPtr.h>

#include "AnalysisHandler.h"


namespace scout
{
/*-------------------------------------------------------------------------*/
/**
 * @class   MpiCommunicationHandler
 * @ingroup scout
 * @brief   MPI Communication management callback routines
 *
 * The MpiCommunicationHandler updates handles collective and point-to-point
 * MPI message exchanges
 */
/*-------------------------------------------------------------------------*/

class MpiCommunicationHandler
    : public AnalysisHandler
{
    class MpiCHImpl;

    const pearl::ScopedPtr< MpiCHImpl > mP;


    public:
        MpiCommunicationHandler();

        ~MpiCommunicationHandler();

        virtual void
        register_pre_pattern_callbacks(const CallbackManagerMap& cbm);
};
}    // namespace scout


#endif    // !SCOUT_MPICOMMUNICATIONHANDLER_H
