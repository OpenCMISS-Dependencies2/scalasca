/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2014                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SALSA_SALSACALLBACK_H
#define SALSA_SALSACALLBACK_H


#include <vector>

#include <mpi.h>

#include <pearl/CallbackData.h>
#include <pearl/CallbackManager.h>
#include <pearl/MpiMessage.h>

#include "MessageChecker.h"
#include "salsa.h"


class SalsaCallback
{
    public:
        SalsaCallback(int            r,
                      int            s,
                      funcs          f,
                      modes          m,
                      MessageChecker mc);

        virtual
        ~SalsaCallback();

        void
        send(const pearl::CallbackManager& cbmanager,
             int                           user_event,
             const pearl::Event&           event,
             pearl::CallbackData*          data);

        void
        recv(const pearl::CallbackManager& cbmanager,
             int                           user_event,
             const pearl::Event&           event,
             pearl::CallbackData*          data);

        void
        enter(const pearl::CallbackManager& cbmanager,
              int                           user_event,
              const pearl::Event&           event,
              pearl::CallbackData*          data);

        double*
        get_results();


    private:
        int                   rank, size;
        std::vector< double > result;
        std::vector< int >    counter;
        MessageChecker        mc;
        funcs                 func;
        modes                 mode;

        // Arrays for MPI message handling - see MPI.pattern in Scout
        std::vector< pearl::MpiMessage* > m_pending;
        std::vector< MPI_Request >        m_requests;
        std::vector< int >                m_indices;
        std::vector< MPI_Status >         m_statuses;


        void
        completion_check();
};


#endif    // !SALSA_SALSACALLBACK_H
