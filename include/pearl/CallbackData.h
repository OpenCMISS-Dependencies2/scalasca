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


#ifndef PEARL_CALLBACKDATA_H
#define PEARL_CALLBACKDATA_H


/*-------------------------------------------------------------------------*/
/**
 *  @file    CallbackData.h
 *  @ingroup PEARL_replay
 *  @brief   Declaration of the class CallbackData.
 *
 *  This header file provides the declaration of the class CallbackData.
 **/
/*-------------------------------------------------------------------------*/


namespace pearl
{
// --- Forward declarations -------------------------------------------------

class Event;


/*-------------------------------------------------------------------------*/
/**
 *  @class   CallbackData
 *  @ingroup PEARL_replay
 *  @brief   Base class for data objects passed to callback function during
 *           replay.
 **/
/*-------------------------------------------------------------------------*/

class CallbackData
{
    public:
        /// @name Constructors & destructor
        /// @{

        virtual
        ~CallbackData();

        /// @}
        /// @name Pre- and postprocessing
        /// @{

        virtual void
        preprocess(const Event& event);

        virtual void
        postprocess(const Event& event);

        /// @}
};
}    // namespace pearl


#endif    // !PEARL_CALLBACKDATA_H
