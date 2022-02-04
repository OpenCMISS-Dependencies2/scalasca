/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "ext-tap-printer.h"
#include "ext-tap-helper.h"

using namespace testing;
using namespace testing::internal;


//--- Event listener callbacks ----------------------------------------------

void
TapPrinter::OnTestProgramStart(const UnitTest& unit_test)
{
    PrintTapTestPlan(unit_test);
}


void
TapPrinter::OnTestEnd(const TestInfo& test_info)
{
    PrintTapTestLine(test_info);

    // Print details on failure
    const bool failed = test_info.result()->Failed();
    if (failed) {
        PrintTapSeparator();
        PrintTapDiagnostics(GetTestDetails(test_info));
        PrintTapSeparator();
    }
}


void
TapPrinter::OnTestIterationEnd(const UnitTest& unit_test,
                               int             iteration)
{
    PrintTapDisabledTests(unit_test);
}
