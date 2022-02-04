/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "ext-tap-helper.h"

#include <cassert>
#include <cstdio>
#include <gtest/gtest.h>

using namespace std;
using namespace testing;


//--- Internal helper functions ---------------------------------------------

namespace
{
    string
    PrefixString(const string& prefix,
                 const string& message)
    {
        string result;

        // Insert prefix before each line
        string::size_type pos = 0;
        string::size_type end = message.find('\n');
        while (end != string::npos) {
            result += prefix + message.substr(pos, end - pos + 1);

            pos = end + 1;
            end = message.find('\n', pos);
        }
        result += prefix + message.substr(pos, string::npos);

        return result;
    }
}   // unnamed namespace


//--- Generic helper functions ----------------------------------------------

string
testing::internal::IntToString(const int value)
{
    char result[32];
    snprintf(result, sizeof(result), "%d", value);

    return string(result);
}


string
testing::internal::GetTestDetails(const TestInfo& test_info)
{
    const TestResult* test_result = test_info.result();

    // Return success message if there was no failure
    const bool passed = test_result->Passed();
    if (passed) {
        return "Success.";
    }

    string details;

    // Iterate over partial results
    const int num_parts = test_result->total_part_count();
    for (int part_index = 0; part_index < num_parts; ++part_index) {
        const TestPartResult& test_part = test_result->GetTestPartResult(part_index);

        // Add details for test parts which failed
        if (test_part.failed()) {
            // Separate multiple test part details
            if (!details.empty()) {
                details += "\n\n";
            }

            // Add source-code location
            details += "Failure in ";
            details += test_part.file_name();
            details += ":";
            details += IntToString(test_part.line_number());
            details += ":\n";

            // Add failure message
            details += PrefixString("    ", test_part.message());
        }
    }

    return details;
}


//--- TAP helper functions --------------------------------------------------

void
testing::internal::PrintTapTestPlan(const UnitTest& unit_test)
{
    const int num_iters = GTEST_FLAG(repeat);
    const int num_tests = unit_test.total_test_count();
    if (num_iters >= 0) {
        printf("1..%d\n", num_iters * num_tests);
        fflush(stdout);
    }
}


void
testing::internal::PrintTapTestLine(const TestInfo& test_info,
                                    const bool      global_failed)
{
    // Print overall result
    const bool failed = global_failed || test_info.result()->Failed();
    if (failed) {
        printf("not ok");
    } else {
        printf("ok    ");
    }

    // Print test name
    const char* case_name = test_info.test_case_name();
    const char* test_name = test_info.name();
    printf(" - %s.%s", case_name, test_name);

    // Optionally print type/value info
    const char* type_param  = test_info.type_param();
    if (type_param) {
        printf(" (using T=%s)", type_param);
    }
    const char* value_param = test_info.value_param();
    if (value_param) {
        printf(" (with value=%s)", value_param);
    }

    // Print SKIP directive for disabled tests
    const bool disabled = !test_info.should_run();
    if (disabled) {
        printf(" # SKIP");
    }
    printf("\n");
    fflush(stdout);
}


void
testing::internal::PrintTapDiagnostics(const string& message)
{
    string diagnostics = PrefixString("#   ", message);

    printf("%s\n", diagnostics.c_str());
    fflush(stdout);
}


void
testing::internal::PrintTapSeparator(const string& title)
{
    assert(title.length() <= 62);

    // Construct separator string
    string separator = "\n---";
    if (!title.empty()) {
        separator += "[ " + title + " ]";
    }
    const int numDashes = 72 - separator.length();
    separator += string(numDashes, '-');
    separator += '\n';

    PrintTapDiagnostics(separator);
}


void
testing::internal::PrintTapDisabledTests(const UnitTest& unit_test)
{
    // Iterate over all test cases
    const int num_cases = unit_test.total_test_case_count();
    for (int case_index = 0; case_index < num_cases; ++case_index) {
        const TestCase* test_case = unit_test.GetTestCase(case_index);

        // Iterate over all tests
        const int num_tests = test_case->total_test_count();
        for (int test_index = 0; test_index < num_tests; ++test_index) {
            const TestInfo* test_info = test_case->GetTestInfo(test_index);

            // Print TAP test line for disabled tests
            const bool disabled = !test_info->should_run();
            if (disabled) {
                PrintTapTestLine(*test_info);
            }
        }
    }
}
