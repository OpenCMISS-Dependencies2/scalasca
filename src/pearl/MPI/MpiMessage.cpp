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

#include <pearl/MpiMessage.h>

#include <pearl/MpiComm.h>

using namespace pearl;


// --- Constructors & destructor --------------------------------------------

MpiMessage::MpiMessage(MPI_Comm comm,
                       int      size)
    : Buffer(size),
      m_comm(comm),
      m_request(MPI_REQUEST_NULL)
{
}


MpiMessage::MpiMessage(const MpiComm& comm,
                       int            size)
    : Buffer(size),
      m_comm(comm.getHandle()),
      m_request(MPI_REQUEST_NULL)
{
}


// --- Sending & receiving --------------------------------------------------

void
MpiMessage::send(int dest,
                 int tag)
{
    MPI_Send(m_data, m_size, MPI_BYTE, dest, tag, m_comm);
}


void
MpiMessage::isend(int dest,
                  int tag)
{
    MPI_Isend(m_data, m_size, MPI_BYTE, dest, tag, m_comm, &m_request);
}


void
MpiMessage::issend(int dest,
                   int tag)
{
    MPI_Issend(m_data, m_size, MPI_BYTE, dest, tag, m_comm, &m_request);
}


bool
MpiMessage::test()
{
    int        flag;
    MPI_Status status;
    MPI_Test(&m_request, &flag, &status);

    return flag;
}


void
MpiMessage::wait()
{
    MPI_Status status;
    MPI_Wait(&m_request, &status);
}


void
MpiMessage::cancel()
{
    MPI_Cancel(&m_request);
    MPI_Request_free(&m_request);
}


void
MpiMessage::recv(int source,
                 int tag)
{
    MPI_Status status;
    int        length;

    // Retrieve message length
    MPI_Probe(source, tag, m_comm, &status);
    MPI_Get_count(&status, MPI_BYTE, &length);
    if (static_cast< size_t >(length) > m_size)
    {
        delete[] m_data;
        m_data     = new uint8_t[length];
        m_capacity = length;
        m_size     = length;
    }

    MPI_Recv(m_data, length, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG,
             m_comm, &status);
}


bool
MpiMessage::probe(int source,
                  int tag)
{
    int        flag = 0;
    MPI_Status status;
    MPI_Iprobe(source, tag, m_comm, &flag, &status);
    if (flag)
    {
        int length;
        MPI_Get_count(&status, MPI_BYTE, &length);
        if (static_cast< size_t >(length) > m_size)
        {
            reserve(static_cast< size_t >(length) - m_size);
        }
        MPI_Recv(m_data, length, MPI_BYTE, status.MPI_SOURCE,
                 status.MPI_TAG, m_comm, &status);
        m_size     = length;
        m_position = 1;

        return true;
    }

    return false;
}


void
MpiMessage::bcast(int root)
{
    int length = m_size;

    // Retrieve message length
    MPI_Bcast(&length, 1, MPI_INT, root, m_comm);
    if (static_cast< size_t >(length) > m_size)
    {
        delete[] m_data;
        m_data     = new uint8_t[length];
        m_capacity = length;
        m_size     = length;
    }

    MPI_Bcast(m_data, length, MPI_BYTE, root, m_comm);
}


// --- Retrieving message information ---------------------------------------

MPI_Comm
MpiMessage::get_comm() const
{
    return m_comm;
}


MPI_Request
MpiMessage::get_request() const
{
    return m_request;
}
