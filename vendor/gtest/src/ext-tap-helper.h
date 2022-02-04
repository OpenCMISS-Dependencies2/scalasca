/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef GTEST_SRC_GTEST_EXT_TAP_HELPER_H_
#define GTEST_SRC_GTEST_EXT_TAP_HELPER_H_


#include <string>


namespace testing
{
    // Forward declarations
    class TestInfo;
    class UnitTest;


    namespace internal
    {
        // Generic helper functions
        std::string
        IntToString(int value);
        std::string
        GetTestDetails(const TestInfo& test_info);

        // TAP helper functions
        void
        PrintTapTestPlan(const UnitTest& unit_test);
        void
        PrintTapTestLine(const TestInfo& test_info,
                         bool            global_failed=false);
        void
        PrintTapDiagnostics(const std::string& message);
        void
        PrintTapSeparator(const std::string& title="");
        void
        PrintTapDisabledTests(const UnitTest& unit_test);
    } // namespace internal
}     // namespace testing


#endif   // !GTEST_SRC_GTEST_EXT_TAP_HELPER_H_
