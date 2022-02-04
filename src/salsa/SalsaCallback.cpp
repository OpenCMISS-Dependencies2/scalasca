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


#include <config.h>

#include "SalsaCallback.h"

#include <algorithm>

#include <pearl/Event.h>
#include <pearl/Region.h>

using namespace pearl;


SalsaCallback::SalsaCallback(int            r,
                             int            s,
                             funcs          f,
                             modes          m,
                             MessageChecker mc)
    : result(s, 0),
      counter(s, 0),
      mc(mc)
{
    rank = r;
    size = s;
    func = f;
    mode = m;
}


SalsaCallback::~SalsaCallback()
{
}


void
SalsaCallback::send(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          data)
{
    // Generate Message object
    MpiMessage* msg = new MpiMessage(*(event->getComm()), 512);

    // Register send event
    msg->put_event(event);

    // Send message
    msg->isend(event->getDestination(), event->getTag());

    // Store Message object in vector
    m_pending.push_back(msg);
    m_requests.push_back(msg->get_request());

    // Check for completion of previous messages
    completion_check();
}


void
SalsaCallback::recv(const CallbackManager& cbmanager,
                    int                    user_event,
                    const Event&           event,
                    CallbackData*          data)
{
    CallbackDataneu* dneu    = static_cast< CallbackDataneu* >(data);
    double           tmp_val = 0.0;

    MpiMessage msg(*(event->getComm()), 512);
    msg.recv(event->getSource(), event->getTag());

    RemoteEvent send_event = msg.get_event(*(dneu->defs));

    if (!mc.is_applicable(send_event))
    {
        return;
    }

    uint64_t    sent      = send_event->getBytesSent();
    timestamp_t send_time = send_event->getTimestamp();
    timestamp_t recv_time = event->getTimestamp();
    timestamp_t diff      = recv_time - send_time;
    int         id        = send_event.get_location().getRank();

    switch (func)
    {
        case COUNT:
            result[id]++;
            break;

        case LENGTH:
            tmp_val = sent;
            break;

        case DURATION:
            tmp_val = diff;
            break;

        case RATE:
            tmp_val = sent / diff;
            break;

        default:
            tmp_val = 0.0;
            break;
    }

    if (func != COUNT)
    {
        switch (mode)
        {
            case MINIMUM:
                if (  (result[id] == 0)
                   || (tmp_val < result[id]))
                {
                    result[id] = tmp_val;
                }
                break;

            case MAXIMUM:
                if (  (result[id] == 0)
                   || (tmp_val > result[id]))
                {
                    result[id] = tmp_val;
                }
                break;

            case AVERAGE:
                result[id] += tmp_val;
                counter[id]++;
                break;

            case SUM:
                result[id] += tmp_val;
                break;
        }
    }

    // Check for completion of previous messages
    completion_check();
}


void
SalsaCallback::enter(const CallbackManager& cbmanager,
                     int                    user_event,
                     const Event&           event,
                     CallbackData*          data)
{
    // Check whether entering MPI_Finalize
    if (is_mpi_finalize(event->getRegion()))
    {
        completion_check();
    }
}


double*
SalsaCallback::get_results()
{
    if (mode == AVERAGE)
    {
        for (int i = 0; i < size; i++)
        {
            if (counter[i] != 0)
            {
                result[i] /= counter[i];
            }
        }
    }

    return &result.front();
}


void
SalsaCallback::completion_check()
{
    // No pending requests? ==> continue
    if (m_pending.empty())
    {
        return;
    }

    // Check for completed messages
    int completed;
    int count = m_pending.size();

    m_indices.resize(count);
    m_statuses.resize(count);


    MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

    // Update array of pending messages
    for (int i = 0; i < completed; ++i)
    {
        int index = m_indices[i];

        delete m_pending[index];
        m_pending[index] = NULL;
    }
    m_pending.erase(std::remove(m_pending.begin(), m_pending.end(),
                                static_cast< MpiMessage* >(NULL)),
                    m_pending.end());
    m_requests.erase(std::remove(m_requests.begin(), m_requests.end(),
                                 static_cast< MPI_Request >(MPI_REQUEST_NULL)),
                     m_requests.end());
}
