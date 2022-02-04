/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef GTEST_SRC_GTEST_EXT_MPI_TAP_PRINTER_H_
#define GTEST_SRC_GTEST_EXT_MPI_TAP_PRINTER_H_


#include <mpi.h>
#include <gtest/gtest.h>


namespace testing
{
// Implements a test event listener for the Google C++ Testing Framework
// producing output conforming to the Test Anything Protocol (TAP, see
// http://testanything.org) for tests using MPI.  In particular, it determines
// the test results across all MPI processes and generates TAP output only
// by a single rank.
//
class MpiTapPrinter
    : public EmptyTestEventListener
{
    public:
        // Fired before any test activity starts.
        virtual void
        OnTestProgramStart(const UnitTest& unit_test);

        // Fired after the test ends.
        virtual void
        OnTestEnd(const TestInfo& test_info);

        // Fired after each iteration of tests finishes.
        virtual void
        OnTestIterationEnd(const UnitTest& unit_test,
                           int             iteration);

        // Fired after all test activities have ended.
        virtual void
        OnTestProgramEnd(const UnitTest& unit_test);


    private:
        // Communicator used by TAP printer
        MPI_Comm mCommunicator;

        // Total number of MPI processes
        int mWorldSize;

        // Global MPI rank
        int mWorldRank;
};
}   // namespace testing


#endif   // !GTEST_SRC_GTEST_EXT_MPI_TAP_PRINTER_H_
