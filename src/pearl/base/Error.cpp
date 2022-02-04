/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2015                                                **
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

#include <pearl/Error.h>

using namespace std;
using namespace pearl;


// --------------------------------------------------------------------------
//
//  class Error
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and sets the error description to the given @a
 *  message.
 *
 *  @param message %Error description
 **/
Error::Error(const string& message)
    : m_message(message)
{
}


/**
 *  Destructor. Destroys the instance.
 **/
Error::~Error() throw()
{
}


// --- Get error message ----------------------------------------------------

/**
 *  Returns a description of the error condition encountered.
 *
 *  @return %Error description
 **/
const char*
Error::what() const throw()
{
    return m_message.c_str();
}


// --------------------------------------------------------------------------
//
//  class RuntimeError
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and sets the error description to the given @a
 *  message.
 *
 *  @param message %Error description
 **/
RuntimeError::RuntimeError(const string& message)
    : Error("PEARL: " + message)
{
}


// --------------------------------------------------------------------------
//
//  class FatalError
//
// --------------------------------------------------------------------------

// --- Constructors & destructor --------------------------------------------

/**
 *  Creates a new instance and sets the error description to the given @a
 *  message.
 *
 *  @param message %Error description
 **/
FatalError::FatalError(const string& message)
    : Error("PEARL Fatal: " + message)
{
}


// --------------------------------------------------------------------------
//
//  class MemoryError
//
// --------------------------------------------------------------------------

// --- Get error message ----------------------------------------------------

/**
 *  Returns a description of the error condition encountered.
 *
 *  @return %Error description
 **/
const char*
MemoryError::what() const throw()
{
    return "Not enough memory!";
}
