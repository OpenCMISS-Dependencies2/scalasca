/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
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


#include <config.h>

#include <pearl/CallbackManager.h>

#include <pearl/Event.h>

using namespace std;
using namespace pearl;


// --- Utility functions ----------------------------------------------------

namespace
{
template< class containerT >
void
execute_callbacks(const containerT&             cbmap,
                  typename containerT::key_type key,
                  const CallbackManager&        cbmanager,
                  int                           user_event,
                  const Event&                  event,
                  CallbackData*                 data)
{
    // Any callbacks defined?
    typename containerT::const_iterator it = cbmap.find(key);
    if (it == cbmap.end())
    {
        return;
    }

    // Execute callback routines
    typename containerT::mapped_type::const_iterator cb = it->second.begin();
    while (cb != it->second.end())
    {
        (*cb)->execute(cbmanager, user_event, event, data);
        ++cb;
    }
}
}    // unnamed namespace


// --- Registering callbacks ------------------------------------------------

void
CallbackManager::register_callback(event_t     event_type,
                                   CallbackPtr callback)
{
    switch (event_type)
    {
        case GROUP_ALL:
            m_event_cbs[ENTER                   ].push_back(callback);
            m_event_cbs[ENTER_CS                ].push_back(callback);
            m_event_cbs[ENTER_PROGRAM           ].push_back(callback);
            m_event_cbs[LEAVE                   ].push_back(callback);
            m_event_cbs[LEAVE_PROGRAM           ].push_back(callback);
            m_event_cbs[MPI_COLLECTIVE_BEGIN    ].push_back(callback);
            m_event_cbs[MPI_COLLECTIVE_END      ].push_back(callback);
            m_event_cbs[MPI_SEND                ].push_back(callback);
            m_event_cbs[MPI_SEND_REQUEST        ].push_back(callback);
            m_event_cbs[MPI_SEND_COMPLETE       ].push_back(callback);
            m_event_cbs[MPI_RECV                ].push_back(callback);
            m_event_cbs[MPI_RECV_REQUEST        ].push_back(callback);
            m_event_cbs[MPI_RECV_COMPLETE       ].push_back(callback);
            m_event_cbs[MPI_REQUEST_TESTED      ].push_back(callback);
            m_event_cbs[MPI_CANCELLED           ].push_back(callback);
            m_event_cbs[THREAD_BEGIN            ].push_back(callback);
            m_event_cbs[THREAD_END              ].push_back(callback);
            m_event_cbs[THREAD_CREATE           ].push_back(callback);
            m_event_cbs[THREAD_WAIT             ].push_back(callback);
            m_event_cbs[THREAD_FORK             ].push_back(callback);
            m_event_cbs[THREAD_JOIN             ].push_back(callback);
            m_event_cbs[THREAD_ACQUIRE_LOCK     ].push_back(callback);
            m_event_cbs[THREAD_RELEASE_LOCK     ].push_back(callback);
            m_event_cbs[THREAD_TASK_CREATE      ].push_back(callback);
            m_event_cbs[THREAD_TASK_COMPLETE    ].push_back(callback);
            m_event_cbs[THREAD_TASK_SWITCH      ].push_back(callback);
            m_event_cbs[THREAD_TEAM_BEGIN       ].push_back(callback);
            m_event_cbs[THREAD_TEAM_END         ].push_back(callback);
            m_event_cbs[RMA_PUT_START           ].push_back(callback);
            m_event_cbs[RMA_PUT_END             ].push_back(callback);
            m_event_cbs[RMA_GET_START           ].push_back(callback);
            m_event_cbs[RMA_GET_END             ].push_back(callback);
            m_event_cbs[MPI_RMA_PUT_START       ].push_back(callback);
            m_event_cbs[MPI_RMA_PUT_END         ].push_back(callback);
            m_event_cbs[MPI_RMA_GET_START       ].push_back(callback);
            m_event_cbs[MPI_RMA_GET_END         ].push_back(callback);
            m_event_cbs[MPI_RMA_GATS            ].push_back(callback);
            m_event_cbs[MPI_RMA_COLLECTIVE_BEGIN].push_back(callback);
            m_event_cbs[MPI_RMA_COLLECTIVE_END  ].push_back(callback);
            m_event_cbs[MPI_RMA_LOCK            ].push_back(callback);
            m_event_cbs[MPI_RMA_UNLOCK          ].push_back(callback);
            break;

        case GROUP_NONBLOCK:
            m_event_cbs[MPI_SEND_REQUEST        ].push_back(callback);
            m_event_cbs[MPI_SEND_COMPLETE       ].push_back(callback);
            m_event_cbs[MPI_RECV_REQUEST        ].push_back(callback);
            m_event_cbs[MPI_RECV_COMPLETE       ].push_back(callback);
            m_event_cbs[MPI_REQUEST_TESTED      ].push_back(callback);
            m_event_cbs[MPI_CANCELLED           ].push_back(callback);
            break;

        case GROUP_ENTER:
            m_event_cbs[ENTER                   ].push_back(callback);
            m_event_cbs[ENTER_CS                ].push_back(callback);
            m_event_cbs[ENTER_PROGRAM           ].push_back(callback);
            break;

        case GROUP_LEAVE:
            m_event_cbs[LEAVE                   ].push_back(callback);
            m_event_cbs[LEAVE_PROGRAM           ].push_back(callback);
            break;

        case GROUP_SEND:
            m_event_cbs[MPI_SEND                ].push_back(callback);
            m_event_cbs[MPI_SEND_REQUEST        ].push_back(callback);
            break;

        case GROUP_RECV:
            m_event_cbs[MPI_RECV                ].push_back(callback);
            m_event_cbs[MPI_RECV_COMPLETE       ].push_back(callback);
            break;

        case GROUP_BEGIN:
            m_event_cbs[MPI_COLLECTIVE_BEGIN    ].push_back(callback);
            m_event_cbs[MPI_RMA_COLLECTIVE_BEGIN].push_back(callback);
            break;

        case GROUP_END:
            m_event_cbs[MPI_COLLECTIVE_END      ].push_back(callback);
            m_event_cbs[MPI_RMA_COLLECTIVE_END  ].push_back(callback);
            break;

        default:
            m_event_cbs[event_type              ].push_back(callback);
            break;
    }
}


void
CallbackManager::register_callback(int         user_event,
                                   CallbackPtr callback)
{
    m_user_cbs[user_event].push_back(callback);
}


// --- Notification routines ------------------------------------------------

void
CallbackManager::notify(const Event&  event,
                        CallbackData* data) const
{
    execute_callbacks(m_event_cbs, event->getType(), *this, 0, event, data);
}


void
CallbackManager::notify(int           user_event,
                        const Event&  event,
                        CallbackData* data) const
{
    execute_callbacks(m_user_cbs, user_event, *this, user_event, event, data);
}
