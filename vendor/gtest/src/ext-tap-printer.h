/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef GTEST_SRC_GTEST_EXT_TAP_PRINTER_H_
#define GTEST_SRC_GTEST_EXT_TAP_PRINTER_H_


#include <gtest/gtest.h>


namespace testing
{
// Implements a test event listener for the Google C++ Testing Framework
// producing output conforming to the Test Anything Protocol (TAP, see
// http://testanything.org).
//
class TapPrinter
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

};
}   // namespace testing


#endif   // !GTEST_SRC_GTEST_EXT_TAP_PRINTER_H_
