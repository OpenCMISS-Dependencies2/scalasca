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


#ifndef CLC_FWDAMORTDATA_H
#define CLC_FWDAMORTDATA_H


#include <pearl/CallbackData.h>


class Synchronizer;


class FwdAmortData
    : public pearl::CallbackData
{
    public:
        /// @name Constructors & destructor
        /// @{

        FwdAmortData(Synchronizer* synchronizer);

        /// @}
        /// @name Amortization flag handling
        /// @{

        void
        setAmortized();

        /// @}
        /// @name Pre- and postprocessing
        /// @{

        virtual void
        preprocess(const pearl::Event& event);

        virtual void
        postprocess(const pearl::Event& event);

        /// @}


    private:
        /// %Synchronizer object to be used
        Synchronizer* mSynchronizer;

        /// Flag indicating whether the forward amortization has already been
        /// applied to an event or not
        bool mIsAmortized;
};


#endif    // !CLC_FWDAMORTDATA_H
