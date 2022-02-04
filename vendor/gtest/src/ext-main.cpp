/****************************************************************************
**  Copyright (c) 2013                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <gtest/gtest.h>

#include "ext-tap-printer.h"

using namespace testing;


int
main(int    argc,
     char** argv)
{
    // Initialization
    InitGoogleTest(&argc, argv);

    // Install TAP printer ("plain" version)
    TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());
    listeners.Append(new TapPrinter);

    // Run the tests...
    const int result = RUN_ALL_TESTS();

    // Finalization
    return result;
}
